# UVC——USB Video Class驱动

 按照顺序分析：

## 驱动入口
```cpp
static int __init uvc_init(void)
{
	int ret = usb_register(&uvc_driver.driver);
	
	if (ret < 0) {
		uvc_debugfs_cleanup();
		return ret;
	}

	return 0;
}
```
驱动入口函数实际上只是调用了一个`usb_register()`函数。但是请注意函数的参数——`uvc_driver.driver`：

```cpp
struct uvc_driver uvc_driver = {
	.driver = {
		.name		= "uvcvideo",
		.probe		= uvc_probe,
		.disconnect	= uvc_disconnect,
		.suspend	= uvc_suspend,
		.resume		= uvc_resume,
		.reset_resume	= uvc_reset_resume,
		.id_table	= uvc_ids,
		.supports_autosuspend = 1,
	},
};
```
参数中定义了`driver的name和probe`函数；

### usb_register_driver()函数

`usb_register()`函数调用`usb_register_driver()`完成USB接口驱动的注册。 在一个新的USB驱动添加的时候都会重新扫描`unattached interfaces`的链表。

这里可以借助`platform`驱动的注册机制展开分析，USB驱动是具有真实`bus`的。因此，UVC的设备驱动中一定会有`usb_bus_type`成员，用于匹配设备和驱动和数据通信。`usb_register_driver()`函数对UVC驱动进行简单赋值之后就调用`driver_register()`函数来完成核心的驱动注册工作——调用`USB总线的probe()函数`和`UVC驱动的probe()函数`。

> **NOTE**: if you want your driver to use the USB major number, you must call usb_register_dev() to enable that functionality.  This function no longer takes care of that.

`usb_register_driver()`代码如下：
```cpp
int usb_register_driver(struct usb_driver *new_driver, struct module *owner,
			const char *mod_name)
{
	int retval = 0;

	if (usb_disabled())
		return -ENODEV;

	new_driver->drvwrap.for_devices = 0;
	new_driver->drvwrap.driver.name = new_driver->name;
	new_driver->drvwrap.driver.bus = &usb_bus_type;
	new_driver->drvwrap.driver.probe = usb_probe_interface;
	new_driver->drvwrap.driver.remove = usb_unbind_interface;
	new_driver->drvwrap.driver.owner = owner;
	new_driver->drvwrap.driver.mod_name = mod_name;
	
	spin_lock_init(&new_driver->dynids.lock);
	INIT_LIST_HEAD(&new_driver->dynids.list);

	retval = driver_register(&new_driver->drvwrap.driver);
	if (retval)
		goto out;

	retval = usb_create_newid_files(new_driver);
	if (retval)
		goto out_newid;
}
```


### uvc_probe()
probe函数实现的主要功能有：

- 1 分配dev所需的内存空间，struct uvc_device *dev; 
- 2 初始化dev，
	- (1) 创建3个链表，entities、chains和streams；
	- (2) 设置usb_device(udev) 和 (usb_interface)intf；
	- (3) 设置quirks?
	- (4) 设置name；
	- (5) 解析  Video Class 的控制描述符.
- 3 注册为media多媒体设备，media_device_register();
- 4 注册为v4l2_device，v4l2_device_register()其实就是dev_set_drvdata(dev, v4l2_dev)；
- 5 初始化设备的控制(操作对象是entities)。添加control information和control mapping，并移除一些黑名单和不支持的control。
- 6  扫描UVC设备(操作对象是chains)。一旦扫描到一个设备就添加到设备链表dev->chains中，如果扫描后得到的dev->chains中的设备链表为空，说明没有找到有效的设备；`uvc_scan_chain_entity(chain, entity)`、u`vc_scan_chain_forward(chain, entity, prev)`和`uvc_scan_chain_backward(chain, &entity)`;
- 7 注册设备链表dev->chains中包含的所有的设备， uvc_register_chains();
- 8 将intr->dev的driver_data设置为dev，usb_set_intfdata(intf, dev)；
- 9 初始化中断URB，uvc_status_init();
- 10 使能USB设备的自动挂起功能，usb_enable_autosuspend()；

`uvc_probe()`函数代码如下：
```cpp

static int uvc_probe(struct usb_interface *intf,
		     const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(intf);
	struct uvc_device *dev;
	int ret;

	if (id->idVendor && id->idProduct)
		uvc_trace(UVC_TRACE_PROBE, "Probing known UVC device %s "
				"(%04x:%04x)\n", udev->devpath, id->idVendor,
				id->idProduct);
	else
		uvc_trace(UVC_TRACE_PROBE, "Probing generic UVC device %s\n",
				udev->devpath);

	/* Allocate memory for the device and initialize it. */
	if ((dev = kzalloc(sizeof *dev, GFP_KERNEL)) == NULL)
		return -ENOMEM;

	INIT_LIST_HEAD(&dev->entities);
	INIT_LIST_HEAD(&dev->chains);
	INIT_LIST_HEAD(&dev->streams);
	atomic_set(&dev->nstreams, 0);
	atomic_set(&dev->nmappings, 0);
	mutex_init(&dev->lock);

	dev->udev = usb_get_dev(udev);
	dev->intf = usb_get_intf(intf);
	dev->intfnum = intf->cur_altsetting->desc.bInterfaceNumber;
	dev->quirks = (uvc_quirks_param == -1)
		    ? id->driver_info : uvc_quirks_param;

	if (udev->product != NULL)
		strlcpy(dev->name, udev->product, sizeof dev->name);
	else
		snprintf(dev->name, sizeof dev->name,
			"UVC Camera (%04x:%04x)",
			le16_to_cpu(udev->descriptor.idVendor),
			le16_to_cpu(udev->descriptor.idProduct));

	/* Parse the Video Class control descriptor. */
	if (uvc_parse_control(dev) < 0) {
		uvc_trace(UVC_TRACE_PROBE, "Unable to parse UVC "
			"descriptors.\n");
		goto error;
	}

	uvc_printk(KERN_INFO, "Found UVC %u.%02x device %s (%04x:%04x)\n",
		dev->uvc_version >> 8, dev->uvc_version & 0xff,
		udev->product ? udev->product : "<unnamed>",
		le16_to_cpu(udev->descriptor.idVendor),
		le16_to_cpu(udev->descriptor.idProduct));

	if (dev->quirks != id->driver_info) {
		uvc_printk(KERN_INFO, "Forcing device quirks to 0x%x by module "
			"parameter for testing purpose.\n", dev->quirks);
		uvc_printk(KERN_INFO, "Please report required quirks to the "
			"linux-uvc-devel mailing list.\n");
	}

	/* Register the media and V4L2 devices. */
#ifdef CONFIG_MEDIA_CONTROLLER
	dev->mdev.dev = &intf->dev;
	strlcpy(dev->mdev.model, dev->name, sizeof(dev->mdev.model));
	if (udev->serial)
		strlcpy(dev->mdev.serial, udev->serial,
			sizeof(dev->mdev.serial));
	strcpy(dev->mdev.bus_info, udev->devpath);
	dev->mdev.hw_revision = le16_to_cpu(udev->descriptor.bcdDevice);
	dev->mdev.driver_version = LINUX_VERSION_CODE;
	if (media_device_register(&dev->mdev) < 0)
		goto error;

	dev->vdev.mdev = &dev->mdev;
#endif
	if (v4l2_device_register(&intf->dev, &dev->vdev) < 0)
		goto error;

	/* Initialize controls. */
	if (uvc_ctrl_init_device(dev) < 0)
		goto error;

	/* Scan the device for video chains. */
	if (uvc_scan_device(dev) < 0)
		goto error;

	/* Register video device nodes. */
	if (uvc_register_chains(dev) < 0)
		goto error;

	/* Save our data pointer in the interface data. */
	usb_set_intfdata(intf, dev);

	/* Initialize the interrupt URB. */
	if ((ret = uvc_status_init(dev)) < 0) {
		uvc_printk(KERN_INFO, "Unable to initialize the status "
			"endpoint (%d), status interrupt will not be "
			"supported.\n", ret);
	}

	uvc_trace(UVC_TRACE_PROBE, "UVC device initialized.\n");
	usb_enable_autosuspend(udev);
	return 0;

error:
	uvc_unregister_video(dev);
	return -ENODEV;
}
```
#### uvc_scan_device()函数  
 扫描设备并添加到`video chains`中，便于之后的设备注册。从`output terminals`开始反向扫描。`dev->entities`中的一个`uvc_entity term`对应一个`dev->chains`，但是我们要找的只有`uvc_entity output terminals`。
 
 - 1. 遍历`dev->entities`；
 - 2. 判断是否为`output terminals`；
 - 3. 分配`chain`的空间；
 - 4. 初始化`chain`：
	 - (1) `entities`；
	 - (2) `dev`；
	 - (3) 清空chain->prio;
- 5 扫描这个output terminals是否为一个`dev->chains`节点；
- 6 如果是，将其添加到`dev->chains`中；反之继续遍历；

代码如下：
```cpp
static int uvc_scan_device(struct uvc_device *dev)
{
	struct uvc_video_chain *chain;
	struct uvc_entity *term;

	list_for_each_entry(term, &dev->entities, list) {
		if (!UVC_ENTITY_IS_OTERM(term))
			continue;

		/* 如果terminal已经包含了一个chain，就略过它。
		 * 当chains有多个output terminals 的时候就可能会出现这种情况。
		 * 在前向扫描时，除了第一个之外，其他所有的 output terminals
		 * 都会被插入到chain中
		 */
		if (term->chain.next || term->chain.prev)
			continue;

		chain = kzalloc(sizeof(*chain), GFP_KERNEL);
		if (chain == NULL)
			return -ENOMEM;

		INIT_LIST_HEAD(&chain->entities);
		mutex_init(&chain->ctrl_mutex);
		chain->dev = dev;
		v4l2_prio_init(&chain->prio);

		term->flags |= UVC_ENTITY_FLAG_DEFAULT;

		if (uvc_scan_chain(chain, term) < 0) {
			kfree(chain);
			continue;
		}

		uvc_trace(UVC_TRACE_PROBE, "Found a valid video chain (%s).\n",
			  uvc_print_chain(chain));

		list_add_tail(&chain->list, &dev->chains);
	}

	if (list_empty(&dev->chains)) {
		uvc_printk(KERN_INFO, "No valid video chain found.\n");
		return -1;
	}

	return 0;
}
```

#### uvc_register_chains()函数 
`uvc_scan_device()`扫描完成之后就开始注册video 设备节点。

遍历`dev->chains`中的`struct uvc_video_chain`节点，然后调用`uvc_register_terms()`，将每个节点注册到dev中。

注册的工作主要是由`uvc_register_video()`函数完成。

```cpp
static int uvc_register_chains(struct uvc_device *dev)
{
	struct uvc_video_chain *chain;
	int ret;

	list_for_each_entry(chain, &dev->chains, list) {
		ret = uvc_register_terms(dev, chain);
		if (ret < 0)
			return ret;

#ifdef CONFIG_MEDIA_CONTROLLER
		ret = uvc_mc_register_entities(chain);
		if (ret < 0) {
			uvc_printk(KERN_INFO, "Failed to register entites "
				"(%d).\n", ret);
		}
#endif
	}

	return 0;
}
```














## uvc协议标准上的描述符布局
IDA(Interface Association Descript)接口描述符
-->标准VC(video control)接口描述符
-->uvc类视频接口描述符(header)
-->输入Terminal接口描述符
-->处理Unit接口描述符
-->编码Unit接口描述符
-->输出Terminal接口描述符
-->标准中断端点描述符
-->uvc类中断端点描述符
-->标准VS(video streaming) 接口描述符
--------------------------------VS(video streaming) Alt.Setting 0

-->uvc类视频接口描述符(header)
-->format负荷格式描述符
-->若干frame
-->静态图像帧格式描述符
-->uvc类视频接口描述符(header)
-->format负荷格式描述符
-->若干frame
-->静态图像帧格式描述符
->颜色匹配描述符

...(1...n)
-->Bulk-in 静态图像数据端点描述符
-->标准VS接口描述符-VS(video streaming) Alt.Setting 1
	-->标准同步输入视频端点描述符
	-->Bulk-in 静态图像数据端点描述符
...(1...n)
-->标准VS接口描述符	--------------------------------VS(video streaming) Alt.Setting n
	-->标准同步输入视频端点描述符
	-->Bulk-in 静态图像数据端点描述符
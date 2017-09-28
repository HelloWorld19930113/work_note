# rtems_px4项目中使用不同波特率串口的参数设置
 参数设置包含两部分：一部分是飞控软件层，一部分是QGC层。

## QGC 在新版本中使用了自动连接设备，它会自动检测系统中的`/dev/ttyUSB*`设备，然后接收其上的信息，如果信息解析成功，则自动连接到这个设备节点。

可以在QGC源码中将其关闭，修改的文件为：
`qgroundcontrol/src/Settings/AutoConnect.SettingsGroup.json`，将其中的`defaultValue`设置为`false`即可。



1. 57600





# I2C驱动

[TOC]

 I2C驱动一般分为两部分：总线驱动和设备驱动。一般是先实现总线驱动，后实现设备驱动。二者相比而言，总线驱动要比设备驱动复杂的多。 

 **几个重要的API:**
`i2c_bus`的注册函数：

```cpp
i2c_bus *i2c_bus_alloc_and_init(size_t size);

bus->i2c_regs = i2c_regs;
bus->irq_vector_num = irq_vector_num;
bus->clock = input_clock;
bus->i2c_bus_id = i2c_bus_id;

bus->transfer = am335x_i2c_transfer;
bus->set_clock = am335x_i2c_set_clock;
bus->destroy = am335x_i2c_destroy;
	
int i2c_bus_register(i2c_bus *bus, char *bus_path);
```

 至此，`i2c总线`已经被注册到`RTEMS系统`中。接下来就可以在这条总线上挂载`i2c设备`了。

`i2c_dev`的注册函数：

```cpp 
i2c_dev *i2c_dev_alloc_and_init( size_t size, char *bus_path, uint16_t address)；

dev->read = meas_airspeed_read;
dev->write = NULL;

int i2c_dev_register(i2c_dev *dev, const char *dev_path);
```
 至此，依附于特定`i2c总线`的`i2c设备`也就被注册到`RTEMS系统`中了。接下来就可以使用通用文件操作API：`open(),write(),read,close()`函数来操作`i2c设备`了。

##I2C总线驱动
###i2c_bus 中的transfer()
 `i2c`驱动框架中，最核心的函数就是`transfer()`了。该函数中需要做好一系列的准备工作，最后发送起始位。
 这些准备工作包括：
- 打包要发送msg的属性（如msg个数，每个msg的长度，每个存放msg的buf）


## I2C设备驱动
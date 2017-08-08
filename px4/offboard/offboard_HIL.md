# PX4 offboard模式下的HIL
为了验证自己的offboard控制模式是否有效，在jMAVSim中搭建HIL环境进行测试。offboard的控制指令通过数传(TELEM2-57600/8/N/1)通信。

## 1.编译jMAVSim

```bash
$ cd src/Firmware/Tools/jMAVSim

# Build and run:
$ ant
```

## 2. 硬件在环仿真 (HITL)
参考PX4官网教程
以quadrotor X硬件在环仿真为例进行，给出配置PX4-HIL仿真的步骤.
a. 使用QGroundControl连接PX4,点击Setup-->点击Airframe-->选择Simulation中hil quadrotor x，然后点击Apply and Restart, Reconnect。
b. 在parameters中设置SYS_AUTOSTART = 1001, 保存参数并且重启。

## 3. 在HITL模式下运行jMAVSim.
这时要确保QGroundControl没有启动，并且未连接到串口。

```bash
$ java -Djava.ext.dirs= -cp lib/*:out/production/jmavsim.jar me.drton.jmavsim.Simulator -serial /dev/ttyACM0 921600 -qgc
```
这时运行上一条命令的中断将会显示autopilot返回的mavlink文本消息；这时启动QGroundControl，PX4就会自动连接到QGroundControl(如果没有连接，可以新建一个udp连接，保持默认端口不变，然后点击“connect” button链接)。

Ready to fly!

**注意：**
	一定要先启动jMAVSim的仿真GUI，再打开地面站。

##配置PX4的TELEM2为外部控制串口

For the companion computer to communicate with the flight controller, a USB2TTL converter is needed to convert the voltage of the communication levels. A brief overview of configuring the companion computer with the pixhawk is shown in this link.

offboard 模式中，推荐使用TELEM2串口进行通信。通过SYS_COMPANION参数可以激活TELEM2串口。这个参数在`qgroundcontrol`的参数列表中可以修改：
```
SYS_COMPANION 921600/57600
```

##编写外部控制指令

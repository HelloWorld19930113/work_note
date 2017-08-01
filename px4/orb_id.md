# uorb中的ORB_ID

[TOC]

##ORB_ID(vehicle_command)
飞行控制命令，包括飞行器的飞行模式(手动/增稳)。

**publish**
>***/src/modules/mavlink/mavlink_receiver.cpp***
>
>*/src/modules/navigator/mission_block.cpp*
>
>*/src/drivers/px4io/px4io.cpp*

**subscribe**
  >***src/modules/commander/commander.cpp***
  >
  >***src/modules/mavlink/mavlink_messages.cpp***
  >
  >***src/modules/navigator/navigator_main.cpp***
  >
  > *src/drivers/px4fmu/fmu.cpp*
 
**主要作用：**



## ORB_ID(parameter_update)

**publish**


**subscribe**
  >***src/modules/commander/commander.cpp***
  >
  >***src/modules/mavlink/mavlink_messages.cpp***
  >





## ORB_ID(actuator_armed)
armed：飞行器起飞，更新频率5Hz

## ORB_ID(vehicle_status)
vehicle status：飞行器状态，更新频率5Hz
**publish**
  >***src/modules/commander/commander.cpp***
  >
  >***src/modules/commander/state_machine_helper.cpp***
  >
 
 在commander中，对status数据进行发布。
```cpp
param_t _param_rc_in_off = param_find("COM_RC_IN_MODE");
...
param_get(_param_rc_in_off, &rc_in_off);
status.rc_input_mode = rc_in_off;
```
`"COM_RC_IN_MODE"`参数的初始值为`1`，因此`status.rc_input_mode == vehicle_status_s::RC_IN_MODE_OFF`，此时并不会检测遥控器shu。

**subscribe**
   >***src/modules/mavlink/mavlink_messages.cpp***
  >







## ORB_ID(vehicle_control_mode)
control mode：飞行器控制模式，更新频率5Hz

```cpp
set_control_mode()；
orb_publish(ORB_ID(vehicle_control_mode), control_mode_pub, &control_mode);
```

set_control_mode()函数根据 navigation_state 来设置飞行器的控制模式；
那么navigation_state是如何确定的？
在commander_thread_main()函数中对其初始化：
`status.nav_state = vehicle_status_s::NAVIGATION_STATE_MANUAL;`


## ORB_ID(vehicle_global_position)  
 表示飞机的全局位置。

**publish**
>***src/modules/ekf2/ekf2_main.cpp***
> 
> ***src/modules/mavlink/mavlink_receiver.cpp***
> 
> ***src/modules/position_estimator_inav/position_estimator_inav_main.cpp***

这三个发布源中，`mavlink_receiver`中实现的是`HIL(硬件在环)`模式。因此重要的发布源也就是扩展卡尔曼滤波中。


**subscribe**
  >***src/modules/commander/commander.cpp***
  >
  >***src/modules/navigator/navigator_main.cpp***
  >
  >***/src/modules/mavlink/mavlink_messages.cpp***
  >
  >***src/modules/attitude_estimator_q/attitude_estimator_q_main.cpp***
  >
  >***/src/modules/fw_att_control/fw_att_control_main.cpp***
  >
  >***src/modules/fw_pos_control_l1/fw_pos_control_l1_main.cpp***

**作用：**

在`MavlinkStreamAltitude`中就会将全局位置的消息发送到`QGC`。一般这种消息是用来在`QGC`界面上显示的。

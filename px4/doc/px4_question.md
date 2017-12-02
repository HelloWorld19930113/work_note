# PX4 中必须弄明白的几个问题     

1. `PWM_Limit`状态机中的`armed`和`pre_armed`      
`PWM_limit State Machine`这个状态机模型控制`PWM`的输出，用于`pre-armed`和`armed`的输入。在油门的`armed`信号产生之后提供一段延时，斜坡上升到`armed`信号。     
**输入**     
```
- armed:浆叶转动；
- pre-armed：舵机转动；
    - pre-armed会覆盖当前armed的状态；
    - 不管当前状态是什么，pre-armed信号会直接进入ON状态；pre-armed信号解除会恢复到之前的状态；
```
**状态**    
```
- INIT 和 OFF：输出disarmed值；
- RAMP：输出值介于最小值和disarmed之间；
- ON：根据遥控器的控制值来设置输出；
```
**状态转换图**
![](../pictures/pwm_limit_state.png)
**那么armed的状态是如何控制的呢？**
。。。
**ORB_ID(actuator_armed)**
```
publish
  src/modules/commander/commander.cpp
subscribe
  src/drivers/pwm_out_sim.cpp
```
由此可知，`armed`是由`commander`来控制的。然后`pwm_out`会判断`_armed`是否置位，如果是就`mix`。最后发布`mix`之后的数据：`orb_publish(ORB_ID(actuator_outputs), _outputs_pub, &outputs);`          

## ORB_ID(actuator_outputs)数据的生产者和消费者   
来源很多，但是大部分是根据遥控器输入mix之后来的；     
去向(只有这一个)      
```
src/modules/simulator/simulator_mavlink.cpp
```

## ORB_ID(actuator_controls_3)
```cpp
 _control_topics[0] = ORB_ID(actuator_controls_0);
	_control_topics[1] = ORB_ID(actuator_controls_1);
	_control_topics[2] = ORB_ID(actuator_controls_2);
	_control_topics[3] = ORB_ID(actuator_controls_3);
```
`orb_copy(_control_topics[i], _control_subs[i], &_controls[i]);`    

## mixer的用法  
```
usage:
	mixer load <device> <filename>
```

## poll()和orb_check()函数之间的区别。   


## sbus接收到的数据要如何作用于舵机？    

 
## Mix是怎么做到的？    

## ORB_ID(actuator_controls_0)~ORB_ID(actuator_controls_3)    
这4个数据分别用在什么地方？    
1. ORB_ID(actuator_controls_0)**    
```
**publish**
  src/modules/vtol_att_control/vtol_att_control_main.cpp
**subscribe**
  /src/drivers/pwm_out_rc_in/pwm_out_rc_in.cpp
```
2. ORB_ID(actuator_controls_1)**    
```
**publish**
  src/modules/vtol_att_control/vtol_att_control_main.cpp
**subscribe**
  /src/drivers/pwm_out_rc_in/pwm_out_rc_in.cpp
```

## ORB_ID(rc_channels)    
尽管信号无效，调试时依然发布`rc_channels topic`；      
```
/* publish rc_channels topic even if signal is invalid, for debug */
int instance;
orb_publish_auto(ORB_ID(rc_channels), &_rc_pub, &_rc, &instance, ORB_PRIO_DEFAULT);
``` 

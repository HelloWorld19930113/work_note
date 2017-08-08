# 位置控制——offboard模式

**NOTE:**
如果开启了加速度控制`ignore_acceleration_force = false`，那么位置控制、速度控制等将不可用！因此，
`ignore_acceleration_force = true`

## offboard 中MAVLink包到姿态控制的数据链路

### 1. 发送`目标位置`的MAVLink数据包
```cpp
	//初始化设置点的成员
	mavlink_set_position_target_local_ned_t sp;
	sp.type_mask = MAVLINK_MSG_SET_POSITION_TARGET_LOCAL_NED_VELOCITY &
				   MAVLINK_MSG_SET_POSITION_TARGET_LOCAL_NED_YAW_RATE /*& 
				   MAVLINK_MSG_SET_POSITION_TARGET_LOCAL_NED_FORCE*/;
	sp.coordinate_frame = MAV_FRAME_LOCAL_NED;
	sp.vx       = 0.0;
	sp.vy       = 0.0;
	sp.vz       = 0.0;
	sp.yaw_rate = 0.0;
	
	//set_position()
	sp.type_mask =
		MAVLINK_MSG_SET_POSITION_TARGET_LOCAL_NED_POSITION;

	sp.coordinate_frame = MAV_FRAME_LOCAL_NED;

	sp.x   = x;
	sp.y   = y;
	sp.z   = z;
	
	//更新设置点
	current_setpoint = sp;
	
	//获取已经填充好的设置点
	mavlink_set_position_target_local_ned_t sp = current_setpoint;  

	//检查系统参数
	if ( not sp.time_boot_ms )
		sp.time_boot_ms = (uint32_t) (get_time_usec()/1000);
	sp.target_system    = system_id;
	sp.target_component = autopilot_id;

	// -----------------------
	//  打包数据
	// -----------------------
	mavlink_msg_set_position_target_local_ned_encode(system_id, companion_id, &message, &sp);

	//将MAVLink数据包发送给串口
	int len = write_message(message);
```
说明：上述打包函数是在多个函数中实现的，为了便于说明问题，将它们放在了一个流程中。因此可能会出现同名的局部变量。

**`NOTE:`**
 有必要对其中的数据更新流程描述一下：
 采用了两个线程来分别处理数据的读写，实际上就是MAVLink数据包的发送和接收。
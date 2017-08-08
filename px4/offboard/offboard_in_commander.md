#offboard模式
 offboard为外部模式，

##外部模式应用场合

外部模式是遵循m2m理念来设计的模式，用外接芯片或机载电脑来命令pixhawk实现飞行，外部是一个大概念，外接通讯模块用地面站通过网络驱动自驾仪飞行也算是外部模式的一种应用场景。

外部模式是一种模块化的设计方式，飞控只做关于飞行的事情，而其他任务，比如视觉识别，障碍分析等，都可以使用外部设备（机载电脑或其他智能设备）来进行，并发送mavlink指令让飞控完成飞行任务。这样的工作模式，各尽其职，飞控完成实时性高的飞行任务，外部设备完成运算量、数据量大的其他任务，具有更高的安全性。从软件结构上，也降低了耦合。

外部模式接收来自 Cortex系列多核处理器的运算结果。通信采用SPI，串口，IIC模式，把多核处理器运算的结果，传输给飞控系统，来控制飞机的位置。

![](http://img.blog.csdn.net/20170313165643119?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvR2VuX1ll/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/Center)

# 进入offboard模式
##1.使用遥控器进入offboard模式
如果使用的地面站，我们会清楚的看到在飞行模式设置中可以设置一个通道来专门进入offboard模式，这种模式只是临时的进入offboard，我不推荐这种进入offboard模式的方式，offboard本就是一种应该全部由代码去完成的模式。


##2.代码中指定offboard模式
上面我们已经讲过了`vehicle_command`。我们可以使用`命令VEHICLE_CMD_NAV_GUIDED_ENABLE`来指定进入`offboard`模式。

需要注意的是，`offboard`模式比较特殊，直接使用命令来切换至`offboard`模式是不行的，我们需要先进行另外一个`topic`的发布----`offboard_control_mode`。

为了保证飞行的安全性，px4规定：必须要以最低`2Hz`的频率发布`offboard_control_mode`主题数据才能保证`offboard online`。如果机载计算机突然死机或者失去了联结，那么px4在`500ms`后没有收到通讯将自动切换到`开启offboard之前的模式`。

所以，我们需要创建一个线程，循环发布`offboard_control_mode`主题，如果做实际项目的时候，那么这个项目应该更改为，不停的接收机载计算机发来的心跳包，如果有就发布`offboard_control_mode`主题，这样才能保证安全性。

接下来，我们就可以使用`vehicle_command`切换到`offboard`模式了。进入`offboard`模式以后，我们会发现，无法使用位置命令，速度命令来命令飞机飞行，这是因为我们没有正确设置发布的`offboard_control_mode`数据。

`offboard_control_mode topic`结构如下：
```cpp
# Off-board control mode
bool ignore_thrust
bool ignore_attitude
bool ignore_bodyrate
bool ignore_position
bool ignore_velocity
bool ignore_acceleration_force
bool ignore_alt_hold
```

我们需要忽略掉加速度控制，才能进行速度、位置、姿态的控制`ignore_acceleration_force=true`;
原因在如下代码中：
```cpp
case vehicle_status_s::NAVIGATION_STATE_OFFBOARD:
control_mode.flag_control_manual_enabled = false;
control_mode.flag_control_auto_enabled = false;
control_mode.flag_control_offboard_enabled = true;
/*
* The control flags depend on what is ignored according to the offboard control mode topic
* Inner loop flags (e.g. attitude) also depend on outer loop ignore flags (e.g. position)
*/
control_mode.flag_control_rates_enabled = !offboard_control_mode.ignore_bodyrate ||
!offboard_control_mode.ignore_attitude ||
!offboard_control_mode.ignore_position ||
!offboard_control_mode.ignore_velocity ||
!offboard_control_mode.ignore_acceleration_force;


control_mode.flag_control_attitude_enabled = !offboard_control_mode.ignore_attitude ||
!offboard_control_mode.ignore_position ||
!offboard_control_mode.ignore_velocity ||
!offboard_control_mode.ignore_acceleration_force;


control_mode.flag_control_rattitude_enabled = false;


control_mode.flag_control_acceleration_enabled = !offboard_control_mode.ignore_acceleration_force &&
 !status.in_transition_mode;


control_mode.flag_control_velocity_enabled = (!offboard_control_mode.ignore_velocity ||
!offboard_control_mode.ignore_position) && !status.in_transition_mode &&
!control_mode.flag_control_acceleration_enabled;


control_mode.flag_control_climb_rate_enabled = (!offboard_control_mode.ignore_velocity ||
!offboard_control_mode.ignore_position) && !control_mode.flag_control_acceleration_enabled;


control_mode.flag_control_position_enabled = !offboard_control_mode.ignore_position && !status.in_transition_mode &&
 !control_mode.flag_control_acceleration_enabled;


control_mode.flag_control_altitude_enabled = (!offboard_control_mode.ignore_velocity ||
!offboard_control_mode.ignore_position) && !control_mode.flag_control_acceleration_enabled;


break;
```
可以看出，如果开启了加速度控制`ignore_acceleration_force = false`，那么位置控制、速度控制等将不可用！

# 使用offboard模式在仿真器中飞行
## 1.position_setpoint_triplet 主题
打开position_setpoint_triplet 主题的定义我们可以看到如下信息
```cpp
struct position_setpoint_triplet_s {
	uint64_t timestamp; // required for logger
	uint8_t nav_state;
	uint8_t _padding0[7]; // required for logger
	struct position_setpoint_s previous;
	struct position_setpoint_s current;
	struct position_setpoint_s next;
};
```
可以看到`position_setpoint_triplet`是三个`position_setpoint`的组合形式。以下是`position_setpoint`结构体定义：
```cpp
struct position_setpoint_s{
	uint8 SETPOINT_TYPE_POSITION=0 # position setpoint
	uint8 SETPOINT_TYPE_VELOCITY=1 # velocity setpoint
	uint8 SETPOINT_TYPE_LOITER=2 # loiter setpoint
	uint8 SETPOINT_TYPE_TAKEOFF=3 # takeoff setpoint
	uint8 SETPOINT_TYPE_LAND=4 # land setpoint, altitude must be ignored, descend until landing
	uint8 SETPOINT_TYPE_IDLE=5 # do nothing, switch off motors or keep at idle speed (MC)
	uint8 SETPOINT_TYPE_OFFBOARD=6 # setpoint in NED frame (x, y, z, vx, vy, vz) set by offboard
	uint8 SETPOINT_TYPE_FOLLOW_TARGET=7  # setpoint in NED frame (x, y, z, vx, vy, vz) set by follow target
	
	bool valid # true if setpoint is valid
	uint8 type # setpoint type to adjust behavior of position controller
	float32 x # local position setpoint in m in NED
	float32 y # local position setpoint in m in NED
	float32 z # local position setpoint in m in NED
	bool position_valid # true if local position setpoint valid
	float32 vx # local velocity setpoint in m/s in NED
	float32 vy # local velocity setpoint in m/s in NED
	float32 vz # local velocity setpoint in m/s in NED
	bool velocity_valid # true if local velocity setpoint valid
	bool alt_valid # do not set for 3D position control. Set to true if you want z-position control while doing vx,vy velocity control.
	float64 lat # latitude, in deg
	float64 lon # longitude, in deg
	float32 alt # altitude AMSL, in m
	float32 yaw # yaw (only for multirotors), in rad [-PI..PI), NaN = hold current yaw
	bool yaw_valid # true if yaw setpoint valid
	bool disable_mc_yaw_control # control yaw for mc (used for vtol weather-vane mode)
	float32 yawspeed # yawspeed (only for multirotors, in rad/s)
	bool yawspeed_valid # true if yawspeed setpoint valid
	float32 loiter_radius # loiter radius (only for fixed wing), in m
	int8 loiter_direction # loiter direction: 1 = CW, -1 = CCW
	float32 pitch_min # minimal pitch angle for fixed wing takeoff waypoints
	float32 a_x # acceleration x setpoint
	float32 a_y # acceleration y setpoint
	float32 a_z # acceleration z setpoint
	bool acceleration_valid # true if acceleration setpoint is valid/should be used
	bool acceleration_is_force # interprete acceleration as force
	float32 acceptance_radius   # navigation acceptance_radius if we're doing waypoint navigation
	float32 cruising_speed # the generally desired cruising speed (not a hard constraint)
	float32 cruising_throttle # the generally desired cruising throttle (not a hard constraint)
}

```

在进入了`offboard`模式以后，我们只需要发布`position_setpoint_triplet`主题就可以命令飞机飞行了。


##2.位置控制模块是怎样来使用position_setpoint_triplet的？

我们首先开看位置控制模块使用`offboard`飞行的代码：
```cpp
void
MulticopterPositionControl::control_offboard(float dt)
{
	if (_pos_sp_triplet.current.valid) {

		if (_control_mode.flag_control_position_enabled && _pos_sp_triplet.current.position_valid) {
			/* control position */
			_pos_sp(0) = _pos_sp_triplet.current.x;
			_pos_sp(1) = _pos_sp_triplet.current.y;
			_run_pos_control = true;

			_hold_offboard_xy = false;

		} else if (_control_mode.flag_control_velocity_enabled && _pos_sp_triplet.current.velocity_valid) {
			/* control velocity */

			/* reset position setpoint to current position if needed */
			reset_pos_sp();

			if (fabsf(_pos_sp_triplet.current.vx) <= FLT_EPSILON &&
			    fabsf(_pos_sp_triplet.current.vy) <= FLT_EPSILON &&
			    _local_pos.xy_valid) {

				if (!_hold_offboard_xy) {
					_pos_sp(0) = _pos(0);
					_pos_sp(1) = _pos(1);
					_hold_offboard_xy = true;
				}

				_run_pos_control = true;

			} else {

				if (_pos_sp_triplet.current.velocity_frame == position_setpoint_s::VELOCITY_FRAME_LOCAL_NED) {
					/* set position setpoint move rate */
					_vel_sp(0) = _pos_sp_triplet.current.vx;
					_vel_sp(1) = _pos_sp_triplet.current.vy;

				} else if (_pos_sp_triplet.current.velocity_frame == position_setpoint_s::VELOCITY_FRAME_BODY_NED) {
					// Transform velocity command from body frame to NED frame
					_vel_sp(0) = cosf(_yaw) * _pos_sp_triplet.current.vx - sinf(_yaw) * _pos_sp_triplet.current.vy;
					_vel_sp(1) = sinf(_yaw) * _pos_sp_triplet.current.vx + cosf(_yaw) * _pos_sp_triplet.current.vy;

				} else {
					PX4_WARN("Unknown velocity offboard coordinate frame");
				}

				_run_pos_control = false;

				_hold_offboard_xy = false;
			}

		}

		if (_control_mode.flag_control_altitude_enabled && _pos_sp_triplet.current.alt_valid) {
			/* control altitude as it is enabled */
			_pos_sp(2) = _pos_sp_triplet.current.z;
			_run_alt_control = true;

			_hold_offboard_z = false;

		} else if (_control_mode.flag_control_climb_rate_enabled && _pos_sp_triplet.current.velocity_valid) {

			/* reset alt setpoint to current altitude if needed */
			reset_alt_sp();

			if (fabsf(_pos_sp_triplet.current.vz) <= FLT_EPSILON &&
			    _local_pos.z_valid) {

				if (!_hold_offboard_z) {
					_pos_sp(2) = _pos(2);
					_hold_offboard_z = true;
				}

				_run_alt_control = true;

			} else {
				/* set position setpoint move rate */
				_vel_sp(2) = _pos_sp_triplet.current.vz;
				_run_alt_control = false;

				_hold_offboard_z = false;
			}
		}

		if (_pos_sp_triplet.current.yaw_valid) {
			_att_sp.yaw_body = _pos_sp_triplet.current.yaw;

		} else if (_pos_sp_triplet.current.yawspeed_valid) {
			_att_sp.yaw_body = _att_sp.yaw_body + _pos_sp_triplet.current.yawspeed * dt;
		}

	} else {
		_hold_offboard_xy = false;
		_hold_offboard_z = false;
		reset_pos_sp();
		reset_alt_sp();
	}
}
```


从`if (_pos_sp_triplet.current.valid)`可以看出，`offboard模式`只使用了三组合中的`current`，所以我们设置`pre`，`next`是没有任何作用的，那么我们现在只需要关心`current setpoint`。

从`if (_control_mode.flag_control_position_enabled && _pos_sp_triplet.current.position_valid)`可以看出，如果我们开启了位置控制，并且`current setpoint`中的`position_valid为true`就会进行位置控制，如果两个条件中有一个不为真，才会去判断是否启动了速度控制，这里我们只分析位置控制。

那么`_control_mode.flag_control_position_enabled`的值是怎么来的呢，常见上文的代码：
control_mode.flag_control_position_enabled = !offboard_control_mode.ignore_position && !status.in_transition_mode && !control_mode.flag_control_acceleration_enabled;
如果我们发布的offboard_control_mode topic没有忽略位置控制 且 没有在垂直起降飞行器的变形阶段 且 没有开启加速度控制模式，那么位置控制就为true，这就是上文所描述的，为什么要关闭加速度控制模式。
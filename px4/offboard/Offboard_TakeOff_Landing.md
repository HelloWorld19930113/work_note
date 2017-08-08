# PX4 Offboard 

## Offboard automatic TakeOff / Landing 自主起飞和降落

Hello,

Using this portion of Code https://github.com/mavlink/c_uart_interface_example/50 and HITL quadcopter (jmavsim) on the pixhawk, I've managed to have a raspberry pi (without ROS) communicating with the autopilot.

I wanted to send long command with MAV_CMD_LAND_LOCAL and MAV_CMD_TAKEOFF_LOCAL instructions.
我想要发送 `MAV_CMD_LAND_LOCAL` 和 `MAV_CMD_TAKEOFF_LOCAL`命令来控制飞行器的起飞和降落。

程序开始时，在`DISARMED`状态下以`4Hz`的频率发送了一个空的`SET_POSITION_TARGET_LOCAL_NED`。

This allow the Offboard mode to be enabled (as seen in QGC) then I arm it by MAV_CMD_COMPONENT_ARM_DISARM (still OK)

Then I stop sending SET_POSITION_TARGET_LOCAL_NED and start sending MAV_CMD_TAKEOFF_LOCAL command at 4Hz but the Autopilot leave offboard mode.

My intend was to send the MAV_CMD_TAKEOFF_LOCAL command at 4Hz until the status reported by EXTENDED_SYS_STATE indicates MAV_LANDED_STATE_IN_AIR. Then send SET_POSITION_TARGET_LOCAL_NED at 4Hz (like in the example) and finaly MAV_CMD_LAND_LOCAL at 4Hz until EXTENDED_SYS_STATE report MAV_LANDED_STATE_ON_GROUND to finally disarm the drone

If i don't send SET_POSITION_TARGET_LOCAL_NED (during takeoff phase, and I suspect it will be the same for landing), the px4 leave the offboard mode. 
What are valid command for offboard mode ? Is there some other commands for autmatic takeoff/landing in offboard mode ?

Takeoff is not so much of a trouble as I can use NED coordonate the ask for an altitude to be reached, the more troublesome is landing as a setpoint with 'down' value set to 0 let my UAV hover at 10cm from the ground (observed once with a F450).

Thank you for your help and advices

=================================================================================
Joseph_Garret_SullivSep '16
Hi there,

I believe in order for offboard control mode to be enabled, offboard_control_mode ORB messages need to be continually published.

These messages are published by the mavlink receiver whenever the pixhawk receives either SET_POSITION_TARGET_LOCAL_NED messages, SET_ATTITUDE_TARGET, or SET_ACTUATOR_CONTROL_TARGET messages.

In other words, unless your companion computer regularly sets a target position, attitude, or motor input, then the vehicle will exit offboard control mode.

If you need to stay in offboard control mode, try publishing attitude targets, where you can simply command the attitude to some vector, such as [0,0,0]

===============================================================================
Thank you for your help.

I was afrad this was the reason. You're giving me an Idea, I can keep sending SET_POSITION_TARGET_LOCAL_NED during takeoff/land phases with a bitmask telling to consider none of the information inside the message (or juste Yaw angle) and check if sending MAV_CMD_TAKEOFF_LOCAL at same time allows me to takeoff

I'm gonna try this and report back.
========================================================================

============================================================================
完美解决：
 不使用 `MAV_CMD_LAND_LOCAL` 和 `MAV_CMD_TAKEOFF_LOCAL`命令来控制飞行器的起飞和降落。在 https://github.com/mavlink/mavlink/pull/670 页面上, SET_POSITION_TARGET_LOCAL_NED 消息有几个可用`bitmask控制`的隐藏特征。

在 Px4/Firmware 中有4个文档中未曾提到过的变量 :

```cpp
// @file PX4/Firmware/src/modules/mavlink/mavlink_receiver.cpp  #L678-L681

bool is_takeoff_sp = (bool)(set_position_target_local_ned.type_mask & 0x1000);
bool is_land_sp = (bool)(set_position_target_local_ned.type_mask & 0x2000);
bool is_loiter_sp = (bool)(set_position_target_local_ned.type_mask & 0x3000);
bool is_idle_sp = (bool)(set_position_target_local_ned.type_mask & 0x4000);
```
通过 ORing`0x2000`这个`bitmask`， `UAV`将会自动降落在地面上(尽管当前的`z点`仍然是在地面的上方某一位置)。
`takeoff`需要ORing`0x1000`这个`bitmask`。

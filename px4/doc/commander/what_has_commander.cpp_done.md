# `commander.cpp`文件到底干了什么

## 所有的`commander`中关键的变量。   
```cpp
static struct vehicle_status_s status = {};
static struct vehicle_roi_s _roi = {};
static struct battery_status_s battery = {};
static struct actuator_armed_s armed = {};
static struct safety_s safety = {};
static struct vehicle_control_mode_s control_mode = {};
static struct offboard_control_mode_s offboard_control_mode = {};
static struct home_position_s _home = {};
static int32_t _flight_mode_slots[manual_control_setpoint_s::MODE_SLOT_MAX];
static struct commander_state_s internal_state = {};

static struct mission_result_s _mission_result = {};

static uint8_t main_state_before_rtl = commander_state_s::MAIN_STATE_MAX;
static unsigned _last_mission_instance = 0;
static manual_control_setpoint_s sp_man = {};       ///< the current manual control setpoint
static manual_control_setpoint_s _last_sp_man = {}; ///< the manual control setpoint valid at the last mode switch
static uint8_t _last_sp_man_arm_switch = 0;

static struct vtol_vehicle_status_s vtol_status = {};
static struct cpuload_s cpuload = {};


static uint8_t main_state_prev = 0;
static bool warning_action_on = false;
static bool last_overload = false;

static struct status_flags_s status_flags = {};

static uint64_t rc_signal_lost_timestamp;       // Time at which the RC reception was lost

static float avionics_power_rail_voltage;       // voltage of the avionics power rail

static bool arm_without_gps = false;
static bool arm_mission_required = false;

static bool _last_condition_global_position_valid = false;

static struct vehicle_land_detected_s land_detector = {};
```
`status`表示的是`PX4`的当前状态。    
`struct vehicle_status_s`的结构体定义：    
```cpp
struct vehicle_status_s {
    uint64_t timestamp; // required for logger
    uint32_t system_id;
    uint32_t component_id;
    uint32_t onboard_control_sensors_present;
    uint32_t onboard_control_sensors_enabled;
    uint32_t onboard_control_sensors_health;
    uint8_t nav_state;
    uint8_t arming_state;
    uint8_t hil_state;
    bool failsafe;
    uint8_t system_type;
    bool is_rotary_wing;
    bool is_vtol;
    bool vtol_fw_permanent_stab;
    bool in_transition_mode;
    bool in_transition_to_fw;
    bool rc_signal_lost;
    uint8_t rc_input_mode;
    bool data_link_lost;
    uint8_t data_link_lost_counter;
    bool engine_failure;
    bool engine_failure_cmd;
    bool mission_failure;
    uint8_t _padding0[3]; // required for logger

#ifdef __cplusplus
    static const uint8_t ARMING_STATE_INIT = 0;
    static const uint8_t ARMING_STATE_STANDBY = 1;
    static const uint8_t ARMING_STATE_ARMED = 2;
    static const uint8_t ARMING_STATE_ARMED_ERROR = 3;
    static const uint8_t ARMING_STATE_STANDBY_ERROR = 4;
    static const uint8_t ARMING_STATE_REBOOT = 5;
    static const uint8_t ARMING_STATE_IN_AIR_RESTORE = 6;
    static const uint8_t ARMING_STATE_MAX = 7;
    static const uint8_t HIL_STATE_OFF = 0;
    static const uint8_t HIL_STATE_ON = 1;
    static const uint8_t NAVIGATION_STATE_MANUAL = 0;
    static const uint8_t NAVIGATION_STATE_ALTCTL = 1;
    static const uint8_t NAVIGATION_STATE_POSCTL = 2;
    static const uint8_t NAVIGATION_STATE_AUTO_MISSION = 3;
    static const uint8_t NAVIGATION_STATE_AUTO_LOITER = 4;
    static const uint8_t NAVIGATION_STATE_AUTO_RTL = 5;
    static const uint8_t NAVIGATION_STATE_AUTO_RCRECOVER = 6;
    static const uint8_t NAVIGATION_STATE_AUTO_RTGS = 7;
    static const uint8_t NAVIGATION_STATE_AUTO_LANDENGFAIL = 8;
    static const uint8_t NAVIGATION_STATE_AUTO_LANDGPSFAIL = 9;
    static const uint8_t NAVIGATION_STATE_ACRO = 10;
    static const uint8_t NAVIGATION_STATE_UNUSED = 11;
    static const uint8_t NAVIGATION_STATE_DESCEND = 12;
    static const uint8_t NAVIGATION_STATE_TERMINATION = 13;
    static const uint8_t NAVIGATION_STATE_OFFBOARD = 14;
    static const uint8_t NAVIGATION_STATE_STAB = 15;
    static const uint8_t NAVIGATION_STATE_RATTITUDE = 16;
    static const uint8_t NAVIGATION_STATE_AUTO_TAKEOFF = 17;
    static const uint8_t NAVIGATION_STATE_AUTO_LAND = 18;
    static const uint8_t NAVIGATION_STATE_AUTO_FOLLOW_TARGET = 19;
    static const uint8_t NAVIGATION_STATE_MAX = 20;
    static const uint8_t RC_IN_MODE_DEFAULT = 0;
    static const uint8_t RC_IN_MODE_OFF = 1;
    static const uint8_t RC_IN_MODE_GENERATED = 2;

#endif
};
```
## `commander.cpp`的中枢    
`commander_thread_main()`函数是`commander`的核心。我们就从这个函数开始分析`commander`指挥官的职责。这个函数一共有进2000行的代码，因此按照功能性划分后再详细分析。      
`commander_thread_main()`中的任务模块：      
1. 前期都是一些参数的获取，具体的参数在用到的任务模块中再介绍；   
2. 初始化模块(led,buzzer)和注册电源按键状态响应函数(board_register_power_state_notification_cb)。   
3. 有一个低优先级运行的处理非紧急任务的线程`commander_low_prio_thread`；    

## 重要变量初始化    
1. 既不接收`manual`控制命令，也不接收`offboard`控制命令。   
```cpp
status_flags.offboard_control_signal_found_once = false;
status_flags.rc_signal_found_once = false;
```    

## `commander`中发布的几个重要`Topic`      
分别是：`ORB_ID(vehicle_status)`、`ORB_ID(actuator_armed)`、`ORB_ID(vehicle_control_mode)`和`ORB_ID(parameter_update)`...    
```cpp
    /* publish initial state */
    status_pub = orb_advertise(ORB_ID(vehicle_status), &status);

    /* armed topic */
    orb_advert_t armed_pub = orb_advertise(ORB_ID(actuator_armed), &armed);

    /* vehicle control mode topic */
    orb_advert_t control_mode_pub = orb_advertise(ORB_ID(vehicle_control_mode), &control_mode);

    /* home position */
    orb_advert_t home_pub = nullptr;

    /* region of interest */
    orb_advert_t roi_pub = nullptr;

    /* command ack */
    orb_advert_t command_ack_pub = nullptr;
```
```
==============================================
===============  while (!thread_should_exit)   =============
==============================================
```

### 更新参数    
首先说明，在参数设置函数中会调用`param_notify_changes()`函数发布`ORB_ID(parameter_update)`主题。    
这里有一个特殊处理，也就是`param_init_forced`对应的情况。`param_init_forced`被初始化为`true`，也就是说初始化的时候是肯定要对参数进行赋值的，参数的初始化是不会依赖于参数是否更新。    
初始化完毕之后，`param_init_forced = false`，之后的每一次参数重新赋值与否就取决于参数是否更新了。    
最后来看看这里初始化的参数都有些什么。    
```cpp
/* 更新参数 */
    if (!armed.armed) {
        // 飞行器的类型，如四旋翼、八旋翼和固定翼等；
        if (param_get(_param_sys_type, &(status.system_type)) != OK) {
            warnx("failed getting new system type");
        }

        /* 禁用依赖于电子增稳机型的手动控制通道覆盖 ？？？*/
        // 确定飞机是否为旋翼(直升机/垂直升降机也是旋翼)
        if (is_rotary_wing(&status) || (is_vtol(&status) && vtol_status.vtol_in_rw_mode)) {
            status.is_rotary_wing = true;

        } else {
            status.is_rotary_wing = false;
        }

        /* 确定飞机是否为垂直升降机 */
        status.is_vtol = is_vtol(&status);

        /* 获取飞机的系统/组件 ID */
        param_get(_param_system_id, &(status.system_id));
        param_get(_param_component_id, &(status.component_id));

        // 获取断路器参数
        get_circuit_breaker_params();

        // 更新>>>>>>>>>>>>>>>>？？状态标志
        status_changed = true;
    }

    /* Safety 安全参数 */
    param_get(_param_enable_datalink_loss, &datalink_loss_act);  // 设置地面站datalink丢失后的安全保护
    param_get(_param_enable_rc_loss, &rc_loss_act); // 设置遥控器信号丢失后的安全保护，默认操作是RTL返回起飞点
    param_get(_param_datalink_loss_timeout, &datalink_loss_timeout);  // 设置地面站datalink信号丢失的超时时间(默认10s)
    param_get(_param_rc_loss_timeout, &rc_loss_timeout); // 设置遥控器信号丢失的超时时间(默认0.5s)
    param_get(_param_rc_in_off, &rc_in_off);  // 设置遥控器数据关闭状态：默认为遥控器开
    status.rc_input_mode = rc_in_off;             // 设置遥控器输入模式：默认为普通遥控器
    param_get(_param_rc_arm_hyst, &rc_arm_hyst);  // armed/disarm 遥控杆就位的等待时间，默认值1000表示1s.
    param_get(_param_min_stick_change, &min_stick_change);  // 设置可捕捉到的遥控杆最小变化值；
    param_get(_param_rc_override, &rc_override);  // 使能/禁用 自动控制模式下的 rc_override

    // percentage (* 0.01) needs to be doubled because RC total interval is 2, not 1
    min_stick_change *= 0.02f;
    rc_arm_hyst *= COMMANDER_MONITORING_LOOPSPERMSEC;  // 考虑线程运行频率对armed/disarm 遥控杆就位时间的影响，保证用户这边是确确实实的1s
    param_get(_param_datalink_regain_timeout, &datalink_regain_timeout);  // 设置地面站datalink恢复的检测时间
    param_get(_param_ef_throttle_thres, &ef_throttle_thres);     // 设置引擎安全保护的油门阈值
    param_get(_param_ef_current2throttle_thres, &ef_current2throttle_thres);   // 设置引擎安全保护的当前油门给油量的比例阈值
    param_get(_param_ef_time_thres, &ef_time_thres);                 // 设置引擎安全保护的等待时间
    param_get(_param_geofence_action, &geofence_action);     // 设置地理围栏动作，默认是警告。
    // 使能/禁用飞机降落后自动disarm，默认是禁用的。如果设置为正值，则经过设置的时间后自动disarm。
    param_get(_param_disarm_land, &disarm_when_landed);   

    // 首次更新参数时一定要保证自动disarm的时间间隔已经设置好(虽然降落后自动disarm是禁用的，
    // 但是也应该设置，以防万一在某个时刻启用了降落后自动disarm)。之后这个时间间隔将会在 
    // main state machine 状态机中根据 arming 的状态进行设置。
    if (param_init_forced) {
        auto_disarm_hysteresis.set_hysteresis_time_from(false,
                            (hrt_abstime)disarm_when_landed * 1000000);
    }

    param_get(_param_low_bat_act, &low_bat_action);     // 设置低电量操作，默认为警告；
    // 设置offboard控制信号丢失后等待offboard_loss_act的时间，默认为0，即立即响应
    param_get(_param_offboard_loss_timeout, &offboard_loss_timeout);  
    // 设置offboard控制丢失后的操作，默认为原地降落
    param_get(_param_offboard_loss_act, &offboard_loss_act);  
    // 设置有RC信号下的offboard控制丢失后的操作，默认为position位置控制
    param_get(_param_offboard_loss_rc_act, &offboard_loss_rc_act);  
    // 设置 arm_switch 是一个开关/按钮。默认为开关。但我们一般使用的是button，长按可以armed/disarm
    param_get(_param_arm_switch_is_button, &arm_switch_is_button);

    param_get(_param_arm_without_gps, &arm_without_gps_param);  // 设置GPS未定位也允许armed，默认是允许的
    arm_without_gps = (arm_without_gps_param == 1);

Require valid mission to arm

Comment: The default allows to arm the vehicle without a valid mission.


    param_get(_param_arm_mission_required, &arm_mission_required_param);  // 有效任务下的armed，默认是允许无任务armed。
    arm_mission_required = (arm_mission_required_param == 1);

    /* Autostart id */
    param_get(_param_autostart_id, &autostart_id); // 自动启动脚本索引——SYS_AUTOSTART

    /* EPH / EPV */
    param_get(_param_eph, &eph_threshold);   // Home点水平设置阈值，位置估计精度小于此值时就会设置Home点。
    param_get(_param_epv, &epv_threshold);   // Home点垂直设置阈值，同上。

    /* flight mode slots */
    param_get(_param_fmode_1, &_flight_mode_slots[0]);  // 飞行模式对应的slot槽。
    param_get(_param_fmode_2, &_flight_mode_slots[1]);
    param_get(_param_fmode_3, &_flight_mode_slots[2]);
    param_get(_param_fmode_4, &_flight_mode_slots[3]);
    param_get(_param_fmode_5, &_flight_mode_slots[4]);
    param_get(_param_fmode_6, &_flight_mode_slots[5]);

    /* pre-flight EKF checks */
    param_get(_param_max_ekf_pos_ratio, &max_ekf_pos_ratio);  // 允许armed的最大EKF的位置更新率。
    param_get(_param_max_ekf_vel_ratio, &max_ekf_vel_ratio);    // 允许armed的最大EKF的速度更新率。
    param_get(_param_max_ekf_hgt_ratio, &max_ekf_hgt_ratio);   // 允许armed的最大EKF的高度更新率。
    param_get(_param_max_ekf_yaw_ratio, &max_ekf_yaw_ratio);  // 允许armed的最大EKF的偏航更新率。
    param_get(_param_max_ekf_dvel_bias, &max_ekf_dvel_bias);   // 允许armed的最大EKF的加速度计速度差偏移。
    param_get(_param_max_ekf_dang_bias, &max_ekf_dang_bias); // 允许armed的最大EKF的陀螺仪角度差偏移。

    /* pre-flight IMU consistency checks */
    Maximum accelerometer inconsistency between IMU units that will allow arming
    param_get(_param_max_imu_acc_diff, &max_imu_acc_diff);   // 允许armed的IMU的加速度计的最大差值
    param_get(_param_max_imu_gyr_diff, &max_imu_gyr_diff);    // 允许armed的IMU的陀螺仪的最大差值

    /* failsafe response to loss of navigation accuracy */
    param_get(_param_posctl_nav_loss_act, &posctl_nav_loss_act);  //导航精度不满足时的位置控制动作

    param_init_forced = false;
```
这一部分主要是对`commander`中涉及到的参数初始化。    
具体参数的描述已经在代码中给出了注释；可以看到，在这里提到的大部分参数在实际的参数调整中很关键。因此，很有必要熟悉这些关键的参数，这样才能在出现意外情况时快速定位并解决问题。     

###  `ORB_ID(manual_control_setpoint)`主题      
这个主题负责的是手动控制点。对应的原始数据是手动控制模式下遥控器的通道值。与遥控器相关，通过对应的通道映射关系(在遥控器校准阶段确定)将遥控器的通道值转换为最底层的`roll/pitch/yaw`通道值，然后再使用这个通道值来控制飞机的姿态。  

###  `ORB_ID(offboard_control_mode)`主题   

###  `mission_state`初始化    
  在`data manager`中读取任务数据，如果有就使用线程的，否则写入初始化值。然后`publish`。    

### RC——遥控器     
`commander`对遥控器的处理比较巧妙。在遥控器没有`Calibration`(校准)之前，`rc_input_blocked`标志位一直是`false`。遥控器在校准的时候并不会有`PWM`输出。     

```cpp
status_flags.rc_input_blocked = false;
status.rc_input_mode = vehicle_status_s::RC_IN_MODE_DEFAULT;
```

### internal_state   

### 飞行模式——`main_state `    
这里一定要清楚飞行模式和导航模式是不同的。   

### 导航模式——`nav_state`    
 
### `armed`状态    

### `failsafe`模式    

### HIL——硬件在环     
硬件在环是使用仿真飞行器来运行`PX4`的，可以用来测试算法和程序控制逻辑。    
```cpp
if (startup_in_hil) {
    status.hil_state = vehicle_status_s::HIL_STATE_ON;
} else {
    status.hil_state = vehicle_status_s::HIL_STATE_OFF;
}
```

### 气压计——`baromete`    
```cpp
/* 假设启动时气压计是不起作用的 */
status_flags.barometer_failure = true;
status_flags.ever_had_barometer_data = false;
```

### 以一定频率接收的信号    
包括遥控器通道信号、`offboard`控制信号和地面站信号。   
```cpp
status.rc_signal_lost = true;
status_flags.offboard_control_signal_lost = true;
status.data_link_lost = true;
status_flags.offboard_control_loss_timeout = false;
```

### misc   
```cpp
status.timestamp = hrt_absolute_time();  // 用来检测数据是否更新

status_flags.condition_system_prearm_error_reported = false;
status_flags.condition_system_hotplug_timeout = false;

status_flags.condition_power_input_valid = true;
avionics_power_rail_voltage = -1.0f;
status_flags.usb_connected = false;

/* Set position and velocity validty to false */
status_flags.condition_global_position_valid = false;
status_flags.condition_global_velocity_valid = false;
status_flags.condition_local_position_valid = false;
status_flags.condition_local_velocity_valid = false;
status_flags.condition_local_altitude_valid = false;
```

## 主线程     
```cpp
int commander_thread_main(int argc, char *argv[])
{
    /* not yet initialized */
    commander_initialized = false;

    bool sensor_fail_tune_played = false;
    bool arm_tune_played = false;
    bool was_landed = true;
    bool was_falling = false;
    bool was_armed = false;

    bool startup_in_hil = false;

    // XXX for now just set sensors as initialized
    status_flags.condition_system_sensors_initialized = true;

#ifdef __PX4_NUTTX
    /* NuttX indicates 3 arguments when only 2 are present */
    argc -= 1;
    argv += 1;
#endif

    if (argc > 2) {
        if (!strcmp(argv[2],"-hil")) {
            startup_in_hil = true;
        } else {
            PX4_ERR("Argument %s not supported, abort.", argv[2]);
            thread_should_exit = true;
        }
    }

    mavlink_log_critical(&mavlink_log_pub, "[zack] Commander is running....");
    /* set parameters */
    param_t _param_sys_type = param_find("MAV_TYPE");
    param_t _param_system_id = param_find("MAV_SYS_ID");
    param_t _param_component_id = param_find("MAV_COMP_ID");
    param_t _param_enable_datalink_loss = param_find("NAV_DLL_ACT");
    param_t _param_offboard_loss_act = param_find("COM_OBL_ACT");
    param_t _param_offboard_loss_rc_act = param_find("COM_OBL_RC_ACT");
    param_t _param_enable_rc_loss = param_find("NAV_RCL_ACT");
    param_t _param_datalink_loss_timeout = param_find("COM_DL_LOSS_T");
    param_t _param_rc_loss_timeout = param_find("COM_RC_LOSS_T");
    param_t _param_datalink_regain_timeout = param_find("COM_DL_REG_T");
    param_t _param_ef_throttle_thres = param_find("COM_EF_THROT");
    param_t _param_ef_current2throttle_thres = param_find("COM_EF_C2T");
    param_t _param_ef_time_thres = param_find("COM_EF_TIME");
    param_t _param_autostart_id = param_find("SYS_AUTOSTART");
    param_t _param_rc_in_off = param_find("COM_RC_IN_MODE");
    param_t _param_rc_arm_hyst = param_find("COM_RC_ARM_HYST");
    param_t _param_min_stick_change = param_find("COM_RC_STICK_OV");
    param_t _param_eph = param_find("COM_HOME_H_T");
    param_t _param_epv = param_find("COM_HOME_V_T");
    param_t _param_geofence_action = param_find("GF_ACTION");
    param_t _param_disarm_land = param_find("COM_DISARM_LAND");
    param_t _param_low_bat_act = param_find("COM_LOW_BAT_ACT");
    param_t _param_offboard_loss_timeout = param_find("COM_OF_LOSS_T");
    param_t _param_arm_without_gps = param_find("COM_ARM_WO_GPS");
    param_t _param_arm_switch_is_button = param_find("COM_ARM_SWISBTN");
    param_t _param_rc_override = param_find("COM_RC_OVERRIDE");
    param_t _param_arm_mission_required = param_find("COM_ARM_MIS_REQ");

    param_t _param_fmode_1 = param_find("COM_FLTMODE1");
    param_t _param_fmode_2 = param_find("COM_FLTMODE2");
    param_t _param_fmode_3 = param_find("COM_FLTMODE3");
    param_t _param_fmode_4 = param_find("COM_FLTMODE4");
    param_t _param_fmode_5 = param_find("COM_FLTMODE5");
    param_t _param_fmode_6 = param_find("COM_FLTMODE6");

    /* pre-flight EKF checks */
    param_t _param_max_ekf_pos_ratio = param_find("COM_ARM_EKF_POS");
    param_t _param_max_ekf_vel_ratio = param_find("COM_ARM_EKF_VEL");
    param_t _param_max_ekf_hgt_ratio = param_find("COM_ARM_EKF_HGT");
    param_t _param_max_ekf_yaw_ratio = param_find("COM_ARM_EKF_YAW");
    param_t _param_max_ekf_dvel_bias = param_find("COM_ARM_EKF_AB");
    param_t _param_max_ekf_dang_bias = param_find("COM_ARM_EKF_GB");

    /* pre-flight IMU consistency checks */
    param_t _param_max_imu_acc_diff = param_find("COM_ARM_IMU_ACC");
    param_t _param_max_imu_gyr_diff = param_find("COM_ARM_IMU_GYR");

    /* failsafe response to loss of navigation accuracy */
    param_t _param_posctl_nav_loss_act = param_find("COM_POSCTL_NAVL");

    // These are too verbose, but we will retain them a little longer
    // until we are sure we really don't need them.

    // const char *main_states_str[commander_state_s::MAIN_STATE_MAX];
    // main_states_str[commander_state_s::MAIN_STATE_MANUAL]            = "MANUAL";
    // main_states_str[commander_state_s::MAIN_STATE_ALTCTL]            = "ALTCTL";
    // main_states_str[commander_state_s::MAIN_STATE_POSCTL]            = "POSCTL";
    // main_states_str[commander_state_s::MAIN_STATE_AUTO_MISSION]      = "AUTO_MISSION";
    // main_states_str[commander_state_s::MAIN_STATE_AUTO_LOITER]           = "AUTO_LOITER";
    // main_states_str[commander_state_s::MAIN_STATE_AUTO_RTL]          = "AUTO_RTL";
    // main_states_str[commander_state_s::MAIN_STATE_ACRO]          = "ACRO";
    // main_states_str[commander_state_s::MAIN_STATE_STAB]          = "STAB";
    // main_states_str[commander_state_s::MAIN_STATE_OFFBOARD]          = "OFFBOARD";

    // const char *nav_states_str[vehicle_status_s::NAVIGATION_STATE_MAX];
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_MANUAL]            = "MANUAL";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_STAB]              = "STAB";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_RATTITUDE]     = "RATTITUDE";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_ALTCTL]            = "ALTCTL";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_POSCTL]            = "POSCTL";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION]      = "AUTO_MISSION";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER]       = "AUTO_LOITER";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_AUTO_RTL]      = "AUTO_RTL";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF]      = "AUTO_TAKEOFF";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_AUTO_RCRECOVER]        = "AUTO_RCRECOVER";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_AUTO_RTGS]     = "AUTO_RTGS";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_AUTO_LANDENGFAIL]  = "AUTO_LANDENGFAIL";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_AUTO_LANDGPSFAIL]  = "AUTO_LANDGPSFAIL";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_ACRO]          = "ACRO";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_AUTO_LAND]         = "LAND";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_DESCEND]       = "DESCEND";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_TERMINATION]       = "TERMINATION";
    // nav_states_str[vehicle_status_s::NAVIGATION_STATE_OFFBOARD]      = "OFFBOARD";

    /* pthread for slow low prio thread */
    pthread_t commander_low_prio_thread;

    /* initialize */
    if (led_init() != OK) {
        PX4_WARN("LED init failed");
    }

    if (buzzer_init() != OK) {
        PX4_WARN("Buzzer init failed");
    }

    if (board_register_power_state_notification_cb(power_button_state_notification_cb) != 0) {
        PX4_ERR("Failed to register power notification callback");
    }

    /* vehicle status topic */
    memset(&status, 0, sizeof(status));

    // We want to accept RC inputs as default
    status_flags.rc_input_blocked = false;
    status.rc_input_mode = vehicle_status_s::RC_IN_MODE_DEFAULT;
    internal_state.main_state = commander_state_s::MAIN_STATE_MANUAL;
    internal_state.timestamp = hrt_absolute_time();
    main_state_prev = commander_state_s::MAIN_STATE_MAX;
    status.nav_state = vehicle_status_s::NAVIGATION_STATE_MANUAL;
    status.arming_state = vehicle_status_s::ARMING_STATE_INIT;

    if (startup_in_hil) {
        status.hil_state = vehicle_status_s::HIL_STATE_ON;
    } else {
        status.hil_state = vehicle_status_s::HIL_STATE_OFF;
    }
    status.failsafe = false;

    /* neither manual nor offboard control commands have been received */
    status_flags.offboard_control_signal_found_once = false;
    status_flags.rc_signal_found_once = false;

    /* assume we don't have a valid baro on startup */
    status_flags.barometer_failure = true;
    status_flags.ever_had_barometer_data = false;

    /* mark all signals lost as long as they haven't been found */
    status.rc_signal_lost = true;
    status_flags.offboard_control_signal_lost = true;
    status.data_link_lost = true;
    status_flags.offboard_control_loss_timeout = false;

    status_flags.condition_system_prearm_error_reported = false;
    status_flags.condition_system_hotplug_timeout = false;

    status.timestamp = hrt_absolute_time();

    status_flags.condition_power_input_valid = true;
    avionics_power_rail_voltage = -1.0f;
    status_flags.usb_connected = false;

    // CIRCUIT BREAKERS
    status_flags.circuit_breaker_engaged_power_check = false;
    status_flags.circuit_breaker_engaged_airspd_check = false;
    status_flags.circuit_breaker_engaged_enginefailure_check = false;
    status_flags.circuit_breaker_engaged_gpsfailure_check = false;
    get_circuit_breaker_params();

    /* Set position and velocity validty to false */
    status_flags.condition_global_position_valid = false;
    status_flags.condition_global_velocity_valid = false;
    status_flags.condition_local_position_valid = false;
    status_flags.condition_local_velocity_valid = false;
    status_flags.condition_local_altitude_valid = false;

    // initialize gps failure to false if circuit breaker enabled
    if (status_flags.circuit_breaker_engaged_gpsfailure_check) {
        status_flags.gps_failure = false;
    } else {
        status_flags.gps_failure = true;
    }

    /* publish initial state */
    status_pub = orb_advertise(ORB_ID(vehicle_status), &status);

    if (status_pub == nullptr) {
        warnx("ERROR: orb_advertise for topic vehicle_status failed (uorb app running?).\n");
        warnx("exiting.");
        px4_task_exit(PX4_ERROR);
    }

    /* Initialize armed with all false */
    memset(&armed, 0, sizeof(armed));
    /* armed topic */
    orb_advert_t armed_pub = orb_advertise(ORB_ID(actuator_armed), &armed);

    /* vehicle control mode topic */
    memset(&control_mode, 0, sizeof(control_mode));
    orb_advert_t control_mode_pub = orb_advertise(ORB_ID(vehicle_control_mode), &control_mode);

    /* home position */
    orb_advert_t home_pub = nullptr;
    memset(&_home, 0, sizeof(_home));

    /* region of interest */
    orb_advert_t roi_pub = nullptr;
    memset(&_roi, 0, sizeof(_roi));

    /* command ack */
    orb_advert_t command_ack_pub = nullptr;
    struct vehicle_command_ack_s command_ack;
    memset(&command_ack, 0, sizeof(command_ack));

    /* init mission state, do it here to allow navigator to use stored mission even if mavlink failed to start */
    orb_advert_t mission_pub = nullptr;
    mission_s mission;

    orb_advert_t commander_state_pub = nullptr;

    orb_advert_t vehicle_status_flags_pub = nullptr;

    if (dm_read(DM_KEY_MISSION_STATE, 0, &mission, sizeof(mission_s)) == sizeof(mission_s)) {
        if (mission.dataman_id >= 0 && mission.dataman_id <= 1) {
            if (mission.count > 0) {
                mavlink_log_info(&mavlink_log_pub, "[cmd] Mission #%d loaded, %u WPs, curr: %d",
                         mission.dataman_id, mission.count, mission.current_seq);
            }

        } else {
            const char *missionfail = "reading mission state failed";
            warnx("%s", missionfail);
            mavlink_log_critical(&mavlink_log_pub, missionfail);

            /* initialize mission state in dataman */
            mission.dataman_id = 0;
            mission.count = 0;
            mission.current_seq = 0;
            dm_write(DM_KEY_MISSION_STATE, 0, DM_PERSIST_POWER_ON_RESET, &mission, sizeof(mission_s));
        }

        mission_pub = orb_advertise(ORB_ID(offboard_mission), &mission);
        orb_publish(ORB_ID(offboard_mission), mission_pub, &mission);
    }

    int ret;

    /* Start monitoring loop */
    unsigned counter = 0;
    unsigned stick_off_counter = 0;
    unsigned stick_on_counter = 0;

    bool low_battery_voltage_actions_done = false;
    bool critical_battery_voltage_actions_done = false;
    bool emergency_battery_voltage_actions_done = false;

    bool status_changed = true;
    bool param_init_forced = true;

    bool updated = false;

    /* Subscribe to safety topic */
    int safety_sub = orb_subscribe(ORB_ID(safety));
    memset(&safety, 0, sizeof(safety));
    safety.safety_switch_available = false;
    safety.safety_off = false;

    /* Subscribe to mission result topic */
    int mission_result_sub = orb_subscribe(ORB_ID(mission_result));

    /* Subscribe to geofence result topic */
    int geofence_result_sub = orb_subscribe(ORB_ID(geofence_result));
    struct geofence_result_s geofence_result;
    memset(&geofence_result, 0, sizeof(geofence_result));

    /* Subscribe to manual control data */
    int sp_man_sub = orb_subscribe(ORB_ID(manual_control_setpoint));
    memset(&sp_man, 0, sizeof(sp_man));

    /* Subscribe to offboard control data */
    int offboard_control_mode_sub = orb_subscribe(ORB_ID(offboard_control_mode));
    memset(&offboard_control_mode, 0, sizeof(offboard_control_mode));

    /* Subscribe to telemetry status topics */
    int telemetry_subs[ORB_MULTI_MAX_INSTANCES];
    uint64_t telemetry_last_heartbeat[ORB_MULTI_MAX_INSTANCES];
    uint64_t telemetry_last_dl_loss[ORB_MULTI_MAX_INSTANCES];
    bool telemetry_preflight_checks_reported[ORB_MULTI_MAX_INSTANCES];
    bool telemetry_lost[ORB_MULTI_MAX_INSTANCES];

    for (int i = 0; i < ORB_MULTI_MAX_INSTANCES; i++) {
        telemetry_subs[i] = -1;
        telemetry_last_heartbeat[i] = 0;
        telemetry_last_dl_loss[i] = 0;
        telemetry_lost[i] = true;
        telemetry_preflight_checks_reported[i] = false;
    }

    /* Subscribe to global position */
    int global_position_sub = orb_subscribe(ORB_ID(vehicle_global_position));
    struct vehicle_global_position_s global_position;
    memset(&global_position, 0, sizeof(global_position));
    /* Init EPH and EPV */
    global_position.eph = 1000.0f;
    global_position.epv = 1000.0f;

    /* Subscribe to local position data */
    int local_position_sub = orb_subscribe(ORB_ID(vehicle_local_position));
    struct vehicle_local_position_s local_position = {};

    /* Subscribe to attitude data */
    int attitude_sub = orb_subscribe(ORB_ID(vehicle_attitude));
    struct vehicle_attitude_s attitude = {};

    /* Subscribe to land detector */
    int land_detector_sub = orb_subscribe(ORB_ID(vehicle_land_detected));
    land_detector.landed = true;

    /*
     * The home position is set based on GPS only, to prevent a dependency between
     * position estimator and commander. RAW GPS is more than good enough for a
     * non-flying vehicle.
     */

    /* Subscribe to GPS topic */
    int gps_sub = orb_subscribe(ORB_ID(vehicle_gps_position));
    struct vehicle_gps_position_s gps_position;
    memset(&gps_position, 0, sizeof(gps_position));
    gps_position.eph = FLT_MAX;
    gps_position.epv = FLT_MAX;

    /* Subscribe to sensor topic */
    int sensor_sub = orb_subscribe(ORB_ID(sensor_combined));
    struct sensor_combined_s sensors;
    memset(&sensors, 0, sizeof(sensors));

    /* Subscribe to differential pressure topic */
    int diff_pres_sub = orb_subscribe(ORB_ID(differential_pressure));
    struct differential_pressure_s diff_pres;
    memset(&diff_pres, 0, sizeof(diff_pres));

    /* Subscribe to command topic */
    int cmd_sub = orb_subscribe(ORB_ID(vehicle_command));
    struct vehicle_command_s cmd;
    memset(&cmd, 0, sizeof(cmd));

    /* Subscribe to parameters changed topic */
    int param_changed_sub = orb_subscribe(ORB_ID(parameter_update));

    /* Subscribe to battery topic */
    int battery_sub = orb_subscribe(ORB_ID(battery_status));
    memset(&battery, 0, sizeof(battery));

    /* Subscribe to subsystem info topic */
    int subsys_sub = orb_subscribe(ORB_ID(subsystem_info));
    struct subsystem_info_s info;
    memset(&info, 0, sizeof(info));

    /* Subscribe to position setpoint triplet */
    int pos_sp_triplet_sub = orb_subscribe(ORB_ID(position_setpoint_triplet));
    struct position_setpoint_triplet_s pos_sp_triplet;
    memset(&pos_sp_triplet, 0, sizeof(pos_sp_triplet));

    /* Subscribe to system power */
    int system_power_sub = orb_subscribe(ORB_ID(system_power));
    struct system_power_s system_power;
    memset(&system_power, 0, sizeof(system_power));

    /* Subscribe to actuator controls (outputs) */
    int actuator_controls_sub = orb_subscribe(ORB_ID_VEHICLE_ATTITUDE_CONTROLS);
    struct actuator_controls_s actuator_controls;
    memset(&actuator_controls, 0, sizeof(actuator_controls));

    /* Subscribe to vtol vehicle status topic */
    int vtol_vehicle_status_sub = orb_subscribe(ORB_ID(vtol_vehicle_status));
    //struct vtol_vehicle_status_s vtol_status;
    memset(&vtol_status, 0, sizeof(vtol_status));
    vtol_status.vtol_in_rw_mode = true;     //default for vtol is rotary wing

    int cpuload_sub = orb_subscribe(ORB_ID(cpuload));
    memset(&cpuload, 0, sizeof(cpuload));

    control_status_leds(&status, &armed, true, &battery, &cpuload);

    /* now initialized */
    commander_initialized = true;
    thread_running = true;

    /* update vehicle status to find out vehicle type (required for preflight checks) */
    param_get(_param_sys_type, &(status.system_type)); // get system type
    status.is_rotary_wing = is_rotary_wing(&status) || is_vtol(&status);
    status.is_vtol = is_vtol(&status);

    bool checkAirspeed = false;
    /* Perform airspeed check only if circuit breaker is not
     * engaged and it's not a rotary wing */
    if (!status_flags.circuit_breaker_engaged_airspd_check &&
        (!status.is_rotary_wing || status.is_vtol)) {
        checkAirspeed = true;
    }

    commander_boot_timestamp = hrt_absolute_time();

    // Run preflight check
    int32_t rc_in_off = 0;
    bool hotplug_timeout = hrt_elapsed_time(&commander_boot_timestamp) > HOTPLUG_SENS_TIMEOUT;

    param_get(_param_autostart_id, &autostart_id);
    param_get(_param_rc_in_off, &rc_in_off);

    int32_t arm_switch_is_button = 0;
    param_get(_param_arm_switch_is_button, &arm_switch_is_button);

    int32_t arm_without_gps_param = 0;
    param_get(_param_arm_without_gps, &arm_without_gps_param);
    arm_without_gps = (arm_without_gps_param == 1);

    int32_t arm_mission_required_param = 0;
    param_get(_param_arm_mission_required, &arm_mission_required_param);
    arm_mission_required = (arm_mission_required_param == 1);

    status.rc_input_mode = rc_in_off;
    if (is_hil_setup(autostart_id)) {
        // HIL configuration selected: real sensors will be disabled
        status_flags.condition_system_sensors_initialized = false;
        set_tune_override(TONE_STARTUP_TUNE); //normal boot tune
    } else {
            // sensor diagnostics done continuously, not just at boot so don't warn about any issues just yet
            status_flags.condition_system_sensors_initialized = Commander::preflightCheck(&mavlink_log_pub, true, true, true, true,
                checkAirspeed, (status.rc_input_mode == vehicle_status_s::RC_IN_MODE_DEFAULT), !status_flags.circuit_breaker_engaged_gpsfailure_check,
                /* checkDynamic */ false, is_vtol(&status), /* reportFailures */ false, /* prearm */ false, hrt_elapsed_time(&commander_boot_timestamp));
            set_tune_override(TONE_STARTUP_TUNE); //normal boot tune
    }

    // user adjustable duration required to assert arm/disarm via throttle/rudder stick
    int32_t rc_arm_hyst = 100;
    param_get(_param_rc_arm_hyst, &rc_arm_hyst);
    rc_arm_hyst *= COMMANDER_MONITORING_LOOPSPERMSEC;

    transition_result_t arming_ret;

    int32_t datalink_loss_act = 0;
    int32_t rc_loss_act = 0;
    int32_t datalink_loss_timeout = 10;
    float rc_loss_timeout = 0.5;
    int32_t datalink_regain_timeout = 0;
    float offboard_loss_timeout = 0.0f;
    int32_t offboard_loss_act = 0;
    int32_t offboard_loss_rc_act = 0;
    int32_t posctl_nav_loss_act = 0;

    int32_t geofence_action = 0;

    /* RC override auto modes */
    int32_t rc_override = 0;


    /* Thresholds for engine failure detection */
    int32_t ef_throttle_thres = 1.0f;
    int32_t ef_current2throttle_thres = 0.0f;
    int32_t ef_time_thres = 1000.0f;
    uint64_t timestamp_engine_healthy = 0; /**< absolute time when engine was healty */

    int32_t disarm_when_landed = 0;
    int32_t low_bat_action = 0;

    /* check which state machines for changes, clear "changed" flag */
    bool arming_state_changed = false;
    bool main_state_changed = false;
    bool failsafe_old = false;

    bool have_taken_off_since_arming = false;

    /* initialize low priority thread */
    pthread_attr_t commander_low_prio_attr;
    pthread_attr_init(&commander_low_prio_attr);
    pthread_attr_setstacksize(&commander_low_prio_attr, PX4_STACK_ADJUSTED(3000));

#ifndef __PX4_QURT
    // This is not supported by QURT (yet).
    struct sched_param param;
    (void)pthread_attr_getschedparam(&commander_low_prio_attr, &param);

    /* low priority */
    param.sched_priority = SCHED_PRIORITY_DEFAULT - 50;
    (void)pthread_attr_setschedparam(&commander_low_prio_attr, &param);
#endif

    pthread_create(&commander_low_prio_thread, &commander_low_prio_attr, commander_low_prio_loop, nullptr);
    pthread_attr_destroy(&commander_low_prio_attr);

    while (!thread_should_exit) {

        arming_ret = TRANSITION_NOT_CHANGED;


        /* update parameters */
        orb_check(param_changed_sub, &updated);

        if (updated || param_init_forced) {

            /* parameters changed */
            struct parameter_update_s param_changed;
            orb_copy(ORB_ID(parameter_update), param_changed_sub, &param_changed);

            /* update parameters */
            if (!armed.armed) {
                if (param_get(_param_sys_type, &(status.system_type)) != OK) {
                    warnx("failed getting new system type");
                }

                /* disable manual override for all systems that rely on electronic stabilization */
                if (is_rotary_wing(&status) || (is_vtol(&status) && vtol_status.vtol_in_rw_mode)) {
                    status.is_rotary_wing = true;

                } else {
                    status.is_rotary_wing = false;
                }

                /* set vehicle_status.is_vtol flag */
                status.is_vtol = is_vtol(&status);

                /* check and update system / component ID */
                param_get(_param_system_id, &(status.system_id));
                param_get(_param_component_id, &(status.component_id));

                get_circuit_breaker_params();

                status_changed = true;
            }

            /* Safety parameters */
            param_get(_param_enable_datalink_loss, &datalink_loss_act);
            param_get(_param_enable_rc_loss, &rc_loss_act);
            param_get(_param_datalink_loss_timeout, &datalink_loss_timeout);
            param_get(_param_rc_loss_timeout, &rc_loss_timeout);
            param_get(_param_rc_in_off, &rc_in_off);
            status.rc_input_mode = rc_in_off;
            param_get(_param_rc_arm_hyst, &rc_arm_hyst);
            param_get(_param_min_stick_change, &min_stick_change);
            param_get(_param_rc_override, &rc_override);
            // percentage (* 0.01) needs to be doubled because RC total interval is 2, not 1
            min_stick_change *= 0.02f;
            rc_arm_hyst *= COMMANDER_MONITORING_LOOPSPERMSEC;
            param_get(_param_datalink_regain_timeout, &datalink_regain_timeout);
            param_get(_param_ef_throttle_thres, &ef_throttle_thres);
            param_get(_param_ef_current2throttle_thres, &ef_current2throttle_thres);
            param_get(_param_ef_time_thres, &ef_time_thres);
            param_get(_param_geofence_action, &geofence_action);
            param_get(_param_disarm_land, &disarm_when_landed);

            // If we update parameters the first time
            // make sure the hysteresis time gets set.
            // After that it will be set in the main state
            // machine based on the arming state.
            if (param_init_forced) {
                auto_disarm_hysteresis.set_hysteresis_time_from(false,
                                    (hrt_abstime)disarm_when_landed * 1000000);
            }

            param_get(_param_low_bat_act, &low_bat_action);
            param_get(_param_offboard_loss_timeout, &offboard_loss_timeout);
            param_get(_param_offboard_loss_act, &offboard_loss_act);
            param_get(_param_offboard_loss_rc_act, &offboard_loss_rc_act);
            param_get(_param_arm_switch_is_button, &arm_switch_is_button);

            param_get(_param_arm_without_gps, &arm_without_gps_param);
            arm_without_gps = (arm_without_gps_param == 1);

            param_get(_param_arm_mission_required, &arm_mission_required_param);
            arm_mission_required = (arm_mission_required_param == 1);

            /* Autostart id */
            param_get(_param_autostart_id, &autostart_id);

            /* EPH / EPV */
            param_get(_param_eph, &eph_threshold);
            param_get(_param_epv, &epv_threshold);

            /* flight mode slots */
            param_get(_param_fmode_1, &_flight_mode_slots[0]);
            param_get(_param_fmode_2, &_flight_mode_slots[1]);
            param_get(_param_fmode_3, &_flight_mode_slots[2]);
            param_get(_param_fmode_4, &_flight_mode_slots[3]);
            param_get(_param_fmode_5, &_flight_mode_slots[4]);
            param_get(_param_fmode_6, &_flight_mode_slots[5]);

            /* pre-flight EKF checks */
            param_get(_param_max_ekf_pos_ratio, &max_ekf_pos_ratio);
            param_get(_param_max_ekf_vel_ratio, &max_ekf_vel_ratio);
            param_get(_param_max_ekf_hgt_ratio, &max_ekf_hgt_ratio);
            param_get(_param_max_ekf_yaw_ratio, &max_ekf_yaw_ratio);
            param_get(_param_max_ekf_dvel_bias, &max_ekf_dvel_bias);
            param_get(_param_max_ekf_dang_bias, &max_ekf_dang_bias);

            /* pre-flight IMU consistency checks */
            param_get(_param_max_imu_acc_diff, &max_imu_acc_diff);
            param_get(_param_max_imu_gyr_diff, &max_imu_gyr_diff);

            /* failsafe response to loss of navigation accuracy */
            param_get(_param_posctl_nav_loss_act, &posctl_nav_loss_act);

            param_init_forced = false;
        }

        orb_check(sp_man_sub, &updated);

        if (updated) {
            orb_copy(ORB_ID(manual_control_setpoint), sp_man_sub, &sp_man);
        }

        orb_check(offboard_control_mode_sub, &updated);

        if (updated) {
            orb_copy(ORB_ID(offboard_control_mode), offboard_control_mode_sub, &offboard_control_mode);
        }

        if (offboard_control_mode.timestamp != 0 &&
            offboard_control_mode.timestamp + OFFBOARD_TIMEOUT > hrt_absolute_time()) {
            if (status_flags.offboard_control_signal_lost) {
                status_flags.offboard_control_signal_lost = false;
                status_flags.offboard_control_loss_timeout = false;
                status_changed = true;
            }

        } else {
            if (!status_flags.offboard_control_signal_lost) {
                status_flags.offboard_control_signal_lost = true;
                status_changed = true;
            }

            /* check timer if offboard was there but now lost */
            if (!status_flags.offboard_control_loss_timeout && offboard_control_mode.timestamp != 0) {
                if (offboard_loss_timeout < FLT_EPSILON) {
                    /* execute loss action immediately */
                    status_flags.offboard_control_loss_timeout = true;

                } else {
                    /* wait for timeout if set */
                    status_flags.offboard_control_loss_timeout = offboard_control_mode.timestamp +
                        OFFBOARD_TIMEOUT + offboard_loss_timeout * 1e6f < hrt_absolute_time();
                }

                if (status_flags.offboard_control_loss_timeout) {
                    status_changed = true;
                }
            }
        }

        for (int i = 0; i < ORB_MULTI_MAX_INSTANCES; i++) {

            if (telemetry_subs[i] < 0 && (OK == orb_exists(ORB_ID(telemetry_status), i))) {
                telemetry_subs[i] = orb_subscribe_multi(ORB_ID(telemetry_status), i);
            }

            orb_check(telemetry_subs[i], &updated);

            if (updated) {
                struct telemetry_status_s telemetry;
                memset(&telemetry, 0, sizeof(telemetry));

                orb_copy(ORB_ID(telemetry_status), telemetry_subs[i], &telemetry);

                /* perform system checks when new telemetry link connected */
                if (/* we first connect a link or re-connect a link after loosing it or haven't yet reported anything */
                    (telemetry_last_heartbeat[i] == 0 || (hrt_elapsed_time(&telemetry_last_heartbeat[i]) > 3 * 1000 * 1000)
                        || !telemetry_preflight_checks_reported[i]) &&
                    /* and this link has a communication partner */
                    (telemetry.heartbeat_time > 0) &&
                    /* and it is still connected */
                    (hrt_elapsed_time(&telemetry.heartbeat_time) < 2 * 1000 * 1000) &&
                    /* and the system is not already armed (and potentially flying) */
                    !armed.armed) {

                    hotplug_timeout = hrt_elapsed_time(&commander_boot_timestamp) > HOTPLUG_SENS_TIMEOUT;
                    /* flag the checks as reported for this link when we actually report them */
                    telemetry_preflight_checks_reported[i] = hotplug_timeout;

                    /* provide RC and sensor status feedback to the user */
                    if (is_hil_setup(autostart_id)) {
                        /* HIL configuration: check only RC input */
                        (void)Commander::preflightCheck(&mavlink_log_pub, false, false, false, false, false,
                                (status.rc_input_mode == vehicle_status_s::RC_IN_MODE_DEFAULT), false,
                                 /* checkDynamic */ true, is_vtol(&status), /* reportFailures */ false, /* prearm */ false, hrt_elapsed_time(&commander_boot_timestamp));
                    } else {
                        /* check sensors also */
                        (void)Commander::preflightCheck(&mavlink_log_pub, true, true, true, true, checkAirspeed,
                                (status.rc_input_mode == vehicle_status_s::RC_IN_MODE_DEFAULT), !arm_without_gps,
                                 /* checkDynamic */ true, is_vtol(&status), /* reportFailures */ hotplug_timeout, /* prearm */ false, hrt_elapsed_time(&commander_boot_timestamp));
                    }

                    // Provide feedback on mission state
                    if (!_mission_result.valid && hotplug_timeout && _home.timestamp > 0) {
                        mavlink_log_critical(&mavlink_log_pub, "Planned mission fails check. Please upload again.");
                    }
                }

                /* set (and don't reset) telemetry via USB as active once a MAVLink connection is up */
                if (telemetry.type == telemetry_status_s::TELEMETRY_STATUS_RADIO_TYPE_USB) {
                    _usb_telemetry_active = true;
                }

                if (telemetry.heartbeat_time > 0) {
                    telemetry_last_heartbeat[i] = telemetry.heartbeat_time;
                }
            }
        }

        orb_check(sensor_sub, &updated);

        if (updated) {
            orb_copy(ORB_ID(sensor_combined), sensor_sub, &sensors);

            /* Check if the barometer is healthy and issue a warning in the GCS if not so.
             * Because the barometer is used for calculating AMSL altitude which is used to ensure
             * vertical separation from other airtraffic the operator has to know when the
             * barometer is inoperational.
             * */
            hrt_abstime baro_timestamp = sensors.timestamp + sensors.baro_timestamp_relative;
            if (hrt_elapsed_time(&baro_timestamp) < FAILSAFE_DEFAULT_TIMEOUT) {
                /* handle the case where baro was regained */
                if (status_flags.barometer_failure) {
                    status_flags.barometer_failure = false;
                    status_changed = true;
                    if (status_flags.ever_had_barometer_data) {
                        mavlink_log_critical(&mavlink_log_pub, "baro healthy");
                    }
                    status_flags.ever_had_barometer_data = true;
                }

            } else {
                if (!status_flags.barometer_failure) {
                    status_flags.barometer_failure = true;
                    status_changed = true;
                    mavlink_log_critical(&mavlink_log_pub, "baro failed");
                }
            }
        }

        orb_check(diff_pres_sub, &updated);

        if (updated) {
            orb_copy(ORB_ID(differential_pressure), diff_pres_sub, &diff_pres);
        }

        orb_check(system_power_sub, &updated);

        if (updated) {
            orb_copy(ORB_ID(system_power), system_power_sub, &system_power);

            if (hrt_elapsed_time(&system_power.timestamp) < 200000) {
                if (system_power.servo_valid &&
                    !system_power.brick_valid &&
                    !system_power.usb_connected) {
                    /* flying only on servo rail, this is unsafe */
                    status_flags.condition_power_input_valid = false;

                } else {
                    status_flags.condition_power_input_valid = true;
                }

                /* copy avionics voltage */
                avionics_power_rail_voltage = system_power.voltage5V_v;

                /* if the USB hardware connection went away, reboot */
                if (status_flags.usb_connected && !system_power.usb_connected) {
                    /*
                     * apparently the USB cable went away but we are still powered,
                     * so lets reset to a classic non-usb state.
                     */
                    mavlink_log_critical(&mavlink_log_pub, "USB disconnected, rebooting.")
                    usleep(400000);
                    px4_shutdown_request(true, false);
                }

                /* finally judge the USB connected state based on software detection */
                status_flags.usb_connected = _usb_telemetry_active;
            }
        }

        check_valid(diff_pres.timestamp, DIFFPRESS_TIMEOUT, true, &(status_flags.condition_airspeed_valid), &status_changed);

        /* update safety topic */
        orb_check(safety_sub, &updated);

        if (updated) {
            bool previous_safety_off = safety.safety_off;
            orb_copy(ORB_ID(safety), safety_sub, &safety);

            /* disarm if safety is now on and still armed */
            if (status.hil_state == vehicle_status_s::HIL_STATE_OFF && safety.safety_switch_available && !safety.safety_off && armed.armed) {
                arming_state_t new_arming_state = (status.arming_state == vehicle_status_s::ARMING_STATE_ARMED ? vehicle_status_s::ARMING_STATE_STANDBY :
                                   vehicle_status_s::ARMING_STATE_STANDBY_ERROR);

                if (TRANSITION_CHANGED == arming_state_transition(&status,
                                          &battery,
                                          &safety,
                                          new_arming_state,
                                          &armed,
                                          true /* fRunPreArmChecks */,
                                          &mavlink_log_pub,
                                          &status_flags,
                                          avionics_power_rail_voltage,
                                          arm_without_gps,
                                          arm_mission_required,
                                          hrt_elapsed_time(&commander_boot_timestamp))) {
                    arming_state_changed = true;
                }
            }

            //Notify the user if the status of the safety switch changes
            if (safety.safety_switch_available && previous_safety_off != safety.safety_off) {

                if (safety.safety_off) {
                    set_tune(TONE_NOTIFY_POSITIVE_TUNE);

                } else {
                    tune_neutral(true);
                }

                status_changed = true;
            }
        }

        /* update vtol vehicle status*/
        orb_check(vtol_vehicle_status_sub, &updated);

        if (updated) {
            /* vtol status changed */
            orb_copy(ORB_ID(vtol_vehicle_status), vtol_vehicle_status_sub, &vtol_status);
            status.vtol_fw_permanent_stab = vtol_status.fw_permanent_stab;

            /* Make sure that this is only adjusted if vehicle really is of type vtol */
            if (is_vtol(&status)) {
                status.is_rotary_wing = vtol_status.vtol_in_rw_mode;
                status.in_transition_mode = vtol_status.vtol_in_trans_mode;
                status.in_transition_to_fw = vtol_status.in_transition_to_fw;
                status_flags.vtol_transition_failure = vtol_status.vtol_transition_failsafe;
                status_flags.vtol_transition_failure_cmd = vtol_status.vtol_transition_failsafe;

                armed.soft_stop = !status.is_rotary_wing;
            }

            status_changed = true;
        }

        // Check if quality checking of position accuracy and consistency is to be performed
        bool run_quality_checks = !status_flags.circuit_breaker_engaged_posfailure_check;

        /* update global position estimate and check for timeout */
        bool gpos_updated =  false;
        orb_check(global_position_sub, &gpos_updated);
        if (gpos_updated) {
            orb_copy(ORB_ID(vehicle_global_position), global_position_sub, &global_position);
            gpos_last_update_time_us = hrt_absolute_time();
        }

        // Perform a separate timeout validity test on the global position data.
        // This is necessary because the global position message is by definition valid if published.
        if ((hrt_absolute_time() - gpos_last_update_time_us) > 1000000) {
            status_flags.condition_global_position_valid = false;
            status_flags.condition_global_velocity_valid = false;
        }

        /* run global position accuracy checks */
        if (gpos_updated) {
            if (run_quality_checks) {
                check_posvel_validity(true, global_position.eph, eph_threshold, global_position.timestamp, &last_gpos_fail_time_us, &gpos_probation_time_us, &status_flags.condition_global_position_valid, &status_changed);
                check_posvel_validity(true, global_position.evh, evh_threshold, global_position.timestamp, &last_gvel_fail_time_us, &gvel_probation_time_us, &status_flags.condition_global_velocity_valid, &status_changed);
            }
        }

        /* update local position estimate */
        bool lpos_updated = false;
        orb_check(local_position_sub, &lpos_updated);

        if (lpos_updated) {
            /* position changed */
            orb_copy(ORB_ID(vehicle_local_position), local_position_sub, &local_position);

            if (run_quality_checks) {
                check_posvel_validity(local_position.xy_valid, local_position.eph, eph_threshold, local_position.timestamp, &last_lpos_fail_time_us, &lpos_probation_time_us, &status_flags.condition_local_position_valid, &status_changed);
                check_posvel_validity(local_position.v_xy_valid, local_position.evh, evh_threshold, local_position.timestamp, &last_lvel_fail_time_us, &lvel_probation_time_us, &status_flags.condition_local_velocity_valid, &status_changed);
            }
        }

        /* update attitude estimate */
        orb_check(attitude_sub, &updated);

        if (updated) {
            /* attitude changed */
            orb_copy(ORB_ID(vehicle_attitude), attitude_sub, &attitude);
        }

        /* update condition_local_altitude_valid */
        check_valid(local_position.timestamp, POSITION_TIMEOUT, local_position.z_valid,
                &(status_flags.condition_local_altitude_valid), &status_changed);

        /* Update land detector */
        orb_check(land_detector_sub, &updated);
        if (updated) {
            orb_copy(ORB_ID(vehicle_land_detected), land_detector_sub, &land_detector);

            if (was_landed != land_detector.landed) {
                if (land_detector.landed) {
                    mavlink_and_console_log_info(&mavlink_log_pub, "Landing detected");
                } else {
                    mavlink_and_console_log_info(&mavlink_log_pub, "Takeoff detected");
                    have_taken_off_since_arming = true;

                    // Set all position and velocity test probation durations to takeoff value
                    // This is a larger value to give the vehicle time to complete a failsafe landing
                    // if faulty sensors cause loss of navigation shortly after takeoff.
                    gpos_probation_time_us = POSVEL_PROBATION_TAKEOFF;
                    gvel_probation_time_us = POSVEL_PROBATION_TAKEOFF;
                    lpos_probation_time_us = POSVEL_PROBATION_TAKEOFF;
                    lvel_probation_time_us = POSVEL_PROBATION_TAKEOFF;
                }
            }

            if (was_falling != land_detector.freefall) {
                if (land_detector.freefall) {
                    mavlink_and_console_log_info(&mavlink_log_pub, "Freefall detected");
                }
            }


            was_landed = land_detector.landed;
            was_falling = land_detector.freefall;
        }

        /* Update hysteresis time. Use a time of factor 5 longer if we have not taken off yet. */
        hrt_abstime timeout_time = disarm_when_landed * 1000000;

        if (!have_taken_off_since_arming) {
            timeout_time *= 5;
        }

        auto_disarm_hysteresis.set_hysteresis_time_from(false, timeout_time);

        // Check for auto-disarm
        if (armed.armed && land_detector.landed && disarm_when_landed > 0) {
            auto_disarm_hysteresis.set_state_and_update(true);
        } else {
            auto_disarm_hysteresis.set_state_and_update(false);
        }

        if (auto_disarm_hysteresis.get_state()) {
            arm_disarm(false, &mavlink_log_pub, "auto disarm on land");
        }

        if (!warning_action_on) {
            // store the last good main_state when not in an navigation
            // hold state
            main_state_before_rtl = internal_state.main_state;

        } else if (internal_state.main_state != commander_state_s::MAIN_STATE_AUTO_RTL
            && internal_state.main_state != commander_state_s::MAIN_STATE_AUTO_LOITER
            && internal_state.main_state != commander_state_s::MAIN_STATE_AUTO_LAND) {
            // reset flag again when we switched out of it
            warning_action_on = false;
        }

        orb_check(cpuload_sub, &updated);

        if (updated) {
            orb_copy(ORB_ID(cpuload), cpuload_sub, &cpuload);
        }

        /* update battery status */
        orb_check(battery_sub, &updated);

        if (updated) {
            orb_copy(ORB_ID(battery_status), battery_sub, &battery);

            /* only consider battery voltage if system has been running 6s (usb most likely detected) and battery voltage is valid */
            if (hrt_absolute_time() > commander_boot_timestamp + 6000000
                && battery.voltage_filtered_v > 2.0f * FLT_EPSILON) {

                /* if battery voltage is getting lower, warn using buzzer, etc. */
                if (battery.warning == battery_status_s::BATTERY_WARNING_LOW &&
                   !low_battery_voltage_actions_done) {
                    low_battery_voltage_actions_done = true;
                    if (armed.armed) {
                        mavlink_log_critical(&mavlink_log_pub, "LOW BATTERY, RETURN TO LAND ADVISED");
                    } else {
                        mavlink_log_critical(&mavlink_log_pub, "LOW BATTERY, TAKEOFF DISCOURAGED");
                    }

                    status_changed = true;
                } else if (!status_flags.usb_connected &&
                       battery.warning == battery_status_s::BATTERY_WARNING_CRITICAL &&
                       !critical_battery_voltage_actions_done) {
                    critical_battery_voltage_actions_done = true;

                    if (!armed.armed) {
                        mavlink_log_critical(&mavlink_log_pub, "CRITICAL BATTERY, SHUT SYSTEM DOWN");

                    } else {
                        if (low_bat_action == 1 || low_bat_action == 3) {
                            // let us send the critical message even if already in RTL
                            if (TRANSITION_CHANGED == main_state_transition(&status, commander_state_s::MAIN_STATE_AUTO_RTL, main_state_prev, &status_flags, &internal_state)) {
                                warning_action_on = true;
                                mavlink_log_emergency(&mavlink_log_pub, "CRITICAL BATTERY, RETURNING TO LAND");

                            } else {
                                mavlink_log_emergency(&mavlink_log_pub, "CRITICAL BATTERY, RTL FAILED");
                            }

                        } else if (low_bat_action == 2) {
                            if (TRANSITION_CHANGED == main_state_transition(&status, commander_state_s::MAIN_STATE_AUTO_LAND, main_state_prev, &status_flags, &internal_state)) {
                                warning_action_on = true;
                                mavlink_log_emergency(&mavlink_log_pub, "CRITICAL BATTERY, LANDING AT CURRENT POSITION");

                            } else {
                                mavlink_log_emergency(&mavlink_log_pub, "CRITICAL BATTERY, LANDING FAILED");
                            }

                        } else {
                            mavlink_log_emergency(&mavlink_log_pub, "CRITICAL BATTERY, RETURN TO LAUNCH ADVISED!");
                        }
                    }

                    status_changed = true;

                } else if (!status_flags.usb_connected &&
                       battery.warning == battery_status_s::BATTERY_WARNING_EMERGENCY &&
                       !emergency_battery_voltage_actions_done) {
                    emergency_battery_voltage_actions_done = true;

                    if (!armed.armed) {
                        mavlink_log_critical(&mavlink_log_pub, "DANGEROUSLY LOW BATTERY, SHUT SYSTEM DOWN");
                        usleep(200000);
                        int ret_val = px4_shutdown_request(false, false);
                        if (ret_val) {
                            mavlink_log_critical(&mavlink_log_pub, "SYSTEM DOES NOT SUPPORT SHUTDOWN");
                        } else {
                            while(1) { usleep(1); }
                        }

                    } else {
                        if (low_bat_action == 2 || low_bat_action == 3) {
                            if (TRANSITION_CHANGED == main_state_transition(&status, commander_state_s::MAIN_STATE_AUTO_LAND, main_state_prev, &status_flags, &internal_state)) {
                                warning_action_on = true;
                                mavlink_log_emergency(&mavlink_log_pub, "DANGEROUS BATTERY LEVEL, LANDING IMMEDIATELY");

                            } else {
                                mavlink_log_emergency(&mavlink_log_pub, "DANGEROUS BATTERY LEVEL, LANDING FAILED");
                            }

                        } else {
                            mavlink_log_emergency(&mavlink_log_pub, "DANGEROUS BATTERY LEVEL, LANDING ADVISED!");
                        }
                    }

                    status_changed = true;
                }

                /* End battery voltage check */
            }
        }

        /* update subsystem */
        orb_check(subsys_sub, &updated);

        if (updated) {
            orb_copy(ORB_ID(subsystem_info), subsys_sub, &info);

            //warnx("subsystem changed: %d\n", (int)info.subsystem_type);

            /* mark / unmark as present */
            if (info.present) {
                status.onboard_control_sensors_present |= info.subsystem_type;

            } else {
                status.onboard_control_sensors_present &= ~info.subsystem_type;
            }

            /* mark / unmark as enabled */
            if (info.enabled) {
                status.onboard_control_sensors_enabled |= info.subsystem_type;

            } else {
                status.onboard_control_sensors_enabled &= ~info.subsystem_type;
            }

            /* mark / unmark as ok */
            if (info.ok) {
                status.onboard_control_sensors_health |= info.subsystem_type;

            } else {
                status.onboard_control_sensors_health &= ~info.subsystem_type;
            }

            status_changed = true;
        }

        /* update position setpoint triplet */
        orb_check(pos_sp_triplet_sub, &updated);

        if (updated) {
            orb_copy(ORB_ID(position_setpoint_triplet), pos_sp_triplet_sub, &pos_sp_triplet);
        }

        /* If in INIT state, try to proceed to STANDBY state */
        if (!status_flags.condition_calibration_enabled && status.arming_state == vehicle_status_s::ARMING_STATE_INIT) {
            arming_ret = arming_state_transition(&status,
                                 &battery,
                                 &safety,
                                 vehicle_status_s::ARMING_STATE_STANDBY,
                                 &armed,
                                 true /* fRunPreArmChecks */,
                                 &mavlink_log_pub,
                                 &status_flags,
                                 avionics_power_rail_voltage,
                                 arm_without_gps,
                                 arm_mission_required,
                                 hrt_elapsed_time(&commander_boot_timestamp));

            if (arming_ret == TRANSITION_CHANGED) {
                arming_state_changed = true;
            } else if (arming_ret == TRANSITION_DENIED) {
                /* do not complain if not allowed into standby */
                arming_ret = TRANSITION_NOT_CHANGED;
            }
        }

        /*
         * Check GPS fix quality. Note that this check augments the position validity
         * checks and adds an additional level of protection.
         */

        orb_check(gps_sub, &updated);

        if (updated) {
            orb_copy(ORB_ID(vehicle_gps_position), gps_sub, &gps_position);
        }

        /* Initialize map projection if gps is valid */
        if (!map_projection_global_initialized()
            && (gps_position.eph < eph_threshold)
            && (gps_position.epv < epv_threshold)
            && hrt_elapsed_time((hrt_abstime *)&gps_position.timestamp) < 1e6) {
            /* set reference for global coordinates <--> local coordiantes conversion and map_projection */
            globallocalconverter_init((double)gps_position.lat * 1.0e-7, (double)gps_position.lon * 1.0e-7,
                          (float)gps_position.alt * 1.0e-3f, hrt_absolute_time());
        }

        /* check if GPS is ok */
        if (!status_flags.circuit_breaker_engaged_gpsfailure_check) {
            bool gpsIsNoisy = gps_position.noise_per_ms > 0 && gps_position.noise_per_ms < COMMANDER_MAX_GPS_NOISE;

            //Check if GPS receiver is too noisy while we are disarmed
            if (!armed.armed && gpsIsNoisy) {
                if (!status_flags.gps_failure) {
                    mavlink_log_critical(&mavlink_log_pub, "GPS signal noisy");
                    set_tune_override(TONE_GPS_WARNING_TUNE);

                    //GPS suffers from signal jamming or excessive noise, disable GPS-aided flight
                    status_flags.gps_failure = true;
                    status_changed = true;
                }
            }

            // Check fix type and data freshness
            if (gps_position.fix_type >= 3 && hrt_elapsed_time(&gps_position.timestamp) < FAILSAFE_DEFAULT_TIMEOUT) {
                /* handle the case where gps was regained */
                if (status_flags.gps_failure && !gpsIsNoisy) {
                    status_flags.gps_failure = false;
                    status_changed = true;
                    if (status_flags.condition_home_position_valid) {
                        mavlink_log_critical(&mavlink_log_pub, "GPS fix regained");
                    }
                }

            } else if (!status_flags.gps_failure) {
                status_flags.gps_failure = true;
                status_changed = true;
                if (status.arming_state == vehicle_status_s::ARMING_STATE_ARMED) {
                    mavlink_log_critical(&mavlink_log_pub, "GPS fix lost");
                }
            }

        }

        /* start mission result check */
        orb_check(mission_result_sub, &updated);

        if (updated) {
            orb_copy(ORB_ID(mission_result), mission_result_sub, &_mission_result);

            status_flags.condition_auto_mission_available = _mission_result.valid && !_mission_result.finished;

            if (status.mission_failure != _mission_result.failure) {
                status.mission_failure = _mission_result.failure;
                status_changed = true;

                if (status.mission_failure) {
                    mavlink_log_critical(&mavlink_log_pub, "mission cannot be completed");
                }
            }
        }

        /* start geofence result check */
        orb_check(geofence_result_sub, &updated);

        if (updated) {
            orb_copy(ORB_ID(geofence_result), geofence_result_sub, &geofence_result);
        }

        // Geofence actions
        if (armed.armed && (geofence_result.geofence_action != geofence_result_s::GF_ACTION_NONE)) {

            static bool geofence_loiter_on = false;
            static bool geofence_rtl_on = false;

            // check for geofence violation
            if (geofence_result.geofence_violated) {
                static hrt_abstime last_geofence_violation = 0;
                const hrt_abstime geofence_violation_action_interval = 10000000; // 10 seconds
                if (hrt_elapsed_time(&last_geofence_violation) > geofence_violation_action_interval) {

                    last_geofence_violation = hrt_absolute_time();

                    switch (geofence_result.geofence_action) {
                        case (geofence_result_s::GF_ACTION_NONE) : {
                            // do nothing
                            break;
                        }
                        case (geofence_result_s::GF_ACTION_WARN) : {
                            // do nothing, mavlink critical messages are sent by navigator
                            break;
                        }
                        case (geofence_result_s::GF_ACTION_LOITER) : {
                            if (TRANSITION_CHANGED == main_state_transition(&status, commander_state_s::MAIN_STATE_AUTO_LOITER, main_state_prev, &status_flags, &internal_state)) {
                                geofence_loiter_on = true;
                            }
                            break;
                        }
                        case (geofence_result_s::GF_ACTION_RTL) : {
                            if (TRANSITION_CHANGED == main_state_transition(&status, commander_state_s::MAIN_STATE_AUTO_RTL, main_state_prev, &status_flags, &internal_state)) {
                                geofence_rtl_on = true;
                            }
                            break;
                        }
                        case (geofence_result_s::GF_ACTION_TERMINATE) : {
                            warnx("Flight termination because of geofence");
                            mavlink_log_critical(&mavlink_log_pub, "Geofence violation: flight termination");
                            armed.force_failsafe = true;
                            status_changed = true;
                            break;
                        }
                    }
                }
            }

            // reset if no longer in LOITER or if manually switched to LOITER
            geofence_loiter_on = geofence_loiter_on
                                    && (internal_state.main_state == commander_state_s::MAIN_STATE_AUTO_LOITER)
                                    && (sp_man.loiter_switch == manual_control_setpoint_s::SWITCH_POS_OFF
                                        || sp_man.loiter_switch == manual_control_setpoint_s::SWITCH_POS_NONE);

            // reset if no longer in RTL or if manually switched to RTL
            geofence_rtl_on = geofence_rtl_on
                                && (internal_state.main_state == commander_state_s::MAIN_STATE_AUTO_RTL)
                                && (sp_man.return_switch == manual_control_setpoint_s::SWITCH_POS_OFF
                                    || sp_man.return_switch == manual_control_setpoint_s::SWITCH_POS_NONE);

            warning_action_on = warning_action_on || (geofence_loiter_on || geofence_rtl_on);
        }

        // revert geofence failsafe transition if sticks are moved and we were previously in a manual mode
        // but only if not in a low battery handling action
        if (rc_override != 0 && !critical_battery_voltage_actions_done && (warning_action_on &&
           (main_state_before_rtl == commander_state_s::MAIN_STATE_MANUAL ||
            main_state_before_rtl == commander_state_s::MAIN_STATE_ALTCTL ||
            main_state_before_rtl == commander_state_s::MAIN_STATE_POSCTL ||
            main_state_before_rtl == commander_state_s::MAIN_STATE_ACRO ||
            main_state_before_rtl == commander_state_s::MAIN_STATE_RATTITUDE ||
            main_state_before_rtl == commander_state_s::MAIN_STATE_STAB))) {

            // transition to previous state if sticks are touched
            if ((_last_sp_man.timestamp != sp_man.timestamp) &&
                ((fabsf(sp_man.x - _last_sp_man.x) > min_stick_change) ||
                 (fabsf(sp_man.y - _last_sp_man.y) > min_stick_change) ||
                 (fabsf(sp_man.z - _last_sp_man.z) > min_stick_change) ||
                 (fabsf(sp_man.r - _last_sp_man.r) > min_stick_change))) {

                // revert to position control in any case
                main_state_transition(&status, commander_state_s::MAIN_STATE_POSCTL, main_state_prev, &status_flags, &internal_state);
                mavlink_log_critical(&mavlink_log_pub, "Autopilot off, returned control to pilot");
            }
        }

        // abort landing or auto or loiter if sticks are moved significantly
        // but only if not in a low battery handling action
        if (rc_override != 0 && !critical_battery_voltage_actions_done &&
            (internal_state.main_state == commander_state_s::MAIN_STATE_AUTO_LAND ||
            internal_state.main_state == commander_state_s::MAIN_STATE_AUTO_MISSION ||
            internal_state.main_state == commander_state_s::MAIN_STATE_AUTO_LOITER)) {
            // transition to previous state if sticks are touched

            if ((_last_sp_man.timestamp != sp_man.timestamp) &&
                ((fabsf(sp_man.x - _last_sp_man.x) > min_stick_change) ||
                 (fabsf(sp_man.y - _last_sp_man.y) > min_stick_change) ||
                 (fabsf(sp_man.z - _last_sp_man.z) > min_stick_change) ||
                 (fabsf(sp_man.r - _last_sp_man.r) > min_stick_change))) {

                // revert to position control in any case
                main_state_transition(&status, commander_state_s::MAIN_STATE_POSCTL, main_state_prev, &status_flags, &internal_state);
                mavlink_log_critical(&mavlink_log_pub, "Autopilot off, returned control to pilot");
            }
        }


        /* Check for mission flight termination */
        if (armed.armed && _mission_result.flight_termination &&
            !status_flags.circuit_breaker_flight_termination_disabled) {

            armed.force_failsafe = true;
            status_changed = true;
            static bool flight_termination_printed = false;

            if (!flight_termination_printed) {
                mavlink_log_critical(&mavlink_log_pub, "Geofence violation: flight termination");
                flight_termination_printed = true;
            }

            if (counter % (1000000 / COMMANDER_MONITORING_INTERVAL) == 0) {
                mavlink_log_critical(&mavlink_log_pub, "Flight termination active");
            }
        }

        /* Only evaluate mission state if home is set,
         * this prevents false positives for the mission
         * rejection. Back off 3 seconds to not overlay
         * home tune.
         */
        if (status_flags.condition_home_position_valid &&
            (hrt_elapsed_time(&_home.timestamp) > 3000000) &&
            _last_mission_instance != _mission_result.instance_count) {

            if (!_mission_result.valid) {
                /* the mission is invalid */
                tune_mission_fail(true);
            } else if (_mission_result.warning) {
                /* the mission has a warning */
                tune_mission_fail(true);
            } else {
                /* the mission is valid */
                tune_mission_ok(true);
            }

            /* prevent further feedback until the mission changes */
            _last_mission_instance = _mission_result.instance_count;
        }

        /* RC input check */
        if (!status_flags.rc_input_blocked && sp_man.timestamp != 0 &&
            (hrt_absolute_time() < sp_man.timestamp + (uint64_t)(rc_loss_timeout * 1e6f))) {
            /* handle the case where RC signal was regained */
            if (!status_flags.rc_signal_found_once) {
                status_flags.rc_signal_found_once = true;
                status_changed = true;

            } else {
                if (status.rc_signal_lost) {
                    mavlink_log_info(&mavlink_log_pub, "MANUAL CONTROL REGAINED after %llums",
                                 (hrt_absolute_time() - rc_signal_lost_timestamp) / 1000);
                    status_changed = true;
                }
            }

            status.rc_signal_lost = false;

            const bool in_armed_state = status.arming_state == vehicle_status_s::ARMING_STATE_ARMED || status.arming_state == vehicle_status_s::ARMING_STATE_ARMED_ERROR;
            const bool arm_button_pressed = arm_switch_is_button == 1 && sp_man.arm_switch == manual_control_setpoint_s::SWITCH_POS_ON;

            /* DISARM
             * check if left stick is in lower left position or arm button is pushed or arm switch has transition from arm to disarm
             * and we are in MANUAL, Rattitude, or AUTO_READY mode or (ASSIST mode and landed)
             * do it only for rotary wings in manual mode or fixed wing if landed */
            const bool stick_in_lower_left = sp_man.r < -STICK_ON_OFF_LIMIT && sp_man.z < 0.1f;
            const bool arm_switch_to_disarm_transition =  arm_switch_is_button == 0 &&
                    _last_sp_man_arm_switch == manual_control_setpoint_s::SWITCH_POS_ON &&
                    sp_man.arm_switch == manual_control_setpoint_s::SWITCH_POS_OFF;

            if (in_armed_state &&
                status.rc_input_mode != vehicle_status_s::RC_IN_MODE_OFF &&
                (status.is_rotary_wing || (!status.is_rotary_wing && land_detector.landed)) &&
                (stick_in_lower_left || arm_button_pressed || arm_switch_to_disarm_transition) ) {

                if (internal_state.main_state != commander_state_s::MAIN_STATE_MANUAL &&
                        internal_state.main_state != commander_state_s::MAIN_STATE_ACRO &&
                        internal_state.main_state != commander_state_s::MAIN_STATE_STAB &&
                        internal_state.main_state != commander_state_s::MAIN_STATE_RATTITUDE &&
                        !land_detector.landed) {
                    print_reject_arm("NOT DISARMING: Not in manual mode or landed yet.");

                } else if ((stick_off_counter == rc_arm_hyst && stick_on_counter < rc_arm_hyst) || arm_switch_to_disarm_transition) {
                    /* disarm to STANDBY if ARMED or to STANDBY_ERROR if ARMED_ERROR */
                    arming_state_t new_arming_state = (status.arming_state == vehicle_status_s::ARMING_STATE_ARMED ? vehicle_status_s::ARMING_STATE_STANDBY :
                                       vehicle_status_s::ARMING_STATE_STANDBY_ERROR);
                    arming_ret = arming_state_transition(&status,
                                         &battery,
                                         &safety,
                                         new_arming_state,
                                         &armed,
                                         true /* fRunPreArmChecks */,
                                         &mavlink_log_pub,
                                         &status_flags,
                                         avionics_power_rail_voltage,
                                         arm_without_gps,
                                         arm_mission_required,
                                         hrt_elapsed_time(&commander_boot_timestamp));

                    if (arming_ret == TRANSITION_CHANGED) {
                        arming_state_changed = true;
                    }
                }
                stick_off_counter++;
            /* do not reset the counter when holding the arm button longer than needed */
            } else if (!(arm_switch_is_button == 1 && sp_man.arm_switch == manual_control_setpoint_s::SWITCH_POS_ON)) {
                stick_off_counter = 0;
            }

            /* ARM
             * check if left stick is in lower right position or arm button is pushed or arm switch has transition from disarm to arm
             * and we're in MANUAL mode */
            const bool stick_in_lower_right = (sp_man.r > STICK_ON_OFF_LIMIT && sp_man.z < 0.1f);
            const bool arm_switch_to_arm_transition = arm_switch_is_button == 0 &&
                    _last_sp_man_arm_switch == manual_control_setpoint_s::SWITCH_POS_OFF &&
                    sp_man.arm_switch == manual_control_setpoint_s::SWITCH_POS_ON;

            if (!in_armed_state &&
                status.rc_input_mode != vehicle_status_s::RC_IN_MODE_OFF &&
                (stick_in_lower_right || arm_button_pressed || arm_switch_to_arm_transition) ) {
                if ((stick_on_counter == rc_arm_hyst && stick_off_counter < rc_arm_hyst) || arm_switch_to_arm_transition) {

                    /* we check outside of the transition function here because the requirement
                     * for being in manual mode only applies to manual arming actions.
                     * the system can be armed in auto if armed via the GCS.
                     */

                    if ((internal_state.main_state != commander_state_s::MAIN_STATE_MANUAL)
                        && (internal_state.main_state != commander_state_s::MAIN_STATE_ACRO)
                        && (internal_state.main_state != commander_state_s::MAIN_STATE_STAB)
                        && (internal_state.main_state != commander_state_s::MAIN_STATE_ALTCTL)
                        && (internal_state.main_state != commander_state_s::MAIN_STATE_POSCTL)
                        && (internal_state.main_state != commander_state_s::MAIN_STATE_RATTITUDE)
                        ) {
                        print_reject_arm("NOT ARMING: Switch to a manual mode first.");

                    } else if (!status_flags.condition_home_position_valid &&
                                geofence_action == geofence_result_s::GF_ACTION_RTL) {
                        print_reject_arm("NOT ARMING: Geofence RTL requires valid home");

                    } else if (status.arming_state == vehicle_status_s::ARMING_STATE_STANDBY) {
                        arming_ret = arming_state_transition(&status,
                                             &battery,
                                             &safety,
                                             vehicle_status_s::ARMING_STATE_ARMED,
                                             &armed,
                                             true /* fRunPreArmChecks */,
                                             &mavlink_log_pub,
                                             &status_flags,
                                             avionics_power_rail_voltage,
                                             arm_without_gps,
                                             arm_mission_required,
                                             hrt_elapsed_time(&commander_boot_timestamp));

                        if (arming_ret == TRANSITION_CHANGED) {
                            arming_state_changed = true;
                        } else {
                            usleep(100000);
                            print_reject_arm("NOT ARMING: Preflight checks failed");
                        }
                    }
                }
                stick_on_counter++;
            /* do not reset the counter when holding the arm button longer than needed */
            } else if (!(arm_switch_is_button == 1 && sp_man.arm_switch == manual_control_setpoint_s::SWITCH_POS_ON)) {
                stick_on_counter = 0;
            }

            _last_sp_man_arm_switch = sp_man.arm_switch;

            if (arming_ret == TRANSITION_CHANGED) {
                arming_state_changed = true;

            } else if (arming_ret == TRANSITION_DENIED) {
                /*
                 * the arming transition can be denied to a number of reasons:
                 *  - pre-flight check failed (sensors not ok or not calibrated)
                 *  - safety not disabled
                 *  - system not in manual mode
                 */
                tune_negative(true);
            }

            /* evaluate the main state machine according to mode switches */
            bool first_rc_eval = (_last_sp_man.timestamp == 0) && (sp_man.timestamp > 0);
            transition_result_t main_res = set_main_state_rc(&status, &global_position, &local_position, &status_changed);

            /* store last position lock state */
            _last_condition_global_position_valid = status_flags.condition_global_position_valid;

            /* play tune on mode change only if armed, blink LED always */
            if (main_res == TRANSITION_CHANGED || first_rc_eval) {
                tune_positive(armed.armed);
                main_state_changed = true;

            } else if (main_res == TRANSITION_DENIED) {
                /* DENIED here indicates bug in the commander */
                mavlink_log_critical(&mavlink_log_pub, "Switching to this mode is currently not possible");
            }

            /* check throttle kill switch */
            if (sp_man.kill_switch == manual_control_setpoint_s::SWITCH_POS_ON) {
                /* set lockdown flag */
                if (!armed.manual_lockdown) {
                    mavlink_log_emergency(&mavlink_log_pub, "MANUAL KILL SWITCH ENGAGED");
                }
                armed.manual_lockdown = true;
            } else if (sp_man.kill_switch == manual_control_setpoint_s::SWITCH_POS_OFF) {
                if (armed.manual_lockdown) {
                    mavlink_log_emergency(&mavlink_log_pub, "MANUAL KILL SWITCH OFF");
                }
                armed.manual_lockdown = false;
            }
            /* no else case: do not change lockdown flag in unconfigured case */
        } else {
            if (!status_flags.rc_input_blocked && !status.rc_signal_lost) {
                mavlink_log_critical(&mavlink_log_pub, "MANUAL CONTROL LOST (at t=%llums)", hrt_absolute_time() / 1000);
                status.rc_signal_lost = true;
                rc_signal_lost_timestamp = sp_man.timestamp;
                status_changed = true;
            }
        }

        /* data links check */
        bool have_link = false;

        for (int i = 0; i < ORB_MULTI_MAX_INSTANCES; i++) {
            if (telemetry_last_heartbeat[i] != 0 &&
                hrt_elapsed_time(&telemetry_last_heartbeat[i]) < datalink_loss_timeout * 1e6) {
                /* handle the case where data link was gained first time or regained,
                 * accept datalink as healthy only after datalink_regain_timeout seconds
                 * */
                if (telemetry_lost[i] &&
                    hrt_elapsed_time(&telemetry_last_dl_loss[i]) > datalink_regain_timeout * 1e6) {

                    /* report a regain */
                    if (telemetry_last_dl_loss[i] > 0) {
                        mavlink_and_console_log_info(&mavlink_log_pub, "data link #%i regained", i);
                    } else if (telemetry_last_dl_loss[i] == 0) {
                        /* new link */
                    }

                    /* got link again or new */
                    status_flags.condition_system_prearm_error_reported = false;
                    status_changed = true;

                    telemetry_lost[i] = false;
                    have_link = true;

                } else if (!telemetry_lost[i]) {
                    /* telemetry was healthy also in last iteration
                     * we don't have to check a timeout */
                    have_link = true;
                }

            } else {

                if (!telemetry_lost[i]) {
                    /* only reset the timestamp to a different time on state change */
                    telemetry_last_dl_loss[i]  = hrt_absolute_time();

                    mavlink_and_console_log_info(&mavlink_log_pub, "data link #%i lost", i);
                    telemetry_lost[i] = true;
                }
            }
        }

        if (have_link) {
            /* handle the case where data link was regained */
            if (status.data_link_lost) {
                status.data_link_lost = false;
                status_changed = true;
            }

        } else {
            if (!status.data_link_lost) {
                if (armed.armed) {
                    mavlink_log_critical(&mavlink_log_pub, "ALL DATA LINKS LOST");
                }
                status.data_link_lost = true;
                status.data_link_lost_counter++;
                status_changed = true;
            }
        }

        /* handle commands last, as the system needs to be updated to handle them */
        orb_check(actuator_controls_sub, &updated);

        if (updated) {
            /* got command */
            orb_copy(ORB_ID_VEHICLE_ATTITUDE_CONTROLS, actuator_controls_sub, &actuator_controls);

            /* Check engine failure
             * only for fixed wing for now
             */
            if (!status_flags.circuit_breaker_engaged_enginefailure_check &&
                status.is_rotary_wing == false &&
                armed.armed &&
                ((actuator_controls.control[3] > ef_throttle_thres &&
                  battery.current_a / actuator_controls.control[3] <
                  ef_current2throttle_thres) ||
                 (status.engine_failure))) {
                /* potential failure, measure time */
                if (timestamp_engine_healthy > 0 &&
                    hrt_elapsed_time(&timestamp_engine_healthy) >
                    ef_time_thres * 1e6 &&
                    !status.engine_failure) {
                    status.engine_failure = true;
                    status_changed = true;
                    mavlink_log_critical(&mavlink_log_pub, "Engine Failure");
                }

            } else {
                /* no failure reset flag */
                timestamp_engine_healthy = hrt_absolute_time();

                if (status.engine_failure) {
                    status.engine_failure = false;
                    status_changed = true;
                }
            }
        }

        /* reset main state after takeoff has completed */
        /* only switch back to posctl */
        if (main_state_prev == commander_state_s::MAIN_STATE_POSCTL) {

            if (internal_state.main_state == commander_state_s::MAIN_STATE_AUTO_TAKEOFF
                    && _mission_result.finished) {

                main_state_transition(&status, main_state_prev, main_state_prev, &status_flags, &internal_state);
            }
        }

        /* check if we are disarmed and there is a better mode to wait in */
        if (!armed.armed) {

            /* if there is no radio control but GPS lock the user might want to fly using
             * just a tablet. Since the RC will force its mode switch setting on connecting
             * we can as well just wait in a hold mode which enables tablet control.
             */
            if (status.rc_signal_lost && (internal_state.main_state == commander_state_s::MAIN_STATE_MANUAL)
                && status_flags.condition_home_position_valid) {
                (void)main_state_transition(&status, commander_state_s::MAIN_STATE_AUTO_LOITER, main_state_prev, &status_flags, &internal_state);
            }
        }

        /* handle commands last, as the system needs to be updated to handle them */
        orb_check(cmd_sub, &updated);

        if (updated) {
            /* got command */
            orb_copy(ORB_ID(vehicle_command), cmd_sub, &cmd);

            /* handle it */
            if (handle_command(&status, &safety, &cmd, &armed, &_home, &global_position, &local_position,
                    &attitude, &home_pub, &command_ack_pub, &command_ack, &_roi, &roi_pub, &status_changed)) {
                status_changed = true;
            }
        }

        /* Check for failure combinations which lead to flight termination */
        if (armed.armed &&
            !status_flags.circuit_breaker_flight_termination_disabled) {
            /* At this point the data link and the gps system have been checked
             * If we are not in a manual (RC stick controlled mode)
             * and both failed we want to terminate the flight */
            if (internal_state.main_state != commander_state_s::MAIN_STATE_MANUAL &&
                internal_state.main_state != commander_state_s::MAIN_STATE_ACRO &&
                internal_state.main_state != commander_state_s::MAIN_STATE_RATTITUDE &&
                internal_state.main_state != commander_state_s::MAIN_STATE_STAB &&
                internal_state.main_state != commander_state_s::MAIN_STATE_ALTCTL &&
                internal_state.main_state != commander_state_s::MAIN_STATE_POSCTL &&
                ((status.data_link_lost && status_flags.gps_failure) ||
                 (status_flags.data_link_lost_cmd && status_flags.gps_failure_cmd))) {
                armed.force_failsafe = true;
                status_changed = true;
                static bool flight_termination_printed = false;

                if (!flight_termination_printed) {
                    mavlink_log_critical(&mavlink_log_pub, "DL and GPS lost: flight termination");
                    flight_termination_printed = true;
                }

                if (counter % (1000000 / COMMANDER_MONITORING_INTERVAL) == 0) {
                    mavlink_log_critical(&mavlink_log_pub, "DL and GPS lost: flight termination");
                }
            }

            /* At this point the rc signal and the gps system have been checked
             * If we are in manual (controlled with RC):
             * if both failed we want to terminate the flight */
            if ((internal_state.main_state == commander_state_s::MAIN_STATE_ACRO ||
                 internal_state.main_state == commander_state_s::MAIN_STATE_RATTITUDE ||
                 internal_state.main_state == commander_state_s::MAIN_STATE_MANUAL ||
                 internal_state.main_state == commander_state_s::MAIN_STATE_STAB ||
                 internal_state.main_state == commander_state_s::MAIN_STATE_ALTCTL ||
                 internal_state.main_state == commander_state_s::MAIN_STATE_POSCTL) &&
                ((status.rc_signal_lost && status_flags.gps_failure) ||
                 (status_flags.rc_signal_lost_cmd && status_flags.gps_failure_cmd))) {
                armed.force_failsafe = true;
                status_changed = true;
                static bool flight_termination_printed = false;

                if (!flight_termination_printed) {
                    warnx("Flight termination because of RC signal loss and GPS failure");
                    flight_termination_printed = true;
                }

                if (counter % (1000000 / COMMANDER_MONITORING_INTERVAL) == 0) {
                    mavlink_log_critical(&mavlink_log_pub, "RC and GPS lost: flight termination");
                }
            }
        }

        /* Get current timestamp */
        const hrt_abstime now = hrt_absolute_time();

        /* First time home position update - but only if disarmed */
        if (!status_flags.condition_home_position_valid && !armed.armed) {
            commander_set_home_position(home_pub, _home, local_position, global_position, attitude);
        }

        /* update home position on arming if at least 500 ms from commander start spent to avoid setting home on in-air restart */
        else if (((!was_armed && armed.armed) || (was_landed && !land_detector.landed)) &&
            (now > commander_boot_timestamp + INAIR_RESTART_HOLDOFF_INTERVAL)) {
            commander_set_home_position(home_pub, _home, local_position, global_position, attitude);

        }

        was_armed = armed.armed;

        /* print new state */
        if (arming_state_changed) {
            status_changed = true;
            arming_state_changed = false;
        }

        /* now set navigation state according to failsafe and main state */
        bool nav_state_changed = set_nav_state(&status,
                                               &armed,
                                               &internal_state,
                                               &mavlink_log_pub,
                                               (link_loss_actions_t)datalink_loss_act,
                                               _mission_result.finished,
                                               _mission_result.stay_in_failsafe,
                                               &status_flags,
                                               land_detector.landed,
                                               (link_loss_actions_t)rc_loss_act,
                                               offboard_loss_act,
                                               offboard_loss_rc_act,
                                               posctl_nav_loss_act);

        if (status.failsafe != failsafe_old)
        {
            status_changed = true;

            if (status.failsafe) {
                mavlink_log_info(&mavlink_log_pub, "Failsafe mode enabled");

            } else {
                mavlink_log_info(&mavlink_log_pub, "Failsafe mode disabled");
            }

            failsafe_old = status.failsafe;
        }

        // TODO handle mode changes by commands
        if (main_state_changed || nav_state_changed) {
            status_changed = true;
            main_state_changed = false;
        }

        /* publish states (armed, control mode, vehicle status) at least with 5 Hz */
        if (counter % (200000 / COMMANDER_MONITORING_INTERVAL) == 0 || status_changed) {
            set_control_mode();
            control_mode.timestamp = now;
            orb_publish(ORB_ID(vehicle_control_mode), control_mode_pub, &control_mode);

            status.timestamp = now;
            orb_publish(ORB_ID(vehicle_status), status_pub, &status);

            armed.timestamp = now;

            /* set prearmed state if safety is off, or safety is not present and 5 seconds passed */
            if (safety.safety_switch_available) {

                /* safety is off, go into prearmed */
                armed.prearmed = safety.safety_off;
            } else {
                /* safety is not present, go into prearmed
                 * (all output drivers should be started / unlocked last in the boot process
                 * when the rest of the system is fully initialized)
                 */
                armed.prearmed = (hrt_elapsed_time(&commander_boot_timestamp) > 5 * 1000 * 1000);
            }
            orb_publish(ORB_ID(actuator_armed), armed_pub, &armed);
        }

        /* play arming and battery warning tunes */
        if (!arm_tune_played && armed.armed && (!safety.safety_switch_available || (safety.safety_switch_available
                            && safety.safety_off))) {
            /* play tune when armed */
            set_tune(TONE_ARMING_WARNING_TUNE);
            arm_tune_played = true;

        } else if (!status_flags.usb_connected &&
               (status.hil_state != vehicle_status_s::HIL_STATE_ON) &&
               (battery.warning == battery_status_s::BATTERY_WARNING_CRITICAL)) {
            /* play tune on battery critical */
            set_tune(TONE_BATTERY_WARNING_FAST_TUNE);

        } else if ((status.hil_state != vehicle_status_s::HIL_STATE_ON) &&
               (battery.warning == battery_status_s::BATTERY_WARNING_LOW)) {
            /* play tune on battery warning */
            set_tune(TONE_BATTERY_WARNING_SLOW_TUNE);

        } else if (status.failsafe) {
            tune_failsafe(true);
        } else {
            set_tune(TONE_STOP_TUNE);
        }

        /* reset arm_tune_played when disarmed */
        if (!armed.armed || (safety.safety_switch_available && !safety.safety_off)) {

            //Notify the user that it is safe to approach the vehicle
            if (arm_tune_played) {
                tune_neutral(true);
            }

            arm_tune_played = false;
        }

        /* play sensor failure tunes if we already waited for hotplug sensors to come up and failed */
        hotplug_timeout = hrt_elapsed_time(&commander_boot_timestamp) > HOTPLUG_SENS_TIMEOUT;

        if (!sensor_fail_tune_played && (!status_flags.condition_system_sensors_initialized && hotplug_timeout)) {
            set_tune_override(TONE_GPS_WARNING_TUNE);
            sensor_fail_tune_played = true;
            status_changed = true;
        }

        /* update timeout flag */
        if(!(hotplug_timeout == status_flags.condition_system_hotplug_timeout)) {
            status_flags.condition_system_hotplug_timeout = hotplug_timeout;
            status_changed = true;
        }

        counter++;

        int blink_state = blink_msg_state();

        if (blink_state > 0) {
            /* blinking LED message, don't touch LEDs */
            if (blink_state == 2) {
                /* blinking LED message completed, restore normal state */
                control_status_leds(&status, &armed, true, &battery, &cpuload);
            }

        } else {
            /* normal state */
            control_status_leds(&status, &armed, status_changed, &battery, &cpuload);
        }

        status_changed = false;

        if (!armed.armed) {
            /* Reset the flag if disarmed. */
            have_taken_off_since_arming = false;
        }

        /* publish vehicle_status_flags */
        publish_status_flags(vehicle_status_flags_pub);

        /* publish internal state for logging purposes */
        if (commander_state_pub != nullptr) {
            orb_publish(ORB_ID(commander_state), commander_state_pub, &internal_state);

        } else {
            commander_state_pub = orb_advertise(ORB_ID(commander_state), &internal_state);
        }

        usleep(COMMANDER_MONITORING_INTERVAL);
    }

    /* wait for threads to complete */
    ret = pthread_join(commander_low_prio_thread, nullptr);

    if (ret) {
        warn("join failed: %d", ret);
    }

    rgbled_set_color_and_mode(led_control_s::COLOR_WHITE, led_control_s::MODE_OFF);

    /* close fds */
    led_deinit();
    buzzer_deinit();
    px4_close(sp_man_sub);
    px4_close(offboard_control_mode_sub);
    px4_close(local_position_sub);
    px4_close(global_position_sub);
    px4_close(gps_sub);
    px4_close(sensor_sub);
    px4_close(safety_sub);
    px4_close(cmd_sub);
    px4_close(subsys_sub);
    px4_close(diff_pres_sub);
    px4_close(param_changed_sub);
    px4_close(battery_sub);
    px4_close(land_detector_sub);

    thread_running = false;

    return 0;
}


```
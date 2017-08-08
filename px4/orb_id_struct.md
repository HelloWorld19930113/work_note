# `PX4`中阅读源码时涉及到的比较重要的一些`ORB_ID()`结构体定义
1. ORB_ID(parameter_update)
struct parameter_update_s {
	uint64_t timestamp; // required for logger
	bool saved;
	uint8_t _padding0[7]; // required for logger
};


2. ORB_ID(manual_control_setpoint)
struct manual_control_setpoint_s {
	uint64_t timestamp; // required for logger
	float x;
	float y;
	float z;
	float r;
	float flaps;
	float aux1;
	float aux2;
	float aux3;
	float aux4;
	float aux5;
	uint8_t mode_switch;
	uint8_t return_switch;
	uint8_t rattitude_switch;
	uint8_t posctl_switch;
	uint8_t loiter_switch;
	uint8_t acro_switch;
	uint8_t offboard_switch;
	uint8_t kill_switch;
	uint8_t arm_switch;
	uint8_t transition_switch;
	uint8_t gear_switch;
	int8_t mode_slot;
	uint8_t _padding0[4]; // required for logger

#ifdef __cplusplus
	static const uint8_t SWITCH_POS_NONE = 0;
	static const uint8_t SWITCH_POS_ON = 1;
	static const uint8_t SWITCH_POS_MIDDLE = 2;
	static const uint8_t SWITCH_POS_OFF = 3;
	static const int8_t MODE_SLOT_NONE = -1;
	static const int8_t MODE_SLOT_1 = 0;
	static const int8_t MODE_SLOT_2 = 1;
	static const int8_t MODE_SLOT_3 = 2;
	static const int8_t MODE_SLOT_4 = 3;
	static const int8_t MODE_SLOT_5 = 4;
	static const int8_t MODE_SLOT_6 = 5;
	static const int8_t MODE_SLOT_MAX = 6;

#endif
};


3. ORB_ID(offboard_control_mode)
struct offboard_control_mode_s {

        uint64_t timestamp; // required for logger
        bool ignore_thrust;
        bool ignore_attitude;
        bool ignore_bodyrate;
        bool ignore_position;
        bool ignore_velocity;
        bool ignore_acceleration_force;
        bool ignore_alt_hold;
        uint8_t _padding0[1]; // required for logger
};

4. ORB_ID(sensor_combined)
struct sensor_combined_s {

	uint64_t timestamp; // required for logger
	float gyro_rad[3];
	float gyro_integral_dt;
	int32_t accelerometer_timestamp_relative;
	float accelerometer_m_s2[3];
	float accelerometer_integral_dt;
	int32_t magnetometer_timestamp_relative;
	float magnetometer_ga[3];
	int32_t baro_timestamp_relative;
	float baro_alt_meter;
	float baro_temp_celcius;

#ifdef __cplusplus
	static const int32_t RELATIVE_TIMESTAMP_INVALID = 2147483647;

#endif
};






















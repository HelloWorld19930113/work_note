
```cpp
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
```
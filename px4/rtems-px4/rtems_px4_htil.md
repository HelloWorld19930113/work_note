# 在`rtems_px4`中启动`HIL模块`

 参考`PX4`中的启动脚本文件，依次设置`HIL`依赖的关键参数，启动`HIL`必须的模块。

## 分析`PX4`中脚本的调用流程
 在`nsh_script.c`文件中执行`nsh_script(vtbl, "init", NSH_INITPATH)`函数，该函数的`NSH_INITPATH`为`init.d/rcS`脚本文件。这个脚本文件是启动的核心，因此有必要对其中的模块详细分析一下。

这个脚本文件中需要注意的点有以下几个。

1. UART 在 `FMUv1/2/3/4` 的映射关系如下:
```bash
# UART1         /dev/ttyS0      IO debug
# USART2        /dev/ttyS1      TELEM1 (flow control)
# USART3        /dev/ttyS2      TELEM2 (flow control)
# UART4
# UART7                         CONSOLE
# UART8                         SERIAL4
```
2. 启动脚本文件是可以以`/etc/rc.txt`定制的，否则就使用默认启动脚本`/init.d/rcS`文件。
3. 在启动脚本执行的最后一定要使用`mavlink boot_complete`来通知`MAVLink`程序系统已经启动完毕。
4. 整个自动启动脚本的流程：
```bash
# 首先启动uorb
uorb start     

# 加载参数
set PARAM_FILE /fs/microsd/params   
param select $PARAM_FILE
   if param load

# 启动系统状态LED指示器
if rgbled start
then
else
    if blinkm start
    then
        blinkm systemstate
    fi
fi

# 设置AUTOSTART启动脚本中使用的AUTOCNF标志
if param compare SYS_AUTOCONFIG 1
then
    # Wipe out params except RC* and total flight time
    param reset_nostart RC* LND_FLIGHT_T_*
    set AUTOCNF yes
else
    set AUTOCNF no

    if param compare BAT_N_CELLS 0
    then
        param set BAT_N_CELLS 3
    fi
fi

# 设置默认参数
set HIL no
set VEHICLE_TYPE none  # 飞行器的机型
set MIXER none
set MIXER_AUX none
set OUTPUT_MODE none
set PWM_OUT none
set PWM_RATE p:PWM_RATE
set PWM_DISARMED p:PWM_DISARMED
set PWM_MIN p:PWM_MIN
set PWM_MAX p:PWM_MAX
set PWM_AUX_OUT none
set PWM_AUX_RATE none
set PWM_ACHDIS none
set PWM_AUX_DISARMED p:PWM_AUX_DISARMED
set PWM_AUX_MIN p:PWM_AUX_MIN
set PWM_AUX_MAX p:PWM_AUX_MAX
set FAILSAFE_AUX none
set MK_MODE none
set FMU_MODE pwm
set AUX_MODE pwm
set FMU_ARGS ""
set MAVLINK_F default
set MAVLINK_COMPANION_DEVICE /dev/ttyS2
set EXIT_ON_END no
set MAV_TYPE none
set FAILSAFE none
set USE_IO yes
set LOGGER_BUF 16

# 设置 USE_IO 标志位
if param compare SYS_USE_IO 1 # SYS_USE_IO 为0表示fmu不使用IO板，默认值为0。
then
    if ver hwcmp MINDPX_V2
    then
        set USE_IO no
    fi
    else
    set USE_IO no
fi

# 设置AUTOSTART对应的参数和环境变量
if param compare SYS_AUTOSTART 0
then
    ekf2 start
else
    sh /etc/init.d/rc.autostart
fi
unset MODE

# 如果使能了吊舱控制并且输出模式是AUX，那么就需要设置aux mixer(覆盖飞行器相关的MIXER_AUX设置，该设置在特定的启动脚本中实现)
if param compare MNT_MODE_IN -1
then
else
    if param compare MNT_MODE_OUT 0
    then
        set MIXER_AUX mount
    fi
fi
unset AUTOCNF

set IO_PRESENT no


# 设置默认输出模式
if [ $OUTPUT_MODE == none ]
then
    if [ $USE_IO == yes ]
    then
        set OUTPUT_MODE io
    else
        set OUTPUT_MODE fmu   # 一般使用的是这个，而在HIL中使用的是hil
    fi
fi

# 在HIL启动脚本中会将这个变量设置为yes
if [ $HIL == yes ]
then
    set OUTPUT_MODE hil
    if ver hwcmp PX4FMU_V1
    then
        set FMU_MODE serial
    fi
else
    gps start      # 启动GPS
fi

# 设置DATAMAN_OPT
set DATAMAN_OPT ""
if dataman start $DATAMAN_OPT
then
fi
unset DATAMAN_OPT

# 启动传感器系统(在commander之前启动，这样飞行前检查就可以正常运行)
if [ $HIL == yes ]
then
    sensors start -h
else
    sh /etc/init.d/rc.sensors
fi
unset HIL

# 启动 commander
if [ $OUTPUT_MODE == hil ]
then
    commander start -hil
else
    commander start
fi

# PWM 接口bank上的传感器， SENS_EN_LL40LS为1说明使用的是PWM接口。
if param compare SENS_EN_LL40LS 1
then
    # clear pins 5 and 6
    set FMU_MODE pwm4
    set AUX_MODE pwm4
fi

# 设置OUTPUT_MODE相关的参数
if [ $OUTPUT_MODE != none ]
then
    # 设置HTL模式下的pwm输出模块
    if [ $OUTPUT_MODE == hil ]
    then
        if pwm_out_sim mode_pwm16
        then
        else
            tone_alarm $TUNE_ERR
        fi
    fi

    # Start IO or FMU for RC PPM input if needed
    if [ $IO_PRESENT == no ]
    then
        if [ $OUTPUT_MODE != fmu -a $OUTPUT_MODE != ardrone ]
        then
            if fmu mode_${FMU_MODE} $FMU_ARGS
            then
            else
                echo "FMU mode_${FMU_MODE} start failed" >> $LOG_FILE
                tone_alarm $TUNE_ERR
            fi
        fi
    fi

fi


# 设置并启动MAVLink模块，MAVLink启动是需要一定的参数的。

if [ $MAVLINK_F == default ]
then
    # Normal mode, use baudrate 57600 (default) and data rate 1000 bytes/s
    set MAVLINK_F "-r 1200 -f"
fi

if [ "x$MAVLINK_F" == xnone ]
then
else
    mavlink start ${MAVLINK_F}
fi
unset MAVLINK_F

# 设置外部模式下的控制串口属性
if param compare SYS_COMPANION 57600
then
    mavlink start -d ${MAVLINK_COMPANION_DEVICE} -b 57600 -m onboard -r 5000 -x -f
unset MAVLINK_COMPANION_DEVICE

# 根据 UAVCAN_ENABLE 的值启动关于uavcan， UAVCAN_ENABLE的默认值为0 
if param greater UAVCAN_ENABLE 0
then
    if uavcan start
    then
        set LOGGER_BUF 6
        uavcan start fw
    else
        tone_alarm ${TUNE_ERR}
    fi
fi

# px4fmu_v2 中的硬件属性
if ver hwcmp PX4FMU_V2
then
    # Check for flow sensor - as it is a background task, launch it last
    px4flow start &
fi

# Logging日志系统， 默认使用sdlog2
if ver hwcmp PX4FMU_V1
then
    if sdlog2 start -r 30 -a -b 2 -t
    then
    fi
else  # PX4FMU_V2在这里, SYS_LOGGER 的默认值为1；SDLOG_MODE的默认值为0
    if param compare SYS_LOGGER 0
    then
        # check if we should increase logging rate for ekf2 replay message logging
        if param greater EKF2_REC_RPL 0
        then
            if sdlog2 start -r 500 -e -b 18 -t
            then
            fi
        else
            if sdlog2 start -r 100 -a -b 9 -t
            then
            fi
        fi
    else
        set LOGGER_ARGS ""
        if logger start -b ${LOGGER_BUF} -t ${LOGGER_ARGS}
        then
        fi
        unset LOGGER_BUF
        unset LOGGER_ARGS
    fi
fi

# 固定翼相关参数设置
if [ $VEHICLE_TYPE == fw ]
then
    if [ $MIXER == none ]
    then
        # 设置固定翼的默认的 mixer
        set MIXER AERT
    fi

    if [ $MAV_TYPE == none ]
    then
        # 设置固定翼的机型 ID： MAV_TYPE = 1 
        set MAV_TYPE 1
    fi

    param set MAV_TYPE ${MAV_TYPE}

    # 加载 mixer 并配置输出
    sh /etc/init.d/rc.interface

    # 启动标准的固定翼 apps
    sh /etc/init.d/rc.fw_apps
fi

#
# 多旋翼相关参数设置
#
if [ $VEHICLE_TYPE == mc ]
then
    if [ $MIXER == none ]
    then
        echo "Mixer undefined"
    fi

    if [ $MAV_TYPE == none ]
    then
        # 根据 mixer 来判断当前飞行器的机型(vehicle type) 
        if [ $MIXER == quad_x -o $MIXER == quad_+ ]   # -o是or， -a是and；
        then
            set MAV_TYPE 2
        fi
        if [ $MIXER == quad_w -o $MIXER == quad_dc ]
        then
            set MAV_TYPE 2
        fi
        if [ $MIXER == quad_h ]
        then
            set MAV_TYPE 2
        fi
        if [ $MIXER == tri_y_yaw- -o $MIXER == tri_y_yaw+ ]
        then
            set MAV_TYPE 15
        fi
        if [ $MIXER == hexa_x -o $MIXER == hexa_+ ]
        then
            set MAV_TYPE 13
        fi
        if [ $MIXER == hexa_cox ]
        then
            set MAV_TYPE 13
        fi
        if [ $MIXER == octo_x -o $MIXER == octo_+ ]
        then
            set MAV_TYPE 14
        fi
        if [ $MIXER == octo_cox -o $MIXER == octo_cox_w ]
        then
            set MAV_TYPE 14
        fi
        if [ $MIXER == coax ]
        then
            set MAV_TYPE 3
        fi
    fi

    # 此时仍然未对 MAV_TYPE 赋值
    if [ $MAV_TYPE == none ]
    then
        echo "Unknown MAV_TYPE"
        param set MAV_TYPE 2  # 设置飞行器的机型为 MAV_TYPE = 2
    else
        param set MAV_TYPE ${MAV_TYPE}
    fi

    # 加载 mixer 并配置输出
    sh /etc/init.d/rc.interface

    # 启动标准的多旋翼 apps
    sh /etc/init.d/rc.mc_apps
fi

#
# 垂直起降机相关参数设置
#
if [ $VEHICLE_TYPE == vtol ]
then
    if [ $MIXER == none ]
    then
        echo "VTOL mixer undefined"
    fi

    if [ $MAV_TYPE == none ]
    then
        # Use mixer to detect vehicle type
        if [ $MIXER == caipirinha_vtol ]
        then
            set MAV_TYPE 19
        fi
        if [ $MIXER == firefly6 ]
        then
            set MAV_TYPE 21
        fi
        if [ $MIXER == quad_x_pusher_vtol ]
        then
            set MAV_TYPE 22
        fi
    fi

    # Still no MAV_TYPE found
    if [ $MAV_TYPE == none ]
    then
        echo "Unknown MAV_TYPE"
        param set MAV_TYPE 19
    else
        param set MAV_TYPE ${MAV_TYPE}
    fi

    # Load mixer and configure outputs
    sh /etc/init.d/rc.interface

    # Start standard vtol apps
    sh /etc/init.d/rc.vtol_apps
fi

#
#  无人车相关参数设置(UGV——Unmanned Ground Vehicles)
#

if [ $VEHICLE_TYPE == ugv ]
then
    if [ $MIXER == none ]
    then
        # Set default mixer for UGV if not defined
        set MIXER stampede
    fi

    if [ $MAV_TYPE == none ]
    then
        # Use MAV_TYPE = 10 (UGV) if not defined
        set MAV_TYPE 10
    fi

    param set MAV_TYPE ${MAV_TYPE}

    # Load mixer and configure outputs
    sh /etc/init.d/rc.interface

    # Start standard UGV apps
    sh /etc/init.d/rc.gnd_apps
fi

#
# 对于高通骁龙, we need a passthrough mode
# 不要运行任何 mavlink instances， 因为我们需要使用串口和Snapdragon通信
#
...

unset MIXER
unset MAV_TYPE
unset OUTPUT_MODE

# 启动 navigator 模块
navigator start

# 通用性启动 (未找到自动启动 ID)
if [ $VEHICLE_TYPE == none ]
then
    echo "No autostart ID found"
fi

# 启动用户自定义插件
set FEXTRAS /fs/microsd/etc/extras.txt
if [ -f $FEXTRAS ]
then
    echo "Addons script: ${FEXTRAS}"
    sh $FEXTRAS
fi
unset FEXTRAS

# 设置 log 文件保存路径 
if ver hwcmp CRAZYFLIE
then
    # CF2 shouldn't have an sd card
else

    if ver hwcmp AEROCORE2
    then
        # AEROCORE2 shouldn't have an sd card
    else

        # Run no SD alarm
        if [ $LOG_FILE == /dev/null ]
        then
            # Play SOS
            tone_alarm error
        fi
    fi
fi

#
# 温度热谱图校准
# TODO move further up and don't start unnecessary services if we are calibrating
#
set TEMP_CALIB_ARGS ""

# SYS_CAL_GYRO 默认值为 0
if param compare SYS_CAL_GYRO 1
then
    set TEMP_CALIB_ARGS "${TEMP_CALIB_ARGS} -g"
    param set SYS_CAL_GYRO 0
fi

# SYS_CAL_ACCEL 默认值为 0
if param compare SYS_CAL_ACCEL 1
then
    set TEMP_CALIB_ARGS "${TEMP_CALIB_ARGS} -a"
    param set SYS_CAL_ACCEL 0
fi

# SYS_CAL_BARO 默认值为 0
if param compare SYS_CAL_BARO 1
then
    set TEMP_CALIB_ARGS "${TEMP_CALIB_ARGS} -b"
    param set SYS_CAL_BARO 0
fi
if [ "x$TEMP_CALIB_ARGS" != "x" ]
then
    send_event temperature_calibration ${TEMP_CALIB_ARGS}
fi
unset TEMP_CALIB_ARGS

# vmount to control mounts such as gimbals, disabled by default.
if param compare MNT_MODE_IN -1
then
else
    if vmount start
    then
    fi
fi

# End of autostart
```
5. 最后的收尾工作
```bash
# 释放 RAM 空间和脚本中的变量.
unset TUNE_ERR

# 启动完成后通知 MAVLink app(s) 
mavlink boot_complete

if [ $EXIT_ON_END == yes ]
then
    echo "NSH exit"
    exit
fi
unset EXIT_ON_END
```


## rtems_px4启动脚本应该怎么实现
 这里需要找出rtems_px4中不同于px4中的启动脚本模块。

1. 在rtems_px4中`sensor`是集成到惯导模块中的，不需要单独启动，只需要启动`sbg`即可。
2. offboard外部模式中使用的辅助控制串口设备号的设置：MAVLINK_COMPANION_DEVICE


参考启动脚本： `init.d/rcS`



## HIL中必要的一些参数如何设置



## `mixer`脚本文件如何输入？



## 其他
 在通过修改QGC源码去除自动连接串口设备之后，使用以下命令：
 ```bash
 java -Djava.ext.dirs= -cp lib/*:out/production/jmavsim.jar me.drton.jmavsim.Simulator -serial /dev/ttyUSB0 115200 -qgc
 ```
 启动`jMAVSim`虚拟机之后，出现了“Init MAVLink”消息。但是`QGC`连接`jMAVSim`虚拟机时出现的现象是：连接上几秒之后会立即断开，间隔很长时候之后会再次出现这种现象。

> 解决思路：
了解`QGC`使用`UDP连接`后的数据链路是否正常。这里当然需要保证源数据获取通路是没有问题的。源数据链路是`PixHawk`和`jMAVSim`虚拟机之间的数据交换。

那么`QGC`中的
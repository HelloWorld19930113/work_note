# MavLink中的四元数和欧拉角转换函数

## 1. 欧拉角转四元数

```cpp
mavlink_euler_to_quaternion(float roll, float pitch, float yaw, 
        float quaternion[4]);
```

## 2. 四元数转欧拉角

```cpp
 mavlink_quaternion_to_euler(set_attitude_target.q,
   &_att_sp.roll_body, &_att_sp.pitch_body, &_att_sp.yaw_body);


# PX4_up_down_API实现

- 1. 设置目标姿态
**`MAVLINK_MSG_ID_SET_ATTITUDE_TARGET 82`**

```cpp
static inline void
mavlink_msg_set_attitude_target_send(
	mavlink_channel_t chan, 
	uint32_t time_boot_ms, 
	uint8_t target_system, 
	uint8_t target_component, 
	uint8_t type_mask, 
	const float *q, 
	float body_roll_rate, 
	float body_pitch_rate, 
	float body_yaw_rate, 
	float thrust);
```

- 2. 设置目标位置
**`MAVLINK_MSG_ID_SET_POSITION_TARGET_LOCAL_NED 84`**

```cpp
static inline void
mavlink_msg_set_position_target_local_ned_send(
	mavlink_channel_t chan, 
	uint32_t time_boot_ms, 
	uint8_t target_system, 
	uint8_t target_component, 
	uint8_t coordinate_frame, 
	uint16_t type_mask, 
	float x, float y, float z, 
	float vx, float vy, float vz, 
	float afx, float afy, float afz, 
	float yaw, 
	float yaw_rate);
```

 不管是使用姿态还是位置设置点，都必须弄清楚其中的各个形参代表的含义，这样在实现时才可以指定正确的姿态值和位置值。

## 姿态控制-->位置控制

- 1. 姿态控制

Px4的姿态控制，是根据SO3数学基础，推导旋转矩阵模型，采用串级PID控制，控制角度环和角速度环。实际上将姿态控制问题转换成平滑轨迹跟踪问题。

![](http://s2.sinaimg.cn/large/002DnvJqgy70LTgthRf21&690)

 以下是在实际控制过程中实现的`tilt-torsion`模式。
 
![](http://s8.sinaimg.cn/large/002DnvJqgy70LTpSR7x67&690)


###算法代码详解：
```cpp
MulticopterAttitudeControl::control_attitude(float dt)
{
	//1.构建姿态设置旋转矩阵R_sp和当前姿态旋转矩阵R；
	math::Matrix<3, 3> R_sp = q_sp.to_dcm();
	math::Matrix<3, 3> R = q_att.to_dcm();
	
	//2.取出两个矩阵的Z轴向量，
	math::Vector<3> R_z(R(0, 2), R(1, 2), R(2, 2));
	math::Vector<3> R_sp_z(R_sp(0, 2), R_sp(1, 2), R_sp(2, 2));
	//根据这两个z轴计算出误差向量（参考坐标系）,并转换到机体坐标系(b系)；
	math::Vector<3> e_R = R.transposed() * (R_z % R_sp_z);
	//注：%已经符号重载过了，并不是求余，这里指的是向量叉积运算(cross product)；后面的*，表示点积(dot product)。
	
	//3.计算两个z轴的误差角度；
	//由公式a×b=︱a︱︱b︱sinθ，a•b=︱a︱︱b︱cosθ，这里的R_z和R_sp_z又是单位向量，模为1，因此误差向量e_R的模就是sinθ，点积就是cosθ。
	float e_R_z_sin = e_R.length();
	float e_R_z_cos = R_z * R_sp_z;
	
	//4.计算yaw误差角度的权重；
	yaw_w = R_sp(2, 2) * R_sp(2, 2);
	
	//5.因为多轴的yaw响应一般比roll/pitch慢一倍，因此将两者解耦，先补偿roll/pitch的变化，计算R_rp；
	//判定两个z轴是否存在误差
	if (e_R_z_sin > 0.0f)
	//将e_R转成单位向量；
	e_R = e_R_z_axis * e_R_z_angle;
	
	//计算e_R_z_axis 的叉积矩阵，根据欧拉旋转公式，求出R_rp；
	//注：这里的R_rp是roll/pitch方向旋转之后的旋转矩阵；
	R_rp = R * (_I + e_R_cp * e_R_z_sin + e_R_cp * e_R_cp * (1.0f - e_R_z_cos));
	
	//6.现在R_rp和R_sp的z轴已经重合了，只需求yaw的误差角度；
	//取出两个矩阵的x轴（现在只有x轴存在误差？）；
	math::Vector<3> R_sp_x(R_sp(0, 0), R_sp(1, 0), R_sp(2, 0));
	math::Vector<3> R_rp_x(R_rp(0, 0), R_rp(1, 0), R_rp(2, 0));
	
	//同样根据向量的叉积和点积求出误差角度的正弦和余弦，再反正切求出角度；
	e_R(2) = atan2f((R_rp_x % R_sp_x) * R_sp_z, R_rp_x * R_sp_x) * yaw_w;
	
	//7.以上旋转方法，适用于小角度的误差，当转动的角度偏大时，还需另一种方法；
	//直接计算参考系到机体系的旋转矩阵，并转换成四元数形式；
	//同样得到一个误差角度向量；
	q_error.from_dcm(R.transposed() * R_sp);
	math::Vector<3> e_R_d = q_error(0) >= 0.0f ? q_error.imag() * 2.0f : -q_error.imag() * 2.0f;
	
	//求出互补系数
	float direct_w = e_R_z_cos * e_R_z_cos * yaw_w;
	//更新e_R，融合两种旋转(绕z轴旋转和直接旋转)，实现互补；
	//融合绕z轴旋转和直接旋转
	e_R = e_R * (1.0f - direct_w) + e_R_d * direct_w;

	//8.求出角速度的期望值，供内环角速度控制使用；
	_rates_sp = _params.att_p.emult(e_R);
	
	//9.yaw响应较慢，因此再加入一个前馈控制；
	_rates_sp(2) += _v_att_sp.yaw_sp_move_rate * yaw_w * _params.yaw_ff;
}
```
 注：参考系到机体系的旋转矩阵的四元数表示形式。
![](http://s5.sinaimg.cn/large/002DnvJqgy70LSwKe0se4&690)


###**参考资料：**
上述用到的相关理论，读者自己查阅以下书籍。
>pixhawk源码 mc_att_control；
>
>High performance full attitude control of a quadrotor on SO(3)；
>
>捷联惯性导航；

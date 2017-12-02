# coding=utf-8

# 代码功能：
#   线性回归
#   y = 0.1*x + 0.3，显示随机生成的这些点
#
#   梯度下降法起作用的现象是每次迭代都会使误差降低。
#   preliminary screen 初步筛选
#   coherent 连贯的
#



import numpy as np
import matplotlib.pyplot as plt
import tensorflow as tf


num_points = 1000

vector_set = []
for i in range(num_points):
    x1 = np.random.normal(0.0, 0.55)
    y1 = x1*0.1 + 0.3 + np.random.normal(0.0, 0.03)  # 符合高斯分布
    vector_set.append([x1, y1])

x_data = [v[0] for v in vector_set]
y_data = [v[1] for v in vector_set]

# plt.plot(x_data, y_data, 'ro', label='original data')
# plt.legend()
# plt.show()

# tensor_flow implement
W = tf.Variable(tf.random_uniform([1], -1.0, 1.0))
b = tf.Variable(tf.zeros([1]))
y = W*x_data + b

loss = tf.reduce_mean(tf.square(y - y_data))
optimizer = tf.train.GradientDescentOptimizer(0.5)
train = optimizer.minimize(loss)
# 初始化变量
init = tf.global_variables_initializer()
sess = tf.Session()
sess.run(init)
# 训练
for step in range(8):
    sess.run(train)
    # print(step,sess.run(W), sess.run(b))
    print(step,sess.run(loss))

# 绘制结果
plt.plot(x_data, y_data, 'ro', label='result')
plt.plot(x_data, sess.run(W)*x_data+sess.run(b))
plt.legend()
plt.show()

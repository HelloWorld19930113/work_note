# coding=utf-8

#  k-means 分类
import numpy as np


num_points = 2000
vectors_set = []
for i in range(num_points):
    if np.random.random() > 0.5:
        vectors_set.append([np.random.normal(0.0, 0.9), np.random.normal(0.0, 0.9)])
    else:
        vectors_set.append([np.random.normal(3.0, 0.5), np.random.normal(1.0, 0.5)])



import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

# df = pd.DataFrame({"x": [v[0] for v in vectors_set],
#                    "y": [v[1] for v in vectors_set],
#                    })
# sns.lmplot("x","y",data=df, fit_reg=False, size=6)
# plt.show()


import tensorflow as tf
vectors = tf.constant(vectors_set)
k = 4
# 从vector中选取(4,2)维度的数据，size[i]=-1,表示截取该维度的全部数据
centroides = tf.Variable(tf.slice(tf.random_shuffle(vectors),[0,0], [k,-1]))

print vectors.get_shape()
print centroides.get_shape()

# 扩展维度为了使两个tensor作subtract
# shape为1代表tensor在该维度上并未分配大小
# tensorFlow具有广播特性，所以tf.subtract会自己找到两个tensor的减法方式
# 它会将不确定的那个维度补全为可以作减法的形式
expanded_vectors = tf.expand_dims(vectors, 0)
expanded_centroides = tf.expand_dims(centroides, 1)
print expanded_vectors.get_shape()
print expanded_centroides.get_shape()

# reduce_sum是以计算和的形式降维，最后保留第2维度
diff = tf.subtract(expanded_vectors, expanded_centroides)
sqr = tf.square(diff)
distances = tf.reduce_sum(sqr, 2)
assignments = tf.argmin(distances, 0)

# tf.concat的两个参数位置换了

means = tf.concat([
    tf.reduce_mean(
        tf.gather(vectors,
                tf.reshape(
                  tf.where(
                    tf.equal(assignments, c)
                  ), [1, -1])
               ),reduction_indices=[1])
    for c in xrange(k)], 0)


update_centroides = tf.assign(centroides, means)

init_op = tf.global_variables_initializer()

sess = tf.Session()
sess.run(init_op)

for step in range(10):
    _,centroid_vals,assignment_values = sess.run([update_centroides, centroides, assignments])


data = {"x": [], "y": [], "cluster": []}
for i in xrange(len(assignment_values)):
  data["x"].append(vectors_set[i][0])
  data["y"].append(vectors_set[i][1])
  data["cluster"].append(assignment_values[i])

df = pd.DataFrame(data)
df = pd.DataFrame(data)
sns.lmplot("x", "y", data=df,
           fit_reg=False, size=7,
           hue="cluster", legend=False)
plt.show()
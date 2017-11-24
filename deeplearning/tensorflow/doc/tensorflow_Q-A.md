# tensorflow 使用中出现的问题汇总

1. tf.global_variables_initializer的错误 
```bash
WARNING:tensorflow:: initialize_all_variables (from tensorflow.python.ops.variables) is deprecated and will be removed after 2017-03-02. 
Instructions for updating: 
Use tf.global_variables_initializer instead. 
initialize_all_variables已被弃用，将在2017-03-02之后删除。 
```
说明更新：使用`tf.global_variables_initializer()`代替。 
按照提示把`tf.initialize_all_variables()`改为`global_variables_initializer()`就可以了


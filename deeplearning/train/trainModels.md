# 如何更好的训练网络

1. 如何在训练中禁用`Test`    
 在`examples/ssd/ssd_xxx.py`文件中作如下修改：   

- 1.1 注释掉`test_`开头的变量；     
```python
solver_param = {
    # Train parameters
    'base_lr': base_lr,
    'weight_decay': 0.0005,
    'lr_policy': "multistep",
    'stepvalue': [50000, 100000, 180000],
    'gamma': 0.1,
    'momentum': 0.9,
    'iter_size': iter_size,
    'max_iter': 200000,
    'snapshot': 10000,
    'display': 10,
    'average_loss': 10,
    'type': "SGD",
    'solver_mode': solver_mode,
    'device_id': device_id,
    'debug_info': False,
    'snapshot_after_train': True,

    # Test parameters
    #'test_iter': [test_iter],
    #'test_interval': 10000,
    'eval_type': "detection",
    'ap_version': "11point",
    #'test_initialization': False,
}
```
 其中，`test_initialization`表示是否可以用上次保存的`snapshot`来继续训练，如果为`True`,则下次开始训练的时候，`caffe`会自动从这个目录下加载最近一次迭代的模型，继续训练，以节省时间。如果是`False`，则会从`0`开始重新迭代。所以这个参数的默认值是`False`。     
因此在第一次训练的时候，不能将这个参数改为了`True`。     

- 1.2 注释掉`test_net`；      
```python
solver = caffe_pb2.SolverParameter(
            train_net=train_net_file,
            #test_net=[test_net_file],
            snapshot_prefix=snapshot_prefix,
            **solver_param
        )
```
- 1.3 重新运行`python examples/ssd/ssd_xxx.py`即可。      


2. 如何保持训练的`log`输出不中断?
```
nohup python examples/ssd/ssd_weimeng.py >weimeng.log 2>&1 &
```
要想动态观测log的内容，使用下面的命令：
```
tail -f weimeng.log
```
其中 `-f` 的作用是显示追加到文件中的内容。
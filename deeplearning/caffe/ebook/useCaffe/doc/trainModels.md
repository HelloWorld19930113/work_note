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
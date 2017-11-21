# 
1. 查看操作系统版本的命令

1.1 命令lsb_release -a

lsb_release提供特定的LSB (Linux Standard Base)和发布版本相关信息。
ubuntu中运行lsb_release -a的结果如下：
```
```

1.2 命令cat /etc/issue

/etc目录下放的是系统的配置文件。
/etc/issue文件通常包括系统的一段短说明或欢迎信息.内容由系统管理员确定。


2. 查看内核版本的命令

2.1 命令uname -a


2.2 命令cat /proc/version

proc文件系统是一个虚拟文件系统，通过它可以使用一种新的方法在linux内核空间和用户空间进行通信。
与普通文件不同的是，这些虚拟文件都是动态创建的。

/proc非常强大，如下所示为对/proc中部分元素执行一次交互查询的结果：
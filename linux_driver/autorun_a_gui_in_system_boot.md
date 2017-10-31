# Linux系统启动时直接运行一个界面程序

![](http://img.blog.csdn.net/20160419165751295)

禁止开机启动的服务;
- 1. 修改`cd /etc/init.d/rcS.d`目录下以`S`开头的文件为`K`开头。
- 2. 将需要启动的程序加入到`etc/rc.local`脚本中。

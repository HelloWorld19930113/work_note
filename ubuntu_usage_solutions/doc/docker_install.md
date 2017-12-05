# 在Ubuntu 14.04安装和使用Docker

`Docker`是一个开源软件，它可以把一个`Linux应用`和它所依赖的一切（比如配置文件）都封装到一个容器。然而，`Docker`与虚拟机不同，它使用了沙箱机制，`Docker容器`不运行操作系统，它共享主机上的操作系统。`Docker`使得在同一个服务器上可以运行更多的应用程序——它通过提供了一个额外的抽象层和操作系统级虚拟自动化。`Docker`使用`Go语言`开发，并以`Apache 2.0`许可证协议发布。    

1. 在`Linux上`安装`Docker`      
`Docker`的 安装资源文件 存放在`Amazon S3`，会间歇性连接失败。所以安装Docker的时候，会比较慢。 
你可以通过执行下面的命令，高速安装`Docker`。      
```
curl -sSL https://get.daocloud.io/docker | sh
```
适用于Ubuntu，Debian,Centos等大部分Linux，会3小时同步一次Docker官方资源.   
```
$ sudo service docker status
docker start/running, process 14394
```  
2. 卸载`Docker`    
```
sudo apt-get remove docker docker-engine
```
卸载`Docker`后,`/var/lib/docker/`目录下会保留原`Docker`的镜像,网络,存储卷等文件. 如果需要全新安装`Docker`,需要删除`/var/lib/docker/`目录     
```
rm -fr /var/lib/docker/
``` 
3. 配置`Docker` 加速器     
```
curl -sSL https://get.daocloud.io/daotools/set_mirror.sh | sh -s http://faf41e68.m.daocloud.io 
```
该脚本可以将`--registry-mirror`加入到你的`Docker`配置文件`/etc/docker/daemon.json`中。适用于`Ubuntu14.04、Debian、CentOS6 、CentOS7、Fedora、Arch Linux、openSUSE Leap 42.1`，其他版本可能有细微不同。      
4. 安装主机监控程序    
安装好`Docker`后，运行主机安装命令。   
```
curl -sSL https://get.daocloud.io/daomonit/install.sh | sh -s a7d911eb0064448e6524a34c6cc3fda1d84bb604 
sudo service daomonit start 
```
9. `Docker`组    
创建一个`docker`用户组，避免使用`root`用户，并让`docker`用户组的用户具有同`root`的权限。   
```
$ usermod -aG docker $USER
```
现在可以退出当前用户，使用`docker`用户登陆了。     
10. `Docker`测试     
执行以下命令，查看输出：   
```
$ docker run hello-world
```
快照中输出显示，`docker`用户工作正常。       

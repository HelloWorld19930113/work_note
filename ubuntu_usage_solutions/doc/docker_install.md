# 在Ubuntu 14.04安装和使用Docker

`Docker`是一个开源软件，它可以把一个`Linux应用`和它所依赖的一切（比如配置文件）都封装到一个容器。然而，`Docker`与虚拟机不同，它使用了沙箱机制，`Docker容器`不运行操作系统，它共享主机上的操作系统。`Docker`使得在同一个服务器上可以运行更多的应用程序——它通过提供了一个额外的抽象层和操作系统级虚拟自动化。`Docker`使用`Go语言`开发，并以`Apache 2.0`许可证协议发布。    

1. `Docker`的需求    
要在`Ubuntu 14.04 x64`安装`Docker`，需要确保`Ubuntu`的版本是`64位`，而且内核版本需大于`3.10`版。可以使用如下命令来检查`Ubuntu`的内核版本:   
```
$ uname -r 
3.13.0-55-generic
```
更新系统，确保软件包列表的有效性:    
```
$ apt-get update
```
如果`Ubuntu`的版本不满足，还需升级`Ubuntu`:    
```
$ apt-get -y upgrade
```
2. 安装`Docker`   
一旦以上需求都满足了，就可以开始安装`Docker`。`Docker`最早只支持`Ubuntu`，后来有了`CentOS`和其它`RedHat`相关的发布包。安装很简单，执行命令：
```
$ sudo apt-get -y install docker.io
```
3. 创建软链接   
```
$ ln -sf /usr/bin/docker.io /usr/local/bin/docker
$ sed -i '$acomplete -F _docker docker' /etc/bash_completion.d/docker.io
```
4. 检查`Docker`服务   
要校验`Docker`服务的状态，执行以下命令，确保`Docker`服务是启动的。   
```
$ sudo service docker status
docker start/running, process 14394
```
要把`Docker`以守护进程的方式运行，执行以下命令：（注意需先关闭`Docker服务`）    
```
$ docker -d &
```
5. `Docker`自启动服务   
把`Docker`安装为自启动服务，让它随服务器的启动而自动运行，执行命令：   
```
$ update-rc.d docker defaults
```
6. `Docker`的用法    
下面讲述`Docker`的用法。要查看`Docker`可以使用的命令，在终端运行`docker`命令，它会打印所有可用的命令列表及使用描述。    
```
$ docker
Usage: docker [OPTIONS] COMMAND [arg...]

A self-sufficient runtime for linux containers.

Options:
  --api-cors-header=                   Set CORS headers in the remote API
  ...
  -v, --version=false                  Print version information and quit

Commands:
    attach    Attach to a running container
    ...
    wait      Block until a container stops, then print its exit code

Run 'docker COMMAND --help' for more information on a command.
```
7. `Docker`容器下载    
下面我们`docker`命令带`pull`选项，拉取一个镜像，即从Docker注册服务器的软件仓库下载一个Docker的镜像。
使用的命令如下：   
```
$ docker pull ubuntu
```
此命令的执行会花去一些时间。    
8. 运行`Docker`容器       
现在可以看到在`Bash Shell`下建立`Ubuntu`容器是非常简单的，只需运行一行命令即可。   
```
-i选项：让输入输出都在标准控制台进行
-t选项：分配一个tty
$ docker run -i -t ubuntu /bin/bash
root@696d5fd32bba:/$
```
故，在输出提示中，可以看到使用的标准`Ubuntu`容器。现在可以在`Ubuntu`的`Docker`容器中使用`Bash Shell`。如果希望停止/断开连接，可以使用组合键`Ctrl-p` + `Ctrl-q`，然后就会返回到早先的窗口。    
9. `Docker`组    
创建一个`docker`用户组，避免使用`root`用户，并让`docker`用户组的用户具有同`root`的权限。   
```
$ usermod -aG docker ubuntu
```
现在可以退出当前用户，使用`docker`用户登陆了。     
10. `Docker`测试     
执行以下命令，查看输出：   
```
docker@ubuntu-14:/root$ docker run hello-world
```
快照中输出显示，`docker`用户工作正常。      
11. `Docker`可用容器      
`Docker`可用的容器可以通过搜索命令查找，社区已经提供了很多可用的容器。如何寻找可用的`Docker`容器，使用以下命令搜索`CentOS`的`Docker`容器。   
```
$ docker search centos
```

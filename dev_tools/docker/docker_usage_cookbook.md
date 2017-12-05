#

1. 查看`docker`程序是否存在——`docker info`    
```
docker info

Containers: 0
	...
Images: 5
Server Version: 17.11.0-ce
Storage Driver: aufs
 Root Dir: /var/lib/docker/aufs
 Backing Filesystem: extfs
 Dirs: 24
 Dirperm1 Supported: true
Logging Driver: json-file
Plugins:
 Volume: local
 Network: bridge host macvlan null overlay
 Log: awslogs fluentd gcplogs gelf journald json-file logentries splunk syslog
...
Architecture: x86_64
CPUs: 4
Total Memory: 7.685GiB
Name: klm
...
Docker Root Dir: /var/lib/docker
Registry: https://index.docker.io/v1/
Insecure Registries:
 127.0.0.0/8
Registry Mirrors:
 http://faf41e68.m.daocloud.io/
```
2. 查看已经安装的`images`——`docker images`    
```
docker images
```
3. 搜索`Docker`可用容器——`docker search`      
`Docker`可用的容器可以通过搜索命令查找，社区已经提供了很多可用的容器。如何寻找可用的`Docker`容器，使用以下命令搜索`CentOS`的`Docker`容器。   
```
$ docker search centos
```
4. 运行`Docker`容器——`docker run`       
4.1 在`bash shell`下建立`ubuntu:14.04`容器    
非常简单，只需运行一行命令即可。  
- `-i`选项：让输入输出都在标准控制台进行
- `-t`选项：分配一个`tty`
```
$ docker run -i -t ubuntu:14.04 /bin/bash
root@696d5fd32bba:/$
```
在输出提示中，可以看到使用的标准`ubuntu:14.04`容器。现在可以在`ubuntu:14.04`的`Docker`容器中使用`bash`。如果希望停止/断开连接，可以使用组合键`Ctrl-D`，然后就会返回到之前的的窗口。    
4.2 创建一个以进程方式运行的容器    
```
$ docker run -d ubuntu:14.04 /bin/sh -c "while true; do echo hello world; sleep 1; done"
```





6. 删除旧的容器    
```
docker ps --filter "status=exited" | grep 'weeks ago' | awk '{print $1}' | xargs --no-run-if-empty docker rm
```
`weeks ago`可以是`minutes`、`hours`或`days`。      


## docker cookbook

|docker command|    说明|
|----|------|
|docker info|查看`docker`程序是否存在|    
|docker images|查看已经安装的`docker images`|    
|docker search xxx|查找社区已经提供的可用`container`|  
|docker logs|查看容器内的标准输出|     
|docker stop|停止容器|      
|docker ps|查看有哪些容器有在运行|     



[1. 部署第一个容器](http://guide.daocloud.io/dcs/3-9152643.html)   

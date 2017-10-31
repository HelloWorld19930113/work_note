# 配置`shadowsocks`

  如果`chrome`报错: `Your connection is not private NET::ERR_CERT_WEAK_SIGNATURE_ALGORITHM`                 
```bash
$ sudo apt-get install libnss3-1d 
``` 

## 安装   
`Linux`不同的发行版执行的命令如下：           
`Debian / Ubuntu`:         
```bash
$ sudo apt-get install python-pip
$ sudo pip install shadowsocks
```
 
`CentOS`:     
```   
yum install python-setuptools && easy_install pip
pip install shadowsocks   
```

## 配置
```
sudo vim /etc/shadowsocks.json     
# 这里的json文件是自己创建的，不是系统自带      
```

配置文件的内容大致如下：      
```
{
    "server":"服务器的ip",
    "server_port":19175,
    "local_address":"127.0.0.1",
    "local_port":1080,
    "password":"密码",
    "timeout":300,
    "method":"aes-256-cfb",
    "fast_open":false
}
```

## 启动ss客户端          
  前两步很简单，可是有人就纳闷了安装好了不知道怎么用，其实可以用`sslocal -help `来查看帮助就知道了        
```
sslocal -c /etc/shadowsocks.json
```
一条命令代理就可以启动了。       

## 设置ubuntu开机后自动运行ss    
   
现在可以科学上网了，可是每次开机都要手动打开终端输入一条命令，虽然这条命令并不长，但是每次都去手动输入，显得自己很low,而且关掉终端代理就关闭了。        

写个脚本        
        
我们可以在比如`/home`下新建个文件叫做`shadow.sh`            ，在里面写上我们启动ss客户端需要的命令，然后保存即可。            
```
#！/bin/bash
#shadow.sh
sslocal -c /etc/shadowsocks.json
```

看可不可以我们到终端执行命令 sh /home/shadow.sh，如果成功的话会有信息输出的。你也可以到浏览器去试试。这个时候你虽然输入的少了，可是关了终端还是会掉的，我们可以让他在后台运行，nohup sh /home/shadow.sh &。             

加入开机运行       

这里我们需要在/etc下编辑一个叫rc,local的文件，需要root权限，在终端先su获取root权限，如果你有root帐号的话，然后vim /etc/rc.local编辑，在exit之前输入nohup bash /home/shadow.sh>/home/d.txt & 保存。          

这个时候你可以reboot重启了，测试下看看能不能后台自动运行，重启你可以先去看下我们要他输出d.txt，你竟然发现是 /home/shadow.sh line 3 :sslocal: command not found,打开浏览器果然是无法链接代理服务器。          

经过一番搜索我们发现远离linux是找不到sslocal这条命令？需要添加路径，我们发现sslocal和ssserver这两个命令是被存在 /usr/local/bin下面的，其实不用去profile添加了，直接把这两个文件移动到/bin下，就可以了        

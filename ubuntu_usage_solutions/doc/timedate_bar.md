# ubuntu 14.04状态栏不显示时间   

有时候我们会看到我们电脑的状态栏那里并没有显示时间，一个原因是日期时间指示器没有工作，另一个可能的原因是用户禁用了时间显示。当你想显示它的 时候，你可能会发现`System Settings->Time & Date->Clock`这里已经变成灰色，不可以选择，那如何来解决它呢？   
首先我们用下面的命令来确认一下是否安装了日期时间指示器：   
```bash
$ sudo apt-get install indicator-datetime
```
如果确认已经安装，那么我们需要重新配置它：   
```bash
$ sudo dpkg-reconfigure --frontend noninteractive tzdata
```
然后重启Unity：   
```bash
$ sudo killall unity-panel-service
```
经过以上步骤之后，时间就会显示在状态栏了。    

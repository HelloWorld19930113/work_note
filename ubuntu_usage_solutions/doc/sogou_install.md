#  搜狗输入法安装/卸载

## 安装搜狗输入法

1. 添加`fcitx`的`PPA`    
```
$ sudo add-apt-repository ppa:fcitx-team/nightly
```
2. 更新软件源，安装一些依赖软件    
```
sudo apt-get update
```
3. 安装`im-config`                 
```
sudo apt-get install im-config
```
4. 安装搜狗输入法依赖的`fcitx`一系列文件，输入命令：            
```
sudo apt-get -f install
```
遇到有Y/N的地方直接输Y        

5. 下载并安装搜狗拼音输入法(.deb)
 搜狗官网下载搜狗拼音输入法，选择你系统对应的软件包，我系统是64位的，所以我选择了`amd64`。使用下面的命令开始安装输入法:    
```
sudo dpkg -i sogoupinyin_2.0.0.0068_amd64.deb
```

6. 安装完成之后使用如下命令启用          
```bash
$ sudo im-config -s fcitx -z default
```
完成之后别忘记重启系统。重启之后点击右上角的那个软件盘即可看到搜狗输入法安装成功，点击即可输入汉字了。      

## 卸载搜狗输入法
首先使用命令查看下安装的搜狗拼音输入法`sudo dpkg -l so*`，然后先卸载搜狗拼音`sudo apt-get purge sogoupinyin`

卸载fcitx，

```bash
$ sudo apt-get purge fcitx
```

彻底卸载fcitx及相关配置，

```bash
$ sudo apt-get autoremove
```

最后别忘注销或者重启系统，如果注销按钮不能使用，可以使用命令`sudo pkill Xorg`，当再次登录系统之后，可以看到搜狗输入法已经完全被卸载干净了。
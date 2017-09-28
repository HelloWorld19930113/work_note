#  搜狗输入法安装/卸载

## 安装搜狗输入法
参考了其他一些资料都说添加fcitx的PPA，但是我不确定有木有用

eric@eric-VirtualBox:/etc/apt$ sudo add-apt-repository ppa:fcitx-team/nightly
 Experimental releases of Fcitx, use with caution.
 More info: https://launchpad.net/~fcitx-team/+archive/ubuntu/nightly
Press [ENTER] to continue or ctrl-c to cancel adding it

gpg: keyring `/tmp/tmp_2dkioxm/secring.gpg' created
gpg: keyring `/tmp/tmp_2dkioxm/pubring.gpg' created
gpg: requesting key 7E5FA1EE from hkp server keyserver.ubuntu.com
gpg: /tmp/tmp_2dkioxm/trustdb.gpg: trustdb created
gpg: key 7E5FA1EE: public key "Launchpad PPA for Fcitx Team PPA" imported
gpg: Total number processed: 1
gpg:               imported: 1  (RSA: 1)
OK
然后刷新软件源，安装一些依赖软件


sudo apt-get update
刷新完成之后，安装im-config


sudo apt-get install im-config
继续安装搜狗输入法依赖的fcitx一系列文件，输入命令：sudo apt-get -f install，遇到有Y/N的地方直接输Y


eric@eric-VirtualBox:~/Desktop$ sudo apt-get -f install

到搜狗官网下载搜狗拼音输入法，选择你系统对应的软件包，我系统是64位的，所以我选择了amd64的
下载地址点我，另外搜狗也提供了安装说明帮助页面，点我查看帮助。
下载完成之后，可以直接将文件放在桌面上进行安装
先看看下载的deb文件

eric@eric-VirtualBox:~/Desktop$ ll
total 18304
drwxr-xr-x  2 eric eric     4096 11月 27 22:06 ./
drwxr-xr-x 18 eric eric     4096 11月 27 21:34 ../
-rw-rw-r--  1 eric eric      627 11月 27 22:06 cache.txt
-rwxr-x---  1 root root 18730988 11月 27 00:01 sogoupinyin_2.0.0.0068_amd64.deb*
eric@eric-VirtualBox:~/Desktop$
输入sudo dpkg -i sogoupinyin_2.0.0.0068_amd64.deb命令开始安装输入法，可使用tab键自动补全命令，安装完成之后使用如下命令启用:

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

最后别忘注销或者重启系统，如果注销按钮不能使用，可以使用命令sudo pkill Xorg，当再次登录系统之后，可以看到搜狗输入法已经完全被卸载干净了。
# sublime
## 1. sublime 中的设置无法保存
	表现为打开的工程在关闭sublime之后就没有了；
	注册好的sublime重新打开之后就恢复到未注册状态。

原因：

是因为在home目录下，关于sublime配置文件的权限有问题：
```bash
$ ls /home/ldd/.config
总用量 120
drwx------ 26 ldd  ldd  4096 7月  29 12:06 ./
drwxr-xr-x 24 ldd  ldd  4096 7月  29 11:41 ../
...
drwxr-xr-x  2 root root 4096 7月   8 18:45 gedit/
...
drwx------  7 root root 4096 7月  29 13:37 sublime-text-3/
```
解决：
```bash
$ sudo chown -hR ldd:ldd sublime-text-3 gedit
```

## 2.注册码 
2.
— — BEGIN LICENSE — —
Nicolas Hennion
Single User License
EA7E-866075
8A01AA83 1D668D24 4484AEBC 3B04512C
827B0DE5 69E9B07A A39ACCC0 F95F5410
729D5639 4C37CECB B2522FB3 8D37FDC1
72899363 BBA441AC A5F47F08 6CD3B3FE
CEFB3783 B2E1BA96 71AAF7B4 AFB61B1D
0CC513E7 52FF2333 9F726D2C CDE53B4A
810C0D4F E1F419A3 CDA0832B 8440565A
35BF00F6 4CA9F869 ED10E245 469C233E
—— END LICENSE ——

3.
— — BEGIN LICENSE — —
Anthony Sansone
Single User License
EA7E-878563
28B9A648 42B99D8A F2E3E9E0 16DE076E
E218B3DC F3606379 C33C1526 E8B58964
B2CB3F63 BDF901BE D31424D2 082891B5
F7058694 55FA46D8 EFC11878 0868F093
B17CAFE7 63A78881 86B78E38 0F146238
BAE22DBB D4EC71A1 0EC2E701 C7F9C648
5CF29CA3 1CB14285 19A46991 E9A98676
14FD4777 2D8A0AB6 A444EE0D CA009B54
—— END LICENSE ——

4.
— — BEGIN LICENSE — —
Alexey Plutalov
Single User License
EA7E-860776
3DC19CC1 134CDF23 504DC871 2DE5CE55
585DC8A6 253BB0D9 637C87A2 D8D0BA85
AAE574AD BA7D6DA9 2B9773F2 324C5DEF
17830A4E FBCF9D1D 182406E9 F883EA87
E585BBA1 2538C270 E2E857C2 194283CA
7234FF9E D0392F93 1D16E021 F1914917
63909E12 203C0169 3F08FFC8 86D06EA8
73DDAEF0 AC559F30 A6A67947 B60104C6
—— END LICENSE ——
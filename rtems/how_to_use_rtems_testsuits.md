# 如何使用`RTEMS testsuits`

## 编译`testsuit`

1.新建一个编译目录

```bash
$ mkdir rtems_build
```

2. 执行以下脚本文件(默认编译sample下的test)

```bash
../jk_rtems-src/configure --target=arm-rtems4.12 --enable-rtemsbsp="jk_figo" --enable-posix --enable-cxx
```

3. 经过`2`之后，会生成一个Makefile文件，直接`make`编译即可；

```bash
$ make -j12
```  

## 编译过程中遇到的问题

1.要想编译除`sample`之外的其他`test`，可以添加参数：

```bash
--enalble-tests=yes
```

2.编译中出现以下错误信息

```
error：pax is missing.
```
 原因是缺少`pax`程序，安装软件：

```bash
$ sudo apt-get install pax 
```
然后重新执行`configure.sh`，并重新`make -j12`

```bash
$ ./configure.sh
$ make -j12
```

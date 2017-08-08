# RTEMS LibBSD编译步骤
 
以beagleboneblack开发板为例：

 Waf
~~~~~~~~~~~~~~~~

LibBSD for RTEMS 是使用 Waf 编译的。这是一个库，包含有许多移植到 RTEMS 的 

Welcome to building LibBSD for RTEMS using Waf. This package is a library
containing various parts of the FreeBSD kernel ported to RTEMS. The library
replaces the networking port of FreeBSD in the RTEMS kernel sources. This
package is designed to be updated from the FreeBSD kernel sources and contains
more than just the networking code.

To build this package you need a current RTEMS tool set for your architecture,
and a recent RTEMS kernel for your BSP configured with networking disabled
built and installed. If you already have this you can skip to step 3 of the
build procedure.

Waf Setup
~~~~~~~~~

You can find the Waf project at:

 https://waf.io/

Waf is not intended to be installed by distribution packages so we recommend
you download a recent waf version and install it in your home directory.

Waf is a Python program so you will also need to have a current Python version
installed and in your path.

Download the latest signed executable file version to $HOME/bin and symlink it
to waf. Add the directory $HOME/bin to your path if it is not already in your
default path.

## 编译和安装 LibBSD
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following instructions show you how to build and install RTEMS Tools and
RTEMS kernel for your BSP in separate paths. Using separate paths for the tools
and BSPs lets you manage what you have installed. If you are happy with a
single path you can use the same path in each stage.

The waf build support for RTEMS requires you provide your BSP name as an
architecture and BSP pair. You must provide both or waf will generate an error
message during the configure phase.

We will build an Xilinx Zynq QEMU BSP using the name
'arm/xilinx_zynq_a9_qemu'.

## Steps

```bash
--------------------------------------------------------------
sandbox="$PWD/sandbox"
mkdir sandbox

cd "$sandbox"
git clone git://git.rtems.org/rtems-source-builder.git
git clone git://git.rtems.org/rtems.git
git clone git://git.rtems.org/rtems-libbsd.git

cd "$sandbox"
cd rtems-source-builder/rtems
../source-builder/sb-set-builder --prefix="$sandbox/rtems-4.12" 4.12/rtems-arm

cd "$sandbox"
cd rtems
PATH="$sandbox/rtems-4.12/bin:$PATH" ./bootstrap

cd "$sandbox"
mkdir b-beagleboneblack
cd b-beagleboneblack
PATH="$sandbox/rtems-4.12/bin:$PATH" "$sandbox/rtems/configure" \
  --target=arm-rtems4.12 --prefix="$sandbox/rtems-4.12" \
  --disable-networking --enable-rtemsbsp=xilinx_zynq_a9_qemu
PATH="$sandbox/rtems-4.12/bin:$PATH" make
PATH="$sandbox/rtems-4.12/bin:$PATH" make install

cd "$sandbox"
cd rtems-libbsd
git submodule init
git submodule update rtems_waf
waf configure --prefix="$sandbox/rtems-4.12" \
  --rtems-bsps=arm/xilinx_zynq_a9_qemu

waf
waf install
qemu-system-arm -no-reboot -serial null -serial mon:stdio -net none \
  -nographic -M xilinx-zynq-a9 -m 256M \
  -kernel build/arm-rtems4.12-xilinx_zynq_a9_qemu/selectpollkqueue01.exe
-------------------------------------------------------------------------------

```


```bash
1. 创建 sandbox 目录:
    $ sandbox="$PWD/sandbox"
    $ mkdir sandbox

2. 拷贝源码文件:

    $ cd "$sandbox"
    $ git clone git://git.rtems.org/rtems-source-builder.git
    $ git clone git://git.rtems.org/rtems.git
    $ git clone git://git.rtems.org/rtems-libbsd.git

3. 编译并安装工具链. 这里使用的工具链路径是: $sandbox/rtems-4.12
    $ cd "$sandbox"
    $ cd rtems-source-builder/rtems
    $ ../source-builder/sb-set-builder --prefix="$sandbox/rtems-4.12" 4.12/rtems-arm

4. RTEMS 源码目录下运行 Bootstrap:
    $ cd "$sandbox"
    $ cd rtems
    $ PATH="$sandbox/rtems-4.12/bin:$PATH" ./bootstrap

5. 编译并安装 BSP 包. 这里使用的 bsp 路径是：/opt/rtems/4.12/bsps

    $ cd "$sandbox"
    $ mkdir b-xilinx_zynq_a9_qemu
    $ cd b-xilinx_zynq_a9_qemu
    $ PATH="$sandbox/rtems-4.12/bin:$PATH" $sandbox/rtems/configure" --target=arm-rtems4.12 --prefix="$sandbox/rtems-4.12" --disable-networking --enable-rtemsbsp=xilinx_zynq_a9_qemu
    
    $ PATH="$sandbox/rtems-4.12/bin:$PATH" make
    $ PATH="$sandbox/rtems-4.12/bin:$PATH" make install

6. 下载 rtems_waf git 子模块. 注意, 一定要指定'rtems_waf'，否则将会下载 FreeBSD 的源码:
   $ cd "$sandbox"
   $ cd rtems-libbsd
   $ git submodule init
   $ git submodule update rtems_waf

7. 运行 Waf 的 configure. 需要指定 RTEMS 工具链的路径 ，可以使用
   '--rtems-archs=arm,sparc,i386' 或
   '--rtems-bsps=arm/xilinx_zynq_a9_qemu,sparc/sis,i386/pc586' 一次编译多个 BSP 包.  
   $ cd "$sandbox"
   $ cd rtems-libbsd
   $ waf configure --prefix="$sandbox/rtems-4.12" --rtems-bsps=arm/xilinx_zynq_a9_qemu


8. 编译并安装.  The LibBSD 包(.a和.h文件) 将会被安装到 --prefix 指定的目录下:
   $ cd "$sandbox"
   $ cd rtems-libbsd
   $ waf
   $ waf install

9.在 QEMU 上运行测试文件:
   $ qemu-system-arm -no-reboot -serial null -serial mon:stdio -net none  -nographic -M xilinx-zynq-a9 -m 256M  -kernel build/arm-rtems4.12-xilinx_zynq_a9_qemu/selectpollkqueue01.exe
```
[1]  waf 可以在命令行指定不同的源码路径和工具链安装路径，这样就可以保证每个版本的 rtems 互不干扰。
 

FreeBSD 开发者支持
~~~~~~~~~~~~~~~~~~~~~~~~~

The --freebsd-option provides a tool you can set special kernel options. This
is a developer tool and should only be used if you are familiar with the
internals of the FreeBSD kernel and what these options do.

The options are listed in:

 https://github.com/freebsd/freebsd/blob/master/sys/conf/NOTES

An example to turn on a verbose kernel boot, verbose sysinit and bus debugging
configure with:

 --freebsd-options=bootverbose,verbose_sysinit,bus_debug

The LibBSD waf support splits the options and converts them to uppercase and
adds them -D options on the compiler command line.


## 注意
- 1. 配置BSP包的时候需要指定--prefix，这在make完成之后进行make install的时候会将编译好的一部分二进制包拷贝到--prefix指定的目录中。因此，之前没有使用过--prefix的就会出现各种问题。
- 2. rtems-libbsd项目是一个将rtems和bsd lib库组合起来的项目。因为bsd lib库中包含强大的网络功能，rtems和bsd组合来实现更为完整的功能。

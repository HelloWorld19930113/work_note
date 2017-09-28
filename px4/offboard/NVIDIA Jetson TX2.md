# Build Kernel and Modules – NVIDIA Jetson TX2
March 25, 2017 kangalow Information, Jetson TX2 37

Note: This article has been superseded. Please see the L4T 28.1 build article.

This article covers building a kernel for L4T 27.1. If you are trying to build a kernel for L4T 27.1, you will need to checkout the vL4T27.1 version after cloning the buildJetsonTX2Kernel.git repository. Note that this is different than the steps followed in the video.

In this article, we cover building the kernel onboard the NVIDIA Jetson TX2. Looky here:



## Background and Motivation

Note: This article is for intermediate users, and the methods within are somewhat experimental. You should be familiar with the purpose of the kernel. You should be able to read shell scripts to understand the steps described.

With the advent of the new Jetson TX2 running L4T 27.1 with the 4.14 kernel, NVIDIA recommends using a host PC when building a system from source. See the Linux for Tegra R27.1 web page where you can get the required GCC 4.8.5 Tool Chain for 64-bit BSP.

The boot load sequence is more sophisticated on the Jetson TX2 in comparison to the TX1. In addition to the Uboot boot loader, there are additional loaders for hardware support. The previously mentioned tool chain is useful in building those features.

If you are building systems which require generating the entirety of Jetson TX2 system, those are good options. For a person like me, it’s a little overkill. Most of the time I just want to compile an extra driver or three as modules to support some extra hardware with the TX2. What to do, what to do …

Hack of course! With a little bit of coffee and swearing I was able to compile the kernel with modules on a Jetson TX2 itself.

## Installation

The script files to build the kernel on the Jetson TX2 are available on the JetsonHacks Github account in the buildJetsonTX2 repository.

$ git clone https://github.com/jetsonhacks/buildJetsonTX2Kernel.git
$ cd buildJetsonTX2Kernel

# 如果你使用的`Tegra`版本是 `L4T 27.1`, 那么需要切换分分支:
$ git checkout vL4T27.1
There are three main scripts. The first script, getKernelSources.sh gets the kernel sources from the NVIDIA developer website, then unpacks the sources into /usr/src/kernel.

**Note**
如何区分自己的版本是`L4T 27.1`还是`L4T 28.1`？
  使用`uname -r`提示的消息是“4.4.15-tegra”`，则说明版本是`L4T 27.1`；
  使用`uname -r`提示的消息是“4.4.38-tegra”`，则说明版本是`L4T 28.1`；


$ ./getKernelSources.sh
After the sources are installed, the script opens an editor on the kernel configuration file. In the video, the local version of the kernel is set. The stock kernel uses -tegra as its local version identifier. Make sure to save the configuration file when done editing. Note that if you want to just compile a module or two for use with a stock kernel, you should set the local version identifier to match.

The second script, makeKernel.sh, fixes up the makefiles so that the source can be compiled on the Jetson, and then builds the kernel and modules specified.

$ ./makeKernel.sh
The modules are then installed in /lib/modules/

The third script, copyImage.sh, copies over the newly built Image and zImage files into the /boot directory.

$ ./copyImage.sh
Once the images have been copied over to the /boot directory, the machine must be restarted for the new kernel to take effect.

Spaces!

The kernel and module sources, along with the compressed versions of the source, are located in /usr/src

After building the kernel, you may want to save the sources off-board to save some space (they take up about 3GB) You can also save the boot images and modules for later use, and to flash other Jetsons from the PC host.

Conclusion

For a lot of use cases, it makes sense to be able to compile the kernel and add modules from the device itself. Note that it is new, and not thoroughly tested at this point. Use it at your own risk.

Note

The video above was made directly after flashing the Jetson TX2 with L4T 27.1 using JetPack 3.0.

========================================================================
37 COMMENTS


Henry
APRIL 3, 2017 AT 3:55 AM
Hi Jim

I’m trying to compile the Kernel for a TX1 using the Jetpack 3.0, will this scripts will work or shall I use the ones you did on the Kernel build for TX1 (but this are not Jetpack 3.0

REPLY

kangalow 
APRIL 3, 2017 AT 8:37 AM
JetPack is an installer that places software on the target and host systems. The actual operating system on the Jetson itself is named L4T (Linux for Tegra). As of this writing, JetPack 3.0 installs L4T 27.1 on the Jetson TX2. For a Jetson TX1, JetPack installs L4T 24.2.1, which is the same version that JetPack 2.3 installed. The L4T versions are not the same, so you must follow the instructions for installing Jetson TX1 kernel here: http://www.jetsonhacks.com/2016/09/28/build-tx1-kernel-and-modules-nvidia-jetson-tx1/
Please note that this is for advance developers.

REPLY

Michael
MAY 4, 2017 AT 1:24 AM
Thanks a lot for your great content and this video! It helped me to finally get my rplidar working on the Jetson TX2 (the cp2102-driver was missing).

REPLY

kangalow 
MAY 4, 2017 AT 1:59 AM
Thank you for the kind words, and thanks for reading!

REPLY

Jan Carlson
MAY 5, 2017 AT 1:24 PM
I put a TX2 SOM (which you can get from Arrow now) on a TX1 Jetson carrier board and am getting significantly enough different error messages during a kernel compile to lead me to believe that these boards are subtly incompatible. I can give you specifics if you want, but I wanted to know if this was expected. Who would buy a SOM if it was? JTX2s are pretty much the same price anyhow.

REPLY

Dragon Wang
MAY 12, 2017 AT 1:52 AM
I get a error when I run the command ‘./copyImage.sh’:
cp: cannot stat ‘arch/arm64/boot/zImage’: No such file or directory
cp: cannot stat ‘arch/arm64/boot/Image’: No such file or directory

But I success in before two step.
Why??

REPLY

kangalow 
MAY 12, 2017 AT 5:30 PM
Are the files actually there?

REPLY

Tawn
MAY 14, 2017 AT 9:37 PM
Before and after building the kernel I get the same error when running tensorflow:

stream_executor/cuda/cuda_driver.cc:509] failed call to cuInit: CUDA_ERROR_NO_DEVICE
I tensorflow/stream_executor/cuda/cuda_diagnostics.cc:145] kernel driver does not appear to be running on this host (jet): /proc/driver/nvidia/version does not exist
W tensorflow/compiler/xla/service/platform_util.cc:61] platform CUDA present but no visible devices found

But I am able to build and run the NVIDIA samples. Is there some cuda device driver install I’m missing? I don’t see anything like that in the kernel config.

REPLY

kangalow 
MAY 15, 2017 AT 11:47 AM
I’m not sure what this has to do with the kernel, as TensorFlow does not involve any kernel modifications.

REPLY

Tawn 
JUNE 6, 2017 AT 2:43 PM
This turned out to be a permissions problem. I made a new user and the account needed to be in the ‘video’ group. The error message really threw me off.

REPLY

kangalow 
JUNE 6, 2017 AT 3:16 PM
Interesting. When people ask questions on this website, I’m always tempted to just say “It must be a permissions problem”. That seems to be the cause of 80% of the issues reported.

REPLY

Pb
MAY 23, 2017 AT 10:52 AM
Hey thanks for all your help and tutorials. Its awesome.

While doing this after i ./getkernelsources.sh I get error “Could not find Qt via plg-config
Makefile:531: recipe for target ‘xconfig’ failed
Make: ***[xconfig] Error 2

REPLY

kangalow 
MAY 23, 2017 AT 11:12 AM
Did you check to see if Qt was properly installed?
$ sudo apt-get install qt5-default pkg-config -y

REPLY

Pb
MAY 23, 2017 AT 12:42 PM
Got it to work. Just had to download the gcc binaries and export the path and it worked 🙂

REPLY

David
JUNE 12, 2017 AT 12:05 AM
I ran this script EXACTLY as directed on the video and the web page and everything seemed to go well. However, after reboot, my wired ethernet refuses to cooperate. Switching on the wifi worked but of course it is considerably slower than wired. Any ideas what I did wrong and/or how to correct it? Reflash everything from scratch yet again? If so, running this kernel modification will probably yield the same result. Help, someone, please!

REPLY

kangalow 
JUNE 12, 2017 AT 7:26 AM
Hi David,
I’ve never seen this error, so I am unable to offer any help. You could try:
apt-get remove –purge resolveconf
then
apt-get install –reinstall resolvconf
but it’s all just guess work on my part. You’ll have to use JetPack to flash your machine again.

REPLY

David
JUNE 12, 2017 AT 12:07 AM
Forgot to mention it is a TX2 and in preparation for using realsense R200.

REPLY

David
JUNE 12, 2017 AT 12:12 AM
Forgot to mention it is a TX2 and in preparation for using realsense R200.

Error message is: active connection removed before it was initialized. Web searches give answers all over the place but none mention the kernel having been rebuilt.

REPLY

Glenn
JUNE 15, 2017 AT 1:00 AM
Hi Jim,
Nice work! Thanks for sharing your knowledge.
I have one big question:
Assuming I have followed your procedures.
I need to build my own driver for IMX290 image sensor, my plan is to copy the existing IMX sensor driver and edit it to work for IMX290. Question: I just need to run the MakeKernel.sh and CopyImage.sh to compile and run my driver? Am I correct? Or is there any other things to do? Sorry I am still new to linux systems. Thanks

REPLY

kangalow 
JUNE 15, 2017 AT 9:35 AM
I have not built any camera drivers. My understanding is that there are some other things you need to do in order to get them to work in addition to compiling the module. The steps in this article will build the module and kernel, which will probably one of the steps. You should ask if others have any experience with this sensor on the NVIDIA Jetson TX2 forum: https://devtalk.nvidia.com/default/board/188/jetson-tx2/

Thanks for reading!

REPLY

Yile
JUNE 27, 2017 AT 9:21 AM
Ran the getKernelSources script and got:
hardware/nvidia/soc/t18x/kernel-dts/tegra186-soc/tegra186-soc-eqos.dtsi
hardware/nvidia/soc/t18x/kernel-dts/tegra186-soc/tegra186-soc-spi.dtsi
hardware/nvidia/soc/t18x/kernel-dts/tegra186-soc/tegra186-soc-prod.dtsi
hardware/nvidia/soc/t18x/kernel-dts/tegra186-soc/tegra186-soc-base.dtsi
hardware/nvidia/soc/t18x/kernel-dts/tegra186-soc/tegra186-soc-thermal.dtsi
hardware/nvidia/soc/t18x/kernel-dts/tegra186-soc/tegra186-soc-power-domain.dtsi
hardware/nvidia/soc/t18x/kernel-dts/tegra186-soc/tegra186-a57-cpuidle.dtsi
hardware/nvidia/soc/t18x/kernel-dts/tegra186-soc/tegra186-soc-ufshc.dtsi
hardware/nvidia/soc/t18x/kernel-dts/tegra186-soc/tegra186-cpus-4A.dtsi
hardware/nvidia/soc/t18x/kernel-dts/tegra186-soc/tegra186-soc-actmon.dtsi
hardware/nvidia/soc/t18x/kernel-dts/tegra186-soc/tegra186-cpus-2D2A.dtsi
hardware/nvidia/soc/t18x/kernel-dts/tegra186-soc/tegra186-soc-reilly-prod.dtsi
hardware/nvidia/soc/t18x/kernel-dts/tegra186-soc/tegra186-soc-uart.dtsi
hardware/nvidia/soc/t18x/kernel-dts/tegra186-soc/tegra186-trusty.dtsi
hardware/nvidia/soc/t18x/kernel-dts/tegra186-soc/tegra186-soc-vcm-prod.dtsi
hardware/nvidia/soc/t18x/kernel-dts/tegra186-soc/tegra186-soc-sata.dtsi
gzip: /proc/config.gz: No such file or directory
HOSTCC scripts/basic/fixdep
CHECK qt
SHIPPED scripts/kconfig/zconf.tab.c

REPLY

kangalow 
JUNE 27, 2017 AT 8:57 PM
Is there a file at /proc/config.gz
I just opened up a brand new system today. After flashing with JetPack 3.0, I built the kernel using these scripts and did not encounter any issues. Is this a fresh system you are working from?

REPLY

foucault
JUNE 30, 2017 AT 6:38 AM
hi Jim, all,
I run a TX2, and boot on SSD
I followed all the process, renaming kernel “-elpimous-V1.0”
after rebooting, “uname -r” tells me 4.4.15-tegra

saving modifs in file was ok, no errors on the 3 .sh execution.

But at start, it doesn’t load my kernel !!

Is the problem due to my SSD boot ?
Any help, please friends
Vincent

REPLY

foucault
JUNE 30, 2017 AT 8:47 AM
Hello,
I didn’t have success. Now, I know why and I send…

[u]Add module on kernel[/u]:

[b]Board [/b]: Jetson TX2
[b]Boot [/b]: SSD
[b]Howto [/b]: http://www.jetsonhacks.com/2017/03/25/build-kernel-and-modules-nvidia-jetson-tx2/

–> just after the command : ./copyImage.sh,

Image is finished, ready to read… But,
Image is on a wrong directory (on the SSD Disk !!!)
You must put it on the TX2 board 32Go disk !

backup image in board disk :
sudo cp /media/nvidia/xxxxxx/boot/Image/media/nvidia/xxxxxx/boot/Image_backup

sudo cp /boot/Image/media/nvidia/xxxxxx/boot/Image

sudo reboot

Enjoy

Vincent.

REPLY

kangalow 
JUNE 30, 2017 AT 10:24 AM
If you have built new modules, you should also copy them over the eMMC so that the eMMC and SSD are consistent.

REPLY

Drew
JULY 2, 2017 AT 7:00 PM
Success. Thanks for writing this post.

I noticed that the /boot directory on my Jetson TX2 did not have a zImage – so when installing a new kernel, I refrained from copying zImage and just copied Image. This appeared to work fine – the board booted into the new kernel without issue.

REPLY

kangalow 
JULY 2, 2017 AT 8:18 PM
You’re welcome, I am glad you found it useful. I believe that there were some changes since the TX1, the zImage doesn’t appear on the TX2. Since this is a “Developer Preview” version, I’m going to wait to see what the next release brings. Thanks for reading!

REPLY

Arthur
JULY 14, 2017 AT 6:53 PM
Hi Jim,
Thanks for all your videos.

I received the following error when I was making the Kernel.

rm: invalid option — ‘0’
Try ‘rm –help’ for more information.
Makefile:1137: recipe for target ‘_modinst_’ failed
make: *** [_modinst_] Error 1

Is there a way I can fix this error? Thanks!

REPLY

kangalow 
JULY 14, 2017 AT 7:51 PM
Hi Arthur,
You’re welcome. Which file were you running when the rm error occurred?

REPLY

Sds
JULY 30, 2017 AT 6:27 PM
I got this error :

nvidia@tegra-ubuntu:~/buildJetsonTX2Kernel$ ./copyImage.sh
cp: cannot stat ‘arch/arm64/boot/Image’: No such file or directory

REPLY

kangalow 
JULY 30, 2017 AT 10:47 PM
The message means that the Image is not there, probably because it did not build correctly. If you are building for JetPack 3.0/L4T 27.1 you must use the tag version vL4T27.1 in the buildJetsonTX2Kernel repository. The master is for the new JetPack 3.1/L4T 28.1 version. Thanks for reading!

REPLY

dean 
AUGUST 3, 2017 AT 3:39 AM
i did tag version vL4T27.1 but it doesn’t work.
may i ask you some suggestion to solve this issue?

REPLY

kangalow 
AUGUST 3, 2017 AT 8:05 AM
You have not provided enough information to provide help. If the Image did not build in earlier steps, it also means that it printed out the reason why it failed. Did you do a fresh flash of L4T 27.1, then:

$ git clone https://github.com/jetsonhacks/buildJetsonTX2Kernel.git
$ cd buildJetsonTX2Kernel
# For L4T 27.1, do the following:
$ git checkout vL4T27.1
REPLY

dean 
AUGUST 3, 2017 AT 4:17 PM
sorry for that i didn’t provide my information enough
i install my TX2 with the film
https://www.youtube.com/watch?v=D7lkth34rgM&t=443s
and then i want to create a kernel to it
so i follow the film
https://www.youtube.com/watch?v=fxWObd1nK4s&feature=youtu.be
and i also change the branch from master to tag vL4T27.1
i did

$ git clone https://github.com/jetsonhacks/buildJetsonTX2Kernel.git
$ cd buildJetsonTX2Kernel
and i didn’t do your suggestion
$ git checkout vL4T27.1
i just do like the film to the following step
i create a new local version
but in the part of usb setting
i point ‘USB CP210x family with UART Bridge controller’
to make my radia sensor work
and i type
‘./makeKernel.sh’
it’s ok until now
but when i type
‘./copyImage.sh’
the terminal show
cp: cannot stat ‘arch/arm64/boot/Image’: No such file or directory

i will do the additional step that you suggestion
$ git checkout vL4T27.1
before ‘./getKernelSource.sh’
the film didn’t do this step , it’s necessary?

thank you very much


kangalow 
AUGUST 3, 2017 AT 5:10 PM
The video is from March, 2017. Since then there has been a new release to L4T 28.1 (JetPack 3.1). The repository has been updated to reflect the new version. However, a tagged version vL4T27.1 was created so that if you have L4T 27.1 you can use those scripts. Therefore you have to checkout those scripts. Hope this helps.


Edward Chen
JULY 31, 2017 AT 8:01 PM
Hi
Thanks for your great tutorial,
one question here:

If I flashed my jetson TX2 board with latest jetpack 3.1,
do I need to modified any of the scripts or settings to keep match to it?
Or the script will figure it out to load the match kernel.

my “$rname -r” returns “4.4.38-tegra” which is different from the demo you showed in the video.

Finally, from the above discussion, you mentioned that I have to make sure the kernel on SSD and eMMC are consistent. How can I do that?

Thanks for your help!

REPLY

kangalow 
JULY 31, 2017 AT 8:08 PM
Here’s the updated article: http://wp.me/p7ZgI9-QI
There are different tagged versions on the Github account. The master branch is the latest version of L4T.

REPLY
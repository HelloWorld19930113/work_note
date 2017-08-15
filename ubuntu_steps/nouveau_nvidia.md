# ubuntu14.04 NVIDIA GeForce 720M 驱动安装
 需要说明的是，这篇文档并没有成功安装NVIDIA驱动。


Ubuntu 14.04 through 16.10 Please note that, nouveau drivers manual removal is required only if you are going to install the proprietary nvidia drivers yourself. If this is not the case then directly install the required graphic drivers from System->Administration->Hardware drivers. Its the recommended and the most convenient way available.

We'll blacklist all the culprit modules, remove all the nvidia* packages and as an extra step we may have to update the initramfs disk because it could be configured to load the modules at startup.

加入黑名单，Blacklist the modules. 打开 `blacklist.conf `文件.

```bash
$ sudo vim /etc/modprobe.d/blacklist.conf

# add the following modules in the file.
blacklist amd76x_edac #this might not be required for x86 32 bit users.
blacklist vga16fb
blacklist nouveau
blacklist rivafb
blacklist nvidiafb
blacklist rivatv
```
保存文件并退出。

删除所有的 nvidia* 软件包。

```bash
$ sudo apt-get remove --purge nvidia*
```

然后开始安装`nvidia`驱动。

1 `Ctrl+Alt+F1`进入文本模式，这时停止桌面服务：

```bash
$ sudo service lightdm stop
```

Now, run the driver package that you downloaded from Nvidia’s website.

```bash
$ sudo ./NVIDIA-Linux-x86-260.19.44.run
```

Note: If you still get the error related to nouveau drivers then you are probably required to update the initramfs disk. Probably it might be configured to load the nouveau drivers. Don't reboot or poweroff, run this command to update the initramfs disk.

```bash
$ sudo update-initramfs -u
```
Now reboot and repeat the step 3. This time things should go smoothly.

nvidia-xconfig

nvidia-xconfig --restore-original-backup 

==================================================
==================================================
Ubuntu 14.04 & Nvidia GTX 960
I have Ubuntu 14.04 64bit and NVIDIA Geforce GTX 960 that caused me a lot of problems to install. Hopefully this can help somebody:


in terminal write
sudo apt-get update && sudo apt-get upgrade && sudo apt-get dist-upgrade && sudo apt-get install build-essential && sudo apt-get install linux-source && sudo apt-get install linux-headers-generic 
sudo gedit /etc/default/grub #change line "GRUB_CMDLINE_LINUX_DEFAULT..."-line to 
GRUB_CMDLINE_LINUX_DEFAULT="nouveau.blacklist=1 quiet splash nomodeset" #(forces low-level graphics to ensure putty does not give black screen)
sudo update-grub2
sudo apt-get remove nvidia* && sudo apt-get autoremove #ensures no former installation clashes with new install
sudo reboot 

After reboot get correct nvidia-driver (chose graphic-card and OS) at “http://www.nvidia.com/Download/index.aspx?lang=en-us" (or search webb with “nvidia download”). Right-click on downloaded file and change if to executable. 
sudo gedit /etc/modprobe.d/blacklist.conf #add these lines at the end:
blacklist vga16fb
blacklist nouveau
blacklist rivafb
blacklist nvidiafb
blacklist rivatv
blacklist lbm-nouveua
options nouveau modeset=0
alias nouveau off
alias lbm-nouveau off 

Open a putty-terminal with Ctrl + Alt + F1 

```bash
sudo service lightdm stop #stops graphic session to enable nvidiainstallation
cd Downloads #(or wherever you downloaded your nvidia-file)
sudo ./{the downloadedfilename.run} #follow installation-instructions (normally yes to all)
sudo nvidia-xconfig  #(if you did not chose “yes” to this in the installation” 
sudo nano /etc/default/grub # change the "GRUB_CMBLINE_LINUX_DEFAULT..."-line to below:
GRUB_CMDLINE_LINUX_DEFAULT="quiet splash nomodeset”
sudo update-grub2 #update grub!
sudo reboot
```

hopefully it works for you now as it did for me...
Last edited by jempa333; March 26th, 2015 at 10:53 PM.


=================================================
Elder Geek
19.2k93586
        
Is there a way to do this during Ubuntu installation? I'm stuck on what appears to be an issue with the Nouveau drivers, see askubuntu.com/questions/507226/…. – Tyson Trautmann Aug 5 '14 at 21:45
    
Thank for his recipe. Worked for me in Ubuntu 14.04, just 2 notes: to stop X needed sudo service lightdm stop (instead of /etc/init.d/gdm stop) and needed one more reboot after update-initramfs. – tomasb Feb 1 '16 at 23:50

       
This worked to pass by the nouveau driverd on Ubuntu 16.04. – Manuel J. Diaz Aug 28 '16 at 1:45        
This worked very nice on Ubuntu 16.10, great answer – nosthertus Jan 10 at 6:57
On xubuntu at least, you don't have to reboot to remove the noveau driver. After stopping the display manager modprobe -r nouveau unloads the noveau driver from the kernel. – gerardw Jul 25 at 15:35

Insert

blacklist nouveau
blacklist lbm-nouveau
options nouveau modeset=0
alias nouveau off
alias lbm-nouveau off
into

/etc/modprobe.d/blacklist.conf

and reboot, before installing the NVidia driver. Worked for me on Ubuntu 16.04 LTS.

shareimprove this answer
edited Sep 14 '16 at 21:38
answered Sep 8 '16 at 14:06

P.K
312
1       
As a famous philosopher once said “Do, or do not! There is no try!” or in Ask Ubuntu style: if you’re sure, then it’s an answer, if you’re not, then it’s a comment. Words like “try”, “might”, “should” are a sign, that a message belongs in the comments, and “do”, “will”, “shall” in answers. – David Foerster Sep 8 '16 at 15:36
        
Its good to have 16.04 instructions because the questions and answers are about 14.04. Can you put '16.04 How To" at the beginning in Heading format? +1 – WinEunuuchs2Unix Sep 14 '16 at 22:09 
add a comment
up vote
2
down vote
Manual removal of nouveau is not required anymore. If you install nvidia drivers from Additional Drivers on Ubuntu, the installation scripts will make sure that nouveau is blacklisted.

However, before you do anything you have to add graphics repo for nvidia drivers:

sudo apt-add-repository ppa:graphics-drivers/ppa
sudo apt-get update
I suggest you have a look at nvidia website for the drivers your card supports and install those from the Additional Drivers tool on Ubuntu.


==================================================
==================================================

It is not related to nvidia drivers. Because by default Ubuntu uses non-nvidia drivers even though you might have nvidia GPUs. I have an nvidia GPU too.

My Ubuntu used to boot fine until something happened which caused the same issue. After reading posts, reading logs and little bit trial and error, turns out the problem is related to lightdm GUI server.

I dont know solution to the problem but there is a quick work around in 3 steps. This will save you from reinstalling Ubuntu.

Step 1: When the error shows up, hit Ctrl+Alt+F1. This will open the commandline interface. Login as root.
Step 2: Remove a particular X11 config file. This file is not really required.

rm /etc/X11/xorg.conf.failsafe
Somehow the existence of the above X11 configuration file causes the OS to throw that error.
Step 3: Restart lightdm with:

service lightdm restart
This will restart lightdm and voila your desktop is back!
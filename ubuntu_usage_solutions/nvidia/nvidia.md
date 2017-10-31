$ sudo subl /etc/modprobe.d/blacklist-nouveau.conf 
$ sudo subl /etc/modprobe.d/nvidia-installer-disable-nouveau.conf 


$ sudo subl /etc/modprobe.d/blacklist.conf


blacklist vga16fb
blacklist rivafb
blacklist nvidiafb
blacklist rivatv
blacklist nouveau
blacklist lbm-nouveau
options nouveau modeset=0
alias nouveau off
alias lbm-nouveau off

# .bashrc文件的配置

```bash

# export PATH=$PATH:/home/ldd/opt/qemu_install/bin
export PATH=$PATH:/home/ldd/opt/devel_tools/rtems-4.12/bin/
export PATH=$PATH:/home/ldd/opt/devel_tools/gcc-arm-none-eabi-5_4-2016q3/bin
export PATH=$PATH:/home/ldd/opt/devel_tools/arm-2009q3/bin

# Java
export JAVA_HOME=/home/ldd/opt/devel_tools/java_8_sun
export JRE_HOME=${JAVA_HOME}/jre
export CLASSPATH=.:${JAVA_HOME}/lib:${JRE_HOME}/lib
export PATH=$PATH:${JAVA_HOME}/bin

#sublime imfix
export PATH=$PATH:/home/ldd/opt/sublime-text-imfix/src
export LD_PRELOAD=/home/ldd/opt/sublime-text-imfix/lib/libsublime-imfix.so
```

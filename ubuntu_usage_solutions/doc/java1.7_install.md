# `ubuntu` 安装 `Java1.7` 
  `ubuntu 14.04 64安装`JDK1.7`。
## 1. 新建文件夹
```bash
ldd@x450v:~$ sudo mkdir /usr/lib/jvm
```
## 2. 解压文件
```bash
ldd@x450v:~$ sudo tar zxvf jdk-7u71-linux-x64.tar.gz -C /usr/lib/jvm
```
## 3. 打开设置环境变量文件
```bash
ldd@x450v:~$ sudo gedit ~/.bashrc
[sudo] password for ldd:
```
## 4. 设置环境变量
```bash
#set java environment
export JAVA_HOME=/usr/lib/jvm/jdk1.7.0_71
export JRE_HOME=${JAVA_HOME}/jre  
export CLASSPATH=.:${JAVA_HOME}/lib:${JRE_HOME}/lib  
export PATH=${JAVA_HOME}/bin:$PATH
```
## 5. 设置默认的`JDK`
```bash
ldd@x450v:~$ sudo update-alternatives --install /usr/bin/java java /usr/lib/jvm/jdk1.7.0_71/bin/java 300
update-alternatives: using /usr/lib/jvm/jdk1.7.0_71/bin/java to provide /usr/bin/java (java) in 自动模式
ldd@x450v-ThinkPad-Edge-E545:~$ sudo update-alternatives --install /usr/bin/javac javac /usr/lib/jvm/jdk1.7.0_71/bin/javac 300
update-alternatives: using /usr/lib/jvm/jdk1.7.0_71/bin/javac to provide /usr/bin/javac (javac) in 自动模式
```
然后执行：
```bash
ldd@x450v:~$ sudo update-alternatives --config java
```
 若是初次安装jdk,会有下面的提示        
   There is only one alternative in link group java (providing /usr/bin/java): 
    `/usr/lib/jvm/jdk1.7.0_60/bin/java`            
 否则,选择合适的`jdk`             

## 6. 检测安装`JDK`是否成功
### 1) java -version
```bash
ldd@x450v:~$ java -version
java version "1.7.0_71"
Java(TM) SE Runtime Environment (build 1.7.0_71-b14)
Java HotSpot(TM) 64-Bit Server VM (build 24.71-b01, mixed mode)
```
### 2) java
```bash
ldd@x450v:~$ java
用法: java [-options] class [args...]
          (执行类)
  或  java [-options] -jar jarfile [args...]
          (执行 jar 文件)
...
```

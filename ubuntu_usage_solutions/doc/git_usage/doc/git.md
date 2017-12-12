# 常见的git命令使用

## 搭建本地`git`服务器        

GitHub就是一个免费托管开源代码的远程仓库。但是对于某些视源代码如生命的商业公司来说，既不想公开源代码，又舍不得给GitHub交保护费，那就只能自己搭建一台Git服务器作为私有仓库使用。         

搭建Git服务器需要准备一台运行Linux的机器，强烈推荐用Ubuntu或Debian，这样，通过几条简单的apt命令就可以完成安装。    

假设你已经有sudo权限的用户账号，下面，正式开始安装。    

1. 安装git   
```
$ sudo apt-get install git
```
2. 创建一个git用户，用来运行git服务              
```
$ sudo adduser git
```
3. 创建证书登录    
收集所有需要登录的用户的公钥，就是他们自己的`id_rsa.pub`文件，把所有公钥导入到`/home/git/.ssh/authorized_keys`文件里，一行一个。            
```bash
$ scp .ssh/id_rsa.pub git@<ServerIP>:~/ssh/tmp_ids
$ ssh git@<ServerIP>
$ cat id_rsa.pub >> authorized_keys
```
4. 初始化`Git`仓库    
```bash
$ sudo mkdir ~/work
$ sudo chown git:git ~/work
```
先选定一个目录作为`Git`仓库，假定是`~/work/sample.git`，在`~/work`目录下输入命令：  
```bash
$ sudo git init --bare sample.git
```
`Git`就会创建一个裸仓库，裸仓库没有工作区，因为服务器上的`Git`仓库纯粹是为了共享，所以不让用户直接登录到服务器上去改工作区，并且服务器上的`Git`仓库通常都以`.git`结尾。然后，把`owner`改为`git`：   
```
$ sudo chown -R git:git sample.git
```
5. 禁用`shell`登录   
出于安全考虑，第二步创建的git用户不允许登录`shell`，这可以通过编辑`/etc/passwd`文件完成。找到类似下面的一行：    
```
git:x:1001:1001:,,,:/home/git:/bin/bash
# 改为：
git:x:1001:1001:,,,:/home/git:/usr/bin/git-shell
```
这样，`git`用户可以正常通过`ssh`使用`git`，但无法登录`shell`，因为我们为`git`用户指定的`git-shell`每次一登录就自动退出。  
6. 克隆远程仓库：   
现在，可以通过`git clone`命令克隆远程仓库了，在各自的电脑上运行：   
```bash
$ git clone git@server:/srv/sample.git
Cloning into 'sample'...
warning: You appear to have cloned an empty repository.
```
剩下的推送就简单了。   
7. 管理公钥  
如果团队很小，把每个人的公钥收集起来放到服务器的`/home/git/.ssh/authorized_keys`文件里就是可行的。如果团队有几百号人，就没法这么玩了，这时，可以用`Gitosis`来管理公钥。   
这里我们不介绍怎么玩Gitosis了，几百号人的团队基本都在500强了，相信找个高水平的Linux管理员问题不大。    
8. 管理权限
有很多不但视源代码如生命，而且视员工为窃贼的公司，会在版本控制系统里设置一套完善的权限控制，每个人是否有读写权限会精确到每个分支甚至每个目录下。因为Git是为Linux源代码托管而开发的，所以Git也继承了开源社区的精神，不支持权限控制。不过，因为Git支持钩子（hook），所以，可以在服务器端编写一系列脚本来控制提交等操作，达到权限控制的目的。Gitolite就是这个工具。   
这里我们也不介绍Gitolite了，不要把有限的生命浪费到权限斗争中。   
9. 小结    
- 搭建Git服务器非常简单，通常10分钟即可完成；   
- 要方便管理公钥，用Gitosis；   
- 要像SVN那样变态地控制权限，用Gitolite。   

## `git cheatsheet`                  
1. 添加远程仓库                 
```bash
$ git remote add origin git@github.com:gzj2013/xxx.git
```
2. 删除远程仓库               
```bash
$ git remote rm origin 
```
3. 添加.gitignore模板         
```bashiles
*~

# log files
*.log

# dot files
.*

# git
!.gitignore

# make file
*~
*.o
*.o.d
core
*.ko
*.mod
*.mod.c
Module.symvers
modules.order
```
新建`.gitignore`
4. git add -u           
只是删除了本地文件，远程仓库的并没有删除，但是此时想要直接删除远程仓库中对应的文件。             
5. 撤销修改         
**场景1**：当你改乱了工作区某个文件的内容，想直接丢弃工作区的修改时，用命令:            
`git checkout -- file`          
**场景2**：当你不但改乱了工作区某个文件的内容，还添加到了暂存区时，想丢弃修改，分两步：       
第一步用命令`git reset HEAD file`就回到了场景1；     
第二步按场景1操作。     
**场景3**：已经提交了不合适的修改到版本库时，想要撤销本次提交(撤销提交后本地修改仍然会保留的)。    
```
git reset HEAD^
```
6. `git` 冲突合并              
```
<<<<<<<   
A的修改 
=======
B的修改
>>>>>>>
```
删除的需要统一，要么所有的地方要么都保留 A 的修改，要么都保留 B 的修改。          
7. 配置`git`使用`ss5`代理                 
```bash
$ git config --global http.proxy 'socks5://127.0.0.1:1080' 
$ git config --global https.proxy 'socks5://127.0.0.1:1080'

# 取消代理
$ git config --global --unset http.proxy
$ git config --global --unset https.proxy
```
8. git撤销上一次还未`push`的`commit `             
```bash
$ git commit --amend
```
9. `git rm`               
`git rm`有两种选择.            
一种是 `git rm --cached "路径/文件"`，不删除物理文件，仅将该文件从`git`远程仓库中删除；   
一种是 `git rm --f "路径/文件"`，不仅将该文件从`git`           远程仓库中删除，还会将物理文件删除（不会回收到垃圾桶）                       
10. git 添加多个远程仓库            
 比如你有`oschina`和`github`           
```
git remote add origin https://github.com/xxx(仓库地址)  #添加github
git remote add oschina https://git.oschina.net/xxxx(仓库地址)  #添加oschina

git push oschina master(分支名)  #提交到oschina
git push origin master(分支名)   #提交到github

git pull oschina master    #从oschina更新
git pull origin master     #从github更新
```
 `git remote add <name> <url>`         
 其中，`name`表示你要给这个远程库起的名字, `url`表示这个库的地址提交的时候，先`add`, `commit`。          
 `git push <name> <branch>`      
 其中，`name`表示你在上一步给它起的名字，`branch`表示某一个分支。      
11. connect to host github.com port 22: Connection timed out                
 访问`github.com`超时。    
```
$ ssh git@github.com 
ssh: connect to host github.com port 22: Connection timed out
```
**解决办法(windows/linux 通用)**：
11.1 修改源地址协议    
```
git config --local -e
```
将`url = git@github.com:username/repo.git`改为`url = https://github.com/username/repo.git`，然后保存，再次执行。    
11.2 添加一个配置文件       
 在`.ssh`目录下创建一个`config`文件，输入如下内容：   
```
Host github.com 
User guozhijie_2006@126.com 
Hostname ssh.github.com 
PreferredAuthentications publickey 
IdentityFile C:/Users/Administrator/.ssh/id_rsa 
Port 443 
```
注意修改你的邮箱，保存并关闭.      

进行测试是否连接上`github.com`      
```
ssh -T git@github.com 

The authenticity of host ‘[ssh.github.com]:443 ([207.97.227.248]:443)’ can’t be established. 
RSA key fingerprint is 16:27:ac:a5:76:28:2d:36:63:1b:56:4d:eb:df:a6:48. 
Are you sure you want to continue connecting (yes/no)? y 
Please type ‘yes’ or ‘no’: yes 
Warning: Permanently added ‘[ssh.github.com]:443,[207.97.227.248]:443’ (RSA) to the list of known hosts. 
Hi gzj2013! You've successfully authenticated, but GitHub does not provide shell access. 
```
出现`Hi xxx!……`表示连接成功。        
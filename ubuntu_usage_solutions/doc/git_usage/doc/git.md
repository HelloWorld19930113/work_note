# 常见的git命令使用

## 1. 添加远程仓库
```bash
$ git remote add origin git@github.com:gzj2013/xxx.git
```

## 2. 删除远程仓库
```bash
$ git remote rm origin 
```


## 3.添加.gitignore模板
```bash
# backup files
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

## 4.git add -u
只是删除了本地文件，远程仓库的并没有删除，但是此时想要直接删除远程仓库中对应的文件。    


## 5.撤销修改

场景1：当你改乱了工作区某个文件的内容，想直接丢弃工作区的修改时，用命令:           
`git checkout -- file`

场景2：当你不但改乱了工作区某个文件的内容，还添加到了暂存区时，想丢弃修改，分两步：   
第一步用命令`git reset HEAD file`就回到了场景1；    
第二步按场景1操作。    

场景3：已经提交了不合适的修改到版本库时，想要撤销本次提交(撤销提交后本地修改仍然会保留的)。    
```
git reset HEAD^
```

## 6. `git` 冲突合并
```
<<<<<<<   
A的修改 
=======
B的修改
>>>>>>>
```
删除的需要统一，要么所有的地方要么都保留 A 的修改，要么都保留 B 的修改。       


## 7. 配置`git`使用`ss5`代理  
```bash
$ git config --global http.proxy 'socks5://127.0.0.1:1080' 
$ git config --global https.proxy 'socks5://127.0.0.1:1080'

# 取消代理
$ git config --global --unset http.proxy
$ git config --global --unset https.proxy
```


## 8. git撤销上一次还未`push`的`commit `

```bash
$ git commit --amend
```

## 9. `git rm` 
`git rm`有两种选择.           
一种是 `git rm --cached "路径/文件"`，不删除物理文件，仅将该文件从`git`远程仓库中删除；  
一种是 `git rm --f "路径/文件"`，不仅将该文件从`git`远程仓库中删除，还会将物理文件删除（不会回收到垃圾桶）          


## 10. git 添加多个远程仓库
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


## 11. connect to host github.com port 22: Connection timed out
 访问`github.com`超时。    
```$ ssh git@github.com 
ssh: connect to host github.com port 22: Connection timed out
```

**解决办法(windows/linux 通用)**：   
 在`.ssh`目录下创建一个`config`文件，输入如下内容：   
```
Host github.com 
User guozhijie_2006@126.com 
Hostname ssh.github.com 
PreferredAuthentications publickey 
IdentityFile C:/Users/Administrator/.ssh/id_rsa 
Port 443 
```
注意修改你的邮箱，保存并关闭    

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
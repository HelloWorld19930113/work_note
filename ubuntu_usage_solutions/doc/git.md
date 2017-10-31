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


## 7. 配置`git`使用代理  

```bash
$ git config --global http.proxy 'socks5://127.0.0.1:1080' 
$ git config --global https.proxy 'socks5://127.0.0.1:1080'
```
 
## 8. git撤销上一次还未`push`的`commit `

```bash
$ git commit --amend
```

## 9. `git rm` 
`git rm`有两种选择.           
一种是 `git rm --cached "路径/文件"`，不删除物理文件，仅将该文件从`git`远程仓库中删除；  
一种是 `git rm --f "路径/文件"`，不仅将该文件从`git`远程仓库中删除，还会将物理文件删除（不会回收到垃圾桶）          

# Git SSH Key 生成步骤

Git是分布式的代码管理工具，远程的代码管理是基于SSH的，所以要使用远程的git则需要SSH的配置。github的SSH配置如下：        

## 1. 设置Git的user name和email       

```bash
$ git config --global user.name "zack"
$ git config --global user.email "guozhijie_2006@126.com"
```
## 2. 生成SSH密钥         

```bash
# 查看是否已经有了ssh密钥
$ cd ~/.ssh

# 生成密钥：
$ ssh-keygen -t rsa -C "guozhijie_2006@126.com"

# 按3个回车，密码为空。

Your identification has been saved in /home/tekkub/.ssh/id_rsa.
Your public key has been saved in /home/tekkub/.ssh/id_rsa.pub.
The key fingerprint is:
………………
```
最后得到了两个文件：id_rsa和id_rsa.pub             

## 3.添加密钥到ssh

```bash
$ ssh-add id_rsa.pub
```

需要之前输入密码。          

## 4.在github上添加ssh密钥，这要添加的是“id_rsa.pub”里面的公钥。         
打开https://github.com/ ，登陆账号，然后在 Settings->SSH and GPG keys 点击 New SSH key 添加。           

## 5.测试：ssh git@github.com
The authenticity of host ‘github.com (207.97.227.239)’ can’t be established.
RSA key fingerprint is 16:27:ac:a5:76:28:2d:36:63:1b:56:4d:eb:df:a6:48.
Are you sure you want to continue connecting (yes/no)? yes
Warning: Permanently added ‘github.com,207.97.227.239′ (RSA) to the list of known hosts.
ERROR: Hi tekkub! You’ve successfully authenticated, but GitHub does not provide shell access
Connection to github.com closed.        

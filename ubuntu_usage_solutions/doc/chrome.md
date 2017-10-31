# chrome提示

 “无法正确打开您的个人资料。某些功能可能无法使用。请检查该个人资料是否存在，以及您是否有权对其中的内容进行读写操作。”，而且同步帐户也无法登录并同步帐户资料，这让我头痛了半天。           
 
## 解决办法：       
1、关闭chrome         
2、找到chrome默认配置文件路径，我的是ubuntu系统，路径为：`~/.config/google-chrome/Default/Web\ Data`            

```bash
$ rm ~/.config/google-chrome/Default/Web\ Data
```
3、删除Web Data文件             
4、重新打开chrome        

=======================友情提醒======================         

Web Data文件的具体路径：(如果找不到，请把隐藏系统文件改为显示）     
```   
Windows XP：C:\Documents and Settings\用户名\Local Settings\Application Data\Google\Chrome\User Data\Default\Web Data
Windows 7：C:\Users\用户名\AppData\Local\Google\Chrome\User Data\Default\Web Data
```
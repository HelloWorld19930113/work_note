# sublime text3 使用中出现的问题

## 1. `sublime` 中的设置无法保存
  表现为打开的工程在关闭`sublime`之后就没有了；注册好的`sublime`重新打开之后就恢复到未注册状态。         

**原因**：   
 是因为在`/home`目录下，关于`sublime`配置文件的权限有问题：       
```bash
$ ls ~/.config
总用量 120
drwx------ 26 ldd  ldd  4096 7月  29 12:06 ./
drwxr-xr-x 24 ldd  ldd  4096 7月  29 11:41 ../
...
drwxr-xr-x  2 root root 4096 7月   8 18:45 gedit/
...
drwx------  7 root root 4096 7月  29 13:37 sublime-text-3/
```

**解决**：
```bash
$ sudo chown -hR ldd:ldd sublime-text-3 gedit
```

## 2. `sublime` 中的输入法不跟随光标
1). 手动安装`IMESupport`
 安装一个`Sublime`的插件([IMESupport(https://github.com/chikatoike/IMESupport)]), 可以解决这个问题.`git clone`到`packages/`目录。          
2). 在`sublime`中`Package Control: Install Package`搜索安装`IMESupport`        


## 3. sublime text3 设置
```
{
	"font_size": 11.0,
	"font_face": "Courier New bold",
	"ignored_packages":
	[
		"Vintage"
	],
	"theme": "Soda Dark 3.sublime-theme",
	"update_check": false,
	"word_wrap": "auto"
}

```
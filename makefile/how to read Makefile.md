## Makefile语法分析



```
ifeq ($(wildcard .git),)
    $(error YOU HAVE TO USE GIT TO DOWNLOAD THIS REPOSITORY. ABORTING.)
endif
```
1.ifeq 是用来判断前面的那个条目是否为空。
2.$(error TEXT…) 用于产生致命错误，并提示后边的”TEXT...“信息给用户。

## Makefile 语法规则
[TOC]
##$(error TEXT…) 
`函数功能`：产生致命错误，并提示“TEXT…”信息给用户，并退出 make的执行。需要说明的是：“error”函数是在函数展开式（函数被调用时）才提示信息并结束 make进程。因此如果函数出现在命令中或者一个递归的变量定义中时，在读取 Makefile 时不会出现错误。而只有包含“error”函数引用的命令被执行，或者定义中引用此函数的递归变量被展开时，才会提示致命信息“TEXT…”同时退出 make。 
`返回值`：空 

`函数说明`： “error”函数一般不出现在直接展开式的变量定义中，否则在make读取Makefile时将会提示致命错误。关于递归展开和直接展开可参考  5.2 两种变量定义 一节 
假设我们的 Makefile 中包含以下两个片断； 
示例 1：
``` 
ifdef ERROR1 
$(error error is $(ERROR1)) 
endif 
```
make读取解析Makefile时，如果只起那已经定义变量“EROOR1”，make将会提示致命错误信息“$(ERROR1)”并退出。关于“ifdef”可参考  7.2.1.3 关键字“ifdef”一小节。 
示例 2： 
```
ERR = $(error found an error!) 
 
.PHONY: err 
err: ; $(ERR) 
```
这个例子，在 make 读取 Makefile 时不会出现致命错误。只有目标“err”被作为一个目标被执行时才会出现。

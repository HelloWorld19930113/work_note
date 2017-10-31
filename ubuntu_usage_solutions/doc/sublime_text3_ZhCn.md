# 解决`Sublime Text3`无法输入中文

介绍如何解决在Ubuntu14.04下Sublime Text 3无法输入中文的问题         

## 1. 前提条件
  在Ubuntu14.04环境下，已有搜狗输入法 for Linux和Sublime Text 3的情况下安装成功。       

## 2. `sublime_imfix.c`文件 
  保存下面的代码到文件`sublime_imfix.c`(位于`/home/ldd/opt/sublime_text`目录)          

```cpp
#include <gtk/gtkimcontext.h>
void gtk_im_context_set_client_window (GtkIMContext *context,
         GdkWindow    *window)
{
    GtkIMContextClass *klass;
    g_return_if_fail (GTK_IS_IM_CONTEXT (context));
    klass = GTK_IM_CONTEXT_GET_CLASS (context);
    if (klass->set_client_window)
    klass->set_client_window (context, window);
    g_object_set_data(G_OBJECT(context),"window",window);
    if(!GDK_IS_WINDOW (window))
    return;
    int width = gdk_window_get_width(window);
    int height = gdk_window_get_height(window);
    if(width != 0 && height !=0)
    gtk_im_context_focus_in(context);
}
```

## 3. 编译共享库`libsublime-imfix.so`       
 将上一步的代码编译成共享库`libsublime-imfix.so`。           
```bash
$ cd /home/ldd/opt/sublime_text
$ gcc -shared -o libsublime-imfix.so sublime_imfix.c  `pkg-config --libs --cflags gtk+-2.0` -fPIC
```

## 4.修改文件/usr/bin/subl

```bash
$ sudo gedit /usr/bin/subl
```
将       
```bash
#!/bin/sh
exec /opt/sublime_text/sublime_text "$@"
```
修改为        
```bash
#!/bin/sh
LD_PRELOAD= /home/ldd/opt/sublime_text/libsublime-imfix.so exec /opt/sublime_text/sublime_text "$@"
```
此时，在命令中执行 subl 将可以使用搜狗for linux的中文输入。             

## 5.  修改文件`sublime_text.desktop`
  为了使用鼠标右键打开文件时能够使用中文输入，还需要修改文件sublime_text.desktop的内容。         
```bash
$ sudo gedit /usr/share/applications/sublime_text.desktop
```
- (1) 将         
```bash
Exec=/opt/sublime_text/sublime_text %F
```
修改为       
```bash
Exec=bash -c "LD_PRELOAD=/home/ldd/opt/sublime_text/libsublime-imfix.so exec /opt/sublime_text/sublime_text %F"
```
- (2) 将`[Desktop Action Window]`中的字符串                 
```bash
Exec=/opt/sublime_text/sublime_text -n
```
修改为         
```bash
Exec=bash -c "LD_PRELOAD=/home/ldd/opt/sublime_text/libsublime-imfix.so exec /opt/sublime_text/sublime_text -n"
```
- (3) 将`[Desktop Action Document]`中的字符串       
```bash
Exec=/opt/sublime_text/sublime_text --command new_file
```
修改为          
```bash
Exec=bash -c "LD_PRELOAD= /home/ldd/opt/sublime_text/libsublime-imfix.so exec /opt/sublime_text/sublime_text --command new_file"
```
** 注意：**
修改时请注意双引号"",否则会导致不能打开带有空格文件名的文件。此处仅修改了`/usr/share/applications/sublime-text.desktop`，但可以正常使用了。          

经过以上步骤我们能在Sublime中输入中文了。       

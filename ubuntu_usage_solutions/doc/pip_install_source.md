# pip 换源、安装、升级，pip超时,报错 解决方案

**背景**：最近国内下载安装pip的速度异常的慢，超时情况出现！      
**原因**：默认pip的镜像下载地址链接速度慢。    
**解决方案**：通过跟换下载的镜像地址来提高速度。    
**推荐使用豆瓣源**    
亲测炒鸡好用！    

1. 下载`pip`源文件      
[本地源文件](../pkgs/pip-9.0.1.tar.gz)     
[pip官网](https://pypi.python.org/pypi/pip)      
下载`Type`为`Source`的包`pip-9.0.1.tar.gz`     
2. 解压`pip-9.0.1.tar.gz`包      
 切换到pip包所在目录，执行解压命令:     
```bash     
tar -zxvf pip-9.0.1.tar.gz     
```     
3. 运行`pip`的`setup.py`文件      
进入解压后的`pip`文件目录，执行     
```bash     
sudo python setup.py install     
```     
4. 配置新的`pip`安装下载镜像地址      
在`～`目录下建一个`.pip`的目录：     
```bash     
mkdir ~/.pip     
# 创建编辑一个pip.conf文件：     
gedit ~/.pippip.conf     
# 在文件中写入以下内容，保存并关闭：     
[global]       
timeout = 6000       
index-url = https://pypi.doubanio.com/simple/       
trusted-host = pypi.doubanio.com      
[install]       
use-mirrors = true       
mirrors = https://pypi.doubanio.com/simple/       
```
原理分析：这里使用了豆瓣`(doubanio)`的镜像在下载地址，在大陆下载的速度惊人。   
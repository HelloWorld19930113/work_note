# sublime text3 c++ 开发环境搭建 
为了更好的使用`sublime`开发`c++`程序，安装强大的`SublimeClang`插件。`SublimeClang`是`Sublime Text`中唯一的`C/C++`自动补全插件，功能强大，自带语法检查功能，不过最近作者已经停止更新了，目前只能在`Sublime
 Text 2`的`Package Control`中可以找到并自动安装，在`SublimeText 3`中只能手动通过源码安装，其代码在`https://github.com/quarnster/SublimeClang`中。具体安装步骤如下：     
1. 安装相关软件    
```bash
sudo apt-get install cmake build-essential clang git
cd ~/.config/sublime-text-3/Packages
git clone --recursive https://github.com/quarnster/SublimeClang SublimeClang
cd SublimeClang
cp /usr/lib/x86_64-linux-gnu/libclang-3.4.so.1 internals/libclang.so      #这一步很重要，如果你的clang库不是3.4版本的话，请将对应版本的库拷贝到internals中
cd src
mkdir build
cd build
cmake ..
make
```
一切成功的话将会在`SublimeClang/internals`目录中生成`libcache.so`库文件。         

重启`Sublime Text`，然后按快捷键`Ctrl + "反撇号"`(`Esc`下面那个键)打开自带的控制输出，看看有没有错误，如果没有错误就说明一切`OK`了。接下来就是配置自己的文件了。          
按下`ctrl + shift + p`快捷键，在弹出的输入框中输入`sublimeclang settings` ，然后选择带`User`那一行，在打开的文件中输入如下信息：            
```
{
    "show_output_panel": false,
    "dont_prepend_clang_includes": true,
    "inhibit_sublime_completions": false,
     "parse_status_messages": false,
     "show_visual_error_marks": false,
     "show_status": false,

    "options":
    [
        "-std=gnu++11",
        "-isystem", "/usr/include",
        "-isystem", "/usr/include/c++/*",
        "-isystem", "/usr/include/c++/4.8",
        "-isystem", "/usr/include/c++/4.8/*",
        "-isystem", "/usr/include/boost",
        "-isystem", "/usr/include/boost/**",
        "-isystem", "/usr/lib/gcc/x86_64-linux-gnu/4.8/include",
        "-isystem", "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/*"
    ]
}
```
**注释**：我的`gcc`版本为`4.8`，如果你的不是请替换对应的版本，在`#include`相应的头文件后保存当前文件，在接下来的操作中将更快的提示所包含在头文件的函数或者变量。          

## 推荐必装扩展插件

根据`Package Control`上的流行度统计，推荐安装以下扩展插件：      

|Package Name |    Description|
|----|----|
|Emmet   |Emmet (ex-Zen Coding) for Sublime Text|
|SublimeLinter |  Interactive code linting framework for Sublime Text 3|
|SideBarEnhancements |Enhancements to Sublime Text sidebar. Files and folders|
|BracketHighlighter | Bracket and tag highlighter for Sublime Text|
|SublimeCodeIntel |   Full-featured code intelligence and smart autocomplete engine|
|HTML5 |  HTML5 bundle for Sublime Text|
|Alignment |  Easy alignment of multiple selections and multi-line selections|
|Git |Plugin for some git integration into sublime text|
|DocBlockr|   Simplifies writing DocBlock comments in Javascript, PHP, CoffeeScript, Actionscript, C & C++|
|ConvertToUTF8 |  A Sublime Text 2 & 3 plugin for editing and saving files encoded in GBK, BIG5, EUC-KR, EUC-JP, |Shift_JIS, etc|
|All Autocomplete|    Extend Sublime autocompletion to find matches in all open files of the current window|
|Markdown Preview |   markdown preview and build plugin for sublime text 2/3|
|MarkdownEditing| Powerful Markdown package for Sublime Text with better syntax understanding and good color schemes.|

简单分门别类地介绍一下主要的扩展插件的用途：          

开发类：Emmet就是大名鼎鼎`的Zen Coding`；`BracketHighlighter`能高亮匹配的一对括号；`DocBlockr`能自动生成类似`JavaDoc`的注释；`auto-save`在`Ctrl+B`编译时自动保存文件。          
增强类：`SideBarEnhancements`能丰富侧边栏的导航菜单；`ConvertToUTF8`支持编译中文等文件；`SyncedSideBar`实时更新侧边栏中的文件夹和文件。   
文档类：用`ST`写`Markdown`也很方便，安装`Markdown Preview`和`MarkdownEditing`两个插件（注意：一定要全局配置`ignored_packages: [“Markdown”]`禁用`ST`默认的`Markdown`插件），就能在`ST`内和浏览器中实时预览`md`文件的渲染效果；`FileDiffs`对比两个文件的差异。       
主题类：`Soda`是排名最高的`Theme`，而`SpaceGray`则是排名最高的`Scheme`，用起来效果的确不错；`Table Editor`也是必装的，上面的表格就是用它编译出来的，操作类似于`Emacs`下的`Orgmode`中的表格，但要注意的几点是：    

- 通过Ctrl+Shift+P->Table Editor: Enable for current view开启。       
- 先输入标题行，回车后在第二行输入|-后，按tab键就将进入Table编辑模式。     
- 表格必须与前面输入的文字之间有空行，否则表格会被当成普通文字渲染。     

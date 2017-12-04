[TOC]

# ROS的launch文件

ROS提供了一个同时启动节点管理器（master）和多个节点的途径，即使用launch文件（launch file）。事实上，在ROS功能包中，launch文件的使用是非常普遍的。任何包含两个或两个以上节点的系统都可以利用launch文件来指定和配置需要使用的节点。launch文件通常的命名方案是以.launch为后缀，launch文件实际上是XML文件。launch文件一般存储在package的launch文件夹下。

##使用launch文件

(1) 运行launch文件

```bash
$ roslaunch package_name launch_file_name
Eg: 
$ rroslaunch turtle_tf turtle_tf_demo.launch
```
Tip1: rosrun只能运行一个nodes， roslaunch可以同时运行多个nodes.
Tip2: 为便于执行，每个node最好是相互独立的。
Tip3: launch文件可以不被包含于package中。此时，只需指出该launch文件的绝对路径，即可运行。

```bash
$ roslaunch absolute_path
Eg：
$ roslaunch /opt/ros/indigo/share/turtle_tf/launch/turtle_tf_demo.launch
```

（2）详细显示(request verbosity)

```bash
$ roslaunch -v package_name launch_file_name
```

（3） 结束launch文件
ctrl+c


## 创建launch文件——launch文件的格式
1. launch文件一般以.launch后缀作为文件名，放在package的launch文件夹下。最简单的launch文件可以仅包含几个nodes。

2. launch文件是XML格式的，每个XML文件都必须要包含一个root element。root element由一对launch标签定义：

```xml
<launch> 
	… 
</launch>
```
`launch文件`中的其他`elements`必须都在这一对`tags`之间。

3. launch文件的核心是一系列`node elements`，每个`node element`启动一个`node`。`node element`看起来如下：

```xml
<!-- 在节点标签末尾的斜杠“/”是必须的，但很容易忘。-->
<node pkg=”package-name” type=”executable-name” name=”node-name”/>

<!-- 你也可以这样显式地给出结束标签:(如果该node中有其他tags，则必须使用这种形式) -->
<node pkg=”…” type=”…” name=”…”></node>
```

如果该节点有子节点，例如 `remap` 或者 `param` 元素，那么该显式结束标签是必不缺少的。
一个node element包含三个必须的属性：`pkg, type, name`。`pkg`和`type`属性指出ROS应该运行哪个`pkg`中的哪个`node`，注意：此处的`type`是可执行文件的名称，而`name`则是可以任意给出的，它覆盖了原有文件中ros::init指定的`node name`。

4. node 日志文件（log file）
运行roslaunch和用rosrun运行单个节点的区别之一是，默认情况下，roslaunch运行的nodes的标准输出会重定向到log file，不显示在控制台。该日志文件的名称是：
**`~/.ros/log/run_id/node_name-number-stout.log`**
其中，run_id是master启动后生成的特殊标识符，number是表示nodes数量的整数。如，turtlesim-1-stdout.log; teleop_key-3-stdout.log。
 
5. 输出到控制台
用output属性， **`output=”screen”`**；这种方法仅显示一个node。

若显示所有nodes的输出，用`--screen`命令行。

```bash
$ roslaunch --screen package_name launch_file_name
```

如果正在运行的文件没有输出，可以查看该node属性集中是否有 output=”screen”.

6. 要求重新启动(request respawning)

开启所有nodes后，roslaunch会监视每个node，记录那些仍然活动的nodes。对于每个node，当其终止后，我们可以使用respawn属性要求roslaunch重启该node。
```
respawn=”true”
```

7. 必需存在的nodes

`required`属性与`respawn`属性是互斥的，不能同时对同一个node使用。
```
required=”true”`
```
当一个required node终止后，所有其他的nodes都会终止，并退出。这种命令有时很有用。比如，当一个很重要的node失败后，整个会话都会被扔掉，那些加上了`respawn`属性的nodes也会停止。

8. 在独立的窗口运行各nodes

我们在各自的termin运行rosrun node_name；但是运行roslaunch时，所有的nodes共用一个相同的terminal，这对于那些需要从控制台输入的nodes很不方便。这时可以使用launch-prefix属性。
```
launch-prefix=”command-prefix”

Eg：launch-prefix=”xterm -e”
等价于：
xterm -e rosrun turtlesim turtle_teleop_key
```

xterm 命令表示新建一个terminal； -e参数告诉xterm执行剩下的命令行。
当然，launch-prefix属性不仅仅限于xterm。它可用于调试（通过gdb或valgrind），或用于降低进程的执行顺序（通过nice）。


## 在namespace中执行nodes

为node设置默认的namespace的常用方法——被称为“pushing down into a namespace”的进程，用于launch文件，并在其node element中指定ns属性。
```
ns=”namespace”
```
launch文件中的node names是relative names。同一个launch文件中，允许不同namespace中出现相同的node names。Roslaunch要求node names必须是base names——不指定任何namespaces的relative names；如果node element中出现node name为global name，则会报错。

 
## 重映射names（remapping names）

除了解析relative names和private names，ROS也支持重映射，用于修改nodes当前使用的名称。
重映射相当于换名，每次重映射需提供一个original name和一个new name。每次node使用它的original name, ROS client library都会将其替换为remapping name。
创建remapping name两种方法：

1. 对于单个node，在命令行进行remapping(remap对象可以是node，topic等)。
```
	original-name:=new-name

	Eg: $ rosrun turtlesim turtlesim_node turtle1/pose:=tim
```

2. 在launch文件内remap names，使用remap element
```
  <remap from=”original_name” to “new_name”>
```
如果remap出现在launch文件开头，作为launch文件的子元素，则该remapping将被用于随后所有的nodes。如果remap作为某个node的子元素，则只用于该节点。

Eg：
```xml
<node pkg=”turtlesim” type=”turtlesim_node” name=”turtle1”>
	<remap from =”turtle1/pose” to “tim”>
</node>
```

注意：在ROS进行remapping之前，remaping的所有name，包括original和new names，都将被解析为global names。所以，remapping之后所有的名字通常都是relative names。

## 其他的launch elements

5.1 including其他文件

为包含其他launch文件，包括这些launch文件的所有nodes和parameters，用include element。

<include file=”path-to-launch-file”>

这种情况下，file属性必须写出该launch文件的全部路径名称，显得很繁琐。因此，常用

<include file=”$(find package_name)/launch_file_name”/>

注意，执行该launch文件时，roslaunch会搜索该package下的所有子目录；因此，必须给出package_name。此外，include也支持ns属性，将它的内容放进指定的namespace。

<include file=”...” ns=”namespace_name”/>


5.2 Launch arguments

为便于launch文件重构，roslaunch支持launch arguments,也成为arguments或者args，类似于局部变量。

注意：尽管argument和parameter有时可互换，但他们在ROS中的意义完全不同。Parameters是ROS系统使用的数值，存在parameter server上，nodes可通过ros::param::get函数编程得到，用户可通过rosparam获取。与之不同，arguments仅在launch文件内部有意义，nodes不能直接获取它们的值。

(1)声明argument

<arg name=”arg_name”>

(2)指定argument的值

Launch文件中的每个argument都必须有指定值。赋值方法有好几种。

第一种，在命令行赋值

$ roslaunch package_name launch_file_name arg-name:=arg_value

第二种，在声明argument时赋值

<arg name=”arg_name” default=”arg_name”/>

<arg name=”arg_name” value=”arg_name”/>

上面两行的区别在于，命令行参数可以覆盖default，但是不能重写value的值。

(3)获取变量值

一旦声明某个argument并赋值后，我们可以通过arg使用该argument.

$(arg arg-name)

如果该行出现，roslaunch将会用给定arg-name的值替换其左边的值。

(4)将argument值传给included launch文件

<include file=”path-to-file”>

<arg name=”arg_name” value=”arg_value”/>

......

</include>

若在launch文件中，launch文件及其包含的launch文件出现出现相同的arguments，则需在launch文件及included launch文件中同时写：

<arg name=”arg_name” value=”$(arg arg_name)”/>

第一个arg_name表示indluded launch文件中的argument,第二个arg_name表示launch文件中的argument.其结果是指定的argument在launch文件及included launch文件中都有相同的值。

 

5.3 创建groups

Group element可以再大型的launch文件中将指定的nodes组织起来。它有两个用处：

其一，group可以将几个nodes放进同一个namespace

<group ns=”namespace”>

<node pkg=”..” .../>

<node pkg=”..” .../>

......

</group>

注意，如果grouped node已经有它自己的namespace，并且是relative name，那么该node的namespace是其relative name，并以group namespace为后缀。

其二，group可以同时启动或者终止一组nodes。

<group if=”0 or 1”>

......

</group>

如果该属性的值是1，一切正常；如果该属性的值为0，那么group内所有的nodes都不会运行。

同理，除了if，还有unless。

<group unless=”0 or 1”>

......

</group>

注意，这些属性的合法值只有0和1.

另外，group element中只能使用ns，if，unless这三个属性。











 
5. 节点属性之命名空间

ns=”namespace”，前提是节点的代码在创建 ros::Pbulisher 和 ros::Subccriber 对象时使用了像 turtle1/pose 这样的相对名称 (而不是/turtle1/pose 这样的全局名称)。同样，launch文件中的节点名称是相对名称。例如<node name="turtlesim_node" pkg="turtlesim " type="turtlesim_node" ns="sim1" />而不能是
node name="/turtlesim_node" ，可以这么不准确的理解：两只连体小龟，他们做一件事是同时在做，比如让他们以不同的速度前进,他们做不到，若做手术分开，把他俩放在不同的房间(命名空间）他俩真正地独立起来，这样每个小龟可以接受不同的命令去做事，而且互不干扰。动文件默认命名空间是全局命名空间“/”，因此，节点的默认命名空间就解析为/sim1，所以对应的全局名称即/sim1/turtlesim_node。
 
5. 名称重映射

重映射是基于替换的思想，每个重映射包含一个原始名称和一个新名称。每当节点使用重映射中的原始名称时，ROS客户端库就会将它默默地替换成其对应的新名称。例如，运行一个 turtlesime 的实例， 如果想要把海龟的姿态数据发布到话题/tim 而不是/turtle1/pose，就可以使用如下命
令：rosrun turtlesim turtlesim_node turtle1/pose:=tim 通过launch文件的方式，只需在launch文件内使用重映射（remap）元素即可：<remap from=”turtle1/pose”to ”tim”/>

 
5. 包含其他文件

如果想在launch文件中包含其他launch文件的内容（包括所有的节点和参数），可以使用`include元素`。、

```xml
<include file=”$(find package-name)/launch_file_name”>
	<arg name="..." value="..."/>
</include>
```
由于直接输入路径信息很繁琐且容易出错，大多数`include元素`都使用`find命令`搜索功能包的位置来替代直接输入路径。
 
6. 启动参数

roslaunch还支持启动参数，有时也简称为参数甚至args。在ROS中prarmeter和argument 是不同的，虽然翻译一样。parameter是运行中的ROS系统使用的数值，存储在参数服务器（parameter server）中，每个活跃的节点都可以通过 ros::param::get 函数来获取parameter的值，用户也可以通过rosparam来获得parameter的值而argument只在launch文件内才有意义他们的值是不能被节点直接获取的。

```xml
<arg name=”arg-name” default=”arg-value”/>
与
<arg name=”arg-name” value=”arg-value”/>
```
两者的唯一区别在于`命令行参数`（比如roslaunch xx yy.launch dd:=t中的dd就是命令行参数)可以覆盖dd参数的`default值`，但是不能覆盖`value值`。

以下是引用参数的方法：

```xml
$(arg arg-name)
```
在每个该替换出现的地方，roslaunch 都将它替换成参数值。


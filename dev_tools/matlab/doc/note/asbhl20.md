# `asbhl20` 仿真模型    

1. `MATLAB`的`mu toolbox`提供了`PDF`文档,查看一下有个`Robust control`是以`airplane`为例。  
2. 就是启动`MATLAB`后,在有提示符`>>`的`command window`下,输入`asbhl20`就可以启动了,....   
启动编译之后，`MATLAB`会生成一个启动脚本——`runfg.bat`，根据对应的`bash`做少量修改就可以得到下面的启动脚本 `run.sh`：    
```bash
#!/bin/bash 

cd /usr/games

FG_ROOT=/usr/games/data
export FG_ROOT

FG_SCENERY=/usr/games/Scenery:/usr/games/WorldScenery
export FG_SCENERY

fgfs --aircraft=HL20 --fdm=null --native-fdm=socket,in,30,127.0.0.1,5502,udp --native-ctrls=socket,out,30,127.0.0.1,5505,udp --fog-fastest --disable-clouds --start-date-lat=2004:06:01:09:00:00 --disable-sound --in-air --enable-freeze --airport=KSFO --runway=10L --altitude=7224 --heading=113 --offset-distance=4.72 --offset-azimuth=0 --fg-root=/usr/games/data
```

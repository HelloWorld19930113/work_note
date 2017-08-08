#sd卡开发中遇到的问题
##cache
***
1 在发送命令过程中，传输数据开始后，如果数据准备完毕就会更新SD_SATA寄存器。可是我在传输数据达到一定数量之后就会出现状态位更新超时(其实就是状态位很长时间不会被更新)。
	出现这个现象的原因是：在向SD_ARG寄存器中写入值之后没有进行适当的延时，因此向SD_CMD寄存器中写入数据的时候并没有

```C
mmc_base->arg = cmd->cmdarg;
	*rtems_task_wake_after(5);*               /*NOTE:necessary*/
	mmc_base->cmd = ( ((cmd->cmdidx )& 0x3f) << 24 ) | flags ;               /* @ Send the cmd */
```


[TOC]
# Board 初始化
  为了理清应用程序从执行第一条BSP代码到第一个应用程序task执行的过程。下面是整个程序的执行步骤：
  ![](https://raw.githubusercontent.com/gzj2013/markdown_src/master/board_start.png)
  其中，Allocate RAM to workspace & heap中就是`bsp_work_area_initialize()`函数函数实现的主要功能。
  
  
  该图描述了汇编语言启动代码到boot_card.c的流程。经过C库、RTEMS、设备驱动初始化并上下文切换到第一个应用task。之后程序正常运行，直到它调用exit()、rtems_shutdown_executive()或其他一些程序终止指令。
  
## 写在前面：需要注意的 ##
  BSP中的配置表是在调试期间默认生成的，不应该在代码中显式的引用。rtems提供了一个helper函数来访问单个域。
  

## 启动代码-start.S ##
  代码位于c/src/lib/libbsp/arm/shared/start/start.S
  这是第一部分被执行的代码。负责充分地初始化处理器和主板以执行BSP的其余部分，主要有：
  
  - 初始化堆栈；
  - 清零未初始化的数据段.bss；
  - 禁用外部中断
  - 从ROM复制已初始化的数据到RAM中。
  <font color=#FF0000>根据一般的经验，在汇编中的启动代码只做必须的工作(代码最少化)，然后创建C运行环境，使用C代码执行完整的初始化序列。 </font>因此，汇编代码调用`boot_card()`函数切换到C语言运行环境并结束汇编指令。
### 程序如何知道start.S就是要执行的第一部分代码？###
  与程序起始地址相关联的label(标签)通常是start。start目标文件是连接到程序映像的第一个目标文件，因此就保证了start代码在.text代码段中的偏移地址为0，一般使用链接脚本.lds来指定镜像文件的存放位置。
```C
    SECTIONS {
	.start : ALIGN_WITH_INPUT {
		bsp_section_start_begin = .;
		KEEP (*(.bsp_start_text))
		KEEP (*(.bsp_start_data))q
		bsp_section_start_end = .;
	} > REGION_START AT > REGION_START
	bsp_section_start_size = bsp_section_start_end - bsp_section_start_begin;
	...
	
   }
```
  
## 初始化代码 ##
   通过分析这部分代码来找到应用层和驱动层(或者内核层)之间的连接点。
   以`raspberrypi`开发板来分析。第一个被执行的文件是start.S文件，经过一系列的初始化之后，就会执行到下述代码块：

```C
	/* start.S */
	...

	/* Branch to start hook 1 */
	bl	bsp_start_hook_1

	/* Branch to boot card */
	mov	r0, #0
	bl	boot_card

	...

```
   `boot_card()`是从汇编代码调用到C代码，这个函数用于初始化整个系统框架。该函数会按照一定的顺序来交叉调用RTEMS和BSP中的函数来实现系统初始化，将可重用代码最大化，并且用C语言实现尽可能多的BSP代码。

   一般在boot_card()之前需要调用bsp_start_hook1()函数。该函数的主要作用是：

   - 拷贝各个段(代码段、数据段、bss段)；
   - 内存管理初始化；
   - 清空bss段；
   - 初始化console的显示模块(显示器帧缓存)；

```C
void BSP_START_TEXT_SECTION bsp_start_hook_1(void)
{
  bsp_start_copy_sections();
  bsp_memory_management_initialize();
  bsp_start_clear_bss();

  rpi_video_init();
}
```
   `bsp_start_hook1()`函数之后就会调用`boot_card()`函数。该函数最终会初始化线程运行环境，不再`return`。

```C
/*
 *  This is the initialization framework routine that weaves together
 *  calls to RTEMS and the BSP in the proper sequence to initialize
 *  the system while maximizing shared code and keeping BSP code in C
 *  as much as possible.
 */
void boot_card( const char *cmdline)
{
  rtems_interrupt_level  bsp_isr_level;

  /*
   *  Make sure interrupts are disabled.
   */
  (void) bsp_isr_level;
  rtems_interrupt_local_disable( bsp_isr_level );

  bsp_boot_cmdline = cmdline;

  rtems_initialize_executive();

  /***************************************************************
   ***************************************************************
   *  APPLICATION RUNS NOW!!!  We will not return to here!!!     *
   ***************************************************************
   ***************************************************************/
}
```

   主要调用的函数为`rtems_initialize_executive()`，该函数实现的主要功能是：

   - **执行已经注册的系统初始化函数**；
   - 将系统状态设置为已启动运行；
   - 如果定义了系统使用多处理器，则为多处理器执行多任务做准备；
   - 启动多任务管理器；

   **NOTE**：对于多任务管理器的启动函数，一般是作为初始化的一部分来调用，是在系统初始化的单任务操作的最后一步调用。之后不再进行单任务操作。

```C
void rtems_initialize_executive(void)
{
  const rtems_sysinit_item *item;

  /* Invoke the registered system initialization handlers */
  RTEMS_LINKER_SET_FOREACH( _Sysinit, item ) {
    ( *item->handler )();
  }

  _System_state_Set( SYSTEM_STATE_UP );

  _SMP_Request_start_multitasking();

  _Thread_Start_multitasking();

  /*******************************************************************
   *******************************************************************
   *******************************************************************
   ******                 APPLICATION RUNS HERE                 ******
   ******              THE FUNCTION NEVER RETURNS               ******
   *******************************************************************
   *******************************************************************
   *******************************************************************/
}
```
### 都有哪些函数被注册了？ ###
   `rtems_initialize_executive()`函数中对已注册的系统初始化函数依次调用执行。那么都有哪些函数在这里被执行了？
   
```C
	bsp_work_area_initialize
	bsp_start
	bsp_libc_init
	bsp_predriver_hook
	bsp_postdriver_hook
	rtems_initialize_data_structures
	rtems_initialize_before_drivers
	rtems_initialize_device_drivers
```
   以上函数在这里将会被全部调用执行。


   

   RTEMS 系统使用CONFIGURE_INIT_TASK_ENTRY_POINT来配置主函数的入口点。



   最接近应用层Init的一个函数：

```C
/*
 *  _RTEMS_tasks_Initialize_user_tasks_body
 *
 *  This routine creates and starts all configured user
 *  initialization threads.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 */

void _RTEMS_tasks_Initialize_user_tasks_body( void )
{
	uint32_t                          index;
	uint32_t                          maximum;
	rtems_id                          id;
	rtems_status_code                 return_value;
	rtems_initialization_tasks_table *user_tasks;
	bool                              register_global_construction;
	rtems_task_entry                  entry_point;

	/*
	*  Move information into local variables
	*/
	user_tasks = Configuration_RTEMS_API.User_initialization_tasks_table;
	maximum    = Configuration_RTEMS_API.number_of_initialization_tasks;

	/*
	*  Verify that we have a set of user tasks to iterate
	*/
	if ( !user_tasks )
		return;

	register_global_construction = true;

	/*
	*  Now iterate over the initialization tasks and create/start them.
	*/
	for ( index=0 ; index < maximum ; index++ ) {
		return_value = rtems_task_create(
			user_tasks[ index ].name,
			user_tasks[ index ].initial_priority,
			user_tasks[ index ].stack_size,
			user_tasks[ index ].mode_set,
			user_tasks[ index ].attribute_set,
			&id
		);
		
		if ( !rtems_is_status_successful( return_value ) )
			_Terminate( INTERNAL_ERROR_RTEMS_API, true, return_value );

		entry_point = user_tasks[ index ].entry_point;

		if ( register_global_construction && entry_point != NULL ) {
			register_global_construction = false;
			entry_point = (rtems_task_entry) _Thread_Global_construction;
		}

		return_value = rtems_task_start(
			id,
			entry_point,
			user_tasks[ index ].argument
		);
		
		if ( !rtems_is_status_successful( return_value ) )
			_Terminate( INTERNAL_ERROR_RTEMS_API, true, return_value );
	}
}
```


引发系统终止的函数_Terminate()
```c
/**
 * @brief Initiates system termination.
 *
 * This routine is invoked when the application or the executive itself
 * determines that a fatal error has occurred or a final system state is
 * reached (for example after exit()).
 *
 * The first action is to disable interrupts.
 *
 * The second action of this function is to call the fatal handler of the user
 * extensions.  For the initial extensions the following conditions are
 * required
 * - a valid stack pointer and enough stack space,
 * - a valid code memory, and
 * - valid read-only data.
 *
 * For the initial extensions the read-write data (including BSS segment) is
 * not required on single processor configurations.  On SMP configurations
 * however the read-write data must be initialized since this function must
 * determine the state of the other processors and request them to shut-down if
 * necessary.
 *
 * Non-initial extensions require in addition valid read-write data.  The BSP
 * may install an initial extension that performs a system reset.  In this case
 * the non-initial extensions will be not called.
 *
 * Once all fatal handler executed the error information will be stored to
 * _Internal_errors_What_happened and the system state is set to
 * SYSTEM_STATE_TERMINATED.
 *
 * The final step is to call the CPU specific _CPU_Fatal_halt().
 *
 * @param[in] the_source The fatal source indicating the subsystem the fatal
 * condition originated in.
 * @param[in] is_internal Indicates if the fatal condition was generated
 * internally to the executive.
 * @param[in] the_error The fatal error code.  This value must be interpreted
 * with respect to the source.
 *
 * @see rtems_fatal_error_occurred() and rtems_fatal().
 */
void _Terminate(
  Internal_errors_Source  the_source,
  bool                    is_internal,
  Internal_errors_t       the_error
) RTEMS_NO_RETURN;
```

```C

RTEMS_SYSINIT_ITEM(
  rtems_initialize_data_structures,
  RTEMS_SYSINIT_DATA_STRUCTURES,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
```
一个重要的系统初始化函数。rtems_initialize_data_structures().

```C
static void rtems_initialize_data_structures(void)
{
  /*
   *  Dispatching and interrupts are disabled until the end of the
   *  initialization sequence.  This prevents an inadvertent context
   *  switch before the executive is initialized.
   *
   *  WARNING: Interrupts should have been disabled by the BSP and
   *           are disabled by boot_card().
   */

  /*
   * Initialize any target architecture specific support as early as possible
   */
  _CPU_Initialize();

   _Thread_Dispatch_initialization();

  _User_extensions_Handler_initialization();
  _ISR_Handler_initialization();

  /*
   * Initialize the internal support API and allocator Mutex
   */
  _Objects_Information_table[OBJECTS_INTERNAL_API] = _Internal_Objects;

  _API_Mutex_Initialization( 2 );
  _API_Mutex_Allocate( &_RTEMS_Allocator_Mutex );
  _API_Mutex_Allocate( &_Once_Mutex );

  _Watchdog_Handler_initialization();
  _TOD_Handler_initialization();

  _Thread_Handler_initialization();

  _Scheduler_Handler_initialization();

  _SMP_Handler_initialize();

  _CPU_set_Handler_initialization();

/* MANAGERS */
  _RTEMS_API_Initialize();

  _Extension_Manager_initialization();

  _POSIX_API_Initialize();

  _System_state_Set( SYSTEM_STATE_BEFORE_MULTITASKING );

  /*
   *  No threads should be created before this point!!!
   *  _Thread_Executing and _Thread_Heir are not set.
   *
   *  At this point all API extensions are in place.  After the call to
   *  _Thread_Create_idle() _Thread_Executing and _Thread_Heir will be set.
   */
  _Thread_Create_idle();

  /*
   *  Scheduling can properly occur now as long as we avoid dispatching.
   */
}

```



> *BSP and Device Driver Development Guide*



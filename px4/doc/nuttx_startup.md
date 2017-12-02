# Nuttx系统启动流程分析

2014-08-05 15:38:15

## 写在前面
  最近弄PX4飞控，使用了Nuttx实时操作系统。相比跑裸机而言，操作系统的复杂度是不一样的。至少来说在初期复杂度增加的可不止一点点。
  然而在初期，最痛苦的还是Nuttx的资料不够，简直是稀少。所以也就只能一点点地啃源码以期望能够理清Nuttx的系统流程与基本轮廓。毕竟跑系统不比跑裸机，自己用到的资源一目了然。引入了C++之后有些东西就更加显得不是那么的清晰了。
  其实到这个文档写完《1.2 命令》的时候我才发现我犯了一个很严重的错误：我一直都在以裸奔的方式在分析PX4，这给我带来了极大的麻烦，使得我一开始就在代码中找寻应用启动的过程。其实Nuttx只是启动了系统而已，对于应用，那是在脚本中启动的。
## 走流程
  Nuttx系统我并没有完全搞透，只是从系统的第一条代码到我们的飞控启动的基本轮廓理清了而已，各中细节还得深入分析。
  那么我们先来讲解系统启动后的第一条代码即入口。
### 入口
  PX4使用的是STM32，入口自然就在stm32_start.c中，其中有一个`_start()`函数对应的是cpu的reset向量表，程序会首先从这里开始运行。`_start()`完成的主要工作有：
  
  - 配置C函数运行需要的栈空间；
  - 第1阶段(A阶段)初始化：
    - 时钟初始化；
    - 配置fpu(浮点处理单元)；
    - 配置串口console(引脚)；
    - 初始化GPIO模块；
  - 第2阶段(B阶段)初始化：
    - 清空.bss段；
  - 第3阶段(C阶段)初始化：
    - 将初始化的data段从Flash中拷贝到SRAM中；
  - 第4阶段(D阶段)初始化：
    - 提前初始化串口； 
  - 第5阶段(E阶段)初始化：
    - 初始化userspace； 
  - 第6阶段(F阶段)初始化：
    - 初始化board； 
  - 设置好空闲栈的大小并进入到`os_start()`；
 
```C
void __start(void)
{
  const uint32_t *src;
  uint32_t *dest;
 
#ifdef CONFIG_ARMV7M_STACKCHECK
  /* Set the stack limit before we attempt to call any functions */
 
  __asm__ volatile ("sub r10, sp, %0" : : "r" (CONFIG_IDLETHREAD_STACKSIZE - 64) : );
#endif
 
  /* Configure the uart so that we can get debug output as soon as possible */
 
  stm32_clockconfig();
  stm32_fpuconfig();
  stm32_lowsetup();
  stm32_gpioinit();
  showprogress('A');
 
  /* Clear .bss.  We'll do this inline (vs. calling memset) just to be
   * certain that there are no issues with the state of global variables.
   */
 
  for (dest = &_sbss; dest < &_ebss; )
    {
      *dest++ = 0;
    }
 
  showprogress('B');
 
  /* Move the intialized data section from his temporary holding spot in
   * FLASH into the correct place in SRAM.  The correct place in SRAM is
   * give by _sdata and _edata.  The temporary location is in FLASH at the
   * end of all of the other read-only data (.text, .rodata) at _eronly.
   */
 
  for (src = &_eronly, dest = &_sdata; dest < &_edata; )
    {
      *dest++ = *src++;
    }
 
  showprogress('C');
 
  /* Perform early serial initialization */
 
#ifdef USE_EARLYSERIALINIT
  up_earlyserialinit();
#endif
  showprogress('D');
 
  /* For the case of the separate user-/kernel-space build, perform whatever
   * platform specific initialization of the user memory is required.
   * Normally this just means initializing the user space .data and .bss
   * segments.
   */
 
#ifdef CONFIG_NUTTX_KERNEL
  stm32_userspace();
  showprogress('E');
#endif
 
  /* Initialize onboard resources */
 
  stm32_boardinitialize();
  showprogress('F');
 
  /* Then start NuttX */
 
  showprogress('\r');
  showprogress('\n');
  os_start();
 
  /* Shoulnd't get here */
 
  for(;;);
}
```
   我们可以先通过注释大概了解这里都做了哪些事情。在这之前还有一个程序在跑：bootloader。这个不是我们今天讨论的话题。在最后我们看到调用了一个os_start()函数也就是接下来系统将启动，对应的源文件是os_start.c：
```C
void os_start(void)
{
  int i;
 
  slldbg("Entry\n");
 
  /* Initialize RTOS Data ***************************************************/
  /* Initialize all task lists */
 
  dq_init(&g_readytorun);
  dq_init(&g_pendingtasks);
  dq_init(&g_waitingforsemaphore);
#ifndef CONFIG_DISABLE_SIGNALS
  dq_init(&g_waitingforsignal);
#endif
#ifndef CONFIG_DISABLE_MQUEUE
  dq_init(&g_waitingformqnotfull);
  dq_init(&g_waitingformqnotempty);
#endif
#ifdef CONFIG_PAGING
  dq_init(&g_waitingforfill);
#endif
  dq_init(&g_inactivetasks);
  sq_init(&g_delayed_kufree);
#if defined(CONFIG_NUTTX_KERNEL) && defined(CONFIG_MM_KERNEL_HEAP)
  sq_init(&g_delayed_kfree);
#endif
 
  /* Initialize the logic that determine unique process IDs. */
 
  g_lastpid = 0;
  for (i = 0; i < CONFIG_MAX_TASKS; i++)
    {
      g_pidhash[i].tcb = NULL;
      g_pidhash[i].pid = INVALID_PROCESS_ID;
    }
 
  /* Assign the process ID of ZERO to the idle task */
 
  g_pidhash[ PIDHASH(0)].tcb = &g_idletcb.cmn;
  g_pidhash[ PIDHASH(0)].pid = 0;
 
  /* Initialize the IDLE task TCB *******************************************/
  /* Initialize a TCB for this thread of execution.  NOTE:  The default
   * value for most components of the g_idletcb are zero.  The entire
   * structure is set to zero.  Then only the (potentially) non-zero
   * elements are initialized. NOTE:  The idle task is the only task in
   * that has pid == 0 and sched_priority == 0.
   */
 
  bzero((void*)&g_idletcb, sizeof(struct task_tcb_s));
  g_idletcb.cmn.task_state = TSTATE_TASK_RUNNING;
  g_idletcb.cmn.entry.main = (main_t)os_start;
 
  /* Set the IDLE task name */
 
#if CONFIG_TASK_NAME_SIZE > 0
  strncpy(g_idletcb.cmn.name, g_idlename, CONFIG_TASK_NAME_SIZE-1);
#endif /* CONFIG_TASK_NAME_SIZE */
 
  /* Configure the task name in the argument list.  The IDLE task does
   * not really have an argument list, but this name is still useful
   * for things like the NSH PS command.
   *
   * In the kernel mode build, the arguments are saved on the task's stack
   * and there is no support that yet.
   */
 
#if defined(CONFIG_CUSTOM_STACK) || !defined(CONFIG_NUTTX_KERNEL)
#if CONFIG_TASK_NAME_SIZE > 0
  g_idletcb.argv[0] = g_idletcb.cmn.name;
#else
  g_idletcb.argv[0] = (char*)g_idlename;
#endif /* CONFIG_TASK_NAME_SIZE */
#endif /* CONFIG_CUSTOM_STACK || !CONFIG_NUTTX_KERNEL */
 
  /* Then add the idle task's TCB to the head of the ready to run list */
 
  dq_addfirst((FAR dq_entry_t*)&g_idletcb, (FAR dq_queue_t*)&g_readytorun);
 
  /* Initialize the processor-specific portion of the TCB */
 
  up_initial_state(&g_idletcb.cmn);
 
  /* Initialize RTOS facilities *********************************************/
  /* Initialize the semaphore facility(if in link).  This has to be done
   * very early because many subsystems depend upon fully functional
   * semaphores.
   */
 
#ifdef CONFIG_HAVE_WEAKFUNCTIONS
  if (sem_initialize != NULL)
#endif
    {
      sem_initialize();
    }
 
  /* Initialize the memory manager */
 
  {
    FAR void *heap_start;
    size_t heap_size;
 
    /* Get the user-mode heap from the platform specific code and configure
     * the user-mode memory allocator.
     */
 
    up_allocate_heap(&heap_start, &heap_size);
    kumm_initialize(heap_start, heap_size);
 
#if defined(CONFIG_NUTTX_KERNEL) && defined(CONFIG_MM_KERNEL_HEAP)
    /* Get the kernel-mode heap from the platform specific code and configure
     * the kernel-mode memory allocator.
     */
 
    up_allocate_kheap(&heap_start, &heap_size);
    kmm_initialize(heap_start, heap_size);
#endif
  }
 
  /* Initialize tasking data structures */
 
#if defined(CONFIG_SCHED_HAVE_PARENT) && defined(CONFIG_SCHED_CHILD_STATUS)
#ifdef CONFIG_HAVE_WEAKFUNCTIONS
  if (task_initialize != NULL)
#endif
    {
      task_initialize();
    }
#endif
 
  /* Initialize the interrupt handling subsystem (if included) */
 
#ifdef CONFIG_HAVE_WEAKFUNCTIONS
  if (irq_initialize != NULL)
#endif
    {
      irq_initialize();
    }
 
  /* Initialize the watchdog facility (if included in the link) */
 
#ifdef CONFIG_HAVE_WEAKFUNCTIONS
  if (wd_initialize != NULL)
#endif
    {
      wd_initialize();
    }
 
  /* Initialize the POSIX timer facility (if included in the link) */
 
#ifndef CONFIG_DISABLE_CLOCK
#ifdef CONFIG_HAVE_WEAKFUNCTIONS
  if (clock_initialize != NULL)
#endif
    {
      clock_initialize();
    }
#endif
 
#ifndef CONFIG_DISABLE_POSIX_TIMERS
#ifdef CONFIG_HAVE_WEAKFUNCTIONS
  if (timer_initialize != NULL)
#endif
    {
      timer_initialize();
    }
#endif
 
  /* Initialize the signal facility (if in link) */
 
#ifndef CONFIG_DISABLE_SIGNALS
#ifdef CONFIG_HAVE_WEAKFUNCTIONS
  if (sig_initialize != NULL)
#endif
    {
      sig_initialize();
    }
#endif
 
  /* Initialize the named message queue facility (if in link) */
 
#ifndef CONFIG_DISABLE_MQUEUE
#ifdef CONFIG_HAVE_WEAKFUNCTIONS
  if (mq_initialize != NULL)
#endif
    {
      mq_initialize();
    }
#endif
 
  /* Initialize the thread-specific data facility (if in link) */
 
#ifndef CONFIG_DISABLE_PTHREAD
#ifdef CONFIG_HAVE_WEAKFUNCTIONS
  if (pthread_initialize != NULL)
#endif
    {
      pthread_initialize();
    }
#endif
 
  /* Initialize the file system (needed to support device drivers) */
 
#if CONFIG_NFILE_DESCRIPTORS > 0
#ifdef CONFIG_HAVE_WEAKFUNCTIONS
  if (fs_initialize != NULL)
#endif
    {
      fs_initialize();
    }
#endif
 
  /* Initialize the network system */
 
#ifdef CONFIG_NET
#if 0
  if (net_initialize != NULL)
#endif
    {
      net_initialize();
    }
#endif
 
  /* The processor specific details of running the operating system
   * will be handled here.  Such things as setting up interrupt
   * service routines and starting the clock are some of the things
   * that are different for each  processor and hardware platform.
   */
 
  up_initialize();
 
  /* Initialize the C libraries (if included in the link).  This
   * is done last because the libraries may depend on the above.
   */
 
#ifdef CONFIG_HAVE_WEAKFUNCTIONS
  if (lib_initialize != NULL)
#endif
    {
      lib_initialize();
    }
 
  /* IDLE Group Initialization **********************************************/
  /* Allocate the IDLE group and suppress child status. */
 
#ifdef HAVE_TASK_GROUP
  DEBUGVERIFY(group_allocate(&g_idletcb));
#endif
 
  /* Create stdout, stderr, stdin on the IDLE task.  These will be
   * inherited by all of the threads created by the IDLE task.
   */
 
  DEBUGVERIFY(group_setupidlefiles(&g_idletcb));
 
  /* Complete initialization of the IDLE group.  Suppress retention
   * of child status in the IDLE group.
   */
 
#ifdef HAVE_TASK_GROUP
  DEBUGVERIFY(group_initialize(&g_idletcb));
  g_idletcb.cmn.group->tg_flags = GROUP_FLAG_NOCLDWAIT;
#endif
 
  /* Bring Up the System ****************************************************/
  /* Create initial tasks and bring-up the system */
 
  DEBUGVERIFY(os_bringup());
 
  /* The IDLELoop**********************************************************/
  /* When control is return to this point, the system is idle. */
 
  sdbg("Beginning Idle Loop\n");
  for (;;)
    {
      /* Perform garbage collection (if it is not being done by the worker
       * thread).  This cleans-up memory de-allocations that were queued
       * because they could not be freed in that execution context (for
       * example, if the memory was freed from an interrupt handler).
       */
 
#ifndef CONFIG_SCHED_WORKQUEUE
      /* We must have exclusive access to the memory manager to do this
       * BUT the idle task cannot wait on a semaphore.  So we only do
       * the cleanup now if we can get the semaphore -- this should be
       * possible because if the IDLE thread is running, no other task is!
       */
 
      if (kmm_trysemaphore() == 0)
        {
          sched_garbagecollection();
          kmm_givesemaphore();
        }
#endif
 
      /* Perform any processor-specific idle state operations */
 
      up_idle();
    }
}
```
我们看到这里初始化了各种资源，最后进入了一个空闲任务。而在空闲任务的前面我们看到对其他任务也进行了初始化，这是在os_bringup.c文件中： NOTE:  The idle task is the only task in that has pid = 0 and sched_priority = 0.

```
int os_bringup(void)
{
  /* Setup up the initial environment for the idle task.  At present, this
   * may consist of only the initial PATH variable.  The PATH variable is
   * (probably) not used by the IDLE task.  However, the environment
   * containing the PATH variable will be inherited by all of the threads
   * created by the IDLE task.
   */

#if !defined(CONFIG_DISABLE_ENVIRON) && defined(CONFIG_PATH_INITIAL)
  (void)setenv("PATH", CONFIG_PATH_INITIAL, 1);
#endif

  /* Start the page fill worker kernel thread that will resolve page faults.
   * This should always be the first thread started because it may have to
   * resolve page faults in other threads
   */

  os_pgworker();

  /* Start the worker thread that will serve as the device driver "bottom-
   * half" and will perform misc garbage clean-up.
   */

  os_workqueues();

  /* Once the operating system has been initialized, the system must be
   * started by spawning the user initialization thread of execution.  This
   * will be the first user-mode thread.
   */

  os_start_application();

  /* We an save a few bytes by discarding the IDLE thread's environment. */

#if !defined(CONFIG_DISABLE_ENVIRON) && defined(CONFIG_PATH_INITIAL)
  (void)clearenv();
#endif

  return OK;
}
```
　　`os_start_application()`函数经过两次调用：`os_do_appstart()`->`os_start_application()`，在这个函数中创建并激活了 init() task。
　　
```C
/****************************************************************************
 * Name: os_start_application
 *
 * Description:
 *   Execute the board initialization function (if so configured) and start
 *   the application initialization thread.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

#if defined(CONFIG_INIT_ENTRYPOINT)
static inline void os_do_appstart(void)
{
  int pid;

#ifdef CONFIG_BOARD_INITIALIZE
  /* Perform any last-minute, board-specific initialization, if so
   * configured.
   */

  board_initialize();
#endif

  /* Start the application initialization task.  In a flat build, this is
   * entrypoint is given by the definitions, CONFIG_USER_ENTRYPOINT.  In
   * the protected build, however, we must get the address of the
   * entrypoint from the header at the beginning of the user-space blob.
   */

  sinfo("Starting init thread\n");

#ifdef CONFIG_BUILD_PROTECTED
  DEBUGASSERT(USERSPACE->us_entrypoint != NULL);
  pid = task_create("init", SCHED_PRIORITY_DEFAULT,
                    CONFIG_USERMAIN_STACKSIZE, USERSPACE->us_entrypoint,
                    (FAR char * const *)NULL);
#else
  pid = task_create("init", SCHED_PRIORITY_DEFAULT,
                    CONFIG_USERMAIN_STACKSIZE,
                    (main_t)CONFIG_USER_ENTRYPOINT,
                    (FAR char * const *)NULL);
#endif
  ASSERT(pid > 0);
}

#elif defined(CONFIG_INIT_FILEPATH)
static inline void os_do_appstart(void)
{
  int ret;

#ifdef CONFIG_BOARD_INITIALIZE
  /* Perform any last-minute, board-specific initialization, if so
   * configured.
   */

  board_initialize();
#endif

  /* Start the application initialization program from a program in a
   * mounted file system.  Presumably the file system was mounted as part
   * of the board_initialize() operation.
   */

  sinfo("Starting init task: %s\n", CONFIG_USER_INITPATH);

  ret = exec(CONFIG_USER_INITPATH, NULL, CONFIG_INIT_SYMTAB,
             CONFIG_INIT_NEXPORTS);
  ASSERT(ret >= 0);
}
```
  这里跟我们关系最大的就是init进程。在Linux中也有init进程，是所有应用程序进程的祖先进程，进程ID为1。这里其实也是类似的。Init进程启动了所有应用程序进程。不过这里需要注意的是<font color = #ff0000 >CONFIG_USER_ENTRYPOINT </font>是在配置文件中指定的，相当于一个宏：
<font color = #ff0000 >CONFIG_USER_ENTRYPOINT="nsh_main" </font>

```
int nsh_main(int argc, char *argv[])
{
  int exitval = 0;
  int ret;
 
  /* Call all C++ static constructors */
 
#if defined(CONFIG_HAVE_CXX) && defined(CONFIG_HAVE_CXXINITIALIZE)
  up_cxxinitialize();
#endif
 
  /* Make sure that we are using our symbol take */
 
#if defined(CONFIG_LIBC_EXECFUNCS) && defined(CONFIG_EXECFUNCS_SYMTAB)
  exec_setsymtab(CONFIG_EXECFUNCS_SYMTAB, 0);
#endif
 
  /* Register the BINFS file system */
 
#if defined(CONFIG_FS_BINFS) && (CONFIG_BUILTIN)
  ret = builtin_initialize();
  if (ret < 0)
    {
     fprintf(stderr, "ERROR: builtin_initialize failed: %d\n", ret);
     exitval = 1;
   }
#endif
 
  /* Initialize the NSH library */
 
  nsh_initialize();
 
  /* If the Telnet console is selected as a front-end, then start the
   * Telnet daemon.
   */
 
#ifdef CONFIG_NSH_TELNET
  ret = nsh_telnetstart();
  if (ret < 0)
    {
     /* The daemon is NOT running.  Report the the error then fail...
      * either with the serial console up or just exiting.
      */
 
     fprintf(stderr, "ERROR: Failed to start TELNET daemon: %d\n", ret);
     exitval = 1;
   }
#endif
 
  /* If the serial console front end is selected, then run it on this thread */
 
#ifdef CONFIG_NSH_CONSOLE
  ret = nsh_consolemain(0, NULL);
 
  /* nsh_consolemain() should not return.  So if we get here, something
   * is wrong.
   */
 
  fprintf(stderr, "ERROR: nsh_consolemain() returned: %d\n", ret);
  exitval = 1;
#endif
 
  return exitval;
}
```
  这里就相当于Linux中启动了shell，然后去执行初始化脚本。

```
int nsh_consolemain(int argc, char *argv[])
{
  FAR struct console_stdio_s *pstate = nsh_newconsole();
  int ret;
 
  DEBUGASSERT(pstate);
 
  /* Execute the start-up script */
 
#ifdef CONFIG_NSH_ROMFSETC
  (void)nsh_initscript(&pstate->cn_vtbl);
#endif
 
  /* Initialize any USB tracing options that were requested */
 
#ifdef CONFIG_NSH_USBDEV_TRACE
  usbtrace_enable(TRACE_BITSET);
#endif
 
  /* Execute the session */
 
  ret = nsh_session(pstate);
 
  /* Exit upon return */
 
  nsh_exit(&pstate->cn_vtbl, ret);
  return ret;
}

```
  在这里实际上`nsh_initscript()`和`nsh_session()`都会去执行命令，但`nsh_session()`从代码上看更像是Linux的命令行参数解析， `nsh_initscript()`会去执行/etc/init.d/rcS脚本。

```
int nsh_initscript(FAR struct nsh_vtbl_s *vtbl)
{
  static bool initialized;
  bool already;
  int ret = OK;
 
  /* Atomic test and set of the initialized flag */
 
  sched_lock();
  already     = initialized;
  initialized = true;
  sched_unlock();
 
  /* If we have not already executed the init script, then do so now */
 
  if (!already)
    {
      ret = nsh_script(vtbl, "init", NSH_INITPATH);
    }
 
  return ret;
}

```
  这里我们对`nsh_script()`的第２个参数NSH_INITPATH展开：

``` C
# define NSH_INITPATH "/etc/init.d/rcS"
```
  所以接下来就应该转到启动脚本。需要提前说明的是：之后我们所需要的很多参数，包括PX4飞控主线程都是在脚本中以命令的方式启动的。所以说如果抛开脚本，单纯看源码是看不出PX4飞控主线程是如何启动的，因为这些内容根本就不在源码中。
  
### 命令
  说到脚本，我们当然知道脚本中最多的是命令。在Nuttx中命令跟Linux还是有很大的区别的。 看到这里，可能有的人会想Nuttx到底跟Linux有什么关系。其实Nuttx只是跟Linux某些地方相似而已。而文中多次提到Linux，只是为分析Nuttx提供参考，仅此而已。
  通过前面的分析，我们很容易想到接下来的代码分析将从`nsh_script()`开始。

```
int nsh_script(FAR struct nsh_vtbl_s *vtbl, FAR const char *cmd,
               FAR const char *path)
{
  char *fullpath;
  FILE *stream;
  char *buffer;
  char *pret;
  int ret = ERROR;
 
  /* The path to the script may be relative to the current working directory */
 
  fullpath = nsh_getfullpath(vtbl, path);
  if (!fullpath)
    {
      return ERROR;
    }
 
  /* Get a reference to the common input buffer */
 
  buffer = nsh_linebuffer(vtbl);
  if (buffer)
    {
      /* Open the file containing the script */
 
      stream = fopen(fullpath, "r");
      if (!stream)
        {
          nsh_output(vtbl, g_fmtcmdfailed, cmd, "fopen", NSH_ERRNO);
          nsh_freefullpath(fullpath);
          return ERROR;
        }
 
      /*Loop, processing each command line in the script file (or
       * until an error occurs)
       */
 
      do
        {
          /* Get the next line of input from the file */
 
          fflush(stdout);
          pret = fgets(buffer, CONFIG_NSH_LINELEN, stream);
          if (pret)
            {
              /* Parse process the command.  NOTE:  this is recursive...
               * we got to cmd_sh via a call to nsh_parse.  So some
               * considerable amount of stack may be used.
               */
 
              ret = nsh_parse(vtbl, buffer);
            }
        }
      while (pret && ret == OK);
      fclose(stream);
    }
 
  nsh_freefullpath(fullpath);
  return ret;
}

```
  这段代码并不难理解，就是打开了我们的脚本文件，然后逐行读取并解析执行。那显然nsh_parse就是解析这些脚本的关键。

```
int nsh_parse(FAR struct nsh_vtbl_s *vtbl, char *cmdline)
{
  /* Does this command correspond to a builtin command?
   * nsh_builtin() returns:
   *
   *   -1 (ERROR)  if the application task corresponding to 'argv[0]' could not
   *               be started (possibly because it doesn not exist).
   *    0 (OK)     if the application task corresponding to 'argv[0]' was
   *               and successfully started.  If CONFIG_SCHED_WAITPID is
   *               defined, this return value also indicates that the
   *               application returned successful status (EXIT_SUCCESS)
   *    1          If CONFIG_SCHED_WAITPID is defined, then this return value
   *               indicates that the application task was spawned successfully
   *               but returned failure exit status.
   *
   * Note the priority if not effected by nice-ness.
   */
 
#if defined(CONFIG_NSH_BUILTIN_APPS) && (!defined(CONFIG_NSH_FILE_APPS) || !defined(CONFIG_FS_BINFS))
  ret = nsh_builtin(vtbl, argv[0], argv, redirfile, oflags);
#endif
    {
      /* Then execute the command in "foreground" -- i.e., while the user waits
       * for the next prompt.  nsh_execute will return:
       *
       * -1 (ERRROR) if the command was unsuccessful
       *  0 (OK)     if the command was successful
       */
 
      ret = nsh_execute(vtbl, argc, argv);
    }
  return nsh_saveresult(vtbl, true);
}

```
  需要说明的是这个函数是删减过的。nsh_builtin和nsh_execute分别对应两个数组：g_builtins和g_cmdmap。什么区别呢？在我看来这两组命令是可以合并到一起的，但如果真的这样做了会有一个问题，用户扩展不方便。于是对其进行区分。像ls这样的命令不需要用户实现及处理，就作为系统命令在g_cmdmap数组中，而作为用户的应用程序如ArduPilot完全有用户决定，便作为用户命令在g_builtins数组中。对于系统命令我们常用的基本都已经实现，通过特定的宏进行选择编译。而用户命令我们只需按照相应规则编写一个入口函数然并设定堆栈大小然后放到g_builtins数组中即可。
  Nuttx本身提供了一个g_builtins数组，但PX4没有使用。PX4使用了一些技巧，利用Makefile在编译的时候自动生成g_builtins数组。这样做好处是我们修改源码的时候可以不关心这个数组，但坏处也显而易见：增加了我们刚接触时阅读源码的壁垒。
  那么关于命令部分我们先讲解到这里。下面我们将稍微看下脚本。
### 启动脚本
  前面我们说过Nuttx的启动脚本是/etc/init.d/rcS，跟Linux是一样的。这样做也有一个好处，就是如果你对Linux比较熟悉即便你完全不看源码只要看下根文件系统你就能够找到启动脚本并进行分析。
  启动脚本并不少，我们不逐条分析。

```
set MODE autostart
set USB autoconnect
if rgbled start
then
        set HAVE_RGBLED 1
        rgbled rgb 16 16 16
else
        set HAVE_RGBLED 0
fi
echo "[init] looking for microSD..."
if mount -t vfat /dev/mmcsd0 /fs/microsd
then
    echo "[init] card mounted at /fs/microsd"
        set HAVE_MICROSD 1
    tone_alarm start
else
    echo "[init] no microSD card found"
        set HAVE_MICROSD 0
    tone_alarm 2
        if [ $HAVE_RGBLED == 1 ]
        then
                rgbled rgb 16 0 0
        fi
fi
if [ -f /fs/microsd/etc/rc ]
then
    echo "[init] reading /fs/microsd/etc/rc"
    sh /fs/microsd/etc/rc
fi
if [ -f /fs/microsd/etc/rc.txt ]
then
    echo "[init] reading /fs/microsd/etc/rc.txt"
    sh /fs/microsd/etc/rc.txt
fi
if [ $USB != autoconnect ]
then
    echo "[init] not connecting USB"
else
    if sercon
    then
       echo "[init] USB interface connected"
    else
       echo "[init] No USB connected"
    fi
fi
if [ -f /etc/init.d/rc.APM -a $HAVE_MICROSD == 1 -a ! -f /fs/microsd/APM/nostart ]
then
    echo Running rc.APM
    sh /etc/init.d/rc.APM
else
        nshterm /dev/ttyACM0 &
fi
我们看到先是设置了环境变量，然后打开了RGB灯，就是PX4飞控上的那个三色高亮LED，设置了这几个等如何闪烁。然后挂载SD卡并执行SD卡中的脚本，接下来自动连接USB，最后是执行rc.APM脚本。所以PX4要用的东西基本上都在rc.APM脚本中启动。
set deviceA /dev/ttyACM0
if [ -f /fs/microsd/APM ]
then
   echo "APM file found - renaming"
   mv /fs/microsd/APM /fs/microsd/APM.old
fi
if [ -f /fs/microsd/APM/nostart ]
then
   echo "APM/nostart found - skipping APM startup"
   sh /etc/init.d/rc.error
fi
if [ -f /bin/reboot ]
then
    echo "binfs already mounted"
else
    echo "Mounting binfs"
        if mount -t binfs /dev/null /bin
        then
                echo "binfs mounted OK"
        else
                sh /etc/init.d/rc.error
        fi
fi
set sketch NONE
if rm /fs/microsd/APM/boot.log
then
   echo "removed old boot.log"
fi
set logfile /fs/microsd/APM/BOOT.LOG
if [ ! -f /bin/ArduPilot ]
then
   echo "/bin/ardupilot not found"
   sh /etc/init.d/rc.error
fi
if mkdir /fs/microsd/APM > /dev/null
then
     echo "Created APM directory"
fi
if [ -f /bin/lsm303d ]
then
     echo "Detected FMUv2 board"
     set BOARD FMUv2
else
     echo "Detected FMUv1 board"
     set BOARD FMUv1
fi
if [ $BOARD == FMUv1 ]
then
   set deviceC /dev/ttyS2
   if [ -f /fs/microsd/APM/AUXPWM.en ]
   then
      set deviceD /dev/null
   else
      set deviceD /dev/ttyS1
   fi
else
   set deviceC /dev/ttyS1
   set deviceD /dev/ttyS2
fi
if uorb start
then
    echo "uorb started OK"
else
    sh /etc/init.d/rc.error
fi
if [ -f /fs/microsd/APM/mkblctrl ]
then
    echo "Setting up mkblctrl driver"
    echo "Setting up mkblctrl driver" >> $logfile
    mkblctrl -d /dev/pwm_output
fi
if [ -f /fs/microsd/APM/mkblctrl_+ ]
then
    echo "Setting up mkblctrl driver +"
    echo "Setting up mkblctrl driver +" >> $logfile
    mkblctrl -mkmode + -d /dev/pwm_output
fi
if [ -f /fs/microsd/APM/mkblctrl_x ]
then
    echo "Setting up mkblctrl driver x"
    echo "Setting up mkblctrl driver x" >> $logfile
    mkblctrl -mkmode x -d /dev/pwm_output
fi
set HAVE_PX4IO false
if px4io start norc
then
    set HAVE_PX4IO true
else
    echo Loading /etc/px4io/px4io.bin
    tone_alarm MBABGP
    if px4io update /etc/px4io/px4io.bin
    then
    echo "upgraded PX4IO firmware OK"
        tone_alarm MSPAA
    else
    echo "Failed to upgrade PX4IO firmware"
        tone_alarm MNGGG
    fi
    sleep 1
    if px4io start norc
    then
        set HAVE_PX4IO true
    tone_alarm start
    fi
fi
if [ $HAVE_PX4IO == true ]
then
    echo "PX4IO board OK"
    if px4io checkcrc /etc/px4io/px4io.bin
    then
        echo "PX4IO CRC OK"
    else
        echo "PX4IO CRC failure"
        echo "PX4IO CRC failure" >> $logfile   
        tone_alarm MBABGP
        if px4io forceupdate 14662 /etc/px4io/px4io.bin
        then
               sleep 1
               if px4io start norc
               then
                  echo "PX4IO restart OK"
                  echo "PX4IO restart OK" >> $logfile   
                  tone_alarm MSPAA
               else
                  echo "PX4IO restart failed"
                  echo "PX4IO restart failed" >> $logfile   
                  tone_alarm MNGGG
                  sh /etc/init.d/rc.error
               fi
        else
               echo "PX4IO update failed"
               echo "PX4IO update failed" >> $logfile   
               tone_alarm MNGGG
        fi
    fi
else
    echo "No PX4IO board found"
    echo "No PX4IO board found" >> $logfile
    if [ $BOARD == FMUv2 ]
    then
       sh /etc/init.d/rc.error
    fi
fi
if [ $BOARD == FMUv1 -a $deviceD == /dev/ttyS1 ]
then
        echo "Setting FMU mode_serial"
        fmu mode_serial
else
        echo "Setting FMU mode_pwm"
        fmu mode_pwm
fi
echo "Starting APM sensors"
if ms5611 start
then
    echo "ms5611 started OK"
else
    sh /etc/init.d/rc.error
fi
if adc start
then
    echo "adc started OK"
else
    sh /etc/init.d/rc.error
fi
if [ $BOARD == FMUv1 ]
then
    echo "Starting FMUv1 sensors"
    if hmc5883 start
    then
        echo "hmc5883 started OK"
        if hmc5883 calibrate
        then
          echo "hmc5883 calibrate OK"
        else
          echo "hmc5883 calibrate failed"
          echo "hmc5883 calibrate failed" >> $logfile
          tone_alarm MSBBB
        fi
    else
        echo "hmc5883 start failed"
        echo "hmc5883 start failed" >> $logfile
        sh /etc/init.d/rc.error
    fi
    if mpu6000 start
    then
       echo "mpu6000  started OK"
    else
       sh /etc/init.d/rc.error
    fi
    if l3gd20 start
    then
       echo "l3gd20 started OK"
    else
       echo "No l3gd20"
       echo "No l3gd20" >> $logfile
    fi
else
    echo "Starting FMUv2 sensors"
    if hmc5883 -C -X start
    then
        echo "Have external hmc5883"
    else
        echo "No external hmc5883"
    fi
    if hmc5883 -C -I -R 4 start
    then
        echo "Have internal hmc5883"
    else
        echo "No internal hmc5883"
    fi
    if mpu6000 -X -R 4 start
    then
       echo "Found MPU6000 external"
       set HAVE_FMUV3 true
    else
       echo "No MPU6000 external"
       set HAVE_FMUV3 false
    fi
    if [ $HAVE_FMUV3 == true ]
    then
        if mpu6000 -R 14 start
        then
               echo "Found MPU6000 internal"
        else
               echo "No MPU6000"
               echo "No MPU6000" >> $logfile
               sh /etc/init.d/rc.error
        fi
        if l3gd20 -X -R 4 start
        then
               echo "l3gd20 external started OK"
        else
               echo "No l3gd20"
               sh /etc/init.d/rc.error
        fi
        if lsm303d -X -R 6 start
        then
               echo "lsm303d external started OK"
        else
               echo "No lsm303d"
               sh /etc/init.d/rc.error
        fi
    else
        if mpu6000 start
        then
               echo "Found MPU6000"
        else
               echo "No MPU6000"
               echo "No MPU6000" >> $logfile
        fi
        if l3gd20 start
        then
               echo "l3gd20 started OK"
        else
               sh /etc/init.d/rc.error
        fi
        if lsm303d start
        then
               echo "lsm303d started OK"
        else
               sh /etc/init.d/rc.error
        fi
    fi
fi
if ets_airspeed start
then
    echo "Found ETS airspeed sensor"
fi
if meas_airspeed start
then
    echo "Found MEAS airspeed sensor"
fi
if ll40ls start
then
    echo "Found ll40ls sensor"
fi
if mb12xx start
then
    echo "Found mb12xx sensor"
fi
echo "Trying PX4IO board"
if mtd start /fs/mtd
then
    echo "started mtd driver OK"
else
    echo "failed to start mtd driver"
    echo "failed to start mtd driver" >> $logfile
    sh /etc/init.d/rc.error          
fi
if mtd readtest /fs/mtd
then
    echo "mtd readtest OK"
else
    echo "failed to read mtd"
    echo "failed to read mtd" >> $logfile
    sh /etc/init.d/rc.error          
fi
if [ $BOARD == FMUv2 ]
then
  if mtd rwtest /fs/mtd
  then
    echo "mtd rwtest OK"
  else
    echo "failed to test mtd"
    echo "failed to test mtd" >> $logfile
    sh /etc/init.d/rc.error          
  fi
fi
echo Starting ArduPilot $deviceA $deviceC $deviceD
if ArduPilot -d $deviceA -d2 $deviceC -d3 $deviceD start
then
    echo ArduPilot started OK
else
    sh /etc/init.d/rc.error
fi
echo "rc.APM finished"

```
  这个脚本比较长。第一句应该是对设备进行重定向，然后对上一次的日志进行备份并创建日志。然后通过lsm303d判断PX4的版本，是V1还是V2。换句话说V1跟V2最大的区别是V2有lsm303d传感器，这是一个地磁传感器，而且内置加计。从这里我们也可以看出相比V1，V2在设计上采用了内置地磁的设计。然后根据硬件版本分别初始化输入输出设备。
  后面那个uorb没搞懂是什么。后面的mkblctrl感觉上像是机型配置相关的，具体是与不是还有待证实。然后是跟新IO板的固件。接下来启动的fmu想必是一个很重要的进程。然后又是根据硬件版本进行初始化，启动传感器。后面那几个估计也是传感器，具体还需要查。Mtd呢肯定是跟存储设备相关的。
  最后，ArduPilot便是对于我们来讲最重要的一个进程，这个进程启动基本上就表示我们的飞控可以正常使用了。
 
 

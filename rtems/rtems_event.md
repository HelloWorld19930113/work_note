# RTEMS_EVENT
    一个`client task`可以使用瞬态事件给另外一个`task`或是`中断服务程序`发送一个请求。

一、API 函数

```cpp
rtems_status_code rtems_event_transient_send(rtems_id id)
rtems_status_code rtems_event_transient_receive(rtems_option option_set, rtems_interval ticks)
void rtems_event_transient_clear(void)
```

二、详细描述

  中断服务函数使用 `rtems_event_transient_send()`函数可以发送一个瞬态事件给`client task`，通知它一个之前的请求结束。`client task`可以使用`rtems_event_transient_receive()`函数来等待接收一个瞬态事件。

 使用`transient event`的程序必须保证这个事件在请求完成或取消时没有被挂起。成功接收到一个瞬态事件后该瞬态事件就会被清除。超时后的瞬态事件行为无法保证。

 `rtems_event_transient_clear()`函数可以无阻塞的强制清除瞬态事件。

三、使用实例

Suppose you have a task that wants to issue a certain request and then waits for request completion.

It can create a request structure and store its task identifier there.

Now it can place the request on a work queue of another task (or interrupt handler).

Afterwards the task waits for the reception of the transient event.

Once the server task is finished with the request it can send the transient event to the waiting task and wake it up.

#include <assert.h>
#include <rtems.h>

typedef struct {
 rtems_id task_id;
 bool work_done;
} request;

void server(rtems_task_argument arg)
{
 rtems_status_code sc;
 request *req = (request *) arg;
 req->work_done = true;
 sc = rtems_event_transient_send(req->task_id);
 assert(sc == RTEMS_SUCCESSFUL);
 sc = rtems_task_delete(RTEMS_SELF);
 assert(sc == RTEMS_SUCCESSFUL);
}

void issue_request_and_wait_for_completion(void)
{
 rtems_status_code sc;
 rtems_id id;
 request req;
 req.task_id = rtems_task_self();
 req.work_done = false;
 sc = rtems_task_create(
   rtems_build_name('S', 'E', 'R', 'V'),
   1,
   RTEMS_MINIMUM_STACK_SIZE,
   RTEMS_DEFAULT_MODES,
   RTEMS_DEFAULT_ATTRIBUTES,
   &id
 );

 sc = rtems_task_start(id, server, (rtems_task_argument) &req);

 sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
}
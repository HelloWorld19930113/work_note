# 常用的rtems API #

1.判断函数返回状态是否为RTEMS_SUCCESSFUL 
```C
/**
 *  @brief Checks if the status code is equal to RTEMS_SUCCESSFUL.
 *
 *  This function returns TRUE if the status code is equal to RTEMS_SUCCESSFUL,
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_is_status_successful(
  rtems_status_code code
)
{
  return (code == RTEMS_SUCCESSFUL);
}
```
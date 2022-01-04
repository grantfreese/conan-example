// header for misc freertos bits

#ifndef __RTOS_H_
#define __RTOS_H_

#ifdef __cplusplus
extern "C"
{
#endif

    // Signal handler for Ctrl_C to cause the program to exit, and generate the profiling info
    void handle_sigint(int signal);


#ifdef __cplusplus
}
#endif

#endif // #ifndef __RTOS_H_

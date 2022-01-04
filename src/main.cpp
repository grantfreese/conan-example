// main.cpp
// Copyright 2021 Aptera Motors

//#define TRACE_ENABLED 1

// local includes
#include "platform.h"

// lib includes
#include "nlohmann/json.hpp"

// posix-only headers TODO: refactor these into platform file
#include <errno.h>  // Error integer and strerror() function
#include <fcntl.h>  // Contains file controls like O_RDWR
#include <unistd.h> // write(), read(), close(), usleep()


// os headers
#include "FreeRTOS.h"
#include "freertos/console.h"
#include "rtos.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

// standard headers
#include <cassert>
#include <cerrno>
#include <fstream>
#include <functional>
#include <iostream>


using namespace freese;
using namespace std::placeholders;
using json = nlohmann::json;

#define mainQUEUE_RECEIVE_TASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_SEND_TASK_PRIORITY       ( tskIDLE_PRIORITY + 1 )

#define mainTASK_SEND_FREQUENCY_MS         pdMS_TO_TICKS( 200UL )
#define mainTIMER_SEND_FREQUENCY_MS        pdMS_TO_TICKS( 2000UL )

#define mainQUEUE_LENGTH                   ( 2 )

#define mainVALUE_SENT_FROM_TASK           ( 100UL )
#define mainVALUE_SENT_FROM_TIMER          ( 200UL )


// freeRTOS stuff
static QueueHandle_t xQueue = NULL;
static TimerHandle_t xTimer = NULL;


static void prvQueueReceiveTask(void* pvParameters);
static void prvQueueSendTask(void* pvParameters);
static void taskMainThread(void* pvParameters);
static void prvQueueSendTimerCallback(TimerHandle_t xTimerHandle);


int main(void)
{
    // SIGINT is not blocked by the posix port
    signal(SIGINT, handle_sigint);

    console_init();

    console_print("Starting echo blinky demo\n");

    const TickType_t xTimerPeriod = mainTIMER_SEND_FREQUENCY_MS;


    // create queue
    xQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(uint32_t));

    if (xQueue != NULL)
    {
        // Start the two tasks as described in the comments at the top of this file
        xTaskCreate(prvQueueReceiveTask,             // function name
                    "Rx",                            // task name (debug only)
                    configMINIMAL_STACK_SIZE,        // task stack size
                    NULL,                            // param to pass to task
                    mainQUEUE_RECEIVE_TASK_PRIORITY, // task priority
                    NULL);                           // task handle

        xTaskCreate(prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL);

        // Task for BQ communications
        xTaskCreate(taskMainThread, "BqComms", 10000, NULL, tskIDLE_PRIORITY+3, NULL);

        //taskMainThread(NULL);

        // Create the software timer, but don't start it yet
        xTimer = xTimerCreate("Timer",                  // timer name (debug only)
                              xTimerPeriod,             // timer period (in ticks)
                              pdTRUE,                   // xAutoReload setting
                              NULL,                     // timer ID
                              prvQueueSendTimerCallback // callback function to execute on timer expiration
        );

        if (xTimer != NULL)
        {
            xTimerStart(xTimer, 0);
        }

        // start tasks and timer
        vTaskStartScheduler();
    }

    // this line should not be reached unless insufficient heap for tasks
    // for (;;)
    // {
    // }
}


void taskMainThread(void* pvParameters)
{
    (void)pvParameters; // suppress unused param warning


    // signal block test
    int err;
    sigset_t sigset;
    int fd;

    // Create a sigset of all the signals that we're interested in
    err = sigemptyset(&sigset);
    assert(err == 0);
    err = sigaddset(&sigset, SIGALRM);
    assert(err == 0);
    err = sigaddset(&sigset, SIGUSR1);
    assert(err == 0);

    // We must block the signals in order for signalfd to receive them
    err = sigprocmask(SIG_BLOCK, &sigset, NULL);
    assert(err == 0);


    TickType_t xNextWakeTime = xTaskGetTickCount();

    std::cout << "main(): init start" << std::endl;

    // load settings from file (on linux), else, load hard-coded defaults
    loadSettings();

    // init platform-specific components
    initPlatform();


    // cleanup
    std::cout << "main thread exiting" << std::endl;
    vTaskDelete(NULL);
}


static void prvQueueSendTask(void* pvParameters)
{
    TickType_t xNextWakeTime;
    const TickType_t xBlockTime  = mainTASK_SEND_FREQUENCY_MS;
    const uint32_t ulValueToSend = mainVALUE_SENT_FROM_TASK;

    (void)pvParameters;                  // suppress unused param warning
    xNextWakeTime = xTaskGetTickCount(); // init xNextWakeTime

    for (;;)
    {
        vTaskDelayUntil(&xNextWakeTime, xBlockTime);
        xQueueSend(xQueue, &ulValueToSend, 0U);
    }
}


static void prvQueueSendTimerCallback(TimerHandle_t xTimerHandle)
{
    const uint32_t ulValueToSend = mainVALUE_SENT_FROM_TIMER;
    (void)xTimerHandle;
    xQueueSend(xQueue, &ulValueToSend, 0U);
}


static void prvQueueReceiveTask(void* pvParameters)
{
    uint32_t ulReceivedValue;

    /* Prevent the compiler warning about the unused parameter. */
    (void)pvParameters;

    for (;;)
    {
        /* Wait until something arrives in the queue - this task will block
         * indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
         * FreeRTOSConfig.h.  It will not use any CPU time while it is in the
         * Blocked state. */
        xQueueReceive(xQueue, &ulReceivedValue, portMAX_DELAY);

        /* To get here something must have been received from the queue, but
         * is it an expected value?  Normally calling printf() from a task is not
         * a good idea.  Here there is lots of stack space and only one task is
         * using console IO so it is ok.  However, note the comments at the top of
         * this file about the risks of making Linux system calls (such as
         * console output) from a FreeRTOS task. */
        if (ulReceivedValue == mainVALUE_SENT_FROM_TASK)
        {
            console_print("Message received from task\n");
        }
        else if (ulReceivedValue == mainVALUE_SENT_FROM_TIMER)
        {
            console_print("Message received from software timer\n");
        }
        else
        {
            console_print("Unexpected message\n");
        }
    }
}

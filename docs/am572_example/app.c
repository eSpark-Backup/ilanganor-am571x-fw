/*
 * Copyright (C) 2018 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/**
 *
 *  \brief  Template application tasks file:
 *          This template application exercises multiple tasks and
 *          peripherals. The different task functions are run under
 *          separate Tasks in TI BIOS.
 *          The appTasksCreate function creates the different tasks.
 *          More tasks can be added in this function as required.
 */

/* Standard header files */
#include <string.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <ti/drv/gpio/GPIO.h>
#include <ti/drv/gpio/soc/GPIO_soc.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>

#include <ti/board/board.h>
/* Local template app header file */
#include "app.h"

/**********************************************************************
 ************************** Function prototypes ***********************
 **********************************************************************/
void biosTaskCreate(ti_sysbios_knl_Task_FuncPtr taskFunctionPtr,
                    char *taskName, int taskPriority, int stackSize);

/* Task functions */
void gpio_toggle_led_task(UArg arg0, UArg arg1);
void uart_task(UArg arg0, UArg arg1);
void spi_test_task(UArg arg0, UArg arg1);
void i2c_eeprom_read_and_display_task(UArg arg0, UArg arg1);
/* Add any additional task function prototypes here */



/**********************************************************************
 ************************** Global Variables **************************
 **********************************************************************/
volatile uint32_t g_endTestTriggered = 0;

/**
 *  @brief Function appTasksCreate : Creates multiple application tasks
 *
 *  @param[in]         None
 *  @retval            none
 */
void appTasksCreate(void)
{

    /* Create multiple tasks with different task priority & stack size */
    //appPrint("\n ======== Starting to create application tasks ========\n");

    /* Create task to toggle led */
    biosTaskCreate(gpio_toggle_led_task,
                   "gpio_toggle_led_task",
                   9, 4096);

    /* Create task to exercise uart */
    //biosTaskCreate(uart_task,
    //               "uart_task",
    //               8, 4096);

    /* Create task to test spi interface */
    //biosTaskCreate(spi_test_task,
    //               "spi_test_task",
    //               6, 4096);

    /* Create task to test spi interface */
    //biosTaskCreate(i2c_eeprom_read_and_display_task,
    //               "i2c_eeprom_read_and_display_task",
    //               7, 4096);

    /* Add additional tasks here */

    //appPrint("\n ======== Application tasks created successfully ========\n");

}

/**
 *  @brief Function gpio_toggle_led_task : Toggles LED through GPIO
 *
 *  @param[in]         arg0, arg1: Arguments ( Currently not used)
 *  @retval            none
 */

void gpio_toggle_led_task(UArg arg0, UArg arg1)
{
    while(1) {

        Log_print0(Diags_ENTRY, "--> test:");
        /* Delay to set period of pulse */
        Task_sleep(MESSAGE_DELAY_VALUE);


        /* Delay to set period of pulse */
        /* Note: If the Clock for the platform is incorrectly
           configured, this may loop here p forever */
        Task_sleep(MESSAGE_DELAY_VALUE);

        /* If end Test is triggered, then exit
         * Note:  The end test can be triggered through commands through uart
         */
        if (g_endTestTriggered)
            break;
    };
    Task_exit();
}
/**
 *  @brief Function biosTaskCreate : Task create function
 *             This function is customized to control
 *             certain parameters of the tasks. More parameters
 *             can be controlled for each task; Refer to SYSBIOS
 *             API for other parameters that can be controlled.
 *  @retval              : 0: success ; -1: fail
 */
void biosTaskCreate(ti_sysbios_knl_Task_FuncPtr taskFunctionPtr,
                    char *taskName, int taskPriority, int stackSize)
{
    Task_Params taskParams;
    Error_Block eb;
    Task_Handle task;

    Error_init(&eb);
    Task_Params_init(&taskParams);
    taskParams.instance->name = taskName;
    taskParams.priority = taskPriority;
    taskParams.stackSize = stackSize;
    task = Task_create(taskFunctionPtr, &taskParams, &eb);
    if (task == NULL) {
       //appPrint("%s: Task_create() failed! \n", taskName);
       BIOS_exit(0);
    }
   // appPrint("\n %s task created.", taskName);

    return;
}

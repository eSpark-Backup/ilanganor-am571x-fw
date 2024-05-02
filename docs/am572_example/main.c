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
 *  \brief  Template application main file:
 *          The main code initializes the platform , calls function
 *          to create application tasks and then starts BIOS.
 *          The initialization include Board specific
 *          initialization and initialization of used peripherals.
 *          The application specific task create function used here are
 *          defined in a separate file: app.c.
 */

/* Standard header files */
#include <string.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* Board header file */
#include <ti/board/board.h>

/* Local template app header file */
#include "app.h"

/**********************************************************************
 ************************** Function prototypes ***********************
 **********************************************************************/
void appExitFunction(Int argument);
void peripheralInit(void);
extern Int ipc_main();
extern Void IpcMgr_ipcStartup(Void);
/**
 *  @brief Function main : Main application function
 *  @retval              : 0: success ; -1: fail
 */
int main(void)
{
    Board_initCfg boardCfg;
    int status;

    /* Add exit function */
    System_atexit(appExitFunction);

    /* First step here is board specific initialization
     * Note that the Board_init is specific to the
     * platform. If running on a newly custom platform
     * the Board library need to be ported and customized
     * for the new platform.
     * See Details of customizing the board library in the
     * PDK/Processor SDK User guide */
    /* Set Board initialization flags: */
    boardCfg =
#ifndef SBL_BOOT
         /* Enabling Board Pinmux, clock when using without SBL boot
          * to act as stand alone application.
          */
         BOARD_INIT_PINMUX_CONFIG |
         BOARD_INIT_MODULE_CLOCK |
#endif
         /* The UART_STDIO initializes the default UART port on the board
          *  and support stdio like UART_printf which is used by appPrint
          */
         BOARD_INIT_UART_STDIO;

    /* Initialize Board */
    //status = Board_init(boardCfg);
    //if (status != BOARD_SOK) {
    //    appPrint("\n Error: Board_init failed: error %d", status);
    //}
    //appPrint("\n Board Init complete");


    /* Second step to Initialize peripherals */
    peripheralInit();

    /* Third step is to create Application tasks */
    appTasksCreate();

    /* Fourth step is to Start BIOS */
    ipc_main();
    IpcMgr_ipcStartup();
    BIOS_start();
    return (0);
}

/**
 *  @brief Function appExitFunction : Just prints end of application
 *             This function is plugged into BIOS to run on exit.
 *  @retval              : 0: success ; -1: fail
 */
void appExitFunction(Int argument)
{
   //appPrint("\n Template app ended\n");
}

/**
 *  @brief Function peripheralInit : Initializes peripherals needed by
 *             application
 *  @retval              : None
 */
void peripheralInit(void) {
    /* UART initialization: This is redundant as it is already done
     * as part of Board_init. Included here for completion */
    //UART_init();
    //appPrint("\n Uart Init complete");

    /* I2C initialization */
    //I2C_init();
    //appPrint("\n I2C Init complete");

    /* GPIO initialization */
    //GPIO_init();
    //appPrint("\n Gpio Init complete");

    /* MCSPI initialization */
    //MCSPI_init();
    //appPrint("\n MCSPI Init complete");

    /* Add any additional peripherals to be initialized here */

    //appPrint("\n ======== Peripheral Initialization complete ========\n");

}

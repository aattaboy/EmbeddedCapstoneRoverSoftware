/*******************************************************************************
 System Tasks File

  File Name:
    system_tasks.c

  Summary:
    This file contains source code necessary to maintain system's polled state
    machines.

  Description:
    This file contains source code necessary to maintain system's polled state
    machines.  It implements the "SYS_Tasks" function that calls the individual
    "Tasks" functions for all the MPLAB Harmony modules in the system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    polled in the system.  These handles are passed into the individual module
    "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "system_config.h"
#include "system_definitions.h"
#include "uart_receiver.h"
#include "uart_transmitter.h"
#include "encoders.h"
#include "motor1.h"
#include "sensor1.h"
#include "pid.h"
#include "rssi_collector.h"


// *****************************************************************************
// *****************************************************************************
// Section: Local Prototypes
// *****************************************************************************
// *****************************************************************************
 
static void _SYS_Tasks ( void );
static void _UART_RECEIVER_Tasks(void);
static void _UART_TRANSMITTER_Tasks(void);
static void _ENCODERS_Tasks(void);
static void _MOTOR1_Tasks(void);
static void _SENSOR1_Tasks(void);
static void _PID_Tasks(void);
static void _RSSI_COLLECTOR_Tasks(void);


// *****************************************************************************
// *****************************************************************************
// Section: System "Tasks" Routine
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void SYS_Tasks ( void )

  Remarks:
    See prototype in system/common/sys_module.h.
*/

void SYS_Tasks ( void )
{
    /* Create OS Thread for Sys Tasks. */
    xTaskCreate((TaskFunction_t) _SYS_Tasks,
                "Sys Tasks",
                1024, NULL, 0, NULL);

    /* Create OS Thread for UART_RECEIVER Tasks. */
    xTaskCreate((TaskFunction_t) _UART_RECEIVER_Tasks,
                "UART_RECEIVER Tasks",
                1024, NULL, 1, NULL);

    /* Create OS Thread for UART_TRANSMITTER Tasks. */
    xTaskCreate((TaskFunction_t) _UART_TRANSMITTER_Tasks,
                "UART_TRANSMITTER Tasks",
                1024, NULL, 1, NULL);

    /* Create OS Thread for ENCODERS Tasks. */
    xTaskCreate((TaskFunction_t) _ENCODERS_Tasks,
                "ENCODERS Tasks",
                1024, NULL, 1, NULL);

    /* Create OS Thread for MOTOR1 Tasks. */
    xTaskCreate((TaskFunction_t) _MOTOR1_Tasks,
                "MOTOR1 Tasks",
                1024, NULL, 1, NULL);

    /* Create OS Thread for SENSOR1 Tasks. */
    xTaskCreate((TaskFunction_t) _SENSOR1_Tasks,
                "SENSOR1 Tasks",
                1024, NULL, 1, NULL);

    /* Create OS Thread for PID Tasks. */
    xTaskCreate((TaskFunction_t) _PID_Tasks,
                "PID Tasks",
                1024, NULL, 1, NULL);

    /* Create OS Thread for RSSI_COLLECTOR Tasks. */
    xTaskCreate((TaskFunction_t) _RSSI_COLLECTOR_Tasks,
                "RSSI_COLLECTOR Tasks",
                1024, NULL, 1, NULL);

    /**************
     * Start RTOS * 
     **************/
    vTaskStartScheduler(); /* This function never returns. */
}


/*******************************************************************************
  Function:
    void _SYS_Tasks ( void )

  Summary:
    Maintains state machines of system modules.
*/

static void _SYS_Tasks ( void )
{
    while(1)
    {
        /* Maintain system services */
        SYS_DEVCON_Tasks(sysObj.sysDevcon);

        /* Maintain Device Drivers */

        /* Maintain Middleware */

        /* Task Delay */
    }
}


/*******************************************************************************
  Function:
    void _UART_RECEIVER_Tasks ( void )

  Summary:
    Maintains state machine of UART_RECEIVER.
*/

static void _UART_RECEIVER_Tasks(void)
{
    while(1)
    {
        UART_RECEIVER_Tasks();
    }
}


/*******************************************************************************
  Function:
    void _UART_TRANSMITTER_Tasks ( void )

  Summary:
    Maintains state machine of UART_TRANSMITTER.
*/

static void _UART_TRANSMITTER_Tasks(void)
{
    while(1)
    {
        UART_TRANSMITTER_Tasks();
    }
}


/*******************************************************************************
  Function:
    void _ENCODERS_Tasks ( void )

  Summary:
    Maintains state machine of ENCODERS.
*/

static void _ENCODERS_Tasks(void)
{
    while(1)
    {
        ENCODERS_Tasks();
    }
}


/*******************************************************************************
  Function:
    void _MOTOR1_Tasks ( void )

  Summary:
    Maintains state machine of MOTOR1.
*/

static void _MOTOR1_Tasks(void)
{
    while(1)
    {
        MOTOR1_Tasks();
    }
}


/*******************************************************************************
  Function:
    void _SENSOR1_Tasks ( void )

  Summary:
    Maintains state machine of SENSOR1.
*/

static void _SENSOR1_Tasks(void)
{
    while(1)
    {
        SENSOR1_Tasks();
    }
}


/*******************************************************************************
  Function:
    void _PID_Tasks ( void )

  Summary:
    Maintains state machine of PID.
*/

static void _PID_Tasks(void)
{
    while(1)
    {
        PID_Tasks();
    }
}


/*******************************************************************************
  Function:
    void _RSSI_COLLECTOR_Tasks ( void )

  Summary:
    Maintains state machine of RSSI_COLLECTOR.
*/

static void _RSSI_COLLECTOR_Tasks(void)
{
    while(1)
    {
        RSSI_COLLECTOR_Tasks();
    }
}


/*******************************************************************************
 End of File
 */


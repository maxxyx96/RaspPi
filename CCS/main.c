/******************************************************************************

 @file  main.c

 @brief Main entry of the BLE SensorTag sample application.

 Group: WCS, BTS
 Target Device: CC2650, CC2640

 ******************************************************************************
 
 Copyright (c) 2014-2020, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: ble_sdk_2_02_02_
 Release Date: 2020-08-18 15:47:20
 *****************************************************************************/

// TI RTOS
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Error.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

// BLE
#include "bcomdef.h"

// Application
#include "sensortag.h"

/* TI-RTOS Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/I2C.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include "Board.h"
#include "SensorI2C.h"
#include "SensorMpu9250.h"
#define TASKSTACKSIZE       2048
#define TMP007_OBJ_TEMP     0x0003  /* Object Temp Result Register */

//Task Declarations for MPU9250 sensor
Task_Struct mpuStruct;
Task_Handle mpuTask;
Char mpuStack[TASKSTACKSIZE];
volatile float accelVal = 0;

#ifndef USE_DEFAULT_USER_CFG

#include "ble_user_config.h"

// BLE user defined configuration
bleUserCfg_t user0Cfg = BLE_USER_CFG;

#endif // USE_DEFAULT_USER_CFG
/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

#ifdef CC1350_LAUNCHXL
#ifdef POWER_SAVING
// Power Notify Object for wake-up callbacks
Power_NotifyObj rFSwitchPowerNotifyObj;
static uint8_t rFSwitchNotifyCb(uint8_t eventType, uint32_t *eventArg,
                                uint32_t *clientArg);
#endif //POWER_SAVING

PIN_State  radCtrlState;
PIN_Config radCtrlCfg[] =
{
  Board_DIO1_RFSW   | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW  | PIN_PUSHPULL | PIN_DRVSTR_MAX, /* RF SW Switch defaults to 2.4GHz path*/
  Board_DIO30_SWPWR | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX, /* Power to the RF Switch */
  PIN_TERMINATE
};
PIN_Handle radCtrlHandle;
#endif //CC1350_LAUNCHXL

/*******************************************************************************
 * EXTERNS
 */

extern void AssertHandler(uint8 assertCause, uint8 assertSubcause);

void readMpu9250(UArg arg0)
{
    // config MPU
    uint16_t rawdata = 0;
    SensorMpu9250_powerOn();
    if (!SensorMpu9250_init())
    {
        System_printf("SensorMPU9250_ cannot init!\n");
        return;
    }
    SensorMpu9250_accSetRange(ACC_RANGE_2G);
    SensorMpu9250_enable(9);
    SensorMpu9250_enableWom(1);
    if (!SensorMpu9250_test())
    {
        System_printf("SensorMPU9250_ did not pass test!\n");
    }

    // read MPU data
    while (1)
    {
        //CS3237 TODO: add code to read MPU accelermoter data. The API is provided in MPU library code.
        if (SensorMpu9250_accRead(&rawdata))
           {
               // See if conversion is necessary
               //When still, converted value is around -0.33, goes up to -1 and above when vigorous movement.
               accelVal = SensorMpu9250_accConvert(rawdata);
               //Convert negatives to positives, and offset the still position value to get more reliable data.
               accelVal = (accelVal < 0) ? (-accelVal - 0.33)  : accelVal - 0.33;

               System_printf("SensorMPU9250 Raw value : %d; Converted Value : %f \n", rawdata, accelVal);
            }
        else
            {
                System_printf("SensorMPU9250_ I2C fault!\n");
            }

        System_flush();
        Task_sleep((UInt) arg0);
    }
}

/*******************************************************************************
 * @fn          Main
 *
 * @brief       Application Main
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
int main()
{
  /* Register Application callback to trap asserts raised in the Stack */
  RegisterAssertCback(AssertHandler);

  PIN_init(BoardGpioInitTable);
#ifdef CC1350_LAUNCHXL
  // Enable 2.4GHz Radio
  radCtrlHandle = PIN_open(&radCtrlState, radCtrlCfg);

#ifdef POWER_SAVING
  Power_registerNotify(&rFSwitchPowerNotifyObj,
                       PowerCC26XX_ENTERING_STANDBY | PowerCC26XX_AWAKE_STANDBY,
                       (Power_NotifyFxn) rFSwitchNotifyCb, NULL);
#endif //POWER_SAVING
#endif //CC1350_LAUNCHXL

#ifndef POWER_SAVING
  /* Set constraints for Standby and Idle mode */
  Power_setConstraint(PowerCC26XX_SB_DISALLOW);
  Power_setConstraint(PowerCC26XX_IDLE_PD_DISALLOW);
#endif // POWER_SAVING

  /* Initialize ICall module */
  ICall_init();

  /* Start tasks of external images - Priority 5 */
  ICall_createRemoteTasks();

  /* Kick off profile - Priority 3 */
  GAPRole_createTask();

  /* Kick off application - Priority 1 */
  SensorTag_createTask();

  // Initialising Task for MPU9250 Sensor
  Task_Params mpuParams;
  Task_Params_init(&mpuParams);
  mpuParams.stackSize = TASKSTACKSIZE;
  mpuParams.stack = &mpuStack;
  mpuParams.arg0 = 5000; // Task_sleep argument
  Task_construct(&mpuStruct, (Task_FuncPtr)readMpu9250, &mpuParams, NULL);

  // Start task
  mpuTask = Task_handle(&mpuStruct);

  /* SysMin will only print to the console when you call flush or exit */
  System_flush();

  BIOS_start();     /* enable interrupts and start SYS/BIOS */

  return 0;
}


/*******************************************************************************
 * @fn          AssertHandler
 *
 * @brief       This is the Application's callback handler for asserts raised
 *              in the stack.  When EXT_HAL_ASSERT is defined in the Stack
 *              project this function will be called when an assert is raised,
 *              and can be used to observe or trap a violation from expected
 *              behavior.
 *
 *              As an example, for Heap allocation failures the Stack will raise
 *              HAL_ASSERT_CAUSE_OUT_OF_MEMORY as the assertCause and
 *              HAL_ASSERT_SUBCAUSE_NONE as the assertSubcause.  An application
 *              developer could trap any malloc failure on the stack by calling
 *              HAL_ASSERT_SPINLOCK under the matching case.
 *
 *              An application developer is encouraged to extend this function
 *              for use by their own application.  To do this, add hal_assert.c
 *              to your project workspace, the path to hal_assert.h (this can
 *              be found on the stack side). Asserts are raised by including
 *              hal_assert.h and using macro HAL_ASSERT(cause) to raise an
 *              assert with argument assertCause.  the assertSubcause may be
 *              optionally set by macro HAL_ASSERT_SET_SUBCAUSE(subCause) prior
 *              to asserting the cause it describes. More information is
 *              available in hal_assert.h.
 *
 * input parameters
 *
 * @param       assertCause    - Assert cause as defined in hal_assert.h.
 * @param       assertSubcause - Optional assert subcause (see hal_assert.h).
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void AssertHandler(uint8 assertCause, uint8 assertSubcause)
{
  // check the assert cause
  switch (assertCause)
  {
    default:
      HAL_ASSERT_SPINLOCK;
  }

  return;
}


/*******************************************************************************
 * @fn          smallErrorHook
 *
 * @brief       Error handler to be hooked into TI-RTOS.
 *
 * input parameters
 *
 * @param       eb - Pointer to Error Block.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void smallErrorHook(Error_Block *eb)
{
  for (;;);
}

#if defined (CC1350_LAUNCHXL) && defined (POWER_SAVING)
/*******************************************************************************
 * @fn          rFSwitchNotifyCb
 *
 * @brief       Power driver callback to toggle RF switch on Power state
 *              transitions.
 *
 * input parameters
 *
 * @param   eventType - The state change.
 * @param   eventArg  - Not used.
 * @param   clientArg - Not used.
 *
 * @return  Power_NOTIFYDONE to indicate success.
 */
static uint8_t rFSwitchNotifyCb(uint8_t eventType, uint32_t *eventArg,
                                uint32_t *clientArg)
{
  if (eventType == PowerCC26XX_ENTERING_STANDBY)
  {
    // Power down RF Switch
    PIN_setOutputValue(radCtrlHandle, Board_DIO30_SWPWR, 0);
  }
  else if (eventType == PowerCC26XX_AWAKE_STANDBY)
  {
    // Power up RF Switch
    PIN_setOutputValue(radCtrlHandle, Board_DIO30_SWPWR, 1);
  }

  // Notification handled successfully
  return Power_NOTIFYDONE;
}
#endif //CC1350_LAUNCHXL || POWER_SAVING


/*******************************************************************************
 */

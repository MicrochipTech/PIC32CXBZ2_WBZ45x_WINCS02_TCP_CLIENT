/*******************************************************************************
  WINC Wireless Driver External Interrupt Handler

  File Name:
    wdrv_winc_eint.c

  Summary:
    External interrupt handler for WINC wireless driver.

  Description:
    External interrupt handler for WINC wireless driver.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#include <stdlib.h>
#include <stdbool.h>
#include "configuration.h"
#include "definitions.h"
#include "wdrv_winc_eint.h"

#if defined(INT_SOURCE_CHANGE_NOTICE) && (WDRV_WINC_INT_SOURCE == INT_SOURCE_CHANGE_NOTICE)
#ifdef PLIB_PORTS_ExistsPinChangeNoticePerPort
#define WDRV_INT_SOURCE WDRV_WINC_INT_SOURCE_CN_PORT
#else
#define WDRV_INT_SOURCE INT_SOURCE_CHANGE_NOTICE
#endif
#elif defined(WDRV_WINC_INT_SOURCE)
#define WDRV_INT_SOURCE WDRV_WINC_INT_SOURCE
#elif defined WDRV_WINC_GPIO_SOURCE
static void eintGPIOCallback(GPIO_PIN pin, uintptr_t context)
{
    WDRV_WINC_ISR(context);
}
#elif defined WDRV_WINC_PIO_SOURCE
static void eintPIOCallback(PIO_PIN pin, uintptr_t context)
{
    WDRV_WINC_ISR(context);
}
#endif

/****************************************************************************
 * Function:        WDRV_WINC_INTInitialize
 * Summary: Initializes interrupts for the WiFi driver.
 *****************************************************************************/
void WDRV_WINC_INTInitialize(SYS_MODULE_OBJ object, int intSrc)
{
#ifdef WDRV_WINC_EIC_SOURCE
    EIC_CallbackRegister(intSrc, WDRV_WINC_ISR, object);
    EIC_InterruptEnable(intSrc);
#elif defined WDRV_WINC_GPIO_SOURCE
    GPIO_PinInterruptCallbackRegister(intSrc, eintGPIOCallback, object);
    GPIO_PinInterruptEnable(intSrc);
    GPIO_PinIntEnable(intSrc, GPIO_INTERRUPT_ON_FALLING_EDGE);
#elif defined WDRV_WINC_PIO_SOURCE
    (void)PIO_PinInterruptCallbackRegister(intSrc, eintPIOCallback, object);
    PIO_PinInterruptEnable(intSrc);
#else
    /* disable the external interrupt */
    SYS_INT_SourceDisable(WDRV_INT_SOURCE);

    /* clear and enable the interrupt */
    SYS_INT_SourceStatusClear(WDRV_INT_SOURCE);

    /* enable the external interrupt */
    SYS_INT_SourceEnable(WDRV_INT_SOURCE);
#endif
}

/****************************************************************************
 * Function:        WDRV_WINC_INTDeinitialize
 * Summary: Deinitializes interrupts for WiFi driver.
 *****************************************************************************/
void WDRV_WINC_INTDeinitialize(int intSrc)
{
#ifdef WDRV_WINC_EIC_SOURCE
    EIC_InterruptDisable(intSrc);
#elif defined WDRV_WINC_GPIO_SOURCE
    GPIO_PinInterruptDisable(intSrc);
#elif defined WDRV_WINC_PIO_SOURCE
    PIO_PinInterruptDisable(intSrc);
#else
    SYS_INT_SourceDisable(WDRV_INT_SOURCE);
#endif
}

//DOM-IGNORE-END

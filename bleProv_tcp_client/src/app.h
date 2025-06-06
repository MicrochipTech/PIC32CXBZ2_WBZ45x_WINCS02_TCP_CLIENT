// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
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
// DOM-IGNORE-END

/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_H
#define _APP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "osal/osal_freertos_extend.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

//Tera Term Color coding 
#define TERM_GREEN "\x1B[32m"
#define TERM_RED   "\x1B[31m"
#define TERM_YELLOW "\x1B[33m"
#define TERM_CYAN "\x1B[36m"
#define TERM_WHITE "\x1B[47m"
#define TERM_RESET "\x1B[0m"
#define TERM_BG_RED "\x1B[41m" 
#define TERM_BOLD "\x1B[1m" 
#define TERM_UL "\x1B[4m"

#define APP_BUILD_HASH_SZ 5
#define APP_DI_IMAGE_INFO_NUM 2

#define NOTIFY_STATE_PROVISIONFAILED	(0)
#define NOTIFY_STATE_WIFICONNECTING		(1)
#define NOTIFY_STATE_PROVISIONED		(2)

/** @brief maximum ssid length */
#define WIFI_PROVISION_MAX_SSID_LENGTH					(32)
/** @brief maximum pass phrase length */
#define WIFI_PROVISION_MAX_PASS_LENGTH					(64)

#define WIFI_PROV_CRED_SECTYPE_POS	(0)  //0 - 1 byte security type
#define WIFI_PROV_CRED_SSIDLEN_POS	(WIFI_PROV_CRED_SECTYPE_POS + 1)  //1 - 1 byte ssid length
#define WIFI_PROV_CRED_SSID_POS		(WIFI_PROV_CRED_SSIDLEN_POS + 1)  // 2 - ssid (max len 30) occupies bytes 2-31
#define WIFI_PROV_CRED_PASSLEN_POS	(WIFI_PROV_CRED_SSID_POS + WIFI_PROVISION_MAX_SSID_LENGTH) // 34 - 1 byte passphrase length
#define WIFI_PROV_CRED_PASS_POS		(WIFI_PROV_CRED_PASSLEN_POS + 1) //35 - passphrase (max len 30) occupies byte 35-65

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
typedef enum
{
    WIFI_DISCONNECTED = 0,
    WIFI_CONNECTED,
    WIFI_CONNECTING,
    WIFI_SCANNING,
    
}WIFI_STATUS;


typedef struct
{
    WIFI_STATUS wifi_status;
	uint8_t sec_type;
	uint8_t ssid_length;
	uint8_t ssid[WIFI_PROVISION_MAX_SSID_LENGTH];
	uint8_t passphrase_length;
	uint8_t passphrase[WIFI_PROVISION_MAX_PASS_LENGTH];
} credentials;


/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Application's state machine's initial state. */
    APP_STATE_INIT=0,
    APP_STATE_SERVICE_TASKS,
    APP_STATE_WINCS_INIT,
    APP_STATE_WINCS_OPEN_DRIVER,
    APP_STATE_WINCS_DEVICE_INFO,
    APP_STATE_WINCS_SET_REG_DOMAIN,
    APP_STATE_WINCS_SET_WIFI_PARAMS,
    APP_STATE_WINCS_WIFI_CONNECT,
    APP_STATE_WINCS_ACTIVE_SCAN,
    APP_STATE_WINCS_ERROR,

} APP_STATES;

typedef enum APP_MsgId_T
{

    APP_MSG_BLE_STACK_EVT,
    APP_MSG_BLE_STACK_LOG,
    APP_TIMER_WIFI_CONNECTION_MSG,
    APP_MSG_BLE_PROV_COMPLETE,
    APP_MSG_BLE_PROV_FAILED,
    APP_MSG_SOCKET_CREATE,
    APP_MSG_SOCKET_SEND,
    APP_MSG_ZB_STACK_EVT,
    APP_MSG_ZB_STACK_CB,
    APP_MSG_STACK_END
} APP_MsgId_T;

typedef struct APP_Msg_T
{
    uint8_t msgId;
    uint8_t msgData[64];
} APP_Msg_T;

typedef struct
{
    /* Version number structure. */
    struct
    {
        /* Major version number. */
        uint16_t major;

        /* Major minor number. */
        uint16_t minor;

        /* Major patch number. */
        uint16_t patch;
    } version;
} APP_DRIVER_VERSION_INFO;

typedef struct
{
    /* Flag indicating if this information is valid. */
    bool isValid;

    /* Version number structure. */
    struct
    {
        /* Major version number. */
        uint16_t major;

        /* Major minor number. */
        uint16_t minor;

        /* Major patch number. */
        uint16_t patch;
    } version;

    /* Build date/time structure. */
    struct
    {
        uint8_t hash[APP_BUILD_HASH_SZ];

        uint32_t timeUTC;
    } build;
} APP_FIRMWARE_VERSION_INFO;


// *****************************************************************************
/*  Device Information

  Summary:
    Defines the device information.

  Description:
    This data type defines the device information of the WINC.

  Remarks:
    None.
*/

typedef struct
{
    /* Flag indicating if this information is valid. */
    bool isValid;

    /* WINC device ID. */
    uint32_t id;

    /* Number of flash images. */
    uint8_t numImages;

    /* Information for each device image. */
    struct
    {
        /* Sequence number. */
        uint32_t seqNum;

        /* Version information. */
        uint32_t version;

        /* Source address. */
        uint32_t srcAddr;
    } image[APP_DI_IMAGE_INFO_NUM];
} APP_DEVICE_INFO;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* The application's current state */
    APP_STATES state;

    /* TODO: Define any additional data used by the application. */
    OSAL_QUEUE_HANDLE_TYPE appQueue;

} APP_DATA;

extern APP_DATA appData;
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_Tasks( void );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_H */

/*******************************************************************************
 End of File
 */


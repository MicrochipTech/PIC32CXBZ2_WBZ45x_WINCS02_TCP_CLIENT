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
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include <time.h>
#include "app.h"
#include "definitions.h"
#include "app_ble.h"
#include "app_timer/app_timer.h"
#include "ble_cms/ble_conn_serv_svc.h"
#include "system/sys_wincs_system_service.h"
#include "system/net/sys_wincs_net_service.h"
#include "wdrv_winc.h"



// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************





// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;
uint16_t conn_hdl;
credentials cred;

DRV_HANDLE wdrvHandle = DRV_HANDLE_INVALID;

extern void APP_ConnService_SendNotification(uint16_t connHandle, uint8_t char_val );

// Define and initialize a TCP client socket configuration from MCC
SYS_WINCS_NET_SOCKET_t g_tcpClientSocket = {
    // Specify the type of binding for the socket
    .bindType = SYS_WINCS_NET_BIND_TYPE0,
    // Set the socket address to the predefined server address
    .sockAddr = SYS_WINCS_NET_SOCK_SERVER_ADDR0,
    // Define the type of socket (e.g., TCP, UDP)
    .sockType = SYS_WINCS_NET_SOCK_TYPE0,
    // Set the port number for the socket
    .sockPort = SYS_WINCS_NET_SOCK_PORT0,
    // Enable or disable TLS for the socket
    .tlsEnable = SYS_WINCS_TLS_ENABLE0,
    // Specify the IP type (e.g., IPv4, IPv6)
    .ipType  = SYS_WINCS_NET_SOCK_TYPE_IPv4_0,
};

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */


void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;


    appData.appQueue = xQueueCreate( 16, sizeof(APP_Msg_T) );
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
    
    memset(&cred, 0, sizeof(credentials));
    
    cred.wifi_status = WIFI_DISCONNECTED;
    cred.sec_type = SYS_WINCS_WIFI_STA_SECURITY;
    cred.ssid_length = (uint8_t)strlen(SYS_WINCS_WIFI_STA_SSID);
    strcpy((char *)cred.ssid, SYS_WINCS_WIFI_STA_SSID);
    cred.passphrase_length = (uint8_t)strlen(SYS_WINCS_WIFI_STA_PWD);
    strcpy((char *)cred.passphrase, SYS_WINCS_WIFI_STA_PWD);
}


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

static inline WDRV_WINC_CONN_STATE APP_SYS_WINCS_WIFI_ConnectedState(DRV_HANDLE wincHandle)
{
    return ((WDRV_WINC_DCPT *)wincHandle)->pCtrl->connectedState;
}

// *****************************************************************************
// Application NET SoSOCK Handler Default!cket Callback Handler
//
// Summary:
//    Handles NET socket events.
//
// Description:
//    This function handles various NET socket events and performs appropriate actions.
//
// Parameters:
//    socket - The socket identifier
//    event - The type of socket event
//    netHandle - Additional data or message associated with the event
//
// Returns:
//    None.
//
// Remarks:
//    None.
// *****************************************************************************
void SYS_WINCS_NET_SockCallbackHandler
(
    uint32_t socket,                    // The socket identifier
    SYS_WINCS_NET_SOCK_EVENT_t event,   // The type of socket event
    SYS_WINCS_NET_HANDLE_t netHandle    // Additional data or message associated with the event
) 
{
    APP_Msg_T appMsg;
    if(socket == g_tcpClientSocket.sockID)
    {
        switch(event)
        {
            /* Net socket connected event code*/
            case SYS_WINCS_NET_SOCK_EVENT_CONNECTED:    
            {
                SYS_CONSOLE_PRINT(TERM_GREEN"[APP] : Connected to Server!\r\n"TERM_RESET );
                break;
            }

            /* Net socket disconnected event code*/
            case SYS_WINCS_NET_SOCK_EVENT_DISCONNECTED:
            {
                SYS_CONSOLE_PRINT(TERM_RED"[APP] : Socket DisConnected!\r\n"TERM_RESET);
                SYS_WINCS_NET_SockSrvCtrl(SYS_WINCS_NET_SOCK_CLOSE, &socket);
                break;
            }

            /* Net socket error event code*/
            case SYS_WINCS_NET_SOCK_EVENT_ERROR:
            {
                SYS_CONSOLE_PRINT(TERM_RED"[APP ERROR] : Socket Error\r\n"TERM_RESET);
                break;
            }

            case SYS_WINCS_NET_SOCK_EVENT_SEND_COMPLETE:
            {
                SYS_CONSOLE_PRINT(TERM_CYAN"[APP] : Socket Send Complete\r\n"TERM_RESET);
                break;
            }

            /* Net socket read event code*/
            case SYS_WINCS_NET_SOCK_EVENT_READ:
            {         
                uint8_t rx_data[100];
                int16_t rcvd_len = 64;
                memset(rx_data,0,64);

                // Read data from the TCP socket
                if((rcvd_len = SYS_WINCS_NET_TcpSockRead(socket, SYS_WINCS_NET_SOCK_RCV_BUF_SIZE, rx_data)) > 0)
                {
                    rcvd_len = strlen((char *)rx_data);
                    rx_data[rcvd_len] = '\n';
                    
                    appMsg.msgId = APP_MSG_SOCKET_SEND;
                    strncpy((char*)appMsg.msgData, (char*)rx_data, sizeof(appMsg.msgData) - 1);
                    appMsg.msgData[sizeof(appMsg.msgData) - 1] = '\0'; // Ensure null termination
                    OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);
                }    
                break;
            }
            case SYS_WINCS_NET_SOCK_EVENT_CLOSED:
            {
                SYS_CONSOLE_PRINT(TERM_RED"[APP] : Socket CLOSED -> socketID: %d\r\n"TERM_RESET,socket);
                break;
            }

            case SYS_WINCS_NET_SOCK_EVENT_TLS_DONE:    
            {
                SYS_CONSOLE_PRINT("[APP] : TLS ->Connected to Server!\r\n" );
                break;
            }

            default:
            {
                SYS_CONSOLE_PRINT("[APP] : SOCK Handler Default! %d\r\n",event );
            }
                break;                  
        }   
    }
    /* TODO:  Add if conditions for any more sockets.
    */
}

       
void SYS_WINCS_WIFI_CallbackHandler
(
    SYS_WINCS_WIFI_EVENT_t event,         // The type of Wi-Fi event
    SYS_WINCS_WIFI_HANDLE_t wifiHandle    // Handle to the Wi-Fi event data
)
{
    APP_Msg_T appMsg;
    switch(event)
    {
        /* Set regulatory domain Acknowledgment */
        case SYS_WINCS_WIFI_REG_DOMAIN_SET_ACK:
        {
            // The driver generates this event callback twice, hence the if condition 
            // to ignore one more callback. This will be resolved in the next release.
            static bool domainFlag = false;
            if( domainFlag == false)
            {
                SYS_CONSOLE_PRINT("Set Reg Domain -> SUCCESS\r\n");
                appData.state = APP_STATE_WINCS_SET_WIFI_PARAMS;
                domainFlag = true;
            }
            
            break;
        } 
        
        /* SNTP UP event code*/
        case SYS_WINCS_WIFI_SNTP_UP:
        {            
            SYS_CONSOLE_PRINT("[APP] : SNTP UP \r\n"); 
            break;
        }
        break;

        /* Wi-Fi connected event code*/
        case SYS_WINCS_WIFI_CONNECTED:
        {
            cred.wifi_status = WIFI_CONNECTED;
            SYS_CONSOLE_PRINT(TERM_GREEN"[APP] : Wi-Fi Connected    \r\n"TERM_RESET);
            break;
        }
        
        /* Wi-Fi disconnected event code*/
        case SYS_WINCS_WIFI_DISCONNECTED:
        {
            SYS_CONSOLE_PRINT(TERM_RED"[APP] : Wi-Fi Disconnected\nReconnecting... \r\n"TERM_RESET);
            SYS_WINCS_WIFI_SrvCtrl(SYS_WINCS_WIFI_STA_CONNECT, NULL);
            break;
        }
        
        case SYS_WINCS_WIFI_CONNECT_FAILED:
        {
            SYS_CONSOLE_PRINT(TERM_RED"[APP] : Wi-Fi Connect Failed\r\n"TERM_RESET);
            break;
        }
        
        /* Wi-Fi DHCP IPv4 complete event code*/
        case SYS_WINCS_WIFI_DHCP_IPV4_COMPLETE:
        {
            SYS_CONSOLE_PRINT("[APP] : DHCP IPv4 : %s\r\n", (char *)wifiHandle);
            appMsg.msgId = APP_MSG_SOCKET_CREATE;
            OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);
            break;
        }
        
        case SYS_WINCS_WIFI_DHCP_IPV6_LOCAL_COMPLETE:
        {
            SYS_CONSOLE_PRINT("[APP] : DHCP IPv6 Local : %s\r\n", (char *)wifiHandle);
            break;
        }
        
        case SYS_WINCS_WIFI_DHCP_IPV6_GLOBAL_COMPLETE:
        {
            SYS_CONSOLE_PRINT("[APP] : DHCP IPv6 Global: %s\r\n", (char *)wifiHandle);
            break;
        }
        
        default:
        {
            SYS_CONSOLE_PRINT("[APP] : Default handler %d\r\n", event);
            break;
        }
    }    
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    APP_Msg_T    appMsg[1];
    APP_Msg_T   *p_appMsg;
    p_appMsg=appMsg;

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
            //appData.appQueue = xQueueCreate( 10, sizeof(APP_Msg_T) );

            APP_BleStackInit();
            vTaskDelay(pdMS_TO_TICKS(5000));
            SYS_CONSOLE_PRINT(TERM_YELLOW"\r\n##################################################################\r\n"TERM_RESET);
            SYS_CONSOLE_PRINT(TERM_BG_RED"  WBZ451 WINCS02 TCP Client Demo with BLE Wi-Fi Provisioning\r\n"TERM_RESET);
            SYS_CONSOLE_PRINT(TERM_YELLOW"##################################################################\r\n"TERM_RESET);
//            vTaskDelay(pdMS_TO_TICKS(100));
            
            BLE_CONN_SERV_Add();

            if (appInitialized)
            {
                appData.state = APP_STATE_WINCS_INIT;
            }
        }
        break;
        case APP_STATE_WINCS_INIT:
        {
            SYS_STATUS status;
            // Get the driver status
            SYS_WINCS_WIFI_SrvCtrl(SYS_WINCS_WIFI_GET_DRV_STATUS, &status);
            // If the driver is ready, move to the next state
            if (SYS_STATUS_READY == status)
            {
                appData.state = APP_STATE_WINCS_OPEN_DRIVER;
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }
        break;
        case APP_STATE_WINCS_OPEN_DRIVER:
        {
            // Open the Wi-Fi driver
            if (SYS_WINCS_FAIL == SYS_WINCS_WIFI_SrvCtrl(SYS_WINCS_WIFI_OPEN_DRIVER, &wdrvHandle))
            {
                appData.state = APP_STATE_WINCS_ERROR;
                break;
            }
            
            // Get the driver handle
            SYS_WINCS_WIFI_SrvCtrl(SYS_WINCS_WIFI_GET_DRV_HANDLE, &wdrvHandle);
            appData.state = APP_STATE_WINCS_DEVICE_INFO;
        }
        break;
        case APP_STATE_WINCS_DEVICE_INFO:
        {
            APP_DRIVER_VERSION_INFO drvVersion;
            APP_FIRMWARE_VERSION_INFO fwVersion;
            APP_DEVICE_INFO devInfo;
            SYS_WINCS_RESULT_t status = SYS_WINCS_BUSY;

            // Get the firmware version
            status = SYS_WINCS_SYSTEM_SrvCtrl(SYS_WINCS_SYSTEM_SW_REV, &fwVersion);

            if(status == SYS_WINCS_PASS)
            {
                // Get the device information
                status = SYS_WINCS_SYSTEM_SrvCtrl(SYS_WINCS_SYSTEM_DEV_INFO, &devInfo);
            }

            if(status == SYS_WINCS_PASS)
            {
                // Get the driver version
                status = SYS_WINCS_SYSTEM_SrvCtrl(SYS_WINCS_SYSTEM_DRIVER_VER, &drvVersion);
            }

            if(status == SYS_WINCS_PASS)
            {
                char buff[30];
                // Print device information
                SYS_CONSOLE_PRINT("WINC: Device ID = %08x\r\n", devInfo.id);
                for (int i = 0; i < devInfo.numImages; i++)
                {
                    SYS_CONSOLE_PRINT("%d: Seq No = %08x, Version = %08x, Source Address = %08x\r\n", 
                            i, devInfo.image[i].seqNum, devInfo.image[i].version, devInfo.image[i].srcAddr);
                }

                // Print firmware version
                SYS_CONSOLE_PRINT(TERM_CYAN "Firmware Version: %d.%d.%d ", fwVersion.version.major,
                        fwVersion.version.minor, fwVersion.version.patch);
                strftime(buff, sizeof(buff), "%X %b %d %Y", localtime((time_t*)&fwVersion.build.timeUTC));
                SYS_CONSOLE_PRINT(" [%s]\r\n", buff);

                // Print driver version
                SYS_CONSOLE_PRINT("Driver Version: %d.%d.%d\r\n"TERM_RESET, drvVersion.version.major, 
                        drvVersion.version.minor, drvVersion.version.patch);
                
                appData.state = APP_STATE_WINCS_SET_REG_DOMAIN;
            }
        }
        break;
        case APP_STATE_WINCS_SET_REG_DOMAIN:
        {
            // Set the callback handler for Wi-Fi events
            SYS_WINCS_WIFI_SrvCtrl(SYS_WINCS_WIFI_SET_CALLBACK, SYS_WINCS_WIFI_CallbackHandler);

            SYS_CONSOLE_PRINT(TERM_YELLOW"[APP] : Setting REG domain to " TERM_UL "%s\r\n"TERM_RESET ,SYS_WINCS_WIFI_COUNTRYCODE);
            // Set the regulatory domain
            if (SYS_WINCS_FAIL == SYS_WINCS_WIFI_SrvCtrl(SYS_WINCS_WIFI_SET_REG_DOMAIN, SYS_WINCS_WIFI_COUNTRYCODE))
            {
                appData.state = APP_STATE_WINCS_ERROR;
                break;
            }
            appData.state = APP_STATE_WINCS_SET_WIFI_PARAMS;
        }
        break;
        case APP_STATE_WINCS_ACTIVE_SCAN:
        {
            SYS_WINCS_WIFI_SCAN_PARAM_t scanParams;
            scanParams.channel = WDRV_WINC_CID_ANY;
            scanParams.scanTime = 500;
            
            if (SYS_WINCS_FAIL == SYS_WINCS_WIFI_SrvCtrl(SYS_WINCS_WIFI_ACTIVE_SCAN, &scanParams))
            {
                appData.state = APP_STATE_WINCS_ERROR;
                break;
            }
            appData.state = APP_STATE_SERVICE_TASKS;
        }
        break;
        case APP_STATE_WINCS_SET_WIFI_PARAMS:
        {
            cred.wifi_status = WIFI_DISCONNECTED;
            
            // Configuration parameters for Wi-Fi station mode
            SYS_WINCS_WIFI_PARAM_t wifi_sta_cfg = {
                .mode        = SYS_WINCS_WIFI_MODE_STA,        // Set Wi-Fi mode to Station (STA)
                .ssid        = (const char *)cred.ssid,        // Set the SSID (network name) for the Wi-Fi connection
                .passphrase  = (const char *)cred.passphrase,         // Set the passphrase (password) for the Wi-Fi connection
                .security    = cred.sec_type,    // Set the security type (e.g., WPA2) for the Wi-Fi connection
                .autoConnect = SYS_WINCS_WIFI_STA_AUTOCONNECT  // Enable or disable auto-connect to the Wi-Fi network
            };                   
            // Set the Wi-Fi parameters
            if (SYS_WINCS_FAIL == SYS_WINCS_WIFI_SrvCtrl(SYS_WINCS_WIFI_SET_PARAMS, &wifi_sta_cfg))
            {
                appData.state = APP_STATE_WINCS_ERROR;
                break;
            }
            appData.state = APP_STATE_WINCS_WIFI_CONNECT;
            APP_TIMER_SetTimer(APP_TIMER_WIFI_CONNECTION, APP_TIMER_5S, false); 
        }
        break;
        case APP_STATE_WINCS_WIFI_CONNECT:
        {
            if (SYS_WINCS_FAIL == SYS_WINCS_WIFI_SrvCtrl(SYS_WINCS_WIFI_STA_CONNECT, NULL))
            {
                appData.state = APP_STATE_WINCS_ERROR;
                break;
            }
            SYS_CONSOLE_PRINT("\r\n\r\n[APP] : Wi-Fi Connecting to : %s\r\n", cred.ssid);
            cred.wifi_status = WIFI_CONNECTING;
            appData.state = APP_STATE_SERVICE_TASKS;
        }
        break;
        case APP_STATE_SERVICE_TASKS:
        {
            if (OSAL_QUEUE_Receive(&appData.appQueue, &appMsg, OSAL_WAIT_FOREVER))
            {

                if(p_appMsg->msgId==APP_MSG_BLE_STACK_EVT)
                {
                    // Pass BLE Stack Event Message to User Application for handling
                    APP_BleStackEvtHandler((STACK_Event_T *)p_appMsg->msgData);
                }
                else if(p_appMsg->msgId==APP_MSG_BLE_STACK_LOG)
                {
                    // Pass BLE LOG Event Message to User Application for handling
                    APP_BleStackLogHandler((BT_SYS_LogEvent_T *)p_appMsg->msgData);
                }
                else if(p_appMsg->msgId==APP_TIMER_WIFI_CONNECTION_MSG)
                {
                    if(cred.wifi_status != WIFI_CONNECTED)
                    {
                        SYS_CONSOLE_PRINT(TERM_YELLOW"[APP] : BLE Adv Started\r\n"TERM_RESET);
                        BLE_GAP_SetAdvEnable(0x01, 0);
                        if(APP_SYS_WINCS_WIFI_ConnectedState(wdrvHandle) != WDRV_WINC_CONN_STATE_DISCONNECTED)
                        {
                            SYS_WINCS_WIFI_SrvCtrl(SYS_WINCS_WIFI_STA_DISCONNECT, NULL);
                        }
                    }
                }
                else if (p_appMsg->msgId==APP_MSG_BLE_PROV_COMPLETE)
                {
                    
                    APP_ConnService_SendNotification(conn_hdl, NOTIFY_STATE_PROVISIONED);
                    //printf("writeDataLength: 0x%x\r\n", p_event->writeDataLength);
                    SYS_CONSOLE_PRINT("Auth Type: %d\r\n", cred.sec_type);
                    SYS_CONSOLE_PRINT("SSID: %s\r\n", cred.ssid);
                    SYS_CONSOLE_PRINT("SSID Len: %d\r\n", cred.ssid_length);
                    SYS_CONSOLE_PRINT("Password: %s\r\n", cred.passphrase);
                    SYS_CONSOLE_PRINT("Password Len: %d\r\n", cred.passphrase_length);

                    if(APP_SYS_WINCS_WIFI_ConnectedState(wdrvHandle) != WDRV_WINC_CONN_STATE_DISCONNECTED)
                    {
                        SYS_WINCS_WIFI_SrvCtrl(SYS_WINCS_WIFI_STA_DISCONNECT, NULL);
                    }
                    appData.state = APP_STATE_WINCS_SET_WIFI_PARAMS;
//                    SYS_CONSOLE_PRINT("State: %d\r\n", APP_Example_get_Wifi_State());
//                    APP_Example_set_Wifi_State(CLOUD_STATE_SCAN_DONE);
                }
                else if (p_appMsg->msgId==APP_MSG_BLE_PROV_FAILED)
                {
                    if( cred.sec_type >= SYS_WINCS_WIFI_SECURITY_WPA2_MIXED)
                    {
                        SYS_CONSOLE_PRINT(TERM_RED"WPA Provisioning not supported in Android\r\nPlease test with iPhone\r\n"TERM_RESET);
                    }
                    else
                    {
                        SYS_CONSOLE_PRINT(TERM_RED"Unsupported Wi-Fi Authentication type selected,\r\nPlease use OPEN or WPA method\r\n"TERM_RESET);
                    }
                    APP_ConnService_SendNotification(conn_hdl, NOTIFY_STATE_PROVISIONFAILED);
                }
                else if(p_appMsg->msgId==APP_MSG_SOCKET_CREATE)
                {
                    // Set the callback handler for NET socket events
                    SYS_WINCS_NET_SockSrvCtrl(SYS_WINCS_NET_SOCK_SET_CALLBACK, SYS_WINCS_NET_SockCallbackHandler);
                    
                    if (SYS_WINCS_FAIL == SYS_WINCS_NET_SockSrvCtrl(SYS_WINCS_NET_SOCK_TCP_OPEN, &g_tcpClientSocket))
                    {
                        appData.state = APP_STATE_WINCS_ERROR;
                        break;
                    }
                }
                else if (p_appMsg->msgId==APP_MSG_SOCKET_SEND)
                {
                    SYS_CONSOLE_PRINT(TERM_YELLOW"Received ->%s\r\n"TERM_RESET, p_appMsg->msgData);
                    //Write the received data back to the TCP socket
                    if (SYS_WINCS_FAIL == SYS_WINCS_NET_TcpSockWrite(g_tcpClientSocket.sockID, strlen((char*)p_appMsg->msgData), p_appMsg->msgData))
                    {
                        appData.state = APP_STATE_WINCS_ERROR;
                    }
                }
            }
        }
        break;
        case APP_STATE_WINCS_ERROR:
        {
            SYS_CONSOLE_PRINT(TERM_RED"[APP_ERROR] : ERROR in Application \r\n"TERM_RESET);
            appData.state = APP_STATE_SERVICE_TASKS;
        }
        break;
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

/*******************************************************************************
 End of File
 */

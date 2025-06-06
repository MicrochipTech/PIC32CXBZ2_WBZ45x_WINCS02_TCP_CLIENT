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

/*******************************************************************************
  Application BLE Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_ble_handler.c

  Summary:
    This file contains the Application BLE functions for this project.

  Description:
    This file contains the Application BLE functions for this project.
 *******************************************************************************/


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include <stdint.h>
#include "osal/osal_freertos_extend.h"
#include "app_ble_handler.h"
#include "ble_cms/ble_conn_serv_svc.h"
#include "app.h"
//#include "wdrv_winc.h"
#include "system/sys_wincs_system_service.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************

GATTS_SendWriteRespParams_T     *gattsSendWriteResp;
GATTS_SendErrRespParams_T       *gattsSendErrResp;

extern APP_DATA appData;
extern uint16_t conn_hdl;
extern credentials cred;
extern void APP_ConnService_SendNotification(uint16_t connHandle, uint8_t char_val );

typedef enum {
    M2M_WIFI_SEC_INVALID = 0,
    /*!< Invalid security type.
    */
    M2M_WIFI_SEC_OPEN,
    /*!< Wi-Fi network is not secured.
    */
    M2M_WIFI_SEC_WPA_PSK,
    /*!< Wi-Fi network is secured with WPA/WPA2 personal(PSK).
    */
    M2M_WIFI_SEC_WEP,
    /*!< Security type WEP (40 or 104) OPEN OR SHARED.
    */
    M2M_WIFI_SEC_802_1X,
    /*!< Wi-Fi network is secured with WPA/WPA2 Enterprise.IEEE802.1x.
     */
    M2M_WIFI_NUM_AUTH_TYPES
    /*!< Upper limit for enum value.
     */
} tenuM2mSecType;
// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

void APP_wbProv_GattSEvtWriteHandler(GATT_EvtWrite_T *p_event)
{
    uint16_t status;
    APP_Msg_T appMsg;

    if ((p_event->attrHandle <= CONN_SERV_START_HDL) ||
        (p_event->attrHandle > CONN_SERV_END_HDL))
    {
        /* Not BLE Custom Service characteristic. */
        //error = ATT_ERRCODE_INVALID_HANDLE;
        return;
    }

    switch(p_event->attrHandle)
    {
        case CONN_SERV_HDL_CHAR_0:                            /**< Handle of characteristic 0. */
        case CONN_SERV_HDL_CHAR_1:                            /**< Handle of characteristic 1. */
            //error = ATT_ERRCODE_APPLICATION_ERROR;
            break;
        case CONN_SERV_HDL_CCCD_0:                            /**< Handle of characteristic 0 CCCD . */
        {
            if ((p_event->writeType == ATT_WRITE_REQ)
                        || (p_event->writeType == ATT_PREPARE_WRITE_REQ)
                        || (p_event->writeType == ATT_WRITE_CMD))
            {
                gattsSendWriteResp = (GATTS_SendWriteRespParams_T *)OSAL_Malloc(sizeof(GATTS_SendWriteRespParams_T));
                if(gattsSendWriteResp == NULL)
                {
                    return;
                }
                gattsSendWriteResp->attrHandle = p_event->attrHandle;
                if(p_event->writeType == ATT_WRITE_REQ)
                    gattsSendWriteResp->responseType = ATT_WRITE_RSP;
                else if(p_event->writeType == ATT_WRITE_REQ)
                    gattsSendWriteResp->responseType = ATT_PREPARE_WRITE_RSP;

                status = GATTS_SendWriteResponse(p_event->connHandle, gattsSendWriteResp);
                if (status == MBA_RES_SUCCESS)
                {
                    OSAL_Free(gattsSendWriteResp);
                    gattsSendWriteResp = NULL;
                }
            }
        }
        break;
        case CONN_SERV_HDL_CHARVAL_1:                         /**< Handle of characteristic 1 value. */
        {
            if ((p_event->writeType == ATT_WRITE_REQ)
                    || (p_event->writeType == ATT_PREPARE_WRITE_REQ)
                    || (p_event->writeType == ATT_WRITE_CMD))
            {
                gattsSendWriteResp = (GATTS_SendWriteRespParams_T *)OSAL_Malloc(sizeof(GATTS_SendWriteRespParams_T));
                if(gattsSendWriteResp == NULL)
                {
                    return;
                }
                gattsSendWriteResp->attrHandle = p_event->attrHandle;
                if(p_event->writeType == ATT_WRITE_REQ)
                    gattsSendWriteResp->responseType = ATT_WRITE_RSP;
                else if(p_event->writeType == ATT_PREPARE_WRITE_REQ)
                    gattsSendWriteResp->responseType = ATT_PREPARE_WRITE_RSP;

                status = GATTS_SendWriteResponse(p_event->connHandle, gattsSendWriteResp);
                if (status == MBA_RES_SUCCESS)
                {
                    OSAL_Free(gattsSendWriteResp);
                    gattsSendWriteResp = NULL;
                }
                memset(&cred, 0, sizeof(credentials));
                cred.sec_type = p_event->writeValue[p_event->valueOffset + WIFI_PROV_CRED_SECTYPE_POS];
                if(cred.sec_type == M2M_WIFI_SEC_OPEN)
                    cred.sec_type = SYS_WINCS_WIFI_SECURITY_OPEN;
                else if(cred.sec_type == M2M_WIFI_SEC_WPA_PSK)
                    cred.sec_type = SYS_WINCS_WIFI_SECURITY_WPA2_MIXED;
                else
                    cred.sec_type = 0xFF;
//                if ( ((cred.sec_type != SYS_WINCS_WIFI_SECURITY_OPEN) && (p_event->writeDataLength < (WIFI_PROVISION_MAX_SSID_LENGTH+WIFI_PROV_CRED_SSID_POS))) /*||
//                    (cred.sec_type ==  WDRV_WINC_AUTH_TYPE_WEP) || (cred.sec_type ==  WDRV_WINC_AUTH_TYPE_802_1X)*/)   
//                {
//                    appMsg.msgId =  APP_MSG_BLE_PROV_FAILED;
//                    OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);
//                    return;
//                }
                cred.ssid_length = p_event->writeValue[ p_event->valueOffset + WIFI_PROV_CRED_SSIDLEN_POS] > WIFI_PROVISION_MAX_SSID_LENGTH ? 
                    WIFI_PROVISION_MAX_SSID_LENGTH : p_event->writeValue[ p_event->valueOffset + WIFI_PROV_CRED_SSIDLEN_POS];
                memcpy(cred.ssid, &p_event->writeValue[p_event->valueOffset + WIFI_PROV_CRED_SSID_POS], cred.ssid_length);
                cred.passphrase_length = p_event->writeValue[p_event->valueOffset + WIFI_PROV_CRED_PASSLEN_POS]> WIFI_PROVISION_MAX_PASS_LENGTH ? 
                    WIFI_PROVISION_MAX_PASS_LENGTH: p_event->writeValue[p_event->valueOffset + WIFI_PROV_CRED_PASSLEN_POS];
                memcpy(cred.passphrase,  &p_event->writeValue[p_event->valueOffset + WIFI_PROV_CRED_PASS_POS], cred.passphrase_length);
                
                appMsg.msgId =  APP_MSG_BLE_PROV_COMPLETE;
                OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);
            }
        }
        break;
        case CONN_SERV_HDL_CHARVAL_0:
        {
            gattsSendErrResp = (GATTS_SendErrRespParams_T *)OSAL_Malloc(sizeof(GATTS_SendErrRespParams_T));
            if (gattsSendErrResp == NULL)
            {
                return;
            }
//            trsRespErrConnHandle = p_event.connHandle;
            gattsSendErrResp->reqOpcode = p_event->writeType;
            gattsSendErrResp->attrHandle = p_event->attrHandle;
            gattsSendErrResp->errorCode = ATT_ERR_INVALID_HANDLE;
            status = GATTS_SendErrorResponse(p_event->connHandle, gattsSendErrResp);
            if (status == MBA_RES_SUCCESS)
            {
                OSAL_Free(gattsSendErrResp);
                gattsSendErrResp = NULL;
            }
        }
        break;
                
    }

}

void APP_BleGapEvtHandler(BLE_GAP_Event_T *p_event)
{
    APP_Msg_T appMsgBle;
    switch(p_event->eventId)
    {
        case BLE_GAP_EVT_CONNECTED:
        {
            SYS_CONSOLE_PRINT("BLE Connected..\r\n");
            conn_hdl = p_event->eventField.evtConnect.connHandle;
        }
        break;

        case BLE_GAP_EVT_DISCONNECTED:
        {
            SYS_CONSOLE_PRINT("BLE Disconnected..0x%x\r\n",p_event->eventField.evtDisconnect.reason);
            conn_hdl = 0xFFFF;

            appMsgBle.msgId = APP_TIMER_WIFI_CONNECTION_MSG;
            OSAL_QUEUE_Send(&appData.appQueue, &appMsgBle, 0);
        }
        break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE:
        {
            SYS_CONSOLE_PRINT("BLE Conn Param Update..0x%x\r\n",p_event->eventField.evtConnParamUpdate.status);
        }
        break;

        case BLE_GAP_EVT_ENCRYPT_STATUS:
        {
            if(p_event->eventField.evtEncryptStatus.status == 0x00)
            {
                SYS_CONSOLE_PRINT("BLE link is encrypted successfully\r\n");
                SYS_CONSOLE_PRINT("\nPlease enter SSID, Passphrase in MBD app.\r\n");
                
            }
            else
            {
                SYS_CONSOLE_PRINT("BLE link encryption Failed\r\n");
            }
            
//            extern TaskHandle_t xWinc_Tasks;
//            vTaskResume(xWinc_Tasks);
        }
        break;

        case BLE_GAP_EVT_ADV_REPORT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_ENC_INFO_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_REMOTE_CONN_PARAM_REQUEST:
        {
            SYS_CONSOLE_PRINT("BLE Remote Conn Param Req:0x%x\r\n", p_event->eventField.evtRemoteConnParamReq.timeout);
        }
        break;

        case BLE_GAP_EVT_EXT_ADV_REPORT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_ADV_TIMEOUT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_TX_BUF_AVAILABLE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_DEVICE_NAME_CHANGED:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_AUTH_PAYLOAD_TIMEOUT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_PHY_UPDATE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_SCAN_REQ_RECEIVED:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_DIRECT_ADV_REPORT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_PERI_ADV_SYNC_EST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_PERI_ADV_REPORT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_PERI_ADV_SYNC_LOST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_ADV_SET_TERMINATED:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_SCAN_TIMEOUT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_TRANSMIT_POWER_REPORTING:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_ADV_COMPL:
        {
            /* TODO: implement your application code.*/

        }
        break;

        case BLE_GAP_EVT_PATH_LOSS_THRESHOLD:
        {
            /* TODO: implement your application code.*/
        }
        break;

        default:
        break;
    }
}

void APP_BleL2capEvtHandler(BLE_L2CAP_Event_T *p_event)
{
    switch(p_event->eventId)
    {
        case BLE_L2CAP_EVT_CONN_PARA_UPD_REQ:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CONN_PARA_UPD_RSP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CB_CONN_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CB_CONN_FAIL_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CB_SDU_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CB_ADD_CREDITS_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CB_DISC_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;        

        default:
        break;
    }
}

void APP_GattEvtHandler(GATT_Event_T *p_event)
{   
    switch(p_event->eventId)
    {
        case GATTC_EVT_ERROR_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_DISC_PRIM_SERV_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_DISC_PRIM_SERV_BY_UUID_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_DISC_CHAR_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_DISC_DESC_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_READ_USING_UUID_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_READ_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_WRITE_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_HV_NOTIFY:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_HV_INDICATE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_READ:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_WRITE:
        {
//            printf("wA:0x%x, wt:0x%x, wLen:%d\r\n",p_event->eventField.onWrite.attrHandle, 
//                    p_event->eventField.onWrite.writeType, p_event->eventField.onWrite.writeDataLength);
            APP_wbProv_GattSEvtWriteHandler(&p_event->eventField.onWrite);
        }
        break;

        case GATTS_EVT_HV_CONFIRM:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case ATT_EVT_TIMEOUT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case ATT_EVT_UPDATE_MTU:
        {
            /* TODO: implement your application code.*/
            //printf("-- ATT_EVT_UPDATE_MTU: %d\r\n", p_event->eventField.onUpdateMTU.exchangedMTU);
            SYS_CONSOLE_PRINT("-- ATT_EVT_UPDATE_MTU: %d\r\n", p_event->eventField.onUpdateMTU.exchangedMTU);
        }
        break;

        case GATTC_EVT_DISC_CHAR_BY_UUID_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_SERVICE_CHANGE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_CLIENT_FEATURE_CHANGE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_CLIENT_CCCDLIST_CHANGE:
        {
            /* TODO: implement your application code.*/
            OSAL_Free(p_event->eventField.onClientCccdListChange.p_cccdList);
        }
        break;

        case GATTC_EVT_PROTOCOL_AVAILABLE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_PROTOCOL_AVAILABLE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        default:
        break;        
    }
}

void APP_BleSmpEvtHandler(BLE_SMP_Event_T *p_event)
{
    switch(p_event->eventId)
    {
        case BLE_SMP_EVT_PAIRING_COMPLETE:
        {
            if(p_event->eventField.evtPairingComplete.status == 0x00)
            {
                SYS_CONSOLE_PRINT("Pairing Completed Successfully\r\n");
            }
            else if (p_event->eventField.evtPairingComplete.status == 0x01)
            {
                SYS_CONSOLE_PRINT("Pairing Failed - Incorrect Pin received\r\n");
            }
            else
            {
                 SYS_CONSOLE_PRINT("Pairing Failed - Time out error (0x%x)\r\n",p_event->eventField.evtPairingComplete.status);
            }
            
        }
        break;

        case BLE_SMP_EVT_SECURITY_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_NUMERIC_COMPARISON_CONFIRM_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_INPUT_PASSKEY_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_DISPLAY_PASSKEY_REQUEST:
        {
            uint8_t app_passkey[6];
            memset(app_passkey,0,sizeof(app_passkey));
            //BLE_SMP_GeneratePasskey(app_passkey);
            app_passkey[0] = 0x31;
            app_passkey[1] = 0x32;
            app_passkey[2] = 0x33;
            app_passkey[3] = 0x34;
            app_passkey[4] = 0x35;
            app_passkey[5] = 0x36;
            SYS_CONSOLE_PRINT("DisplayPass Key: \r\n\n");
            for(uint8_t i = 0; i<6; i++)
            {
                SYS_CONSOLE_PRINT("%c",app_passkey[i]);
            }
            SYS_CONSOLE_PRINT("\r\n\n");
            BLE_SMP_PasskeyReply(p_event->eventField.evtDisplayPasskeyReq.connHandle, app_passkey);
        }
        break;

        case BLE_SMP_EVT_NOTIFY_KEYS:
        {
            APP_ConnService_SendNotification(conn_hdl, NOTIFY_STATE_WIFICONNECTING);
        }
        break;

        case BLE_SMP_EVT_PAIRING_REQUEST:
        {
            SYS_CONSOLE_PRINT("Accepting Pairing Req\r\n");
            //BLE_SMP_AcceptPairingRequest(p_event->eventField.evtPairingReq.connHandle);     //sankar
        }
        break;

        case BLE_SMP_EVT_INPUT_OOB_DATA_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_INPUT_SC_OOB_DATA_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_KEYPRESS:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_GEN_SC_OOB_DATA_DONE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        default:
        break;        
    }
}

void APP_DmEvtHandler(BLE_DM_Event_T *p_event)
{
    switch(p_event->eventId)
    {
        case BLE_DM_EVT_DISCONNECTED:
        {
            /* TODO: implement your application code.*/
        }
        break;
        
        case BLE_DM_EVT_CONNECTED:
        {
            BLE_DM_ProceedSecurity(p_event->connHandle, 1);         //sankar
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_SECURITY_START:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_SECURITY_SUCCESS:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_SECURITY_FAIL:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_PAIRED_DEVICE_FULL:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_PAIRED_DEVICE_UPDATED:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_CONN_UPDATE_SUCCESS:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_CONN_UPDATE_FAIL:
        {
            /* TODO: implement your application code.*/
        }
        break;

        default:
        break;
    }
}
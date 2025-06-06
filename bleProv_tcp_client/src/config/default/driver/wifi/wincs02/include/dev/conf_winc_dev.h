/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/

#ifndef CONF_WINC_DEV_H
#define CONF_WINC_DEV_H

#include "configuration.h"
#include "definitions.h"


#define WINC_CONF_LOCK_STORAGE(NAME)        OSAL_MUTEX_HANDLE_TYPE NAME
#define WINC_CONF_LOCK_CREATE(NAME)         OSAL_MUTEX_Create(NAME)
#define WINC_CONF_LOCK_DESTROY(NAME)        OSAL_MUTEX_Delete(NAME)
#define WINC_CONF_LOCK_ENTER(NAME)          (OSAL_RESULT_SUCCESS == OSAL_MUTEX_Lock(NAME, OSAL_WAIT_FOREVER))
#define WINC_CONF_LOCK_LEAVE(NAME)          OSAL_MUTEX_Unlock(NAME)

#define WINC_CONF_ENABLE_NC_BERKELEY_SOCKETS

#endif /* CONF_WINC_DEV_H */

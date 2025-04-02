# PIC32CXBZ2 WBZ45x WINCS02 TCP CLIENT

<img src="Docs/IoT-Made-Easy-Logo.png" width=100>


> "IoT Made Easy!" 

Devices: **| WBZ451 | RNWF02 |**<br>
Features: **| Wi-Fi |**


## ⚠ Disclaimer

<p><span style="color:red"><b>
THE SOFTWARE ARE PROVIDED "AS IS" AND GIVE A PATH FOR SELF-SUPPORT AND SELF-MAINTENANCE. This repository contains example code intended to help accelerate client product development. </br>

For additional Microchip repos, see: <a href="https://github.com/Microchip-MPLAB-Harmony" target="_blank">https://github.com/Microchip-MPLAB-Harmony</a>

Checkout the <a href="https://microchipsupport.force.com/s/" target="_blank">Technical support portal</a> to access our knowledge base, community forums or submit support ticket requests.
</span></p></b>

## Contents

1. [Introduction](#step1)
1. [Bill of materials](#step2)
1. [Software Setup](#step3)
1. [Hardware Setup](#step4)
1. [Harmony MCC Configuration](#step5)
1. [Board Programming](#step6)
1. [Run the demo](#step7)

## 1. Introduction<a name="step1">

This application demonstrates a traditional open TCP client in the Host Companion mode using the WBZ451 Curiosity board. In this application the RNWF02 will host a TCP Client in Wi-Fi STA mode. The user is required to configure the Wi-Fi credentials for Home-AP and provide the necessary details for the remote TCP server configuration. By default the application will connect to Home-AP and begin a TCP client socket connection with a TCP server. After establishing a successful TCP server-client connection, the data exchange will take place.

## 2. Bill of materials<a name="step2">

| TOOLS | QUANTITY |
| :- | :- |
| [PIC32 WFI32E Curiosity Board](https://www.microchip.com/en-us/development-tool/ev12f11a) | 1 |
| [EVB-LAN8670-RMII](https://www.microchip.com/en-us/development-tool/EV06P90A) | 2 |
| [SAM E54 CURIOSITY ULTRA DEVELOPMENT BOARD](https://microchip.com/en-us/development-tool/dm320210) | 1 |

## 3. Software Setup<a name="step3">

- [MPLAB X IDE ](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide#tabs)

    - Version: 6.20
	- XC32 Compiler v4.45
	- MPLAB® Code Configurator v5.5.1
	- PIC32MZ-W_DFP v1.4.243
	- MCC Harmony
		- wireless_system_rnwf: v3.0.0
		- wireless_wifi: v3.12.0"}
		- csp: v3.19.7
		- core: v3.14.0
		- wireless_pic32cxbz_wbz: v1.4.0
		- wireless_ble: v1.3.0
		- wireless_system_pic32cxbz_wbz: v1.6.0
		- CMSIS_5: 5.9.0
		- FreeRTOS-Kernel: v11.1.0
		- wolfssl: v5.4.0
		- crypto: v3.8.2

- Any Serial Terminal application like [TERA TERM](https://download.cnet.com/Tera-Term/3000-2094_4-75766675.html) terminal application

- [MPLAB X IPE v6.20](https://microchipdeveloper.com/ipe:installation)

## 4. Hardware Setup<a name="step4">

- In the WBZ451 Curiositiy board please remove the resistor R122 and R124. Mount the RNWF02 Add on board on WBZ451 Curiosity board. 

## 5. Harmony MCC Configuration<a name="step5">

### Getting started with TCP Client application in WBZ451 Curiosity board 

| Tip | New users of MPLAB Code Configurator are recommended to go through the [overview](https://onlinedocs.microchip.com/pr/GUID-1F7007B8-9A46-4D03-AEED-650357BA760D-en-US-6/index.html?GUID-B5D058F5-1D0B-4720-8649-ACE5C0EEE2C0) |
| :- | :- |

**Step 1** - Connect the WBZ451 Curiosity board setup to the device/system using a micro-USB cable.

**Step 2** - Create a new MCC Harmony project.

**Step 3** - The "MCC - Harmony Project Graph" below depicts the harmony components utilized in this project.

![](Docs/Project_graph.png)

- [Generate](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-9C28F407-4879-4174-9963-2CF34161398E) the code.

- Clean and build the project. To run the project, select "Make and program device" button.

## 6. Board Programming<a name="step6">

### Program the precompiled hex file using MPLAB X IPE

- The application hex file can be found in the hex folder.

- Follow the steps provided in the link to [program the precompiled hex file](https://microchipdeveloper.com/ipe:programming-device) using MPLABX IPE to program the pre-compiled hex image. 

### Build and program the application using MPLAB X IDE

Follow the steps provided in the link to [Build and program the application](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32cxbz2_wbz45/tree/master/apps/ble/advanced_applications/ble_sensor#build-and-program-the-application-guid-3d55fb8a-5995-439d-bcd6-deae7e8e78ad-section).

## 7. Run the demo<a name="step7">

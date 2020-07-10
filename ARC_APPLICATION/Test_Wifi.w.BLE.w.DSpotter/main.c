/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */
#include "embARC.h"
#include "embARC_debug.h"

#include "board.h"
#include "esp8266.h"

// #include "rn4020.h"
#include "cc2541.h"

#include "Spotter.h"
#include "recorder.h"
#include "CYBase.mod.h"
#include "Group_1.mod.h"
#include "Group_2.mod.h"

#include "dev_uart.h"
#include "ez_sio.h"

#include <stdio.h>
#include <string.h>


/* RN4020 Pin Define */
/*
#define RN4020_UART_ID DFSS_UART_3_ID
#define RN4020_WAKE_SW_GPIO DFSS_GPIO_4B0_ID
#define RN4020_WAKE_SW_PIN 2
#define RN4020_WAKE_HW_GPIO DFSS_GPIO_4B0_ID
#define RN4020_WAKE_HW_PIN 1
#define RN4020_CMD_GPIO DFSS_GPIO_4B0_ID
#define RN4020_CMD_PIN 3
*/
/* End RN4020 Pin Define */ 

/* RN4200 UUID Define */
/*
uint8_t test_private_services_uuid[RN4020_PRIVATE_UUID_LENGTH_BYTES] =
				{0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

uint8_t test_private_characteristic_0_uuid[RN4020_PRIVATE_UUID_LENGTH_BYTES] =
				{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
*/
/* End RN4020 UUID Define */

/* Instance Declaration */
ESP8266_DEFINE(esp8266);
// RN4020_DEFINE(rn4020_ble, RN4020_UART_ID, RN4020_WAKE_SW_GPIO, RN4020_WAKE_SW_PIN
	// , RN4020_WAKE_HW_GPIO, RN4020_WAKE_HW_PIN, RN4020_CMD_GPIO, RN4020_CMD_PIN);
/* End Instance Declaration */

/* ESP8266 Buffer Define */
char ssid[100];
char pswd[100];
static char http_normal_req[] = "GET / HTTP/1.1\r\n\r\n\r\n";
static char http_initial_req[] = "GET /all HTTP/1.1\r\n\r\n";
static char server_ip[] = "192.168.137.1";
static int server_port = 5000;
static char http_client_buf[512];
uint8_t trialNum;
/* End ESP8266 Buffer Define */

/* ESP8266 Definitions */
#define ESP8266_MAX_CONN_TRIAL  3
#define ESP8266_WIFI_CONN_SUCCESS 2
/* End ESP8266 Definitions */

/* HM10 Buffer Define */
// EZ_SIO* hm10_uart;
// char hm10_rxbf[200];
cc2541_def* cc2541;
/* End HM10 Buffer Define */

/* DSpotter Memory Define */
//use fixed size memory to avoid heap too small
#define pMem ((void*)0x30000000)
#define nMemSize (0x00020000)
#define RECORD_TIMEOUT  5000
int16_t* pData;
uint32_t spottedMS;
uint8_t recNum = 0xff;
/* End DSPotter Memory Define */

/* MAC Addr Buffer Define */
char MACAddrArr[6][13];
/* End MAC Addr Buffer Define */

static int initRecognize(){
	int nRet;
	nRet= Recorder_Init();
	if(nRet!= 0){
		printf("record init fail: %d\r\n", nRet);
		return 1;
	}
	Spotter_Reset();
	Recorder_Start();	
	return nRet;
}

int main(void)
{
  /* HM10 Init */
  EMBARC_PRINTF("========================== Init HM10 =================================\n");
  cc2541 = cc2541_init(DFSS_UART_2_ID, UART_BAUDRATE_115200, 200, 200);
  if(cc2541==NULL)
    EMBARC_PRINTF("CC2541 Didn't Init Successfully.\n");
  /* End HM10 Init */

	/* ESP8266 Init */
  EMBARC_PRINTF("============================ Init ESP8266 ============================\n");
	esp8266_init(esp8266, UART_BAUDRATE_115200);
	at_test(esp8266->p_at);
  /* End ESP8266 Init */

	/* ESP8266 Set Mode */
	EMBARC_PRINTF("============================ Set Mode ============================\n");

	if(esp8266_wifi_mode_get(esp8266, true) != ESP8266_STA){
    esp8266_wifi_mode_set(esp8266, ESP8266_STA, true);
  }
  /* End ESP8266 Set Mode */

	/* ESP8266 Connect Wifi */
	EMBARC_PRINTF("============================ Connect WiFi ============================\n");
  esp8266_passthr_end(esp8266);
  board_delay_ms(1050, 1);
  // EMBARC_PRINTF("%d\n", esp8266_conn_status(esp8266));
  while(esp8266->wifi_connected == false){
    for(trialNum = 0; trialNum < ESP8266_MAX_CONN_TRIAL; trialNum ++){
      if(esp8266_conn_status(esp8266) == ESP8266_WIFI_CONN_SUCCESS)
        break;
    }
    if(esp8266->wifi_connected == false){
      EMBARC_PRINTF("Can't connect to Wifi...\n");
      if(cc2541_get_connection_role(cc2541) == 1){
        EMBARC_PRINTF("Setting CC2541 to Client.\n");
        cc2541_set_connection_role(cc2541, 0);
      }
      memset(ssid, 0, 100);
      memset(pswd, 0, 100);
      memset(cc2541->rxBuff, 0, 200);
      // ssid[0] = '\"';
      // pswd[0] = '\"';
      while(ssid[1] == 0 || pswd[1] == 0){
        // if(rb_isempty(&cc2541->uart->rcv_rb) == 0){
          cc2541_read(cc2541);
          if(cc2541->rxBuff[0]){
            char* token = strtok(cc2541->rxBuff, ";");
            sprintf(ssid, "\"%s\"", token);
            token = strtok(NULL, "\n");
            sprintf(pswd, "\"%s\"", token);
            EMBARC_PRINTF("Connected: %s\n", cc2541->rxBuff);
          }
          // else{
          //   input[0] = 0;
          // }
          board_delay_ms(100, 1);
          // EMBARC_PRINTF("Expected\n");
          // if(input[0] == 's'){
          //   sscanf(input, "ssid:%s", ssid + 1);
          //   strcat(ssid, "\"");
          // }
          // else if(input[0] == 'p'){
          //   sscanf(input, "pswd:%s", pswd + 1);
          //   strcat(pswd, "\"");
          // }
        // }
        // EMBARC_PRINTF("Expected\n");
      }
      esp8266_wifi_connect(esp8266, ssid, pswd, true);
    }
  }

  while(cc2541_test_AT(cc2541) != 0)
    EMBARC_PRINTF("Please disconnect BLE...\n");
  cc2541_set_connection_role(cc2541, 1);
  board_delay_ms(100, 1);
  /* End ESP8266 Connect Wifi */


	/* ESP8266 Connect Server */
	EMBARC_PRINTF("============================ Connect to Server ============================\n");

	if(esp8266_tcp_connect(esp8266, server_ip, server_port)!=AT_ERROR){
    board_delay_ms(1000, 1);
    int retVal = esp8266_passthr_start(esp8266);
    EMBARC_PRINTF("Succedd? %d", retVal);
    memcpy(MACAddrArr[0], "882583F020A8", 12);
  }
  /* End ESP8266 Connect Server */
  
  /* CC2541 Connect */
  ez_sio_write(cc2541->uart, "AT+INQ\r\n", 9);
  board_delay_ms(10000, 1);
  ez_sio_write(cc2541->uart, "AT+CONN1\r\n", 11);
  /* End CC2541 Connect */

  int nRet;
	printf("\r\nDSpotter version: %s\r\n", Spotter_GetVersion());

	nRet= Spotter_Init((void*)pCYBase_mod, (const void*[]){(void*)pGroup_1_mod, (void*)pGroup_2_mod, 0}, pMem, nMemSize);
	if(nRet!= 0){
		goto Exit;
	}
	Spotter_Reset();
	printf("Begin recognize\r\n");
  memset(cc2541->rxBuff, 0, 200);
  uint32_t startMS = OSP_GET_CUR_MS();
  initRecognize();
	while (1) {    
		pData= Recorder_GetBuffer();
		if(pData== 0) continue;
		nRet= Spotter_AddSample(pData, Recorder_GetBufferSize());
		if(nRet== SPOTTER_SUCCESS){
			int gpID = Spotter_GetResultGroupId(),
          cmdID = Spotter_GetResultId();
      printf("Spotter detect result gpID=%d cmdID=%d\n", gpID, cmdID);//辨識成功會在這邊顯示
      if(gpID == 0){
        recNum = 0x80;
        spottedMS = OSP_GET_CUR_MS();
      }
      else if(gpID == 1 && recNum == 0x80){
        recNum = cmdID;
      }
		}
    if(OSP_GET_CUR_MS() - spottedMS > RECORD_TIMEOUT && recNum == 0x80){
      printf("Timed out\n");
      recNum = 0xff;
    }
    if(recNum < 0x80){
      printf("Second Stage%d\n", recNum);
      ez_sio_write(cc2541->uart, (recNum == 0)?"on":"off", 3);
      int index = 0;
      recNum=0xfe;
    }
    Recorder_NextBuffer();
    if(OSP_GET_CUR_MS()  - startMS > 2000){
      esp8266_passthr_write(esp8266, http_normal_req, sizeof(http_normal_req) );
      at_read(esp8266->p_at, http_client_buf, 512);
      if(strstr(http_client_buf, "reg")){
        EMBARC_PRINTF("%s", http_client_buf);
      }
      else if(strstr(http_client_buf, "trig")){
        EMBARC_PRINTF("######START:%s\n", http_client_buf);
        ez_sio_write(cc2541->uart, "on", 2);
      }
      memset(http_client_buf, '\0', 512);
      startMS = OSP_GET_CUR_MS();
    }    
  }
  Recorder_Stop();
	Recorder_Release();
	printf("Done\r\n");
Exit:
 	Spotter_Release();
	return E_OK;
}
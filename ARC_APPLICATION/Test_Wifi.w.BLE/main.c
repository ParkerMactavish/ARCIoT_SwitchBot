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
static char http_client_req[] = 
    "GET /hello/ HTTP/1.1\r\n\r\n";
static char http_client_buf[2048];
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
  for(trialNum = 0; trialNum < ESP8266_MAX_CONN_TRIAL; trialNum ++){
    if(esp8266_conn_status(esp8266) == ESP8266_WIFI_CONN_SUCCESS)
      break;
  }
  if(trialNum == ESP8266_MAX_CONN_TRIAL){
    EMBARC_PRINTF("Can't connect to Wifi...\n");
    if(cc2541_get_connection_role(cc2541) == 1){
      EMBARC_PRINTF("Setting CC2541 to Client.\n");
      cc2541_set_connection_role(cc2541, 0);
    }
    memset(ssid, 0, 100);
    memset(pswd, 0, 100);
    memset(cc2541->rxBuff, 0, 200);
    ssid[0] = '\"';
    pswd[0] = '\"';
    char* input;
    while(ssid[1] == 0 || pswd[1] == 0){
      cc2541_readline(cc2541);

      if(cc2541->rxBuff[0]){
        input = cc2541->rxBuff;
        EMBARC_PRINTF("Connected: %s\n", input);
      }
      if(input[0] == 's'){
        sscanf(input, "ssid:%s", ssid + 1);
        strcat(ssid, "\"");
      }
      else if(input[0] == 'p'){
        sscanf(input, "pswd:%s", pswd + 1);
        strcat(ssid, "\"");
      }
    }
    esp8266_wifi_connect(esp8266, ssid, pswd, true);
    cc2541_set_connection_role(cc2541, 1);
    board_delay_ms(100, 1);
    //EMBARC_PRINTF("%s", hm10_uart->rcv_rb);
    while(1){

    }
  }
	
  /* End ESP8266 Connect Wifi */


	/* ESP8266 Connect Server */
	EMBARC_PRINTF("============================ Connect to Server ============================\n");

	if(esp8266_tcp_connect(esp8266, "192.168.137.1", 8000)!=AT_ERROR){
    board_delay_ms(1000, 1);
    esp8266_passthr_start(esp8266);
    board_delay_ms(1000, 1);
  }
  /* End ESP8266 Connect Server */

	while (1) {
    board_delay_ms(1000, 1);
    esp8266_passthr_write(esp8266, http_client_req, sizeof(http_client_req)-1);
	  memset(http_client_buf, 0, 2048);
    at_read(esp8266->p_at, http_client_buf, 2048);
    EMBARC_PRINTF("Received: %s\n", http_client_buf);
	}
	return E_OK;
}
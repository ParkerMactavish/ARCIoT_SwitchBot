/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

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
#include "dev_uart.h"
#include "hm1x.h"
#include "ez_sio.h"

uint32_t baudrate_list[] = {4800, 9600, 19200, 38400, 57600, 115200};

HM1X_DEFINE(hm1x, HM_1X_UART_ID);

uint8_t buff[100];
EZ_SIO* hm10_uart;

int main(void)
{
  io_mux_init();
  int32_t retVal = io_arduino_config(ARDUINO_PIN_0, ARDUINO_UART, IO_PINMUX_ENABLE);
  EMBARC_PRINTF("RetVal:%d\n", retVal);

  hm10_uart = ez_sio_open(hm1x->uart_id, UART_BAUDRATE_9600, 2048, 2048);
	uint8_t rcv_buf[200];
	int32_t rcv_cnt;
	uint32_t baudrate = UART_BAUDRATE_9600;
	uint32_t i = 0;
	/** change the init baudrate according to your ble module settings */
	EMBARC_PRINTF("Init HM1X with baudrate %dbps\r\n", baudrate);
	//hm1x_init(hm1x, baudrate);
  ez_sio_write(hm10_uart, (char*)"AT\r\n", 5);
  board_delay_ms(100, 1);
  ez_sio_read(hm10_uart, rcv_buf, 2);
  EMBARC_PRINTF("%s\n", rcv_buf);
  board_delay_ms(100, 1);
  memset(rcv_buf, 0, 200);

  ez_sio_write(hm10_uart, (char*)"AT+NAMEcentral\r\n", 16);
  board_delay_ms(100, 1);
  ez_sio_read(hm10_uart, rcv_buf, 200);  
  EMBARC_PRINTF("%s\n", rcv_buf);
  memset(rcv_buf, 0, 200);
  board_delay_ms(100, 1);
  
  // ez_sio_write(hm10_uart, (char*)"AT+IBEA1\r\n\0", 10);
  // board_delay_ms(1000, 1);
  // ez_sio_read(hm10_uart, rcv_buf, 200);  
  // EMBARC_PRINTF("%s\n", rcv_buf);
  // memset(rcv_buf, 0, 200);
  // board_delay_ms(1000, 1);

  // ez_sio_write(hm10_uart, (char*)"AT+ROLE1\r\n\0", 10);
  // board_delay_ms(1000, 1);
  // ez_sio_read(hm10_uart, rcv_buf, 200);  
  // EMBARC_PRINTF("%s\n", rcv_buf);
  // memset(rcv_buf, 0, 200);
  // board_delay_ms(1000, 1);

  // ez_sio_write(hm10_uart, (char*)"AT+IMME1\r\n\0", 10);
  // board_delay_ms(1000, 1);
  // ez_sio_read(hm10_uart, rcv_buf, 200);  
  // EMBARC_PRINTF("%s\n", rcv_buf);
  // memset(rcv_buf, 0, 200);
  // board_delay_ms(1000, 1);

  // ez_sio_write(hm10_uart, (char*)"AT+RESET\r\n\0", 10);
  // board_delay_ms(1000, 1);
  // ez_sio_read(hm10_uart, rcv_buf, 200);  
  // EMBARC_PRINTF("%s\n", rcv_buf);
  // memset(rcv_buf, 0, 200);
  // board_delay_ms(1000, 1);

  // ez_sio_write(hm10_uart, (char*)"AT+CON882583F02CBE\r\n", 20);
  // board_delay_ms(1000, 1);
  // ez_sio_read(hm10_uart, rcv_buf, 200);  
  // EMBARC_PRINTF("%s\n", rcv_buf);
  // memset(rcv_buf, 0, 200);
  // board_delay_ms(1000, 1);

  while(1){
    if(!rb_isempty(&hm10_uart->rcv_rb)){
      board_delay_ms(100, 1);
      ez_sio_read(hm10_uart, rcv_buf, 200);
      EMBARC_PRINTF("%s\n", rcv_buf);      
      memset(rcv_buf, 0, 200);
    }
  }
	/**
	 * For Detail use of HM1X, please refer to its documentation,
	 * but take notice of its wrong description
	 * English: https://github.com/danasf/hm10-android-arduino/blob/master/HM-10/datasheet.pdf
	 */

	/*do {
		EMBARC_PRINTF("++++Do following test with baudrate %dbps++++\r\n", baudrate);*/
		/** Here is a test sequence for HM1X, no need to add into HM1X init process */
		/*hm1x_flush(hm1x);

		if (hm1x_test_command(hm1x) == 0) {
			EMBARC_PRINTF("test command Successfully\r\n");
		} else {
			EMBARC_PRINTF("test command Failed\r\n");
		}

		if (hm1x_restart(hm1x) == 0) {
			EMBARC_PRINTF("Restart Successfully\r\n");
		} else {
			EMBARC_PRINTF("Restart Failed\r\n");
		}

		if (hm1x_set_mode(hm1x, BLE_HM1X_MODE_0) == 0) {
			EMBARC_PRINTF("SET MODE to %d\r\n", BLE_HM1X_MODE_0);
		} else {
			EMBARC_PRINTF("SET MODE Failed\r\n");
		}

		if (hm1x_set_type(hm1x, BLE_HM1X_TYPE_0) == 0) {
			EMBARC_PRINTF("SET TYPE to %d\r\n", BLE_HM1X_TYPE_0);
		} else {
			EMBARC_PRINTF("SET TYPE Failed\r\n");
		}

		if (hm1x_set_role(hm1x, BLE_HM1X_SLAVE_ROLE) == 0) {
			EMBARC_PRINTF("SET ROLE to %d\r\n", BLE_HM1X_SLAVE_ROLE);
		} else {
			EMBARC_PRINTF("SET ROLE Failed\r\n");
		}

		if ((rcv_cnt=hm1x_get_param(hm1x, "ROLE", (char *)rcv_buf)) && rcv_cnt >= 0) {
			EMBARC_PRINTF("GET ROLE:%s\r\n", rcv_buf);
		} else {
			EMBARC_PRINTF("GET ROLE Failed\r\n");
		}

		if ((rcv_cnt=hm1x_get_param(hm1x, "MODE", (char *)rcv_buf)) && rcv_cnt >= 0) {
			EMBARC_PRINTF("GET MODE:%s\r\n", rcv_buf);
		} else {
			EMBARC_PRINTF("GET MODE Failed\r\n");
		}

		if ((rcv_cnt=hm1x_get_param(hm1x, "TYPE", (char *)rcv_buf)) && rcv_cnt >= 0) {
			EMBARC_PRINTF("GET TYPE:%s\r\n", rcv_buf);
		} else {
			EMBARC_PRINTF("GET TYPE Failed\r\n");
		}*/

		/** how to use hm1x_exec_cmd_chkresp */
		/*if ((rcv_cnt=hm1x_exec_cmd_chkresp(hm1x, "AT+BATT?", "OK+Get:", (char *)rcv_buf, 2)) && rcv_cnt >= 0) {
			EMBARC_PRINTF("GET Battery Level:%s\r\n", rcv_buf);
		} else {
			EMBARC_PRINTF("GET Battery Failed\r\n");
		}
		if ((rcv_cnt=hm1x_exec_cmd_chkresp(hm1x, "AT+ADDR?", "OK+ADDR:", (char *)rcv_buf, 2)) && rcv_cnt >= 0) {
			EMBARC_PRINTF("GET HM1X MAC Address:%s\r\n", rcv_buf);
		} else {
			EMBARC_PRINTF("GET HM1X MAC Failed\r\n");
		}
		if ((rcv_cnt=hm1x_exec_cmd_chkresp(hm1x, "AT+NAME?", "OK+NAME:", (char *)rcv_buf, 2)) && rcv_cnt >= 0) {
			EMBARC_PRINTF("GET HM1X NAME:%s\r\n", rcv_buf);
		} else {
			EMBARC_PRINTF("GET HM1X NAME Failed\r\n");
		}*/

		/** how to use hm1x_exec_command */
		/*if ((rcv_cnt=hm1x_exec_command(hm1x, "AT+VERS?", (char *)rcv_buf, 500)) && rcv_cnt >= 0) {
			EMBARC_PRINTF("GET HM1X Version:%s\r\n", rcv_buf);
		} else {
			EMBARC_PRINTF("GET HM1X Version Failed\r\n");
		}

		EMBARC_PRINTF("++++Test with baudrate %dbps finished++++\r\n", baudrate);*/
		/**
		 * Here Set Baudrate to another one,
		 * but you also need to reconfigure the HM1X UART
		 * Then go through the test again
		 */
		/*if (hm1x_set_baud(hm1x, baudrate_list[i]) >=0) {
			if (hm1x_restart(hm1x) == 0) {
				baudrate = baudrate_list[i];
				EMBARC_PRINTF("Re-Init the HM1X BLE Baudrate to %dbps, and restart the test\r\n", baudrate);
				hm1x_init(hm1x, baudrate);
			} else {
				break;
			}
		} else {
			break;
		}*/
		/** Test sequence end */
	/*} while (i++ < sizeof(baudrate_list)-1);

	EMBARC_PRINTF("\r\nTry to reset the baudrate to %dbps\r\n", UART_BAUDRATE_9600);
	if (hm1x_set_baud(hm1x, UART_BAUDRATE_9600) >=0) {
		if (hm1x_restart(hm1x) == 0) {
			hm1x_init(hm1x, UART_BAUDRATE_9600);
			baudrate = UART_BAUDRATE_9600;
		}
	}
	if (baudrate != UART_BAUDRATE_9600) {
		EMBARC_PRINTF("!!!CAUTION:!!!Next Time run this test may fail due to initial baudrate changed\r\n");
	}
	EMBARC_PRINTF("Do send and receive test under baudrate:%dbps\r\n", baudrate);

	EMBARC_PRINTF("Start HM1X-BLE Send Receive Test, please connect to HM1X-BLE using its app.\r\n");
	EMBARC_PRINTF("The main loop will receive data from HM1X, and send back what it received.\r\n");

	while (1) {
		rcv_cnt = hm1x_read(hm1x, rcv_buf, sizeof(rcv_buf));
		rcv_buf[rcv_cnt] = '\0';
		if (rcv_cnt) {
			EMBARC_PRINTF("%s", rcv_buf);
			hm1x_write(hm1x, rcv_buf, rcv_cnt);
		}
	}*/

	return E_SYS;	/* system error */
}
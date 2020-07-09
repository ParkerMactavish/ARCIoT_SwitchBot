#include "cc2541.h"
#include "board.h"

cc2541_def* cc2541_init(uint32_t uartId, uint32_t uartBaudRate, uint32_t txBuffSize, uint32_t rxBuffSize){
  io_mux_init();
  io_arduino_config(ARDUINO_PIN_0, ARDUINO_UART, IO_PINMUX_ENABLE);
  cc2541_def* cc2541;
  cc2541 = malloc(sizeof(cc2541_def));
  cc2541->uart = ez_sio_open(uartId, uartBaudRate, txBuffSize, rxBuffSize);
  cc2541->rxBuff = malloc(sizeof(char)*rxBuffSize);
  ez_sio_write(cc2541->uart, "AT\r\n", 5);
  if(cc2541_readline(cc2541) == 0)
    return cc2541;
  else return NULL;
}

int cc2541_get_connection_role(cc2541_def* cc2541){
  ez_sio_write(cc2541->uart, "AT+ROLE\r\n", 10);
  cc2541_readline(cc2541);
  int role;
  sscanf(cc2541->rxBuff, "+ROLE=%d", &role);
  return role;
}

int cc2541_set_connection_role(cc2541_def* cc2541, int role){
  if(role == 0){
    ez_sio_write(cc2541->uart, "AT+ROLE0\r\n", 11);
    cc2541_readline(cc2541);
    if(strstr(cc2541->rxBuff, "ROLE0")){
      cc2541_readline(cc2541);
      return 0;
    }
    cc2541_readline(cc2541);
    return 1;
  }
  else if(role == 1){
    ez_sio_write(cc2541->uart, "AT+ROLE1\r\n", 11);
    cc2541_readline(cc2541);
    if(strstr(cc2541->rxBuff, "ROLE1")){
      cc2541_readline(cc2541);
      return 0;
    }
    cc2541_readline(cc2541);
    return 1;
  }
}

uint32_t cc2541_readline(cc2541_def* cc2541){
  uint32_t cur_ofs = 0;
	uint32_t read_cnt;
	uint32_t cur_time = OSP_GET_CUR_MS();
  memset(cc2541->rxBuff, 0, 200);
  board_delay_ms(100, 1);
	do {
		read_cnt = ez_sio_read(cc2541->uart, &cc2541->rxBuff[cur_ofs], 1);
    if(cc2541->rxBuff[cur_ofs] == '\n')
      break;
		cur_ofs += read_cnt;
	} while ((OSP_GET_CUR_MS()-cur_time) < CC2541_TIMEOUT);
  if(cur_ofs > 0){
    cc2541->rxBuff[cur_ofs - 1] = '\0';
    EMBARC_PRINTF("CC2541: \"%s\" (%d)\r\n", cc2541->rxBuff, strlen(cc2541->rxBuff));
  }

	if (strstr(cc2541->rxBuff, "OK")!= NULL) {
		return 0;
	}

	return -1;
}
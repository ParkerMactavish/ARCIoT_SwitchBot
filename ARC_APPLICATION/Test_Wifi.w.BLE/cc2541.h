#ifndef _CC2541_H
#define _CC2541_H

#include "stdio.h"
#include "stdlib.h"
#include "embARC_debug.h"
#include "embARC.h"
#include "board.h"
#include "string.h"
#include "ez_sio.h"

typedef struct{
  EZ_SIO* uart;
  char* rxBuff;
} cc2541_def;

#define CC2541_TIMEOUT  10000

cc2541_def* cc2541_init(uint32_t uartId, uint32_t uartBaudRate, uint32_t txBuffSize, uint32_t rxBuffSize);
uint32_t cc2541_readline(cc2541_def* cc2541);
int cc2541_get_connection_role(cc2541_def* cc2541);
int cc2541_set_connection_role(cc2541_def* cc2541, int role);

#endif
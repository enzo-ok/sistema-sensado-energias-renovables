#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#define ADDRESS_AP 0
#define BROADCAST_ADDRESS 0xFFFF



//----------- CONFIGURACIONES: -------------------------------------------------
#define NETWORK_REQUESTS 5
#define NETWORK_RESPONSES 5

#define NETWORK_BROADCASTS 3

#define LONG_TRAMA        LONG_DATA_FIELD + 9    // bytes
#define LONG_DATA_FIELD   8    // bytes
#define APP_LONG_DATA	  LONG_DATA_FIELD + 3
#define BAUD_RATE         10    // Kbps

#define TIMEOUT_RETRANS   500  // mseg.
#define CANT_RETRANS      5    // times
#define CANT_TX_RETRY     10   // times

#define MS_PER_PACKET (((LONG_TRAMA + 12) * 8) / BAUD_RATE) + 1

#define RANGO_REQ      100

#define LONG_BUFFER_LAST_RX    10

//- COMMANDS DEFINITIONS -------------------------------------------------------
// Command types -> Request
//               -> Response and one-way commands



//------------------------------------------------------------------------------
//- ERRORS DEFINITION: ---------------------------------------------------------
#define RESPONSE_OK     0
#define TX_OK           0
#define CMD_OK          1
#define TIMEOUT_ERR     2
#define TX_ERR          3
//------------------------------------------------------------------------------

extern uint16_t net_myAddr;

struct str_nodestatus {
	uint16_t Vbat;      	// Battery Level
	int8_t rx_rssiDbm;     // RSSI Level
	uint8_t rx_lqi;         // Link Quality Indicator
};
extern struct str_nodestatus net_nodeStatus;

struct _str_comm {   //PDU
	uint16_t dir_destino;
	uint16_t dir_origen;
	char data[LONG_DATA_FIELD];
	char command;
};
typedef struct _str_comm str_comm;

//--------------- Servicios que provee: --------------------------------------
char net_SendRequest(uint16_t dir_dest, char comando, char *datos);
char net_SendResponse(uint16_t dir_dest, char comando, char *datos);
char net_SendBroadcast(char comando, char retrans, int retrans_time_ms,char id_paquete ,char *datos);
void net_GetDataIn(str_comm *datos);
void net_ProcessNetwork(void);         //Se debe ejecutar en el bucle del main()
void net_InitNetwork(void);             //Inicializacion del stack de red.
void net_SetSemilla(int semilla);
void net_setMessageCallback(char comando, void func(void));

int8_t net_getRSSI(void);
unsigned int _net_Rnd(unsigned int max);
char _net_GetNewPacketID(void);


//------------------------------ BSP Functions --------------------------------
//TODO: tenian __monitor, habria que desabilitar las interrupciones
void net_RxPacketISR(void);
void net_TimerISR_1ms(void);

extern uint8_t RF_ReceivePacket(uint8_t *addr, uint8_t *rxBuffer, uint8_t *length, int8_t *rssiDbm, uint8_t *lqi);
extern uint8_t RF_SendPacket(uint8_t addr, uint8_t *txBuffer, uint8_t size);
extern void bsp_msDelay(uint16_t ms);


#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif

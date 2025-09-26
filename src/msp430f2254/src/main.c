#define MY_ADDRESS		0			//MASTER
#define DEST_ADDRESS	1
//#define MAX_ADDRESS	2

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <msp430f2254.h>
#include "bsp/bsp.h"
#include "bsp/hal/TI_MSP_UART.h"
#include "libnetwork/Network.h"
#include "common-lib/trama_info.h"

//Prototipos de funcion-------------------------------------------
void app_reqSample(void);
void app_respSample(void);

void sendDataAP(str_comm *datos);
void app_app_ProcessUart(void);
void app_BufferFormat(uint8_t *buffer);


void app_initMessages(void);
void app_SwitchISR(void);
void app_GetParametersFlash(void);
void app_SaveParametersFlash(void);
void app_ProcessUart();

inline void set_TimeStamp(uint32_t ts);
inline uint32_t get_TimeStamp(void);
void send_routine(uint8_t retrans, uint8_t pktID);

//------------------------------------------------------------------------------

//-Definiciones-----------------------------------------------------------------

//definiciones para procesar la uart
#define ADDRESS_CHAR_SIZE 5
#define COMMAND_CHAR_SIZE 3
#define LOAD_CHAR_SIZE 16
//SI SE CAMBIA EL TAMA�O DE ALGUNO DE ESTOS HAY QUE CAMBIAR EL TAMA�O DEL BUFFER DE LA UART

//definiciones para la calibracion
#define NO_CALIBRADO			0
#define COMIENZO_CALIBRACION	1
#define FIN_CALIBRACION 		2
#define COMIENZO_TRABAJO		3
#define CALIBRADO				5

//definiciones de Tiempos
#define CYCLE_PERIOD_2SEG		10 		// Periodo de ciclo = CYCLE_PERIOD x 2 [seg]
#define TS_COMPENSATION_TIME	918		// Tiempo de compensacion del TimeStamp por delay de comunicacion [unidades de TAR] (1/32768 seg)

#define WIN_TIME_MS				2000 	// Tiempo de duracion total de la ventana de un ciclo [mseg]
#define WIN_DIVISION_TIME    	WIN_TIME_MS/5

typedef enum {
	RADIO_OFF = 0, RADIO_ON
} sleepRadioState;
typedef enum {
	T1ms_OFF = 0, T1ms_ON
} sleepTimer1msState;

const void* LedVerde;
const void* LedRojo;
const void* Switch1;

#define MADRE 1
#define HIJO 2

//-Posiciones de indices de parametros de configuracion en flash ----------------
#define FLASH_DATA_SIZE 10

//-Variables--------------------------------------------------------------------
uint16_t net_myAddr = MY_ADDRESS;
char app_dataBuffer[LONG_DATA_FIELD]; //un buffer para guardar la carga util de las tramas
uint8_t uartRxBuf[UART_BUFFER_SIZE]; //buffer para la uart

char app_DataRFBuffer[LONG_DATA_FIELD];

struct _str_comm str_formated;

uint16_t tStamp_H = 0;
uint8_t EstadoCalibration = 0;

uint32_t timeStamp_inicial;
uint32_t timeStamp_final;
uint32_t timestamprx;
int32_t diferencia[2];

uint32_t nextCorrect;
uint32_t nextCycle;
uint32_t timeCycle = CYCLE_PERIOD_2SEG; // t o d o: Analizar si en necesario mantener esta variable

uint16_t timeoutSwitch;
uint16_t timeout_winEnd = 0;
uint16_t timeout_winDivEnd = 0;
uint16_t timeout_sendRoutine = 0;

//------------------------------------------------------------------------------

//-Estructuras de la aplicacion-------------------------------------------------
struct str_nodestatus net_nodeStatus;

struct str_appFlags {
	uint8_t timerA_rebase :1;
	uint8_t sync :1;
	uint8_t insideWindow :1;
	uint8_t CalibInProc :1;
	sleepRadioState sleepRxState :1;
	sleepTimer1msState sleepT1msState :1;
} appFlags;

//-Estructuras de los modulos---------------------------------------------------
struct module_data {
	uint16_t address;
	char cncd_data[LONG_DATA_FIELD];
	char type;
} mod_data;

// bandera de eventos
struct str_appFlags_Event {
	uint8_t Switch :1;
	uint8_t Cycle :1;
} appFlags_Event;

union u_flash {
	struct {
		int16_t offsetCorrection; // Cuanto se corrige (Se calcula por proceso de calibracion)
		uint8_t notCalibrated;
		uint8_t nodeType; //1=Madre;2=Hijo
		uint8_t rssiMin;
		uint8_t rssiMax;
		uint32_t timeCorrect; //Cada cuanto corrige (Se calcula por proceso de calibracion)
	} app_flash;
	uint8_t flashBuffer[FLASH_DATA_SIZE];
} flash;

uint8_t rx_bufferSerie[5];
uint8_t tx_bufferSerie[60];
volatile uint16_t timeStampTest;

//------------------------------------------------------------------------------
//	Function: Main
//	Desc.:
//	Parametros: void
//	Devuelve: void
//------------------------------------------------------------------------------
void main(void) {

	int i=0;
	char app_DataTxBuffer[LONG_DATA_FIELD];
	bsp_Init();
	//juego de luces
	for(i = 0; i<3; i++){
		led_On(LedVerde);
		bsp_msDelay(300);
		led_Off(LedVerde);
		bsp_msDelay(200);
	}
	net_InitNetwork();
	app_initMessages();
	while (1) {
		int cant = LONG_DATA_FIELD;
		net_ProcessNetwork();
		app_ProcessUart();

		net_SendRequest(DEST_ADDRESS, REQ_SAMPLE, app_DataTxBuffer);
		UART_Tx_string(app_DataRFBuffer,cant);
		bsp_msDelay(10000);
	}
}

void app_SwitchISR(void) {
	appFlags_Event.Switch = 1;
}

//------------------------------------------------------------------------------
//	Funcciones externas
//	Desc.: son las funciones externas del BSP o del Networks
//	Parametros: void
//	Devuelve: void
//------------------------------------------------------------------------------
uint8_t app_TimerISR_1ms(void) {

	uint8_t sleepMode = 0;


	return sleepMode;
}

uint8_t app_TimerISR_2s(void) {

	uint8_t sleepMode = 0;


	return sleepMode;
}

//------------------------------------------------------------------------------
//	Function: sendDataAP
//	Desc.: envia por el puerto serie tramas con un formato determinado
//	Parametros: * buffer (puntero del buffer a ser enviado)
//	Devuelve: void
//------------------------------------------------------------------------------
void sendDataAP(str_comm *datos) {
	uint8_t i;

	sprintf((char *)uartRxBuf, "%05u|%03u|", datos->dir_origen, datos->command);
	serie_SendString((char *)uartRxBuf, 0);

	for (i = 0; i < (LONG_DATA_FIELD); ++i) {
		sprintf((char *)uartRxBuf, "%02X", datos->data[i]);
		serie_SendString((char *)uartRxBuf, 0);
	}
	serie_SendString("\r", 0);
}


//------------------------------------------------------------------------------
//	Function: recieveDataAP
//	Desc.: recive los datos del PC y dependiendo que llega, funciona como pasa mano.
//	Parametros: void
//	Devuelve: void
//------------------------------------------------------------------------------
void app_ProcessUart(void) {
	struct _str_comm pc_str_comm;
	uint8_t i;
	//El enter en hexa es 0x0D
	if (!(UART_data_peak() == 0x0D)) {
		return;
	}
	UART_get_data((char *)uartRxBuf);
	app_BufferFormat(uartRxBuf);

	if (str_formated.dir_destino == 0) {
		pc_str_comm.dir_origen = 0;
		pc_str_comm.command = 0;

		for (i = 0; i <= (LONG_DATA_FIELD - 1); i++) {
			pc_str_comm.data[i] = 0xFF;
		}

		sendDataAP(&pc_str_comm);

	} else {
		switch (str_formated.command) {

			default:
			net_SendRequest(str_formated.dir_destino, str_formated.command,
					str_formated.data);
			break;
		}
	}

}

//------------------------------------------------------------------------------
//	Function: app_BufferFormat
//	Desc.: formatea el buffer y lo deja listo para mandarlo por la network
//	Parametros: el puntero al buffer que se desea convertir y el tamaño del mismo
//	Devuelve: el buffer formateado queda guardado en str_formated
//------------------------------------------------------------------------------
void app_BufferFormat(uint8_t *buffer) {
	uint8_t i, j = 0;
	uint8_t command = 0;
	uint8_t load[LONG_DATA_FIELD];
	uint16_t addr = 0;
	uint16_t factor;

	factor = 10000;
	for (i = 0; i < ADDRESS_CHAR_SIZE; i++) {
		buffer[i] -= 0x30;
		addr += buffer[i] * factor;
		factor = factor / 10;
	}
	factor = 100;
	for (i = (ADDRESS_CHAR_SIZE + 1);
			i < (ADDRESS_CHAR_SIZE + COMMAND_CHAR_SIZE + 1); i++) {
		buffer[i] -= 0x30;
		command += buffer[i] * factor;
		factor = factor / 10;
	}

	for (i = (ADDRESS_CHAR_SIZE + COMMAND_CHAR_SIZE + 2);
			i < (ADDRESS_CHAR_SIZE + COMMAND_CHAR_SIZE + LOAD_CHAR_SIZE + 2);
			i++) {
		if (buffer[i] >= 0x30 && buffer[i] <= 0x39)
			buffer[i] -= 0x30;
		if (buffer[i] >= 0x40 && buffer[i] <= 0x46)
			buffer[i] -= 0x37;
	}
	j = 0;
	for (i = (ADDRESS_CHAR_SIZE + COMMAND_CHAR_SIZE + 2);
			i < (ADDRESS_CHAR_SIZE + COMMAND_CHAR_SIZE + LOAD_CHAR_SIZE + 2);
			i += 2) {
		load[j] = ((buffer[i] << 4) | buffer[i + 1]);
		j++;
	}

	str_formated.dir_origen = ADDRESS_AP;
	str_formated.dir_destino = addr;
	str_formated.command = command;

	for (i = 0; i < LONG_DATA_FIELD; i++)
		str_formated.data[i] = load[i];

	return;
}

void timeOut(void) {
	struct _str_comm app_str_comm;

	app_str_comm.dir_origen = 0;
	app_str_comm.command = 1;

	sendDataAP(&app_str_comm);
}


//------------------------------------------------------------------------------
//	Function: app_initMessages
//	Desc.: configura los arreglos de punteros a funciones
//	Parametros: void
//	Devuelve: void
//------------------------------------------------------------------------------
void app_initMessages(void) {

	net_setMessageCallback(REQ_SAMPLE, app_reqSample);
	net_setMessageCallback(RESP_SAMPLE, app_respSample);

}

//------------------------------------------------------------------------------
//	Function: app_reqSample
//	Desc.: se llama desde el network cuando llega un REQ_SAMPLE (el AP solicita una muestra)
//	Parametros: void
//	Devuelve: void
//------------------------------------------------------------------------------
void app_reqSample(void) {

	struct _str_comm app_str_comm;

	net_GetDataIn(&app_str_comm);	// Aca obtendrias en app_str_comm la Data que contenia la trama reqSample

	net_SendResponse(app_str_comm.dir_origen, RESP_SAMPLE, app_dataBuffer);
}


void app_respSample(void){
	char info [LONG_DATA_FIELD];
	struct _str_comm app_str_comm;

	net_GetDataIn(&app_str_comm);	// Aca obtendrias en app_str_comm la Data que contenia la trama reqSample
	int k = 0;

	for(k = 0; k < LONG_DATA_FIELD; k++){

		info [k] = app_str_comm.data[k];
		app_DataRFBuffer[k] = info [k];
	}
	//k=3312;

	if(app_str_comm.data[0] == 1)
		led_Toggle(LedVerde);
}





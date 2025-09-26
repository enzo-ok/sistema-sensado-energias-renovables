#ifndef TRAMAINFO_H
#define TRAMAINFO_H


//-REQUESTS Y RESPONSES---------------------------------------------------------
// Request:
#define REQ_CONFIG 000
#define	REQ_SAMPLE 001
#define REQ_STATE 002
#define REQ_COM_END 003
#define REQ_FIND 004

#define	REQ_CALIBRATION NETWORK_REQUESTS //BROADCAST
#define REQ_ROUTINE NETWORK_REQUESTS + 1 //BROADCAST
#define REQ_KEEP_ALIVE NETWORK_REQUESTS + 2	 //BROADCAST
// Response:
#define	RESP_CONFIG_ACK 100
#define	RESP_SAMPLE 101
#define	RESP_STATE 102
#define	RESP_COM_END_ACK 103
#define RESP_FIND 104




struct str_Config {
	uint16_t NodoType;				//Tipo de nodo: Madre o Cria
	uint16_t rssi_min;
	uint16_t rssi_max;
	uint16_t NonUsed;			//Espacio para igualar el tamaï¿½o de las cargas
};
struct str_RespSample {
	uint16_t AddrRecieved; //Direccion del nodo con el que se tomo la muestra (No es ni Tx ni Rx de la comunicacion)
	int16_t rssi;	//Indicador de fuerza de la seï¿½al recibida en la muestra
	uint32_t Timestamp;	//Este campo se utiliza para saber cuando se tomo la muestra
};

struct str_ReqSample {
	uint8_t NumberOfSample; //Direccion del nodo con el que se tomo la muestra (No es ni Tx ni Rx de la comunicacion)
	uint8_t  SA1; //Valor del Sensor Analógico 1
	uint16_t SA2; //Valor del Sensor Analógico 2
	uint16_t SD1; //Valor del Sensor Digital 1
	uint16_t SD2; //Valor del Sensor Digital 2

};

struct _str_CalibrationData {
	uint32_t TimeStamp;	//Time Stamp se utilizan para realizar la calibracion
	uint8_t Stage;		//Indica el Inicio y fin de la etapa de calibracion
	uint8_t retrans;	//numero de retransmicion
	uint16_t NonUsed;
};
typedef struct _str_CalibrationData str_CalibrationData;

struct str_Req_State {
	uint16_t Battery;				//Indica el nivel de bateria del nodo
	uint8_t State;			//Para indicar en que estado se encuentra el nodo
	uint8_t NodoType;				//Tipo de nodo: Madre o Cria
	uint16_t Samples;			//Cantidad de muestras almacenadas en el nodo
	uint16_t NonUsed;
};

struct str_Resp_State {
	uint32_t TimeStamp;
	uint16_t NonUsed;
	uint16_t NonUse1;
};
struct str_Routine {			//estructura que se manda en cada ciclo
	uint32_t TimeStamp;				//Time Stamp
	uint16_t State; 			//Para indicar en que estado se encuentra el nodo
	uint16_t retrans;	//numero de retransmicion
};


#endif

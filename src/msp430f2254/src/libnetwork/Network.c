/*******************************************************************************
 Filename: network.c

 Author: Agustin Laprovitta
 Oct 2011 - Universidad Catolica de Cordoba
 *******************************************************************************/

#include    "Network.h"
#include 	"../bsp/system.h"
#include 	"../bsp/bsp.h"

/*
 ********************************************************************************
 *                           FUNCTION PROTOTYPES
 ********************************************************************************
 */
void _net_ArmarTrama(uint16_t dir_dest, char ID_paq, char comando, char *datos);
unsigned int _net_Rnd(unsigned int max);
char _net_SendPacket(void);
void _net_AnalizarTrama(void);
char _net_Checksum(volatile char *ChkBuffer, char longitud, char PosChksum);
char _net_GetNewPacketID(void);

/*
 ********************************************************************************
 *                                VARIABLES
 ********************************************************************************
 */

struct str_net_flags {   // Flags de la capa de Red
	char RXpacket :1; // Llego un paquete por RF
	char TXpacket :1; // Paquete listo para enviar
	char TxMacError :1;
	char TXerror :1;
	char responseOK :1;
} net_flags;

struct str_net_status {
	char rxPackID;        // ID del ultimo paquete recibido
	char txPackID;        //ID del paquete a transmitir
	int iTimeOutRetrans;
	char cCantRetrans;
	char rxLastReqID[LONG_BUFFER_LAST_RX];
	char rxLastReqAddr[LONG_BUFFER_LAST_RX];
} net_status;

struct str_trama {
	char direccion;
	char dummy;
	char IDpaq;
	uint16_t dir_destino;
	uint16_t dir_origen;   //esta seria otro nodo para saltos
	char comando;
	char data[LONG_DATA_FIELD];
	char checksum;
};

union u_trama {
	struct str_trama trama;
	uint8_t Buffer[LONG_TRAMA + 1];
} rx, tx;

void (*requests[NETWORK_REQUESTS + NETWORK_BROADCASTS])(void);
void (*responses[NETWORK_RESPONSES])(void);

uint32_t randSemilla;

volatile int broadcast_timeOut;
//------------------------------------------------------------------------------
// Function:  net_InitNetwork() 
// Descr.:    Inicializa las variables de node_status
// Parametros: void
// Devuelve:   void
//------------------------------------------------------------------------------  
void net_InitNetwork(void) {
	uint8_t i;
	net_status.rxPackID = 0;
	net_status.txPackID = 0;
	//net_status.cPaquetesEnviados = 0;
	net_status.cCantRetrans = 0;
	net_status.iTimeOutRetrans = 0;
	net_flags.RXpacket = 0;
	for (i = 0; i < LONG_BUFFER_LAST_RX; i++) {
		net_status.rxLastReqID[i] = 0;
		net_status.rxLastReqAddr[i] = 0;
	}

	for (i = 0; i < (NETWORK_REQUESTS + NETWORK_BROADCASTS); i++) {
		requests[i] = 0;
	}
	for (i = 0; i < NETWORK_RESPONSES; i++) {
		responses[i] = 0;
	}

}

//------------------------------------------------------------------------------
// Function:  net_setMessageCallback(char tipo, char comando, funct *)
// Descr.:    Envia una trama tipo Request y realiza el control de retrans
// Parametros:
// Devuelve:  TX_ERR, TIMEOUT_ERR o RESPONSE_OK
//------------------------------------------------------------------------------
void net_setMessageCallback(char comando, void func(void)) {
	if (comando < NETWORK_REQUESTS + NETWORK_BROADCASTS) {
		requests[comando] = func;
	} else if (comando >= 100 && comando < 100 + NETWORK_RESPONSES) {
		responses[comando - 100] = func;
	}
}

//------------------------------------------------------------------------------
// Function:  net_SendRequest() 
// Descr.:    Envia una trama tipo Request y realiza el control de retrans
// Parametros: 
// Devuelve:  TX_ERR, TIMEOUT_ERR o RESPONSE_OK
//------------------------------------------------------------------------------  
char net_SendRequest(uint16_t dir_dest, char comando, char *datos) {

	net_status.txPackID = _net_GetNewPacketID();
	_net_ArmarTrama(dir_dest, net_status.txPackID, comando, datos); //coloca todos los datos en el bufferTX

	net_flags.responseOK = 0;
	net_status.iTimeOutRetrans = 0;
	net_status.cCantRetrans = 0;

	if (_net_SendPacket()) {
		net_status.iTimeOutRetrans = TIMEOUT_RETRANS; //Setea las retransmisiones para que la interrupcion se encargue
		net_status.cCantRetrans = (CANT_RETRANS + 1); // de esperar y retransmitir
	}

	while ((!net_flags.TXerror) && (!net_flags.responseOK) && (net_status.cCantRetrans > 0)) {
		net_ProcessNetwork();
	}

	if (net_flags.TXerror)
		return (TX_ERR);
	else if (net_flags.responseOK)
		return (RESPONSE_OK);
	else
		return (TIMEOUT_ERR);
}

//------------------------------------------------------------------------------
// Function:  net_SendResponse() 
// Descr.:    Envia una trama tipo Response 
// Parametros: 
// Devuelve:  TRUE or FALSE
//------------------------------------------------------------------------------              
char net_SendResponse(uint16_t dir_dest, char comando, char *datos) {

	_net_ArmarTrama(dir_dest, net_status.rxPackID, comando, datos); //coloca todos los datos en el bufferTX
	return (_net_SendPacket());
}

//------------------------------------------------------------------------------
// Function:  net_SendBroadcast()
// Descr.:    Envia una trama tipo Broadcast
// Parametros: Void
// Devuelve:  Void
//------------------------------------------------------------------------------
char net_SendBroadcast(char comando, char retrans, int retrans_time_ms, char id_paquete, char *datos) {
	uint8_t counter, send_status, cant_retrans_ok = 0;

	if (!id_paquete) {
		net_status.txPackID = _net_GetNewPacketID();
		_net_ArmarTrama(BROADCAST_ADDRESS, net_status.txPackID, comando, datos); //coloca todos los datos en el bufferTX
	} else {

		_net_ArmarTrama(BROADCAST_ADDRESS, id_paquete, comando, datos); //coloca todos los datos en el bufferTX
	}
	for (counter = retrans; counter > 0; counter--) {

		broadcast_timeOut = retrans_time_ms;

		do {
			send_status = RF_SendPacket(tx.trama.direccion, &tx.Buffer[1], (uint8_t) LONG_TRAMA);

		} while (!send_status && broadcast_timeOut);

		if (send_status) {
			cant_retrans_ok++;
		}

		while (broadcast_timeOut)
			;
	}

	return cant_retrans_ok;
}

//------------------------------------------------------------------------------
// Function:    net_GetDataIn() 
// Descr.:      Envia los datos recividos al nivel de App  
// Parametros:  *datos (destino) 
// Devuelve:    void
//------------------------------------------------------------------------------  
void net_GetDataIn(str_comm *datos) {

	char i;

	datos->dir_destino = rx.trama.dir_destino;
	datos->dir_origen = rx.trama.dir_origen;
	datos->command = rx.trama.comando;

	for (i = 0; i < LONG_DATA_FIELD; i++)
		datos->data[i] = rx.trama.data[i];
}

//------------------------------------------------------------------------------
// Function:    _net_ArmarTrama
// Descr.:      Arma la trama para luego ser enviada
// Parametros:  char dir_dest, char ID_paq, char comando, char *datos
// Devuelve:    void
//------------------------------------------------------------------------------              
void _net_ArmarTrama(uint16_t dir_dest, char ID_paq, char comando, char *datos) {

	char i;

	tx.trama.direccion = dir_dest;  //esta es la del CC2500
	tx.trama.IDpaq = ID_paq;
	tx.trama.dir_destino = dir_dest; //del protocolo (para hacer hops)
	tx.trama.dir_origen = (uint16_t) net_myAddr;
	tx.trama.comando = comando;

	for (i = 0; i < LONG_DATA_FIELD; i++)
		tx.trama.data[i] = *(datos + i);

	tx.trama.checksum = _net_Checksum(tx.Buffer, LONG_TRAMA, LONG_TRAMA - 1);
}

uint8_t _net_LastProcessedTrama(char dir_origen, char IDpaq) {
	uint8_t index;
	uint8_t i;
	for (index = 0; index < LONG_BUFFER_LAST_RX; index++) {
		if (dir_origen == net_status.rxLastReqAddr[index]) {
			if (IDpaq == net_status.rxLastReqID[index]) {
				return 0;
			}
			index++;
			break;
		}
	}
	// Si llegué acá, en index tengo
	for (i = index-1; i > 0; i--) {
		net_status.rxLastReqAddr[i] = net_status.rxLastReqAddr[i-1];
		net_status.rxLastReqID[i] = net_status.rxLastReqID[i-1];
	}
	net_status.rxLastReqAddr[0] = dir_origen;
	net_status.rxLastReqID[0] = IDpaq;
	return 1;

}

//------------------------------------------------------------------------------
//  Funcion:   char Analizar Trama()
//  Descrip.:  
//  Return:    CMD_OK, o cero en caso que no sea para mi
//------------------------------------------------------------------------------
void _net_AnalizarTrama(void) {

	if (rx.trama.checksum == _net_Checksum(rx.Buffer, LONG_TRAMA, LONG_TRAMA - 1)) { // CHKSUM OK
		if (rx.trama.dir_destino == net_myAddr || rx.trama.dir_destino == BROADCAST_ADDRESS) { // Packet for me or broadcast

			net_status.rxPackID = rx.trama.IDpaq;

			if (rx.trama.comando < RANGO_REQ) { // Controla si el comando es de Request o Response.
				//----------------------------------------------------------------------
				// REQUEST
				//----------------------------------------------------------------------
				if (_net_LastProcessedTrama(rx.trama.dir_origen, rx.trama.IDpaq)) { // Ya se proceso este Request?

					if (rx.trama.comando < (NETWORK_REQUESTS + NETWORK_BROADCASTS) && requests[rx.trama.comando] != 0) {
						requests[rx.trama.comando]();
					}

				} else {                        // Si... Reenvio el Response
					if (rx.trama.dir_destino != BROADCAST_ADDRESS)
						net_flags.TXpacket = 1;
				}
			} else {
				//----------------------------------------------------------------------
				// RESPONSES
				//----------------------------------------------------------------------
				if (net_status.txPackID == rx.trama.IDpaq) {
					net_status.iTimeOutRetrans = 0;
					net_status.cCantRetrans = 0;
					net_flags.responseOK = 1; //TODO:ver si hay que anidar el if que sigue
				}

				if (rx.trama.comando >= 100 && rx.trama.comando < 100 + NETWORK_RESPONSES
						&& responses[rx.trama.comando - 100] != 0) {
					responses[rx.trama.comando - 100]();
				}
			}
		}
	}
}
//------------------------------------------------------------------------------
// Funcion: _net_Rnd();
// Calcula un nro. pseudoaleatorio
//------------------------------------------------------------------------------
unsigned int _net_Rnd(unsigned int max) {

	randSemilla = randSemilla * 1103515245 + 12345;
	return ((char) (randSemilla >> 8) % max);
}

void net_SetSemilla(int semilla) {
	randSemilla = semilla;
}

//------------------------------------------------------------------------------
// Funcion: _net_getNewPacketID();
// Calcula un nuevo ID de Trama
//------------------------------------------------------------------------------
char _net_GetNewPacketID(void) {

	static char newID = 0;

	newID++;

	if (!newID)
		newID = 1;

	return (newID);
}

//------------------------------------------------------------------------------
// Funcion: _net_SendPacket();
//		envia la trama al CC2500 previamente armada
//------------------------------------------------------------------------------
char _net_SendPacket(void) {
	char i = CANT_TX_RETRY;
	unsigned int pck_wait;

	net_flags.TxMacError = 0;      // Reset Medium Access Control Error flag

	do {
		if (RF_SendPacket(tx.trama.direccion, &tx.Buffer[1], (char) LONG_TRAMA)) {
			net_flags.TxMacError = 0;
		} else {
			net_flags.TxMacError = 1;
			i--;
			pck_wait = _net_Rnd((CANT_TX_RETRY - i) * 2) + 1;
			bsp_msDelay(pck_wait * MS_PER_PACKET); // Wait for Medium Access.
		}
	} while (net_flags.TxMacError && i);

	if (!net_flags.TxMacError) {
		net_flags.TXerror = 0;
		return (1);
	} else {
		net_flags.TXerror = 1;
		return (0);
	}
}

//------------------------------------------------------------------------------
// Funcion: net_ProcessNetwork()
// Analiza las flags de la red y actua enviando o analizando paquetes
//------------------------------------------------------------------------------
void net_ProcessNetwork(void) {

	if (net_flags.RXpacket) {
		net_flags.RXpacket = 0;
		_net_AnalizarTrama();
	}

	if (net_flags.TXpacket) {  //TRUE: Hay un paquete listo para ser enviado
		net_flags.TXpacket = 0;
		_net_SendPacket(); //_net_SendPacket devuelve TRUE si pudo enviar (canal libre)

	}
}

/*-------------------------------------------------------------------
 NOMBRE:	Checksum
 DESCRIPCION:   Calcula la Suma de Comprobacion de un buffer
 ENTRADAS:	Puntero del buffer, la longitud del mismo y su posicion.
 SALIDAS:	Devuelve Suma de Comprobacion.
 ---------------------------------------------------------------------*/
char _net_Checksum(volatile char *ChkBuffer, char longitud, char PosChksum) {

	unsigned int sum = 0;
	unsigned int temp;

	for (temp = 0; temp < longitud; temp++) {
		if (temp != PosChksum)
			sum += *(ChkBuffer + temp);
	}

// Tomamos solo 8 bits de la suma de 16 bits y sumamos los carries
	while (sum >> 8) {
		temp = (sum >> 8) & 0x00FF;
		sum = (sum & 0x00FF) + temp;
	}
// Hacemos el complemento
	sum = ~sum;

	return (char) (sum);
}

//------------------------------------------------------------------------------
// Funcion: net_RxPacketISR
// Funcion de recepecion de un paquete de datos por Trnasceiver
//------------------------------------------------------------------------------
void net_RxPacketISR(void) {

	uint8_t len = LONG_TRAMA;     // Len of pkt to be RXed

	if (RF_ReceivePacket(&rx.trama.direccion, &rx.Buffer[1], &len, &net_nodeStatus.rx_rssiDbm, &net_nodeStatus.rx_lqi)) { // Fetch packet from CCxxxx
		net_flags.RXpacket = 1;                         // CRC Packet OK
	}
}

//------------------------------------------------------------------------------
// Funcion: net_TimerISR_1ms
// Funcion de temporizacion de los servicios de la Red
//------------------------------------------------------------------------------
void net_TimerISR_1ms(void) {

	if (net_status.iTimeOutRetrans) {
		net_status.iTimeOutRetrans--;

		if (!net_status.iTimeOutRetrans) { //entra cuando se cumple el timeout
			if (net_status.cCantRetrans) {
				net_status.iTimeOutRetrans = TIMEOUT_RETRANS;
				if (net_status.cCantRetrans > 1) {
					net_flags.TXpacket = 1;
				}
				net_status.cCantRetrans--;
			}
		}
	}

	if (broadcast_timeOut)
		broadcast_timeOut--;

}

int8_t net_getRSSI(void) {
	return net_nodeStatus.rx_rssiDbm;
}

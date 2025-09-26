//----------------------------------------------------------------------------
//  Description:  This file contains functions that allow the use the USCI_A
//  module in UART mode for MSP430 devices. 
//
//  MSP430 Pheriperal Interface Code Library v1.0
//
//  Agustin Laprovitta
//  Communication Lab. - UCC
//  March 2011
//  IAR Embedded Workbench v5.10
//----------------------------------------------------------------------------

#ifndef INCLUDE_H

#endif
#include "TI_MSP_UART.h"
#include "../system.h"

//******************************************************************************
// Variables
//******************************************************************************
uint8_t uart_Buffer[UART_BUFFER_SIZE];
uint8_t index = 0;
uint8_t overwrite = 0;
uint8_t counter = 0;
//******************************************************************************
// Funcion: Init_UART
// Configuracion Inicial del UCA 0 como UART
//******************************************************************************
void Init_UART(void) {
                        
  UART_UCAxCTL1 |= (UCSWRST);
  
  /* USCI Ax Control Register 0 */
  UART_UCAxCTL0 = 0x00;  
  /* USCI Ax Control Register 1 */
  UART_UCAxCTL1 |= (UCSSEL0 + UCSSEL1);
  
 /* USCI Ax Baud Rate  */
 /* USCI Ax Modulation Control */
#if MSP430_UART_SMCLK_FREQ == SMCLK_1MHZ             // 1MHz as BRCLK
  #if MSP430_UART_BAUD_RATE == BAUD_RATE_9600BPS        // 9600bps
    UART_UCAxBR1 = 0x00;	
    UART_UCAxBR0 = 0x68;
    UART_UCAxMCTL = 0x02;
  #endif
  #if MSP430_UART_BAUD_RATE == BAUD_RATE_19200BPS    // 19200bps
    UART_UCAxBR1 = 0x00;	
    UART_UCAxBR0 = 0x34;
    UART_UCAxMCTL = 0x00;
  #endif
  #if MSP430_UART_BAUD_RATE == BAUD_RATE_56000BPS    // 56000bps
    UART_UCAxBR1 = 0x00;	
    UART_UCAxBR0 = 0x11;
    UART_UCAxMCTL = 0x0E;
  #endif
  #if MSP430_UART_BAUD_RATE == BAUD_RATE_115200BPS    // 115200bps
    UART_UCAxBR1 = 0x00;	
    UART_UCAxBR0 = 0x08;
    UART_UCAxMCTL = 0x0C;
  #endif
#endif
#if MSP430_UART_SMCLK_FREQ == SMCLK_4MHZ             // 4MHz as BRCLK
  #if MSP430_UART_BAUD_RATE == BAUD_RATE_9600BPS      // 9600bps
    UART_UCAxBR1 = 0x01;	
    UART_UCAxBR0 = 0xA0;
    UART_UCAxMCTL = 0x0C;
  #elif MSP430_UART_BAUD_RATE == BAUD_RATE_19200BPS    // 19200bps
    UART_UCAxBR1 = 0x00;	
    UART_UCAxBR0 = 0xD0;
    UART_UCAxMCTL = 0x06;
  #elif MSP430_UART_BAUD_RATE == BAUD_RATE_56000BPS    // 56000bps
    UART_UCAxBR1 = 0x00;	
    UART_UCAxBR0 = 0x47;
    UART_UCAxMCTL = 0x08;
  #elif MSP430_UART_BAUD_RATE == BAUD_RATE_115200BPS    // 115200bps
    UART_UCAxBR1 = 0x00;	
    UART_UCAxBR0 = 0x22;
    UART_UCAxMCTL = 0x0C;
  #endif
#endif
#if MSP430_UART_SMCLK_FREQ == SMCLK_8MHZ             // 8MHz as BRCLK
  #if MSP430_UART_BAUD_RATE == BAUD_RATE_9600BPS      // 9600bps
    UART_UCAxBR1 = 0x03;	
    UART_UCAxBR0 = 0x41;
    UART_UCAxMCTL = 0x04;
  #elif MSP430_UART_BAUD_RATE == BAUD_RATE_19200BPS    // 19200bps
    UART_UCAxBR1 = 0x01;	
    UART_UCAxBR0 = 0xA0;
    UART_UCAxMCTL = 0x0C;
  #elif MSP430_UART_BAUD_RATE == BAUD_RATE_56000BPS    // 56000bps
    UART_UCAxBR1 = 0x00;	
    UART_UCAxBR0 = 0x8E;
    UART_UCAxMCTL = 0x0E;
  #elif MSP430_UART_BAUD_RATE == BAUD_RATE_115200BPS    // 115200bps
    UART_UCAxBR1 = 0x00;	
    UART_UCAxBR0 = 0x45;
    UART_UCAxMCTL = 0x08;
  #endif
#endif
#if MSP430_UART_SMCLK_FREQ == SMCLK_12MHZ             // 12MHz as BRCLK
  #if MSP430_UART_BAUD_RATE == BAUD_RATE_9600BPS      // 9600bps
    UART_UCAxBR1 = 0x04;	
    UART_UCAxBR0 = 0xE2;
    UART_UCAxMCTL = 0x00;
  #elif MSP430_UART_BAUD_RATE == BAUD_RATE_19200BPS    // 19200bps
    UART_UCAxBR1 = 0x02;	
    UART_UCAxBR0 = 0x71;
    UART_UCAxMCTL = 0x00;
  #elif MSP430_UART_BAUD_RATE == BAUD_RATE_56000BPS    // 56000bps
    UART_UCAxBR1 = 0x00;	
    UART_UCAxBR0 = 0xD6;
    UART_UCAxMCTL = 0x04;
  #elif MSP430_UART_BAUD_RATE == BAUD_RATE_115200BPS    // 115200bps
    UART_UCAxBR1 = 0x00;	
    UART_UCAxBR0 = 0x68;
    UART_UCAxMCTL = 0x02;
  #endif
#endif
#if MSP430_UART_SMCLK_FREQ == SMCLK_16MHZ             // 16MHz as BRCLK
  #if MSP430_UART_BAUD_RATE == BAUD_RATE_9600BPS      // 9600bps
    UART_UCAxBR1 = 0x06;	
    UART_UCAxBR0 = 0x82;
    UART_UCAxMCTL = 0x0C;
  #elif MSP430_UART_BAUD_RATE == BAUD_RATE_19200BPS    // 19200bps
    UART_UCAxBR1 = 0x03;	
    UART_UCAxBR0 = 0x41;
    UART_UCAxMCTL = 0x04;
  #elif MSP430_UART_BAUD_RATE == BAUD_RATE_56000BPS    // 56000bps
    UART_UCAxBR1 = 0x01;	
    UART_UCAxBR0 = 0x1D;
    UART_UCAxMCTL = 0x0C;
  #elif MSP430_UART_BAUD_RATE == BAUD_RATE_115200BPS    // 115200bps
    UART_UCAxBR1 = 0x00;	
    UART_UCAxBR0 = 0x08A;
    UART_UCAxMCTL = 0x0E;
  #endif
#endif

  UART_PORTSEL |= (UART_PIN_TX + UART_PIN_RX);
  UART_PORTDIR |= (UART_PIN_TX);
  UART_PORTDIR &= ~(UART_PIN_RX);
  
  UART_UCAxCTL1 &= ~(UCSWRST);
  
  UART_IFG &= ~UART_UCAxRXIFG;
	UART_IE |= (UART_UCAxRXIE);
}

//*****************************************************************
// Funcion: UART_Tx_char();
//                escrbe un char por el UART
//******************************************************************
void UART_Tx_char(char dato) {
  UART_UCAxTXBUF = dato;
	while (!(UART_IFG & UART_UCAxTXIFG))
		;
}        

//*****************************************************************
// Funcion: UART_Tx_string();
// Descr.:    escribe un string por el UART
// Paramatros: cant = 0 (escribe hasta encontrar un null (0))
//             cant != 0 (imprime el numero de chars asignado)
//*****************************************************************
// Puede usarse como:
//   sprintf((char *)&buffer[0], "variable: %d", var);
//   UART_Tx_string(buffer);
//*****************************************************************
void UART_Tx_string(char *dato, char cant) {
  char i;
  
	if (!cant) {                //si paso un cero imprime hasta null (un string)
		while (*dato) {                         //mientras que no tenga un null
			UART_UCAxTXBUF = *dato++; //escribo un caracter e incremento el puntero
			while (!(UART_IFG & UART_UCAxTXIFG))
				;
    }
	} else {
		for (i = 0; i <= cant; i++) {           //pasando la cantidad a imprimir
			UART_UCAxTXBUF = *(dato++); //escribo un caracter e incremento el puntero
			while (!(UART_IFG & UART_UCAxTXIFG))
				;
    }
  }
}

//*****************************************************************
// Funcion: UART_get_data
// Descr.: Guarda los datos que llegan en un buffer circular
// Parametros: El ultimo dato que llego.
//******************************************************************
void UART_recieve_data(char dato) {
	uart_Buffer[index] = dato;
	index++;

	if (counter != UART_BUFFER_SIZE)
		counter++;

	if (!(index < UART_BUFFER_SIZE)) {
		index = 0;
	}

}

//*****************************************************************
// Funcion: UART_get_data
// Descr.: Es para pasar los datos que llegaron a un buffer de la aplicacion
// Parametros: Puntero al buffer
//			   Tamano del buffer
//		Devuelve la cantidad de datos que se escribieron
//******************************************************************
char UART_get_data(char * buffer) {
	uint8_t i = 0;
	if (!counter)
		return 0;

	if ((index == counter) || ((counter - index) == (UART_BUFFER_SIZE - 1))) {
		for (i = 0; i < counter; i++) {
			buffer[i] = uart_Buffer[i];
		}
	} else {
		for (i = 0; i < counter; i++) {
			buffer[i] = uart_Buffer[(counter + index + i) % UART_BUFFER_SIZE];
		}
	}

	UART_buffer_clean();

	return counter;
}

//*****************************************************************
// Funcion: UART_data_ready
// Descr.: Devuelve la cantidad de caracteres recibidos
//******************************************************************
char UART_data_ready(void) {
	return counter;
}

//*****************************************************************
// Funcion: UART_data_peak
// Descr.: Devuelve el ultimo dato recibido
//******************************************************************
char UART_data_peak(void) {
	if (index == 0)
		return uart_Buffer[UART_BUFFER_SIZE - 1];
	else
		return uart_Buffer[index - 1];
}

//*****************************************************************
// Funcion: UART_buffer_clean
// Descr.:  limpia el buffer
//******************************************************************
void UART_buffer_clean(void) {
	uint8_t i;
	for (i = 0; i < UART_BUFFER_SIZE; i++)
		uart_Buffer[i] = 0;

	index = 0;
	counter = 0;
	overwrite = 0;

}

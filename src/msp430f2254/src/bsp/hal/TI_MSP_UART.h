//----------------------------------------------------------------------------
//  Description:  Header file for TI_MSP_UART.c
//
//  MSP430 Pheriperal Interface Code Library v1.0
//
//  Agustin Laprovitta
//  Communication Lab. - UCC
//  March 2011
//  IAR Embedded Workbench v5.10
//----------------------------------------------------------------------------

// You MUST set the value of MSP430_UART_SMCLK_FREQ and MSP430_UART_BAUD_RATE
// in you app definition section
// and call Init_UART() function in the app configuration section.


//******************************************************************************
// Definiciones Especificas
//******************************************************************************
#define UART_BUFFER_SIZE 23
//******************************************************************************

// Ensure to add UART Register Definition in you BSP App
// UART Register Definition

#define UART_UCAxCTL0       UCA0CTL0
#define UART_UCAxCTL1       UCA0CTL1
#define UART_UCAxBR1        UCA0BR1 	
#define UART_UCAxBR0        UCA0BR0 
#define UART_UCAxMCTL       UCA0MCTL
#define UART_UCAxTXBUF      UCA0TXBUF
#define UART_UCAxRXBUF      UCA0RXBUF
#define UART_IE             IE2
#define UART_IFG            IFG2

#define UART_UCAxRXIE       UCA0RXIE
#define UART_UCAxTXIE       UCA0TXIE
#define UART_UCAxRXIFG      UCA0RXIFG
#define UART_UCAxTXIFG      UCA0TXIFG

// UART Pin Port Definition
#define UART_PORTSEL  P3SEL
#define UART_PORTDIR  P3DIR
#define UART_PIN_RX   BIT5
#define UART_PIN_TX   BIT4

// UART ISR Vector
#define UART_ISR_RX_VECTOR  USCIAB0RX_VECTOR
#define UART_ISR_TX_VECTOR  USCIAB0TX_VECTOR



//#define MSP430_UART_SMCLK_FREQ    // BRCLK assigned to SMCLK
#define SMCLK_1MHZ  1               // 1MHz
#define SMCLK_4MHZ  4               // 4MHz
#define SMCLK_8MHZ  8               // 8MHz
#define SMCLK_12MHZ 12               // 12MHz
#define SMCLK_16MHZ 16               // 16MHz

//#define MSP430_UART_BAUD_RATE     // Baud Rate in [BPS]
#define BAUD_RATE_9600BPS   9600       // 9600bps  
#define BAUD_RATE_19200BPS  19200       // 19200bps
#define BAUD_RATE_56000BPS  56000       // 5600bps
#define BAUD_RATE_115200BPS 115200       // 115200bps

#define MSP430_UART_SMCLK_FREQ SMCLK_16MHZ
#define MSP430_UART_BAUD_RATE  BAUD_RATE_115200BPS



//******************************************************************************
// Funcion: Init_UART
// Configuracion Inicial del USCI A como UART
//******************************************************************************
void Init_UART(void);
//*****************************************************************
// Funcion: UART_Tx_char();
//                escrbe un char por el UART
//******************************************************************
void UART_Tx_char(char dato);     

//*****************************************************************
// Funcion: UART_Tx_string(char *dato, char cant);
// Descr.:    escribe un string por el UART
// Paramatros: cant = 0 (escribe hasta encontrar un null (0))
//             cant != 0 (imprime el numero de chars asignado)
//*****************************************************************
// Puede usarse como:
//   sprintf((char *)&buffer[0], "variable: %d", var);
//   UART_Tx_string(buffer);
//*****************************************************************
void UART_Tx_string(char *dato, char cant);

//*****************************************************************
// Funcion: UART_recieve_data
// Descr.: Guarda los datos que llegan en un buffer circular
// Parametros: El ultimo dato que llego.
//******************************************************************
void UART_recieve_data(char dato);

//*****************************************************************
// Funcion: UART_get_data
// Descr.: Es para pasar los datos que llegaron a un buffer de la aplicacion
// Parametros: Puntero al buffer
//			   Tamano del buffer
//		Devuelve la cantidad de datos que se escribieron
//******************************************************************
char UART_get_data(char * buffer);

//*****************************************************************
// Funcion: UART_data_ready
// Descr.: Devuelve la cantidad de caracteres recibidos
//******************************************************************
char UART_data_ready(void);

//*****************************************************************
// Funcion: UART_data_peak
// Descr.: Devuelve el ultimo dato recibido
//******************************************************************
char UART_data_peak(void);

//*****************************************************************
// Funcion: UART_buffer_clean
// Descr.:  limpia el buffer
//******************************************************************
void UART_buffer_clean(void);




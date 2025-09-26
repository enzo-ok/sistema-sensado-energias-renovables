/**
 * @brief BSP generico de inicializacion de hardware.
 */

#include "system.h"
#include "bsp.h"
#include "hal/hal_pin.h"
#include "hal/hal_usci_spi.h"
#include "hal/hal_cc2500.h"
#include "hal/ADC10.h"
#include "hal/TI_MSP_UART.h"
#include <math.h>
#include "hal/MspFlash.h"

#define UART 1


/**
 * @defgroup Pines Pines del BSP
 * @{
 */

const pinConfig pinLedVerde = { HAL_PORT1, 0, HAL_PIN_DIG_OUT, 0, BIT0 };

const pinConfig pinLedRojo = { HAL_PORT1, 1, HAL_PIN_DIG_OUT, 0, BIT1 };

const pinConfig pinSW1 = { HAL_PORT1, 2, HAL_PIN_DIG_IN_R, 1, BIT2 };

const pinConfig pinActMedicion = { HAL_PORT4, 2, HAL_PIN_DIG_OUT, 0, BIT2 };
const pinConfig pinADBateria = { HAL_PORT4, 3, HAL_PIN_ANA_IN, 0, BIT3 };

// Vset CONFIGURATION PIN VOLTAGE REGULATOR
const pinConfig pinVset = { HAL_PORT4, 1, HAL_PIN_DIG_OUT, 0, BIT1 };
// USCI SPI - CC2500 PINS
const pinConfig cc_gdo0 = { HAL_PORT2, 0, HAL_PIN_INT_HILO, 0, BIT0 };
const pinConfig cc_gdo2 = { HAL_PORT2, 2, HAL_PIN_DIG_IN, 0, BIT2 };
const pinConfig cc_spinCS = { HAL_PORT2, 1, HAL_PIN_DIG_OUT, 1, BIT1 };
const pinConfig cc_spiSIMO = { HAL_PORT3, 1, HAL_PIN_SEC_FUNC_OUT, 0, BIT1 };
const pinConfig cc_spiSOMI = { HAL_PORT3, 2, HAL_PIN_SEC_FUNC, 0, BIT2 };
const pinConfig cc_spiCLK = { HAL_PORT3, 3, HAL_PIN_SEC_FUNC_OUT, 0, BIT3 };
#ifdef UART
// USCI UART PINS
const pinConfig uart_TX = { HAL_PORT3, 4, HAL_PIN_SEC_FUNC_OUT, 0, BIT4 };
const pinConfig uart_RX = { HAL_PORT3, 5, HAL_PIN_SEC_FUNC, 0, BIT5 };
#endif
/// @}

// ----------------------------- EXTERN APP FUNCTIONS --------------------------------------
extern uint8_t app_TimerISR_1ms(void);
extern void net_TimerISR_1ms(void);
extern uint8_t app_TimerISR_2s(void);
extern void app_SwitchISR(void);
extern void flash_init(void);
extern void net_RxPacketISR(void);

// --------------------------------- BSP VARIABLES -----------------------------------------
uint16_t bsp_contMs;

char rf_rxBuffer[10];
char rf_rxLen;
char rf_rxRssi;
char rf_rxLqi;
char rf_rxAddr;

// CC2500
char paTable[1] = { 0xFF };
char paTableLen = 1;


extern uint16_t net_myAddr;

/**
 * @brief Inicializacion del BSP
 */
void bsp_Init(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	DCOCTL = CALDCO_16MHZ;		// MCLK = DCO = 16MHz;
	BCSCTL1 = CALBC1_16MHZ;

	BCSCTL1 &= ~XTS;			// ACLK = LFXT1 = LF xtal
	BCSCTL3 &= ~LFXT1S_3;		// range = 32.768khz
	BCSCTL3 |= XCAP_3;

//	do {
//		IFG1 &= ~OFIFG;
//		for (i = 0xFF; i > 0; i--)
//			// Espero a que el osc arranque
//			;
//	} while (IFG1 & OFIFG);
//
//	BCSCTL2 |= SELM_0; // MCLK = LFXT1 = 32.768khz

#ifdef UART
	Init_UART();
#endif
	 //Pines que no se usan se ponen, se configuran por disminuir el consumo
		P1DIR |= 0xF8;
		P1OUT &= 0x07;

		P2DIR |= 0x38;
		P2OUT &= 0xC7;

		P3DIR |= 0xC0;
		P3OUT &= 0x3F;

		P4DIR |= 0xF1;
		P4OUT &= 0x0E;

	//......................................................................
	// Inicializo los leds y pulsadores
	LedVerde = hal_pin_init(&pinLedVerde);
	LedRojo = hal_pin_init(&pinLedRojo);
	Switch1 = hal_pin_init(&pinSW1);

	hal_pin_config(&pinSW1, HAL_PIN_INT_HILO);
	hal_pin_init(&pinVset);
	hal_pin_init(&pinActMedicion);
	hal_pin_init(&pinADBateria);

	// Configuracion pines CC2500
	hal_pin_init(&cc_gdo0);
	hal_pin_init(&cc_gdo2);

	// Configuracion pines y SPI CC2500
	spi1_setup((uint8_t*) &UCB0CTL0);
	hal_pin_init(&cc_spinCS);
	hal_pin_init(&cc_spiSIMO);
	hal_pin_init(&cc_spiSOMI);
	hal_pin_init(&cc_spiCLK);
	spi1_start();

	// Inicializacion del transceiver CC2500
	hal_cc2500_powerupReset();               // Reset CCxxxx
	hal_cc2500_WriteRFSettings();             // Write RF settings to config reg
	hal_cc2500_writeBurstReg(TI_CCxxx0_PATABLE, paTable, paTableLen); //Write PATABLE
	hal_cc2500_writeReg(TI_CCxxx0_ADDR, (uint8_t)net_myAddr);          // Device address.
	hal_cc2500_strobe(TI_CCxxx0_SIDLE);  // Initialize CC2500 in MODO RECEPCION!

	// Inicializacion del timmer
	TACTL = (TASSEL_1 + ID_0 + MC_2);

	TACCTL0 = (CM_0 + CCIS_2);
	TACCR0 = 0x0000;
	TACCTL0 |= CCIE;

	TACCTL1 = (CM_0 + CCIS_2);
	TACCR1 += 33;
	TACCTL1 &= ~(CCIFG);
	TACCTL1 |= CCIE;

	__enable_interrupt();
}

int bsp_GetSemilla() {
	return TAR;
}

void bsp_msDelay(uint16_t ms) {
	bsp_contMs = ms;
	while (bsp_contMs)
		;
}

// ----------------------- Led Services ---------------------------
void led_On(const void* l) {
	hal_pin_out((const pinConfig *) l, 1);
}

void led_Off(const void* l) {
	hal_pin_out((const pinConfig *) l, 0);
}
void led_Toggle(const void* l) {
	hal_pin_toggle((const pinConfig *) l);
}



void system_sleep(uint8_t rxState,uint8_t t1msState) {
	if(!rxState){
		RF_sleep();
		//hal_pin_out(&pinVset, 1);  //Regulador con salida en 2.2v
	}
	if(!t1msState){
		TACCTL1 &= ~CCIE;
	}
	LPM3;
}

inline void system_wakeup(void) {

	//hal_pin_out(&pinVset, 0);  // regulador con la salida en 3.3v
	RF_PwrUpRx();
	TACCTL1 |= CCIE;
}

// ----------------------- Swich Services -------------------------
uint8_t sw_state(const void* s) {
	return hal_pin_in((const pinConfig *) s);
}


/*
 *********************************************************************************************************
 *                                     RF_SendPacket
 *
 * Description   : This function sends a Packet through the RF transceiver
 * Argument      : None
 * Returns       : None
 *********************************************************************************************************
 */
uint8_t RF_SendPacket(uint8_t addr, uint8_t *txBuffer, uint8_t size) {
	char cacho;
	led_On(LedRojo);
	cacho = hal_cc2500_sendPacket(addr, txBuffer, size);
	led_Off(LedRojo);
	return cacho;
}

/*
 *********************************************************************************************************
 *                                     RF_ReceivePacket
 *
 * Description   : This function receives a Packet from the RF transceiver
 * Argument      : None
 * Returns       : None
 *********************************************************************************************************
 */
uint8_t RF_ReceivePacket(uint8_t *addr, uint8_t *rxBuffer, uint8_t *length, int8_t *rssi,
		uint8_t *lqi) {
	char cacho;
		led_On(LedVerde);
		cacho = hal_cc2500_receivePacket(addr, rxBuffer, length, (uint8_t *)rssi, lqi);
		led_Off(LedVerde);
		return cacho;
}

/*
 *********************************************************************************************************
 *                                     RF_PwrUpRx
 *
 * Description   : This function sets the RF transceiver in Rx Mode
 * Argument      : None
 * Returns       : None
 *********************************************************************************************************
 */
void RF_PwrUpRx(void) {
	hal_cc2500_strobe(TI_CCxxx0_SRX);
}

void RF_sleep(void) {
	hal_cc2500_strobe(TI_CCxxx0_SIDLE);
	hal_cc2500_strobe(TI_CCxxx0_SPWD);
}

inline uint16_t timerA_GetTAR(void) {
	return TAR;
}

inline void timerA_SetTAR(uint16_t tar) {
	TAR = tar;
}

inline void timerA_StopCount(void) {
	TACTL &= ~(MC0 + MC1);
}

inline void timerA_StartCount(void) {
	TACTL |= MC_2;
}


void serie_SendString(char *dato, char cant){

	UART_Tx_string(dato,cant);

}

unsigned int TensionBateria(void) {
	return AD10_Convert(INCH_12);
}
#ifdef FLASH
//--------------------------Flash Functions to Bsp-----------------------------

void bsp_erase(unsigned char *flash) {
	erase(flash);
}

void bsp_write_data_inFlash(int data_length, unsigned char sample_buff[],
		unsigned int flash_byte_position) {

	write_data_inFlash(data_length, &sample_buff[0], flash_byte_position);

}

void bsp_read_flash(unsigned int position_segment, unsigned char *dest, int len) {

	read_flash(position_segment, dest, len);

}

void bsp_write_empty_cycle(unsigned char empty_cycle_byte,
		unsigned int flash_byte_adress) {

	write_empty_cycle(empty_cycle_byte, flash_byte_adress);

}

void bsp_write_parameter_inFlash(int data_length, unsigned char sample_buff[],
		unsigned int flash_byte_position) {

	write_parameter_inFlash(data_length, &sample_buff[0], flash_byte_position);

}

void bsp_read_parameter_inFlash(unsigned int position_segment,
		unsigned char *dest, int len) {

	read_flash_parameter(position_segment, dest, len);

}

void bsp_erase_parameter_segment() {
	erase_parameter_segment();
}


void bsp_erase_sample_segment() {
	erase_sample_segment();
}
#endif
// ----------------------- Interrupt services Routines -------------------------
#pragma vector = PORT2_VECTOR
__interrupt
void port2_ISR(void) {


	if (hal_pin_intGetFlag(&cc_gdo0)) {
		net_RxPacketISR();
		LPM3_EXIT;
		system_wakeup();
		hal_pin_intClearFlag(&cc_gdo0);
	}
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt
void Ta0_ISR(void) {

	if(app_TimerISR_2s()){
		LPM3_EXIT;
		system_wakeup();
	}
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt
void Ta1_ISR(void) {

	if (TACCTL1 & CCIFG) {
		TACCR1 = TAR + 33;
		TACCTL1 &= ~CCIFG;

		if (bsp_contMs) {
			bsp_contMs--;
		}

		net_TimerISR_1ms();
		if(app_TimerISR_1ms()){
			LPM3_EXIT;
			system_wakeup();
		}
	}

}

#pragma vector = PORT1_VECTOR
__interrupt void Port1_ISR(void) {

	if (hal_pin_intGetFlag(&pinSW1)) {
		LPM3_EXIT;
		system_wakeup();
		app_SwitchISR();
		hal_pin_intClearFlag(&pinSW1);
	}
}

#pragma vector=UART_ISR_RX_VECTOR
__interrupt void UART_ISR(void) {
	if (UART_IFG & UART_UCAxRXBUF)
		UART_recieve_data(UART_UCAxRXBUF);

}

#pragma vector= ADC10_VECTOR, TIMERB1_VECTOR, USCIAB0TX_VECTOR, WDT_VECTOR,  TIMERB0_VECTOR, NMI_VECTOR, RESET_VECTOR
__interrupt
void ISR_trap(void) {
	/* For debugging purposes, you can trap the CPU & code execution here with an
	 infinite loop */
	while (1)
		;
}

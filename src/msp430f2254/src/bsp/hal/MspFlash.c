/**
 @file MapFlash.c
 @brief Read/Write/Erase flash memory library for MSP430


 @author Tosetti Mat�as
 @date 10/03/2015

 */

#include "MspFlash.h"
#include <stdint.h>
#include "msp430f2254.h" //This library define de .h for the specific device
/* Flash Clock selection. Write and erase operation are controlled by the flash timing
 generator. The flash timming generator operating frequency (fFTG) is 333,333kHZ defined here by selecting
 the Master Clock Frecuency (actually 1M in this proyect) divided by 3.
 *The Flash clock must be between 257 and 476 kHz to operate correctly
 */

//-------------------------Flash bdp Definitions---------------------------------------

#define NUMBER_OF_WRITABLE_SEGMENTS 6

#define SAMPLE_SEGMENT_SIZE 512 // main segment size (smallest flasheable area)
#define MEMORY_SAMPLE_SIZE (NUMBER_OF_WRITABLE_SEGMENTS*SAMPLE_SEGMENT_SIZE)
#define PARAMETER_SEGMENT_SIZE 64
#define FLASH_SAMPLE_ORIGIN 0xC000
#define FLASH_PARAMETER_ORIGIN 0x01080// 0xC200 //Desde la posicion en memoria 0x01080 hasta la 0X010C0 (Segmento B) se usar� para el registro de par�metros



// ----------------------- Memory segmentation -------------------------
//Memory for Sample registration
#pragma LOCATION(memory_parameter_segment, FLASH_PARAMETER_ORIGIN);
unsigned char memory_parameter_segment[PARAMETER_SEGMENT_SIZE];


#pragma LOCATION(memory_segment, FLASH_SAMPLE_ORIGIN);
unsigned char memory_segment[MEMORY_SAMPLE_SIZE];



#define FLASHCLOCK FSSEL_1 + 0x2D//FN1
#define TRUE 1
#define FALSE 0


/*
 * ERASE INDIVIDUAL SEGMENT ONLY
 *
 * ATTENTION!: make sure pointer is in the segment you wish to erase, otherwise you may erase you program or some data
 *
 * When a flash segment is initiated from within flash memory, all timing is controlled by the flash controller,
 * the CPU is held while the erase cycle completes. After the erase cycle completes, the CPU resumes code execution
 * with the instruction following the dummy write.
 *
 * When initiating an erase cycle from within flash memory, it is possible to erase the code needed for execution
 * after the erase. If this occurs, CPU execuction is unpredictable after the erase cycle.
 *
 * For more information see the two flowchart in the user�s guide at page 313 and 314.
 */

void erase(unsigned char *flash) {
	WDTCTL = WDTPW | WDTHOLD;        // Disable WDT
#ifdef __MSP430_HAS_FLASH2__ //This indicates that the erase cycle will be initiated within the flash memory
	FCTL2 = FWKEY + FLASHCLOCK; // Key for writing register + Flash Clock selection
	FCTL3 = FWKEY; // Clear memory flash LOCK bit, N� 4 at FCTL3 register
	FCTL1 = FWKEY + ERASE; //Enable erase individual segment only
	*flash = 0; // Dummy write to initiate erase Segment
	FCTL3 = FWKEY + LOCK; // Done, set LOCK for locking flash memory
#else //This perform an erase cycle within the ram
			while (FCTL3 & BUSY) // Wait for till busy flag is 0
			{
				__no_operation(); //perform no operation. todo: understand the format of this.
			}

			FCTL3 = FWKEY; // Clear Lock bit
			FCTL1 = FWKEY+ERASE;// Set Erase bit
			*flash = 0;// Dummy write, erase Segment

			while (FCTL3 & BUSY)// Wait for till busy flag is 0
			{
				__no_operation();
			}

			FCTL3 = FWKEY+LOCK;       // Set LOCK bit
#endif
	WDTCTL = WDTPW | WDTHOLD; // Enable WDT todo: I may erase this instruction since I mustn�t enable the watchdog
}
void erase_parameter_segment() {

	erase(&memory_parameter_segment[0]);

}

void erase_sample_segment() { //todo: hacer que borre todos los segmentos asignados al registro de muestra
	unsigned int i;
	for (i = 0; i < NUMBER_OF_WRITABLE_SEGMENTS; i++) {
		erase(&memory_segment[i * SAMPLE_SEGMENT_SIZE]);
	}

}
/*READ CYCLE
 *
 * load from memory, at segment boundary
 */

void read(unsigned char *flash, unsigned char *dest, int len) {
	while (len--) //This cycle will last as much as the length of data requested
		*(dest++) = *(flash++); //storage in address pointed
}

void read_flash(unsigned int position_segment, unsigned char *dest, int len) {
	read(&memory_segment[position_segment], dest, len);
}

void read_flash_parameter(unsigned int position_segment, unsigned char *dest,
		int len) {
	read(&memory_parameter_segment[position_segment], dest, len);
}

/**
 *	/fn
 *
 * save in to flash (at segment boundary)
 */

/**
 @fn WRITE

 @brief Write Function to write as much bytes as wished in an specified location within a Flash segment define previously
 (See:muestra[AMOUNT_FLASH_SAMPLE_SLOTS])

 Configuration to initiate a write cycle:
 MSP430 datasheet shows the flow diagram to implement a write function by setting three registers FCTL1, FCTL2 and FCTL3.
 For understanding the code read the user�s guide, Flash Memory Controller Section: See:http://www.ti.com/lit/ug/slau144j/slau144j.pdf.

 @param info[] buffer with the information to be written in the Flash. Its lenght is specify previosuly by a define. See:SAMPLE_LENGTH.
 @param sample_number integer that represents the position where the information will be written within the segment.

 @returns 1 if the operation was succesful.
 0 if the number of sample is higher than the specified by a definition (see:AMOUNT_FLASH_SAMPLE_SLOTS).

 Important part of the Code:

 \verbatim
 for (i = 0; i < SAMPLE_LENGTH; i++) {

 muestra[sample_number].byte[i] = info[i];}
 \endverbatim

 */

void write_data_inFlash(int data_length, unsigned char sample_buff[],
		unsigned int flash_byte_position) {

	unsigned int i;

	WDTCTL = WDTPW | WDTHOLD;        // Disable WDT
#ifdef __MSP430_HAS_FLASH2__ //This indicates that the writing cycle will be initiated within the flash memory
	FCTL2 = FWKEY + FLASHCLOCK; // Key for writing register + Flash Clock selection
	FCTL3 = FWKEY; // Clear memory flash LOCK bit, N� 4 at FCTL3 register
	FCTL1 = FWKEY + WRT; // Enable write mode.

	for (i = 0; i < data_length; i++) { //writting process
		memory_segment[flash_byte_position] = sample_buff[i];
		flash_byte_position++;

	}

	FCTL1 = FWKEY; //Done. Clear WRT bit. Write mode is off now.
	FCTL3 = FWKEY + LOCK; // Done, set LOCK for locking flash memory

#else //This perform a writing cycle within the ram. THIS CODE IS NOT CORRECTLY WRITTEN
	FCTL3 = FWKEY;            // Clear Lock bit
	FCTL1 = FWKEY+WRT;// Enable write mode.
	while(len--)// Copy data
	*(flash++) = *(src++);
	FCTL1 = FWKEY;//Done. Clear WRT bit. Write mode is off now.
	FCTL3 = FWKEY+LOCK;// Done, set LOCK for locking flash memory
#endif
	WDTCTL = WDTPW | WDTHOLD; // Enable WDT  todo: I may erase this instruction since I mustn�t enable the watchdog
	return; //if the writte was performed correctly, return TRUE
}

void write_parameter_inFlash(int data_length, unsigned char sample_buff[],
		unsigned int flash_byte_position) {

	unsigned int i;

	WDTCTL = WDTPW | WDTHOLD;        // Disable WDT
#ifdef __MSP430_HAS_FLASH2__ //This indicates that the writing cycle will be initiated within the flash memory
	FCTL2 = FWKEY + FLASHCLOCK; // Key for writing register + Flash Clock selection
	FCTL3 = FWKEY; // Clear memory flash LOCK bit, N� 4 at FCTL3 register
	FCTL1 = FWKEY + WRT; // Enable write mode.

	for (i = 0; i < data_length; i++) { //writting process
		memory_parameter_segment[flash_byte_position] = sample_buff[i];
		flash_byte_position++;

	}

	FCTL1 = FWKEY; //Done. Clear WRT bit. Write mode is off now.
	FCTL3 = FWKEY + LOCK; // Done, set LOCK for locking flash memory

#else //This perform a writing cycle within the ram. THIS CODE IS NOT CORRECTLY WRITTEN
	FCTL3 = FWKEY;            // Clear Lock bit
	FCTL1 = FWKEY+WRT;// Enable write mode.
	while(len--)// Copy data
	*(flash++) = *(src++);
	FCTL1 = FWKEY;//Done. Clear WRT bit. Write mode is off now.
	FCTL3 = FWKEY+LOCK;// Done, set LOCK for locking flash memory
#endif
	WDTCTL = WDTPW | WDTHOLD; // Enable WDT  todo: I may erase this instruction since I mustn�t enable the watchdog
	return; //if the writte was performed correctly, return TRUE
}

void write_empty_cycle(unsigned char empty_cycle_byte,
		unsigned int flash_byte_adress) {

	WDTCTL = WDTPW | WDTHOLD;        // Disable WDT
#ifdef __MSP430_HAS_FLASH2__ //This indicates that the writing cycle will be initiated within the flash memory
	FCTL2 = FWKEY + FLASHCLOCK; // Key for writing register + Flash Clock selection
	FCTL3 = FWKEY; // Clear memory flash LOCK bit, N� 4 at FCTL3 register
	FCTL1 = FWKEY + WRT; // Enable write mode.

//writting process
	memory_segment[flash_byte_adress] = empty_cycle_byte;

	FCTL1 = FWKEY; //Done. Clear WRT bit. Write mode is off now.
	FCTL3 = FWKEY + LOCK; // Done, set LOCK for locking flash memory

#else //This perform a writing cycle within the ram. THIS CODE IS NOT CORRECTLY WRITTEN
	FCTL3 = FWKEY;            // Clear Lock bit
	FCTL1 = FWKEY+WRT;// Enable write mode.
	while(len--)// Copy data
	*(flash++) = *(src++);
	FCTL1 = FWKEY;//Done. Clear WRT bit. Write mode is off now.
	FCTL3 = FWKEY+LOCK;// Done, set LOCK for locking flash memory
#endif
	WDTCTL = WDTPW | WDTHOLD; // Enable WDT  todo: I may erase this instruction since I mustn�t enable the watchdog
	return; //if the writte was performed correctly, return TRUE
}

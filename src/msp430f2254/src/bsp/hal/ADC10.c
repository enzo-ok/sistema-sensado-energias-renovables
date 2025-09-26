//----------------------------------------------------------------------------
//  Description:  This file contains functions that allow the use the ADC10
//  module in MSP430f22xx devices. 
//
//  MSP430 Pheriperal Interface Code Library v1.0
//
//  Agustin Laprovitta
//  Communication Lab. - UCC
//  March 2011
//  IAR Embedded Workbench v5.10
//----------------------------------------------------------------------------
#ifndef INCLUDE_H
#include "ADC10.h"
#include "../system.h"

#endif

//******************************************************************************
// Funcion: Init_ADC10
// Configuracion Inicial del Modulo ADC10.
//******************************************************************************
void Init_ADC10(ADC_Init val) {

	switch (val.Referencia) {
	case ADC10_REF_VCC:
		ADC10CTL0 |= (SREF_0 + ADC10SHT_2 + ADC10SR + ADC10ON);
		break;
	case ADC10_REF_INT_2_5V:
		ADC10CTL0 |= (SREF_1 + ADC10SHT_2 + ADC10SR + REFBURST + REF2_5V + REFON
				+ ADC10ON);
		break;
	case ADC10_REF_INT_1_5V:
		ADC10CTL0 |=
				(SREF_1 + ADC10SHT_2 + ADC10SR + REFBURST + REFON + ADC10ON);
		break;
	case ADC10_REF_BUFF_EXT:
		ADC10CTL0 |= (SREF_3 + ADC10SHT_2 + ADC10SR + REFBURST + ADC10ON);
		break;
	}

	/*
	 #if (MSP430x2_ADC10_REF == ADC10_REF_VCC)
	 ADC10CTL0 |= (SREF_0 + ADC10SHT_2 + ADC10SR + ADC10ON);
	 #endif
	 #if (MSP430x2_ADC10_REF == ADC10_REF_INT_2_5V)
	 ADC10CTL0 |= (SREF_1 + ADC10SHT_2 + ADC10SR + REFBURST + REF2_5V + REFON + ADC10ON);
	 #endif
	 #if (MSP430x2_ADC10_REF == ADC10_REF_INT_1_5V)
	 ADC10CTL0 |= (SREF_1 + ADC10SHT_2 + ADC10SR + REFBURST + REFON + ADC10ON);
	 #endif
	 #if (MSP430x2_ADC10_REF == ADC10_REF_BUFF_EXT)
	 ADC10CTL0 |= (SREF_3 + ADC10SHT_2 + ADC10SR + REFBURST + ADC10ON);
	 #endif
	 */

	ADC10CTL1 |= (CONSEQ_0 + ADC10SSEL_0 + ADC10DIV_4 + SHS_0);

	// Disable Data Transfer Control
	ADC10DTC1 = 0;
}

//******************************************************************************
// Funcion: AD10_Convert
// Dispara la conversion AD del canal indicado y espera mientras realiza la misma
//    Entrada: CH = Constante del canal de entrada (INCH_0, INCH_5, etc)
//    Salida: conversion AD (10 bit menos significativos)
//******************************************************************************
unsigned int AD10_Convert(unsigned int CH) {

	unsigned int sample;

	ADC10CTL1 &= 0x0FFF;
	ADC10CTL1 |= (CH);                // Select ADC Channel
	ADC10CTL0 |= (ENC + ADC10SC);     // Start Conversion

	while (ADC10CTL1 & ADC10BUSY)
		;     // Wait while the conversion is in progress

	sample = ADC10MEM;
	ADC10CTL0 &= (~ENC);
	return (sample);
}

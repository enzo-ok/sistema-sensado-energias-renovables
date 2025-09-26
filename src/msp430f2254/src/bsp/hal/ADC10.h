
// You MUST set the value of MSP430x2_ADC10_REF in you app definition section
// and call Init_ADC10() function in the app configuration section.
// Ensure to configure ADC10AE0 and ADC10AE1 Register in you app
// Example:
//#define MSP430x2_ADC10_REF ADC10_REF_INT_2_5V
//#define MSP430x2_ADC10_REF ADC10_REF_INT_1_5V     // ADC10 Reference Definition

// #define MSP430x2_ADC10_REF
/*
 #define ADC10_REF_VCC       1   // Vr+ = Vcc | Vr- = Vss
 #define ADC10_REF_INT_2_5V  2   // Vr+ = Internar Reference 2.5V | Vr- = Vss
 #define ADC10_REF_INT_1_5V  3   // Vr+ = Internar Reference 1.5V  | Vr- = Vss
 #define ADC10_REF_BUFF_EXT  4   // Vr+ = Buffered External Reference Veref+  | Vr- = Vss
 */

typedef enum {
	ADC10_REF_VCC = 0x01,   // Vr+ = Vcc | Vr- = Vss
	ADC10_REF_INT_2_5V = 0x02,   // Vr+ = Internar Reference 2.5V | Vr- = Vss
	ADC10_REF_INT_1_5V = 0x03,   // Vr+ = Internar Reference 1.5V  | Vr- = Vss
	ADC10_REF_BUFF_EXT = 0x04 // Vr+ = Buffered External Reference Veref+  | Vr- = Vss

} ADC_Vref;

typedef struct {

	ADC_Vref Referencia;

} ADC_Init;
/*

 typedef enum{
 ADC10_REF_VCC		= 0x00,   // Vr+ = Vcc | Vr- = Vss
 ADC10_REF_INT		= 0x01,   // Vr+ = Internar Reference| Vr- = Vss
 ADC10_REF_EXT		= 0x02,   // Vr+ =External Reference Veref+ VeREF+  | Vr- = Vss
 ADC10_REF_BUFF_EXT  = 0x03,  // Vr+ = Buffered External Reference Veref+  | Vr- = Vss
 ADC10_REF_NEG_EXT  	= 0x04, 	//Vr+ = VCC | Vr− = VREF−/ VeREF−
 //ADC10_REF_EXT		= 0x05,//Vr+ = VREF+ | Vr− = VREF−/ VeREF−
 //ADC10_REF_EXT		=0x06,,//Vr+ = VeREF+ | Vr− = VREF−/ VeREF−
 //ADC10_REF_EXT		=0x07//Vr+ = Buffered VeREF+ | Vr− = VREF−/ VeREF−
 }ADC_Vref;



 typedef enum{
 ADC10_REF_OFF	= 0x0, //Reference buffer supports up to ~200 ksps
 ADC10_REF_ON	= 0x1 //Reference buffer supports up to ~50 ksps
 }ADC_ReferenceOutput;

 typedef enum{//REFON must also be set
 ADC10_REF_1_5V	= 0x0, //Internal Reference 1.5V
 ADC10_REF_2_5V	= 0x1 //Internal Reference 2.5V
 }ADC_ReferenceVoltage;

 typedef enum{//REFON must also be set
 ADC10_REF_GEN_OFF	= 0x0, //Reference generator off
 ADC10_REF_GEN_ON	= 0x1 //Reference generator on
 }ADC_Referencegenerator;

 typedef enum{//REFON must also be set
 ADC10_OFF	= 0x0, //ADC off
 ADC10_ON	= 0x1 //ADC on
 }ADC_On_Off;

 typedef enum{//REFON must also be set
 ADC10_INTERRUPT_OFF	= 0x0, //Interrupt disabled
 ADC10_INTERRUPT_ON	= 0x1 //interrupt enabled
 }ADC_Interrupt;

 typedef enum{
 ADC10_DISABLE_MULTIPLE = 0x0,
 ADC10_ENABLE_MULTIPLE  = 0x1
 }ADC_MultipleSampleConversion;


 typedef enum{
 ADC10_REF_CONTINUOUSLY	= 0x0, //Reference buffer on continuously
 ADC10_REF_ONLYSAMPLE	= 0x1 //Reference buffer on only during sample-and-conversion
 }ADC_ReferenceBurst;

 typedef enum{
 ADC10_4X	= 0x00, //4 x ADC10CLKs
 ADC10_8X	= 0x01, //8 x ADC10CLKs
 ADC10_16X	= 0x02, //16 x ADC10CLKs
 ADC10_64X	= 0x03//64 x ADC10CLKs
 }ADC_SampleHold;

 typedef enum{
 ADC10_200	= 0x0, //Reference buffer supports up to ~200 ksps
 ADC10_50	= 0x1, //Reference buffer supports up to ~50 ksps
 }ADC_SamplingRate;

 typedef enum{
 ADC10_NO_INTERRUPT	= 0x0, //No interrupt pending
 ADC10_YES_INTERRUPT	= 0x1, //Interrupt pending
 }ADC_Interrupt_Flag;

 typedef enum{
 ADC10_DISABLE_CONVERSION	= 0x0, //ADC10 disabled
 ADC10_ENABLE_CONVERSION		= 0x1, //ADC10 enabled
 }ADC_Enable_Conversion;

 typedef enum{
 ADC10_DISABLE	= 0x0, //No sample-and-conversion start
 ADC10_ENABLE	= 0x1, //Start sample-and-conversion
 }ADC_Start_Conversion;


 typedef struct S_ADC10CTL0{

 ADC_Vref Referencia;
 ADC_SampleHold SampleHold;
 ADC_SamplingRate SamplingRate;
 ADC_ReferenceOutput ReferenceOutput;
 ADC_ReferenceBurst ReferenceBurst;
 ADC_MultipleSampleConversion MultipleSampleConversion;
 ADC_ReferenceVoltage ReferenceVoltage;
 ADC_Referencegenerator Referencegenerator;
 ADC_On_Off On_Off;
 ADC_Interrupt Interrupt;
 ADC_Interrupt_Flag InterruptFlag;
 ADC_Enable_Conversion EnableConversion;
 ADC_Start_Conversion StartConversion;
 }ADC_S_ADC10CTL0;

 union ADC_U_ADC10CTL0{
 struct 		S_ADC10CTL0 campos;
 uint16_t	MASK_ADC10CTL0[16];

 };


 */

//******************************************************************************
// Funcion: Init_ADC10
// Configuracion Inicial del Modulo ADC10.
//******************************************************************************
void Init_ADC10(ADC_Init val);

//******************************************************************************
// Funcion: AD10_Convert
// Dispara la conversion AD del canal indicado y espera mientras realiza la misma
//    Entrada: CH = Constante del canal de entrada (INCH_0, INCH_5, etc)
//    Salida: conversion AD (10 bit menos significativos)
//******************************************************************************
unsigned int AD10_Convert(unsigned int CH);

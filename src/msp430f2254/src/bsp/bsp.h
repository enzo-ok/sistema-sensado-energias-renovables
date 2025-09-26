#ifndef BSP_H_
#define BSP_H_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>


extern const void* LedVerde;
extern const void* LedRojo;
extern const void* Switch1;


// ----------------------- BSP Services ---------------------------
void bsp_Init(void);
void bsp_msDelay(uint16_t ms);
int bsp_GetSemilla();

// ----------------------- Led Services ---------------------------
void led_On(const void* l);
void led_Off(const void* l);
void led_Toggle(const void* l);

uint8_t sw_state(const void* s);
inline uint16_t timerA_GetTAR(void);
inline void timerA_SetTAR(uint16_t tar);
inline void timerA_StopCount(void);
inline void timerA_StartCount(void);

uint8_t RF_ReceivePacket(uint8_t *addr, uint8_t *rxBuffer, uint8_t *length, int8_t *rssi, uint8_t *lqi);
uint8_t RF_SendPacket(uint8_t addr, uint8_t *txBuffer, uint8_t size);
void RF_PwrUpRx(void);
void RF_sleep(void);
uint8_t RF_rxDataReady(void);
void RF_getRxPacket(char *buffer, uint8_t size);


void serie_SendString(char *dato, char cant);


unsigned int TensionBateria(void);
/*
 * Rx_ON=1 sleep con radio ON
 * Rx_ON=0 sleep con radio OFF
 *
 */
void system_sleep(uint8_t rxState,uint8_t t1msState);
inline void system_wakeup();

//-------------------------Flash bsp Function declarations-----------------------------
void bsp_read_flash(unsigned int position_segment, unsigned char *dest,	int len);
void bsp_write_data_inFlash(int data_length, unsigned char sample_buff[],unsigned int flash_byte_position);
void bsp_erase(unsigned char *flash);

void bsp_write_empty_cycle(unsigned char empty_cycle_byte,unsigned int flash_byte_adress);

void bsp_write_parameter_inFlash(int data_length, unsigned char sample_buff[],unsigned int flash_byte_position);

void bsp_read_parameter_inFlash(unsigned int position_segment,unsigned char *dest, int len);

void bsp_erase_parameter_segment(void);
void bsp_erase_sample_segment(void);

//-------------------------Flash bdp Definitions--------------------------------------
#define NUMBER_OF_WRITABLE_SEGMENTS 7
#define SAMPLE_SEGMENT_SIZE 512 // main segment size (smallest flasheable area)
#define MEMORY_SAMPLE_SIZE (NUMBER_OF_WRITABLE_SEGMENTS*SAMPLE_SEGMENT_SIZE)
#define PARAMETER_SEGMENT_SIZE 64

#endif


#ifndef INC_I2C_EMULATOR_H_
#define INC_I2C_EMULATOR_H_


#include "main.h"
#include "dwt_delay.h"


#define I2C_FAST_MODE 1.25 //nanoseconds - 1.25 microseconds 400 KHz - Fast mode
#define I2C_NORMAL_MODE 5 //nanoseconds - 5 us  100 KHz -Normal mode
#define I2C_CP3_MODE 1000

void I2C_CLEAR_SDA(void);
void I2C_SET_SDA(void);
void I2C_CLEAR_SCL(void);
void I2C_SET_SCL(void);
void I2C_DELAY(uint32_t delay);


void I2C_Init(int mode);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write_bit(uint8_t b);
uint8_t I2C_Read_bit(void);
uint8_t I2C_Read_SDA(void);
_Bool I2C_Write_byte(uint8_t b, _Bool start, _Bool stop);
uint8_t I2C_Read_byte(_Bool ack, _Bool stop);
_Bool I2C_Send_byte(uint8_t address, uint8_t data);
uint8_t I2C_Receive_byte(uint8_t address);
_Bool I2C_Send_byte_data(uint8_t address, uint8_t reg, uint8_t data);
uint8_t I2C_Receive_byte_data(uint8_t address, uint8_t reg);
_Bool I2C_Transmit(uint8_t address, uint8_t data[], uint8_t size);
_Bool I2C_Receive(uint8_t address, uint8_t reg, uint8_t *data, uint8_t size);
_Bool Tx(uint8_t dat);
uint8_t Rx(_Bool ack);
void dly();



#endif /* INC_I2C_EMULATOR_H_ */

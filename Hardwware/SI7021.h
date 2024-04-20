/*
 * @Description: SI7021 头文件
 * @Author: Haolin Wang
 * @Date: 2023-03-29 16:24:36
 * @LastEditTime: 2023-06-03 11:04:27
 * @Note: 包括 si7021 内部各寄存器地址
 */

#ifndef SI7021_H_
#define SI7021_H_

#include "gpio.h"
/*设备地址 7位地址加一位读写*/
#define SI7021_ADDRESS_READ (0x40 << 1) | 0x01
#define SI7021_ADDRESS_WRITE (0x40 << 1)

/*SI7021传感器操作指令集*/
#define SI7021_MEASURE_HOLD 0xE5		// 湿度测量（主机模式）
#define SI7021_MEASURE_NOHOLD 0xF5		// 湿度测量（非主机模式）
#define SI7021_MEASURE_TEMP_HOLD 0xE3	// 温度测量（主机模式）
#define SI7021_MEASURE_TEMP_NOHOLD 0xF3 // 温度测量（非主机模式）
#define SI7021_READ_PREV_TEMP 0xE0		// 从先前的 RH 测量中读取温度值
#define SI7021_RESET 0xFE				// 重置
#define SI7021_WRITE_USER_REG1 0xE6		// 写 RH/T 用户寄存器 1
#define SI7021_READ_USER_REG1 0xE7		// 读 RH/T 用户寄存器 1
#define SI7021_WRITE_HEATER_REG 0x51	// 写加热器控制寄存器
#define SI7021_READ_HEATER_REG 0x11		// 读取加热器控制寄存器
#define SI7021_READ_ID1 0xFA0F			// 读取电子 ID 第一个字节
#define SI7021_READ_ID2 0xFCC9			// 读取电子 ID 第二个字节
#define SI7021_FIRMWARE_VERSION 0x84B8	// 阅读固件版本

/*传感器分辨率参数设置*/
#define SI7021_RESOLUTION_RH12_TEMP14 0			   // 湿度：12 bits, 温度：14 bits
#define SI7021_RESOLUTION_RH8_TEMP12 1			   // 湿度： 8 bits, 温度：12 bits
#define SI7021_RESOLUTION_RH10_TEMP13 (1 << 7)	   // 湿度：10 bits, 温度：13 bits
#define SI7021_RESOLUTION_RH11_TEMP11 (1 << 7 | 1) // 湿度：11 bits, 温度：11 bits

/*加热器相关命令（配合另一个温度传感器使用作温度补偿）*/
#define SI7021_MEASURE_FAILED 0xFFFF // 出现测量错误
#define SI7021_HEATER_ON (0x1 << 2)	 // 内置加热器开
#define SI7021_HEATER_OFF 0			 // 内置加热器关
#define SI7021_HEATER_POWER_3MA 0	 // 输出功率3mA
#define SI7021_HEATER_POWER_9MA 0x1
#define SI7021_HEATER_POWER_15MA 0x2
#define SI7021_HEATER_POWER_27MA 0x4
#define SI7021_HEATER_POWER_51MA 0x8
#define SI7021_HEATER_POWER_94MA 0xf

/* 环境温湿度部分 API */
void SI7021_iic_start(void);										// 发送IIC开始信号
void SI7021_iic_stop(void);											// 发送IIC停止信号
void SI7021_iic_ack(void);										// IIC发送ACK信号
void SI7021_iic_nack(void);										// IIC不发送ACK信号
uint8_t SI7021_iic_wait_ack(void);								// IIC等待ACK信号

void SI7021_send_byte(uint8_t data);								 // 发送一个字节
uint8_t SI7021_read_byte(uint8_t ack);								 // 读取一个字节
void SI7021_write_command(uint8_t REG_address);					 // 向SI7021发送指令
void SI7021_read_data(uint8_t REG_address, uint16_t *value); // 读取SI7021数据
int32_t SI7021_read_previous_temperature(void);			// 温度读取（测量湿度会自动检测一次温度）
uint32_t SI7021_measure_humidity(void);						// 湿度测量（温度测量将同时进行）
void SI7021_init(void);							 // 传感器初始化

/* 体表温度部分 API */
void si7021_iic_start(void);								     // 发送IIC开始信号
void si7021_iic_stop(void);										 // 发送IIC停止信号
void si7021_iic_ack(void);									     // IIC发送ACK信号
void si7021_iic_nack(void);										 // IIC不发送ACK信号
uint8_t si7021_iic_wait_ack(void);								 // IIC等待ACK信号

void si7021_send_byte(uint8_t data);						 // 发送一个字节
uint8_t si7021_read_byte(uint8_t ack);						 // 读取一个字节
void si7021_write_command(uint8_t REG_address);					 // 向si7021发送指令
void si7021_read_data(uint8_t REG_address, uint16_t *value); // 读取si7021数据
int32_t si7021_measure_temperature(void);						 // 温度测量
void si7021_init(void);							 // 传感器初始化

#endif /* SI7021_H_ */

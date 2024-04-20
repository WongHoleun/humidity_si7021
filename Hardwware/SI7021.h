/*
 * @Description: SI7021 ͷ�ļ�
 * @Author: Haolin Wang
 * @Date: 2023-03-29 16:24:36
 * @LastEditTime: 2023-06-03 11:04:27
 * @Note: ���� si7021 �ڲ����Ĵ�����ַ
 */

#ifndef SI7021_H_
#define SI7021_H_

#include "gpio.h"
/*�豸��ַ 7λ��ַ��һλ��д*/
#define SI7021_ADDRESS_READ (0x40 << 1) | 0x01
#define SI7021_ADDRESS_WRITE (0x40 << 1)

/*SI7021����������ָ�*/
#define SI7021_MEASURE_HOLD 0xE5		// ʪ�Ȳ���������ģʽ��
#define SI7021_MEASURE_NOHOLD 0xF5		// ʪ�Ȳ�����������ģʽ��
#define SI7021_MEASURE_TEMP_HOLD 0xE3	// �¶Ȳ���������ģʽ��
#define SI7021_MEASURE_TEMP_NOHOLD 0xF3 // �¶Ȳ�����������ģʽ��
#define SI7021_READ_PREV_TEMP 0xE0		// ����ǰ�� RH �����ж�ȡ�¶�ֵ
#define SI7021_RESET 0xFE				// ����
#define SI7021_WRITE_USER_REG1 0xE6		// д RH/T �û��Ĵ��� 1
#define SI7021_READ_USER_REG1 0xE7		// �� RH/T �û��Ĵ��� 1
#define SI7021_WRITE_HEATER_REG 0x51	// д���������ƼĴ���
#define SI7021_READ_HEATER_REG 0x11		// ��ȡ���������ƼĴ���
#define SI7021_READ_ID1 0xFA0F			// ��ȡ���� ID ��һ���ֽ�
#define SI7021_READ_ID2 0xFCC9			// ��ȡ���� ID �ڶ����ֽ�
#define SI7021_FIRMWARE_VERSION 0x84B8	// �Ķ��̼��汾

/*�������ֱ��ʲ�������*/
#define SI7021_RESOLUTION_RH12_TEMP14 0			   // ʪ�ȣ�12 bits, �¶ȣ�14 bits
#define SI7021_RESOLUTION_RH8_TEMP12 1			   // ʪ�ȣ� 8 bits, �¶ȣ�12 bits
#define SI7021_RESOLUTION_RH10_TEMP13 (1 << 7)	   // ʪ�ȣ�10 bits, �¶ȣ�13 bits
#define SI7021_RESOLUTION_RH11_TEMP11 (1 << 7 | 1) // ʪ�ȣ�11 bits, �¶ȣ�11 bits

/*�����������������һ���¶ȴ�����ʹ�����¶Ȳ�����*/
#define SI7021_MEASURE_FAILED 0xFFFF // ���ֲ�������
#define SI7021_HEATER_ON (0x1 << 2)	 // ���ü�������
#define SI7021_HEATER_OFF 0			 // ���ü�������
#define SI7021_HEATER_POWER_3MA 0	 // �������3mA
#define SI7021_HEATER_POWER_9MA 0x1
#define SI7021_HEATER_POWER_15MA 0x2
#define SI7021_HEATER_POWER_27MA 0x4
#define SI7021_HEATER_POWER_51MA 0x8
#define SI7021_HEATER_POWER_94MA 0xf

/* ������ʪ�Ȳ��� API */
void SI7021_iic_start(void);										// ����IIC��ʼ�ź�
void SI7021_iic_stop(void);											// ����IICֹͣ�ź�
void SI7021_iic_ack(void);										// IIC����ACK�ź�
void SI7021_iic_nack(void);										// IIC������ACK�ź�
uint8_t SI7021_iic_wait_ack(void);								// IIC�ȴ�ACK�ź�

void SI7021_send_byte(uint8_t data);								 // ����һ���ֽ�
uint8_t SI7021_read_byte(uint8_t ack);								 // ��ȡһ���ֽ�
void SI7021_write_command(uint8_t REG_address);					 // ��SI7021����ָ��
void SI7021_read_data(uint8_t REG_address, uint16_t *value); // ��ȡSI7021����
int32_t SI7021_read_previous_temperature(void);			// �¶ȶ�ȡ������ʪ�Ȼ��Զ����һ���¶ȣ�
uint32_t SI7021_measure_humidity(void);						// ʪ�Ȳ������¶Ȳ�����ͬʱ���У�
void SI7021_init(void);							 // ��������ʼ��

/* ����¶Ȳ��� API */
void si7021_iic_start(void);								     // ����IIC��ʼ�ź�
void si7021_iic_stop(void);										 // ����IICֹͣ�ź�
void si7021_iic_ack(void);									     // IIC����ACK�ź�
void si7021_iic_nack(void);										 // IIC������ACK�ź�
uint8_t si7021_iic_wait_ack(void);								 // IIC�ȴ�ACK�ź�

void si7021_send_byte(uint8_t data);						 // ����һ���ֽ�
uint8_t si7021_read_byte(uint8_t ack);						 // ��ȡһ���ֽ�
void si7021_write_command(uint8_t REG_address);					 // ��si7021����ָ��
void si7021_read_data(uint8_t REG_address, uint16_t *value); // ��ȡsi7021����
int32_t si7021_measure_temperature(void);						 // �¶Ȳ���
void si7021_init(void);							 // ��������ʼ��

#endif /* SI7021_H_ */

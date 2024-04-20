/**
****************************************************************************************
* @Description: �¶ȴ�����si7021����ģ��
* 					����ģ��IIC��Ӳ��IIC�����汾
* @Author: Haolin Wang
* @Date: 2023-03-29 16:24:36
* @LastEditTime: 2023-04-06 18:16:36
* @Note: 1.��ͨ����ʽ������ʪ��ֵʱ����Ϊ216 - 1��65535��175/65535 = 0.002670328���������������
* ��������float�ľ���Ϊ6~7λ��Ч���֣�ֱ�Ӽ���Ļ��൱�ڰ�С�����7λ֮�������ʡ�Ե��ˣ�
* ����ɼ����������ʪ�Ⱦ�����Щ��ʧ���������������ķ��������ù�ʽ����ֵ��*100��
* �����Ļ������������ʪ��ֵҲ�� *100�����������С��λ�ͻ����λ�����Ƚ�Ϊ׼ȷ��
* �ڸ������������ǰ�� *0.01��������������ֵ����
* 2. ��λ�������ûᷢ������ �޷��ش�����
****************************************************************************************
*/

#include "stdio.h"
#include "tim.h"
#include "gpio.h"
#include "SI7021.h"

/* ģ��IIC IO���� */
#define si7021_iic_scl(x)                                                                                                                                           \
	do                                                                                                                                                       \
	{                                                                                                                                                        \
		x ? HAL_GPIO_WritePin(si7021_SCL_GPIO_Port, si7021_SCL_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(si7021_SCL_GPIO_Port, si7021_SCL_Pin, GPIO_PIN_RESET); \
	} while (0) // SCL

#define si7021_iic_sda(x)                                                                                                                                           \
	do                                                                                                                                                       \
	{                                                                                                                                                        \
		x ? HAL_GPIO_WritePin(si7021_SDA_GPIO_Port, si7021_SDA_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(si7021_SDA_GPIO_Port, si7021_SDA_Pin, GPIO_PIN_RESET); \
	} while (0) // SDA

#define si7021_READ_SDA HAL_GPIO_ReadPin(si7021_SDA_GPIO_Port, si7021_SDA_Pin) // ��ȡSDA

#define DLY_TIM_Handle_si7021 (&htim11) // ��ʱ��ѡ��

/**
****************************************************************************************
* @Funticon name: ΢���ʱ
* @Berif:
* @Note:
* @param {uint16_t} nus	��ʱʱ��
* @return {*}
****************************************************************************************
*/
static void delay_us(uint16_t nus)
{
	__HAL_TIM_SET_COUNTER(DLY_TIM_Handle_si7021, 0);
	__HAL_TIM_ENABLE(DLY_TIM_Handle_si7021);
	while (__HAL_TIM_GET_COUNTER(DLY_TIM_Handle_si7021) < nus)
	{
	}
	__HAL_TIM_DISABLE(DLY_TIM_Handle_si7021);
}

/**
****************************************************************************************
* @Funticon name: ����IIC��ʼ�ź�
* @Berif:
* @Note:
* @return {*}
****************************************************************************************
*/
void si7021_iic_start(void)
{
	si7021_iic_sda(1);
	si7021_iic_scl(1);
	delay_us(10); // si7021 ����4.7us
	si7021_iic_sda(0);	  // START�ź�: ��SCLΪ��ʱ, SDA�Ӹ߱�ɵ�, ��ʾ��ʼ�ź�
	delay_us(10); // si7021 ����4us
	si7021_iic_scl(0);	  // ǯסI2C���ߣ�׼�����ͻ��������
}

/**
****************************************************************************************
* @Funticon name: ����IICֹͣ�ź�
* @Berif:
* @Note:
* @return {*}
****************************************************************************************
*/
void si7021_iic_stop(void)
{
	si7021_iic_sda(0); // STOP�ź�: ��SCLΪ��ʱ, SDA�ӵͱ�ɸ�, ��ʾֹͣ�ź�
	delay_us(10);
	si7021_iic_scl(1);
	delay_us(10);
	si7021_iic_sda(1); // ����I2C���߽����ź�
	delay_us(10);
}

/**
****************************************************************************************
* @Funticon name: �ȴ�Ӧ���źŵ���
* @Berif:
* @Note:
* @return {*}	1������Ӧ��ʧ�ܣ�
* 				0������Ӧ��ɹ�
****************************************************************************************
*/
uint8_t si7021_iic_wait_ack(void)
{
	uint8_t waittime = 0;
	uint8_t rack = 0;

	si7021_iic_sda(1); // �����ͷ�SDA��(��ʱ�ⲿ������������SDA��)
	delay_us(10);
	si7021_iic_scl(1); // SCL=1, ��ʱ�ӻ����Է���ACK
	delay_us(10);

	while (si7021_READ_SDA) // �ȴ�Ӧ��
	{
		waittime++;

		if (waittime > 250)
		{
			si7021_iic_stop();
			rack = 1;
			break;
		}
	}

	si7021_iic_scl(0); // SCL=0, ����ACK���
	delay_us(10);
	return rack;
}

/**
****************************************************************************************
* @Funticon name: ����ACKӦ��
* @Berif:
* @Note:
* @return {*}
****************************************************************************************
*/
void si7021_iic_ack(void)
{
	si7021_iic_sda(0); // SCL 0 -> 1  ʱ SDA = 0,��ʾӦ��
	delay_us(10);
	si7021_iic_scl(1); // ����һ��ʱ��
	delay_us(10);
	si7021_iic_scl(0);
	delay_us(10);
	si7021_iic_sda(1); // �����ͷ�SDA��
	delay_us(10);
}

/**
****************************************************************************************
* @Funticon name: ������ACKӦ��
* @Berif:
* @Note:
* @return {*}
****************************************************************************************
*/
void si7021_iic_nack(void)
{
	si7021_iic_sda(1); // SCL 0 -> 1  ʱ SDA = 1,��ʾ��Ӧ��
	delay_us(10);
	si7021_iic_scl(1); // ����һ��ʱ��
	delay_us(10);
	si7021_iic_scl(0);
	delay_us(10);
}

/**
****************************************************************************************
* @Funticon name: IIC����һ���ֽ�
* @Berif:
* @Note:
* @param {uint8_t} data	Ҫ���͵�����
* @return {*}
****************************************************************************************
*/
void si7021_send_byte(uint8_t data)
{
	uint8_t t;

	for (t = 0; t < 8; t++)
	{
		si7021_iic_sda((data & 0x80) >> 7); // ��λ�ȷ���
		delay_us(10);
		si7021_iic_scl(1);
		delay_us(10);
		si7021_iic_scl(0);
		data <<= 1; // ����1λ,������һ�η���
	}
	si7021_iic_sda(1); // �������, �����ͷ�SDA��
}

/**
****************************************************************************************
* @Funticon name: IIC��ȡһ���ֽ�
* @Berif: ack=1ʱ������ack; ack=0ʱ������nack
* @Note:
* @param {uint8_t} ack	�Ƿ�Ӧ��
* @return {*}
****************************************************************************************
*/
uint8_t si7021_read_byte(uint8_t ack)
{
	uint8_t i, receive = 0;

	for (i = 0; i < 8; i++) // ����1���ֽ�����
	{
		receive <<= 1; // ��λ�����,�������յ�������λҪ����
		si7021_iic_scl(1);
		delay_us(10);

		if (si7021_READ_SDA)
		{
			receive++;
		}

		si7021_iic_scl(0);
		delay_us(10);
	}

	if (!ack)
	{
		si7021_iic_nack(); // ����nACK
	}
	else
	{
		si7021_iic_ack(); // ����ACK
	}

	return receive;
}

/**
****************************************************************************************
* @Funticon name: ���ֽ�д�봫����
* @Berif: ���ڷ������ݵ�si7021
* @Note:
* @param {uint8_t} REG_address	�Ĵ�����ַ
* @return {*}
****************************************************************************************
*/
void si7021_write_command(uint8_t REG_address)
{
	si7021_iic_start();

	si7021_send_byte(SI7021_ADDRESS_WRITE);
	si7021_iic_wait_ack();

	si7021_send_byte(REG_address);
	si7021_iic_wait_ack();

	si7021_iic_stop();
}

/**
****************************************************************************************
* @Funticon name: ���ֽڶ�ȡ
* @Berif: ���ڶ�ȡsi7021����������
* @Note: 2���ֽ�����
* @param {uint8_t} REG_address	�Ĵ�����ַ
* @param {uint16_t} *value	����ֵ
* @return {*}
****************************************************************************************
*/
void si7021_read_data(uint8_t REG_address, uint16_t *value)
{
	uint8_t si7021_BUF[2] = {0}; // �������鶨��

	si7021_iic_start(); // IIC��ʼ�ź�

	si7021_send_byte(SI7021_ADDRESS_WRITE); // дָ��
	si7021_iic_wait_ack();						 // �ȴ�Ӧ��

	si7021_send_byte(REG_address);
	si7021_iic_wait_ack(); // �ȴ�Ӧ��

	HAL_Delay(19); // �ȴ���ʪ��ת����ɣ�����Ӧ�ô���18ms

	si7021_iic_start();
	si7021_send_byte(SI7021_ADDRESS_READ); // ��ָ��
	si7021_iic_wait_ack();
	si7021_BUF[0] = si7021_read_byte(1); // �������� ��8λ ���ݲ�����Ӧ���ź�
	si7021_BUF[1] = si7021_read_byte(0); // �������� ��8λ ���ݲ����ͷ�Ӧ���ź�

	si7021_iic_stop(); // ֹͣ�ź�

	*value = ((si7021_BUF[0] << 8) + si7021_BUF[1]); // ���ߵ�8λ���ݺϳ�Ϊ16λ����
}

/**
****************************************************************************************
* @Funticon name: �¶Ȳ���
* @Berif: ��ȡ��ǰͨ�����á�si7021_measure_humidity��������ɵ��¶Ȳ���
* @Note: ��ȡʪ�Ȳ��������¶ȣ�0xE0�� ʱ������ִ�в��������������ʪ�Ȳ����ڼ�������¶�ֵ��
* @param {I2C_HandleTypeDef} *hi2c
* @return {*}
****************************************************************************************
*/
int32_t si7021_measure_temperature(void)
{
	uint16_t temp;

	si7021_read_data(SI7021_MEASURE_TEMP_NOHOLD, &temp); // ��ȡ��ǰ�¶�
	return (temp * 17572 / 65536 - 4685);				// ʪ�ȼ��㹫ʽ  �� %RH Ϊ��λ���������ʪ��ֵ ������100��												   // �¶�ת����ʽ �ѽ�����һ�ٱ�
}

uint32_t SI7021_measure_humidity(void)
{
	uint16_t humi;

	si7021_read_data(SI7021_MEASURE_NOHOLD, &humi); // NOHOLD_MASTERģʽ�¶�ȡʪ��
	return (humi * 12500 / 65536 - 600);				// ʪ�ȼ��㹫ʽ  �� %RH Ϊ��λ���������ʪ��ֵ ������100��
}

/**
****************************************************************************************
* @Funticon name: si7021��ʼ��
* @Berif: ���þ�����������ʼ��������si7021
* @Note: ��ʹ�ò���ϵͳ����Ҫ���������ں�����ǰ�����Χ��·��ʼ��
* @param {I2C_HandleTypeDef} *hi2c
* @return {*}
****************************************************************************************
*/
void si7021_init(void)
{
	si7021_iic_sda(1); // ��ʼ��ƽ��Ϊ��
	si7021_iic_scl(1);
	delay_us(10);

	si7021_write_command(SI7021_RESET);
	delay_us(5);
}

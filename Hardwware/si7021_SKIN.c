/**
****************************************************************************************
* @Description: 温度传感器si7021驱动模块
* 					包含模拟IIC和硬件IIC两个版本
* @Author: Haolin Wang
* @Date: 2023-03-29 16:24:36
* @LastEditTime: 2023-04-06 18:16:36
* @Note: 1.在通过公式计算温湿度值时，因为216 - 1是65535，175/65535 = 0.002670328…，结果除不尽，
* 但浮点型float的精度为6~7位有效数字，直接计算的话相当于把小数点后7位之后的数据省略掉了，
* 会造成计算出来的温湿度精度有些损失，所以有了这样的方法：先让公式的数值都*100，
* 这样的话计算出来的温湿度值也会 *100，这样计算的小数位就会多两位，精度较为准确，
* 在赋给最后结果变量前再 *0.01，将结果变回正常值即可
* 2. 复位函数调用会发生错误 无法回传数据
****************************************************************************************
*/

#include "stdio.h"
#include "tim.h"
#include "gpio.h"
#include "SI7021.h"

/* 模拟IIC IO操作 */
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

#define si7021_READ_SDA HAL_GPIO_ReadPin(si7021_SDA_GPIO_Port, si7021_SDA_Pin) // 读取SDA

#define DLY_TIM_Handle_si7021 (&htim11) // 定时器选择

/**
****************************************************************************************
* @Funticon name: 微妙级延时
* @Berif:
* @Note:
* @param {uint16_t} nus	延时时间
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
* @Funticon name: 产生IIC起始信号
* @Berif:
* @Note:
* @return {*}
****************************************************************************************
*/
void si7021_iic_start(void)
{
	si7021_iic_sda(1);
	si7021_iic_scl(1);
	delay_us(10); // si7021 至少4.7us
	si7021_iic_sda(0);	  // START信号: 当SCL为高时, SDA从高变成低, 表示起始信号
	delay_us(10); // si7021 至少4us
	si7021_iic_scl(0);	  // 钳住I2C总线，准备发送或接收数据
}

/**
****************************************************************************************
* @Funticon name: 产生IIC停止信号
* @Berif:
* @Note:
* @return {*}
****************************************************************************************
*/
void si7021_iic_stop(void)
{
	si7021_iic_sda(0); // STOP信号: 当SCL为高时, SDA从低变成高, 表示停止信号
	delay_us(10);
	si7021_iic_scl(1);
	delay_us(10);
	si7021_iic_sda(1); // 发送I2C总线结束信号
	delay_us(10);
}

/**
****************************************************************************************
* @Funticon name: 等待应答信号到来
* @Berif:
* @Note:
* @return {*}	1，接收应答失败；
* 				0，接收应答成功
****************************************************************************************
*/
uint8_t si7021_iic_wait_ack(void)
{
	uint8_t waittime = 0;
	uint8_t rack = 0;

	si7021_iic_sda(1); // 主机释放SDA线(此时外部器件可以拉低SDA线)
	delay_us(10);
	si7021_iic_scl(1); // SCL=1, 此时从机可以返回ACK
	delay_us(10);

	while (si7021_READ_SDA) // 等待应答
	{
		waittime++;

		if (waittime > 250)
		{
			si7021_iic_stop();
			rack = 1;
			break;
		}
	}

	si7021_iic_scl(0); // SCL=0, 结束ACK检查
	delay_us(10);
	return rack;
}

/**
****************************************************************************************
* @Funticon name: 产生ACK应答
* @Berif:
* @Note:
* @return {*}
****************************************************************************************
*/
void si7021_iic_ack(void)
{
	si7021_iic_sda(0); // SCL 0 -> 1  时 SDA = 0,表示应答
	delay_us(10);
	si7021_iic_scl(1); // 产生一个时钟
	delay_us(10);
	si7021_iic_scl(0);
	delay_us(10);
	si7021_iic_sda(1); // 主机释放SDA线
	delay_us(10);
}

/**
****************************************************************************************
* @Funticon name: 不产生ACK应答
* @Berif:
* @Note:
* @return {*}
****************************************************************************************
*/
void si7021_iic_nack(void)
{
	si7021_iic_sda(1); // SCL 0 -> 1  时 SDA = 1,表示不应答
	delay_us(10);
	si7021_iic_scl(1); // 产生一个时钟
	delay_us(10);
	si7021_iic_scl(0);
	delay_us(10);
}

/**
****************************************************************************************
* @Funticon name: IIC发送一个字节
* @Berif:
* @Note:
* @param {uint8_t} data	要发送的数据
* @return {*}
****************************************************************************************
*/
void si7021_send_byte(uint8_t data)
{
	uint8_t t;

	for (t = 0; t < 8; t++)
	{
		si7021_iic_sda((data & 0x80) >> 7); // 高位先发送
		delay_us(10);
		si7021_iic_scl(1);
		delay_us(10);
		si7021_iic_scl(0);
		data <<= 1; // 左移1位,用于下一次发送
	}
	si7021_iic_sda(1); // 发送完成, 主机释放SDA线
}

/**
****************************************************************************************
* @Funticon name: IIC读取一个字节
* @Berif: ack=1时，发送ack; ack=0时，发送nack
* @Note:
* @param {uint8_t} ack	是否应答
* @return {*}
****************************************************************************************
*/
uint8_t si7021_read_byte(uint8_t ack)
{
	uint8_t i, receive = 0;

	for (i = 0; i < 8; i++) // 接收1个字节数据
	{
		receive <<= 1; // 高位先输出,所以先收到的数据位要左移
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
		si7021_iic_nack(); // 发送nACK
	}
	else
	{
		si7021_iic_ack(); // 发送ACK
	}

	return receive;
}

/**
****************************************************************************************
* @Funticon name: 单字节写入传感器
* @Berif: 用于发送数据到si7021
* @Note:
* @param {uint8_t} REG_address	寄存器地址
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
* @Funticon name: 多字节读取
* @Berif: 用于读取si7021传感器数据
* @Note: 2个字节数据
* @param {uint8_t} REG_address	寄存器地址
* @param {uint16_t} *value	数据值
* @return {*}
****************************************************************************************
*/
void si7021_read_data(uint8_t REG_address, uint16_t *value)
{
	uint8_t si7021_BUF[2] = {0}; // 缓存数组定义

	si7021_iic_start(); // IIC起始信号

	si7021_send_byte(SI7021_ADDRESS_WRITE); // 写指令
	si7021_iic_wait_ack();						 // 等待应答

	si7021_send_byte(REG_address);
	si7021_iic_wait_ack(); // 等待应答

	HAL_Delay(19); // 等待温湿度转换完成，至少应该大于18ms

	si7021_iic_start();
	si7021_send_byte(SI7021_ADDRESS_READ); // 读指令
	si7021_iic_wait_ack();
	si7021_BUF[0] = si7021_read_byte(1); // 读传感器 高8位 数据并发送应答信号
	si7021_BUF[1] = si7021_read_byte(0); // 读传感器 低8位 数据并发送非应答信号

	si7021_iic_stop(); // 停止信号

	*value = ((si7021_BUF[0] << 8) + si7021_BUF[1]); // 将高低8位数据合成为16位数据
}

/**
****************************************************************************************
* @Funticon name: 温度测量
* @Berif: 读取先前通过调用“si7021_measure_humidity（）”完成的温度测量
* @Note: 获取湿度测量附加温度（0xE0） 时将不会执行测量但返回在相对湿度测量期间测量的温度值。
* @param {I2C_HandleTypeDef} *hi2c
* @return {*}
****************************************************************************************
*/
int32_t si7021_measure_temperature(void)
{
	uint16_t temp;

	si7021_read_data(SI7021_MEASURE_TEMP_NOHOLD, &temp); // 读取先前温度
	return (temp * 17572 / 65536 - 4685);				// 湿度计算公式  以 %RH 为单位测量的相对湿度值 已扩大100倍												   // 温度转换公式 已近扩大一百倍
}

uint32_t SI7021_measure_humidity(void)
{
	uint16_t humi;

	si7021_read_data(SI7021_MEASURE_NOHOLD, &humi); // NOHOLD_MASTER模式下读取湿度
	return (humi * 12500 / 65536 - 600);				// 湿度计算公式  以 %RH 为单位测量的相对湿度值 已扩大100倍
}

/**
****************************************************************************************
* @Funticon name: si7021初始化
* @Berif: 设置句柄，并软件初始化传感器si7021
* @Note: 若使用操作系统，需要在主函数内核启动前完成外围电路初始化
* @param {I2C_HandleTypeDef} *hi2c
* @return {*}
****************************************************************************************
*/
void si7021_init(void)
{
	si7021_iic_sda(1); // 初始电平设为高
	si7021_iic_scl(1);
	delay_us(10);

	si7021_write_command(SI7021_RESET);
	delay_us(5);
}

/*
 * OLED.h
 *
 *  Created on: 2018年7月11日
 *      Author: Sunset
 *
 *  @ Tutorial
 *      @ Abstract
 *      本程序为MSP430F5529编写，使用GPIO模拟IIC协议控制12864OLED屏幕
 *      可能适用于其他MSP430系列单片机，但未经测试不能保证兼容性。
 *
 *      @ Coordinate
 *      屏幕坐标系以左上角为原点，横向为x轴，纵向为y轴，纵向每8排为一个Page
 *      #########################
 *      原点
 *      。------------->
 *      |           x轴（0~128）
 *      |
 *      |
 *      \/  y轴（0~64）
 *
 *      #########################
 *
 *      @ Usage
 *          @ PreConfig
 *          1.根据需要，在本头文件中更改SDA、SCL的管脚定义，及OLED屏幕的IIC地址
 *          2.在头文件中设置字符及数字的显示尺寸（默认为16px大字体）
 *          3.在主程序中配置SDA、SCL管脚为输出模式
 *          @
 *          1.初始化SSD1306配置：OLEDInit();
 *          2.可使用的函数:
 *          -------功能类-------
 *          RefreshDisplay();
 *          ClearDisplay();
 *          ------波形显示------
 *          ShowPoint();
 *          ShowPoint16();
 *          ShowWave16FS();
 *          ShowRTWave16();
 *          ShowRTWave16FS();
 *          ---字符、数字显示----
 *          ShowChar();
 *          ShowNum();
 *          ShowString();
 *          -----图片显示-------
 *          ShowPicture();
 *          函数功能介绍及用法见各函数的注释
 *
 *
 */

#ifndef OLED_H_
#define OLED_H_

#include "OLEDFont.h"

#define uint8 unsigned char
#define uint16 unsigned short
#define uint32 unsigned int

#define      LARGE_SIZE              16
#define      SMALL_SIZE              8

// 定义SDA和SCL的管脚
#define     SDA_P                   P3OUT
#define     SDA_PIN                 BIT0
#define     SCL_P                   P3OUT
#define     SCL_PIN                 BIT1
// 定义OLED屏IIC总线地址
#define     SLAVE_ADDRESS           0x78

// 设置显示字符、数字的大小（1Page或2Page高）
#define		 FONTSIZE				 LARGE_SIZE

#define		 Set_Bit(val, bitn)		 (val |= (/*1 <<*/(bitn)))
#define		 Clr_Bit(val, bitn)		 (val &= ~(/*1<<*/(bitn)))
#define		 SDA_Set()				 (Set_Bit(SDA_P, SDA_PIN))
#define		 SCL_Set()				 (Set_Bit(SCL_P, SCL_PIN))
#define		 SDA_Clr()				 (Clr_Bit(SDA_P, SDA_PIN))
#define		 SCL_Clr()				 (Clr_Bit(SCL_P, SCL_PIN))

#define     CPU_F ((double)1000000)
#define     Delay_us(x) __delay_cycles((long)(24*CPU_F*(double)x/1000000.0))
#define     Delay_ms(x) __delay_cycles((long)(24*CPU_F*(double)x/1000.0))


void IIC_Start()
{
    // 发送IIC起始信号
	SCL_Set();
	SDA_Set();
	__no_operation();
	SDA_Clr();
	__no_operation();
	SCL_Clr();
}

void IIC_Stop()
{
    // 发送IIC结束信号
	SDA_Clr();
	__no_operation();
	SCL_Set();
	__no_operation();
	SDA_Set();
}

void Write_IIC_Byte(uint8 IIC_Byte)
{
    // 向IIC发送1Byte数据
	uint8 i;
	//uint8 Ack_Bit;                    // 应答信号
	for(i = 8; i > 0; i--)
	{
		if(IIC_Byte & 0x80)		        //1?0?
			SDA_Set();
		else
			SDA_Clr();
		SCL_Set();
		__no_operation();
		SCL_Clr();
		IIC_Byte<<=1;			        //loop
	}
	SDA_Set();		                    // 释放IIC SDA总线为主器件接收从器件产生应答信号
	__no_operation();
	SCL_Set();                          // 第9个时钟周期
	__no_operation();
	//Ack_Bit = SDA;		            // 读取应答信号
	SCL_Clr();
}

void Write_IIC_Command(uint8 IIC_Command)
{
    // 发送命令
    IIC_Start();
    Write_IIC_Byte(SLAVE_ADDRESS);               //Slave address,SA0=0
    Write_IIC_Byte(0x00);			    //write command
    Write_IIC_Byte(IIC_Command);
    IIC_Stop();
}

void Write_IIC_Data(uint8 IIC_Data)
{
    // 发送数据
    IIC_Start();
    Write_IIC_Byte(SLAVE_ADDRESS);
    Write_IIC_Byte(0x40);			//write data
    Write_IIC_Byte(IIC_Data);
    IIC_Stop();
}

void OLEDInit()
{
    // 初始化屏幕配置
	Write_IIC_Command(0xAE);   //display off
	Write_IIC_Command(0x20);	//Set Memory Addressing Mode
	Write_IIC_Command(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	Write_IIC_Command(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	Write_IIC_Command(0xc8);	//Set COM Output Scan Direction
	Write_IIC_Command(0x00);    //---set low column address
	Write_IIC_Command(0x10);    //---set high column address
	Write_IIC_Command(0x40);    //--set start line address
	Write_IIC_Command(0x81);    //--set contrast control register
	Write_IIC_Command(0x7f);
	Write_IIC_Command(0xa1);    //--set segment re-map 0 to 127
	Write_IIC_Command(0xa6);    //--set normal display
	Write_IIC_Command(0xa8);    //--set multiplex ratio(1 to 64)
	Write_IIC_Command(0x3F);    //
	Write_IIC_Command(0xa4);    //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	Write_IIC_Command(0xd3);    //-set display offset
	Write_IIC_Command(0x00);    //-not offset
	Write_IIC_Command(0xd5);    //--set display clock divide ratio/oscillator frequency
	Write_IIC_Command(0xf0);    //--set divide ratio
	Write_IIC_Command(0xd9);    //--set pre-charge period
	Write_IIC_Command(0x22);    //
	Write_IIC_Command(0xda);    //--set com pins hardware configuration
	Write_IIC_Command(0x12);
	Write_IIC_Command(0xdb);    //--set vcomh
	Write_IIC_Command(0x20);    //0x20,0.77xVcc
	Write_IIC_Command(0x8d);    //--set DC-DC enable
	Write_IIC_Command(0x14);    //
	Write_IIC_Command(0xaf);    //--turn on oled panel
}


void SetPosition(uint8 x, uint8 page)
{
    /*
     * @ Parameters
     * uint8 x:     起始x坐标
     * uint8 page:  page(y坐标)
     * @ Abstract
     * 写显存前发送位置命令（Page Addressing Mode）
     */
    Write_IIC_Command(0xB0 + page);
    Write_IIC_Command(((x & 0xF0) >> 4) | 0x10);
    Write_IIC_Command((x & 0x0F) | 0x01);
}

void SetPositionPoint(uint8 x, uint8 page)
{
    /*
     * @ Parameters
     * uint8 x:     起始x坐标
     * uint8 page:  page(y坐标)
     * @ Abstract
     * 写显存前发送位置命令，用于画点（Page Addressing Mode）
     */
    Write_IIC_Command(0xB0 + page);
    x %= 128;
    uint8 lower = x%16;
    uint8 higher = x/16;
    Write_IIC_Command(0x00 + lower);
    Write_IIC_Command(0x10 + higher);
}

void RefreshDisplay(uint8 data)
{
    /*
     * @ Parameters
     * uint8 data:  待填充的值（0或1）
     * @ Abstract
     * 使整个屏幕全亮或全灭
     */
    uint8 m,n;
    for(m = 8; m > 0; m--)
    {
        SetPosition(0, m);
        for(n = 128; n > 0; n--)
            {
                Write_IIC_Data(data);
            }
    }
}

void ShowPicture(uint8 PicBuffer[])
{
    /*
     * @ Parameters
     * uint8 PicBuffer[]:   图片缓存区
     * @ Abstract
     * 显示一幅图片
     */
  uint8 x,y;
  uint32 i = 0;
  for(y = 0; y < 8; y++)
    {
      SetPosition(0, y);
      for(x = 0; x < 128; x++)
        {
    	  Write_IIC_Data(PicBuffer[i++]);
        }
    }
}

void ShowPoint(uint8 x, uint8 y)
{
	/*
	 * @ Parameters
	 * uint8 x:     x坐标
	 * uint8 y:     y坐标
	 * @ Abstract
	 * 在指定位置显示点（用于绘制波形），会将当前page中的整列重新绘制
	 */
	SetPositionPoint(x, y);
	uint8 Yoffset = y%8;
	Write_IIC_Data(FPointFilled[Yoffset]);

}

void ShowPoint16(uint8 x, uint8 page, uint8 wave)
{
	/*
	 * @ Parameters
	 * uint8 x:     x坐标
	 * uint8 page:  page（y坐标）
	 * uint8 wave:  波形数值（1~16）
	 * @ Abstract
	 * 在x，page位置绘制高度为16px（2Page）的波形，会将当前page中的整列重新绘制
	 * @ Remarks
	 * 可将选择两种字库，FPoint16和FPoint16Filled(波形点底部填充像素)
	 *
	 */
	SetPositionPoint(x, page);
	Write_IIC_Data(FPoint16Filled[wave*2]);
	SetPositionPoint(x, page+1);
	Write_IIC_Data(FPoint16Filled[wave*2+1]);
}

void ShowWave16FS(uint8 * xPos, uint8 Page, uint8 * WaveIter, uint8 Wave[], uint8 WaveLength)
{
	/*
	 * @ Parameters
	 * uint8 * xPos：       x坐标的指针
	 * uint 8 Page：        波形所在的Page
	 * uint8 * WaveIter：   波形数组的迭代指针
	 * uint8 Wave[]：       波形数组
	 * uint8 WaveLength：   波形长度
	 * @ Abstract
	 * 在指定Page处循环显示高度为16px（2Page）的已存储的波形
	 */
	uint8 i;

	for(i = 5; i > 1; i--)
		ShowPoint16(* xPos+i, Page, 0);

	if(*WaveIter == WaveLength)
		*WaveIter = 0;
	ShowPoint16((* xPos)++, Page, Wave[(*WaveIter)++]);
}

void ShowRTWave16FS(uint8 *xPos, uint8 Page, uint8 Wave)
{
    /*
     * @ Parameters
     * uint8 * xPos：       x坐标的指针
     * uint 8 Page：        波形所在的Page
     * uint8 Wave：         波形
     * @ Abstract
     * 在指定Page处循环显示高度为16px（2Page）的实时波形
     */
	uint8 i;

	for(i = 5; i > 1; i--)
		ShowPoint16(* xPos+i, Page, 17);
	ShowPoint16((* xPos)++, Page, Wave);
}

void ShowRTWave16(uint8 *xPos, uint8 startPos, uint8 Page, uint8 Wave)
{
    /*
     * @ Parameters
     * uint8 * xPos：       x坐标的指针
     * uint8 startPos:      x坐标起始位置
     * uint 8 Page：        波形所在的Page
     * uint8 Wave：         波形
     * @ Abstract
     * 在指定Page，指定x坐标起始位置处循环显示高度为16px（2Page）的实时波形
     */
	uint8 i;
	if(*xPos == 124)
		*xPos = startPos;

	for(i = 5; i > 1; i--)
		ShowPoint16(* xPos+i, Page, 17);
	ShowPoint16((* xPos)++, Page, Wave);
}

void ShowChar(uint8 x, uint8 page, uint8 chr)
{
	/*
	 * @ Parameters
	 * uint8 x:     x坐标
	 * uint8 page:  page（y坐标）
	 * uint8 chr:   字符
	 * @ Abstract
	 * 在指定位置显示一个字符
	 */
	uint8 i;
	uint8 shift = chr - ' ';
	if(x > 127)
	{
		x = 0;
		page += 2;
	}
	if(FONTSIZE == 16)
	{
		// 大字体模式
		SetPosition(x, page);
		for(i = 0; i < 8; i++)
			Write_IIC_Data(F8X16[shift*16 + i]);
		SetPosition(x, page+=1);
		for(i = 0; i < 8; i++)
			Write_IIC_Data(F8X16[shift*16 + i + 8]);
	}
	else
	{
		// 小字体模式
		SetPosition(x, page+1);
		for(i = 0; i < 6; i++)
			Write_IIC_Data(F6X8[shift][i]);
	}
}

void ShowNum(uint8 x, uint8 y, uint16 num, uint16 len)
{
    /*
     * @ Parameters
     * uint8 x:     x坐标
     * uint8 page:  page（y坐标）
     * uint16 num:  数字
     * uint16 len:  数字位数（以10位表示法计）
     * @ Abstract
     * 在指定位置显示一串十位数字
     */
    uint8 temp;
	while(len > 0)
	{
	    temp = num % 10;        // num最低位
	    ShowChar(x + 8*(len-1), y, temp+'0');
	    num /= 10;
	    len--;
	}
}

void ClearDisplay()
{
	// 清屏幕
	int i,n;
	for(i = 0; i < 8; i++)
	{
		SetPosition(0, i);
		for(n = 0; n < 128; n++)
			Write_IIC_Data(0);
	}
}

void ShowString(uint8 x, uint8 page, uint8 *str)
{
    /*
     * @ Parameters
     * uint8 x:     x坐标
     * uint8 page:  page（y坐标）
     * @ Abstract
     * 在指定位置显示字符串
     */
	uint8 i = 0;
	while(str[i] != '\0')
	{
		ShowChar(x, page, str[i]);
		x += 8;
		if(x > 120)
		{
			x = 0;
			page += 2;
		}
		i++;
	}
}


#endif /* OLED_H_ */

/*
 * OLED.h
 *
 *  Created on: 2018��7��11��
 *      Author: Sunset
 *
 *  @ Tutorial
 *      @ Abstract
 *      ������ΪMSP430F5529��д��ʹ��GPIOģ��IICЭ�����12864OLED��Ļ
 *      ��������������MSP430ϵ�е�Ƭ������δ�����Բ��ܱ�֤�����ԡ�
 *
 *      @ Coordinate
 *      ��Ļ����ϵ�����Ͻ�Ϊԭ�㣬����Ϊx�ᣬ����Ϊy�ᣬ����ÿ8��Ϊһ��Page
 *      #########################
 *      ԭ��
 *      ��------------->
 *      |           x�ᣨ0~128��
 *      |
 *      |
 *      \/  y�ᣨ0~64��
 *
 *      #########################
 *
 *      @ Usage
 *          @ PreConfig
 *          1.������Ҫ���ڱ�ͷ�ļ��и���SDA��SCL�ĹܽŶ��壬��OLED��Ļ��IIC��ַ
 *          2.��ͷ�ļ��������ַ������ֵ���ʾ�ߴ磨Ĭ��Ϊ16px�����壩
 *          3.��������������SDA��SCL�ܽ�Ϊ���ģʽ
 *          @
 *          1.��ʼ��SSD1306���ã�OLEDInit();
 *          2.��ʹ�õĺ���:
 *          -------������-------
 *          RefreshDisplay();
 *          ClearDisplay();
 *          ------������ʾ------
 *          ShowPoint();
 *          ShowPoint16();
 *          ShowWave16FS();
 *          ShowRTWave16();
 *          ShowRTWave16FS();
 *          ---�ַ���������ʾ----
 *          ShowChar();
 *          ShowNum();
 *          ShowString();
 *          -----ͼƬ��ʾ-------
 *          ShowPicture();
 *          �������ܽ��ܼ��÷�����������ע��
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

// ����SDA��SCL�Ĺܽ�
#define     SDA_P                   P3OUT
#define     SDA_PIN                 BIT0
#define     SCL_P                   P3OUT
#define     SCL_PIN                 BIT1
// ����OLED��IIC���ߵ�ַ
#define     SLAVE_ADDRESS           0x78

// ������ʾ�ַ������ֵĴ�С��1Page��2Page�ߣ�
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
    // ����IIC��ʼ�ź�
	SCL_Set();
	SDA_Set();
	__no_operation();
	SDA_Clr();
	__no_operation();
	SCL_Clr();
}

void IIC_Stop()
{
    // ����IIC�����ź�
	SDA_Clr();
	__no_operation();
	SCL_Set();
	__no_operation();
	SDA_Set();
}

void Write_IIC_Byte(uint8 IIC_Byte)
{
    // ��IIC����1Byte����
	uint8 i;
	//uint8 Ack_Bit;                    // Ӧ���ź�
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
	SDA_Set();		                    // �ͷ�IIC SDA����Ϊ���������մ���������Ӧ���ź�
	__no_operation();
	SCL_Set();                          // ��9��ʱ������
	__no_operation();
	//Ack_Bit = SDA;		            // ��ȡӦ���ź�
	SCL_Clr();
}

void Write_IIC_Command(uint8 IIC_Command)
{
    // ��������
    IIC_Start();
    Write_IIC_Byte(SLAVE_ADDRESS);               //Slave address,SA0=0
    Write_IIC_Byte(0x00);			    //write command
    Write_IIC_Byte(IIC_Command);
    IIC_Stop();
}

void Write_IIC_Data(uint8 IIC_Data)
{
    // ��������
    IIC_Start();
    Write_IIC_Byte(SLAVE_ADDRESS);
    Write_IIC_Byte(0x40);			//write data
    Write_IIC_Byte(IIC_Data);
    IIC_Stop();
}

void OLEDInit()
{
    // ��ʼ����Ļ����
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
     * uint8 x:     ��ʼx����
     * uint8 page:  page(y����)
     * @ Abstract
     * д�Դ�ǰ����λ�����Page Addressing Mode��
     */
    Write_IIC_Command(0xB0 + page);
    Write_IIC_Command(((x & 0xF0) >> 4) | 0x10);
    Write_IIC_Command((x & 0x0F) | 0x01);
}

void SetPositionPoint(uint8 x, uint8 page)
{
    /*
     * @ Parameters
     * uint8 x:     ��ʼx����
     * uint8 page:  page(y����)
     * @ Abstract
     * д�Դ�ǰ����λ��������ڻ��㣨Page Addressing Mode��
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
     * uint8 data:  ������ֵ��0��1��
     * @ Abstract
     * ʹ������Ļȫ����ȫ��
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
     * uint8 PicBuffer[]:   ͼƬ������
     * @ Abstract
     * ��ʾһ��ͼƬ
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
	 * uint8 x:     x����
	 * uint8 y:     y����
	 * @ Abstract
	 * ��ָ��λ����ʾ�㣨���ڻ��Ʋ��Σ����Ὣ��ǰpage�е��������»���
	 */
	SetPositionPoint(x, y);
	uint8 Yoffset = y%8;
	Write_IIC_Data(FPointFilled[Yoffset]);

}

void ShowPoint16(uint8 x, uint8 page, uint8 wave)
{
	/*
	 * @ Parameters
	 * uint8 x:     x����
	 * uint8 page:  page��y���꣩
	 * uint8 wave:  ������ֵ��1~16��
	 * @ Abstract
	 * ��x��pageλ�û��Ƹ߶�Ϊ16px��2Page���Ĳ��Σ��Ὣ��ǰpage�е��������»���
	 * @ Remarks
	 * �ɽ�ѡ�������ֿ⣬FPoint16��FPoint16Filled(���ε�ײ��������)
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
	 * uint8 * xPos��       x�����ָ��
	 * uint 8 Page��        �������ڵ�Page
	 * uint8 * WaveIter��   ��������ĵ���ָ��
	 * uint8 Wave[]��       ��������
	 * uint8 WaveLength��   ���γ���
	 * @ Abstract
	 * ��ָ��Page��ѭ����ʾ�߶�Ϊ16px��2Page�����Ѵ洢�Ĳ���
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
     * uint8 * xPos��       x�����ָ��
     * uint 8 Page��        �������ڵ�Page
     * uint8 Wave��         ����
     * @ Abstract
     * ��ָ��Page��ѭ����ʾ�߶�Ϊ16px��2Page����ʵʱ����
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
     * uint8 * xPos��       x�����ָ��
     * uint8 startPos:      x������ʼλ��
     * uint 8 Page��        �������ڵ�Page
     * uint8 Wave��         ����
     * @ Abstract
     * ��ָ��Page��ָ��x������ʼλ�ô�ѭ����ʾ�߶�Ϊ16px��2Page����ʵʱ����
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
	 * uint8 x:     x����
	 * uint8 page:  page��y���꣩
	 * uint8 chr:   �ַ�
	 * @ Abstract
	 * ��ָ��λ����ʾһ���ַ�
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
		// ������ģʽ
		SetPosition(x, page);
		for(i = 0; i < 8; i++)
			Write_IIC_Data(F8X16[shift*16 + i]);
		SetPosition(x, page+=1);
		for(i = 0; i < 8; i++)
			Write_IIC_Data(F8X16[shift*16 + i + 8]);
	}
	else
	{
		// С����ģʽ
		SetPosition(x, page+1);
		for(i = 0; i < 6; i++)
			Write_IIC_Data(F6X8[shift][i]);
	}
}

void ShowNum(uint8 x, uint8 y, uint16 num, uint16 len)
{
    /*
     * @ Parameters
     * uint8 x:     x����
     * uint8 page:  page��y���꣩
     * uint16 num:  ����
     * uint16 len:  ����λ������10λ��ʾ���ƣ�
     * @ Abstract
     * ��ָ��λ����ʾһ��ʮλ����
     */
    uint8 temp;
	while(len > 0)
	{
	    temp = num % 10;        // num���λ
	    ShowChar(x + 8*(len-1), y, temp+'0');
	    num /= 10;
	    len--;
	}
}

void ClearDisplay()
{
	// ����Ļ
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
     * uint8 x:     x����
     * uint8 page:  page��y���꣩
     * @ Abstract
     * ��ָ��λ����ʾ�ַ���
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

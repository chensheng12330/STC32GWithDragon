#include "MAX7219.h"
#include "config.h"

//��ַ�����ݷ����ӳ���
void Write7219(unsigned char address,unsigned char dat)
{  
    unsigned char i;
    CS=0;    //����Ƭѡ�ߣ�ѡ������
    //���͵�ַ
    for (i=0;i<8;i++)        //��λѭ��8��             
    {  
       CLK=0;        //����ʱ������
       DIN=(bit)(address&0x80); //ÿ��ȡ���ֽ�     
       address<<=1;             //����һλ
       CLK=1;        //ʱ�������أ����͵�ַ
    }
    //��������
    for (i=0;i<8;i++)              
    {  
       CLK=0;
       DIN=(bit)(dat&0x80);    
       dat<<=1; 
       CLK=1;        //ʱ�������أ���������
    }
    CS=1;    //���ͽ�������������������                      
}
//MAX7219��ʼ��������MAX7219�ڲ��Ŀ��ƼĴ���
void Initial(void)                
{
    Write7219(SHUT_DOWN,0x01);         //������������ģʽ��0xX1��
    Write7219(DISPLAY_TEST,0x00);      //ѡ����ģʽ��0xX0��
    Write7219(DECODE_MODE,0xff);       //ѡ��ȫ����ģʽ
    Write7219(SCAN_LIMIT,0x07);        //8ֻLEDȫ��
    Write7219(INTENSITY,0x0F);         //���ó�ʼ����
}
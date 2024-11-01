#include "HX711.h"
#include "config.h"

u32 HX711_Read(void)	//����128
{
	u32 count; 
	u8 i; 
  HX711_DOUT=1; 
	_nop_();
	_nop_();
  HX711_SCK=0; 
  count=0;
  while(HX711_DOUT); 
  for(i=0;i<24;i++)
	{ 
	  HX711_SCK=1; 
	  count=count<<1; 
		HX711_SCK=0; 
		if(HX711_DOUT)
		count++; 
	 	_nop_();
		_nop_();
	} 
 	HX711_SCK=1; 
  //count=count^0x800000;//��25�������½�����ʱ��ת������
	_nop_();
	_nop_();
	HX711_SCK=0;  
	return(count);
}
#include <Matrix_key.h>
#include <delay.h>

u8 Matrix_key()
{
	u8 temp,keyvalue;		//temp��P2ֵ���棬keyvalue:������
	static u8 keystate;				//����״̬
	KEY_PORT = 0xF0;					//��ɨ��
	if(KEY_PORT != 0xF0 && keystate == 0)			//�ж��Ƿ��а������£���������Ѿ��ǰ���״̬�򲻽���if
	{
		delayms(10);				//����ȥ��
		if(KEY_PORT != 0xF0 && keystate == 0)		//�ٴ��ж��Ƿ��а������£���������Ѿ��ǰ���״̬�򲻽���if
		{
			keystate = 1;		//����״̬Ϊ1����������
			temp = KEY_PORT;			//��ȡP2ֵ
			switch(temp)
			{
				case 0xE0:keyvalue = 1;break;			//��1���а������£���ֵ����1
				case 0xD0:keyvalue = 2;break;			//��2���а������£���ֵ����2
				case 0xB0:keyvalue = 3;break;			//��3���а������£���ֵ����3
				case 0x70:keyvalue = 4;break;			//��4���а������£���ֵ����4
			}
			KEY_PORT = 0x0F;				//��ת��ƽ����ɨ��
			if(KEY_PORT != 0x0F)		//�ж��Ƿ��а�������
			{
				temp = KEY_PORT;			//��ȡP2ֵ
				switch(temp)
				{
					case 0x07:keyvalue += 0;break;		//��1�а��£���ֵkeyvalue+0
					case 0x0B:keyvalue += 4;break;		//��2�а��£���ֵkeyvalue+4
					case 0x0D:keyvalue += 8;break;		//��3�а��£���ֵkeyvalue+8
					case 0x0E:keyvalue += 12;break;		//��4�а��£���ֵkeyvalue+12
				}
			}
			
		}
	}
	if(KEY_PORT == 0xF0)
	{
		keystate = 0;		//�ɿ����������㰴��״̬
		keyvalue = 0;		//�ɿ����������㰴����
	}
	return keyvalue;								//���ذ�����
}
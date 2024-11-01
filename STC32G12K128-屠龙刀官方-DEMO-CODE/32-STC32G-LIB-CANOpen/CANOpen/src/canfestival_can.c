#include "STC32G_CAN.h"
#include "can.h"
#include "objdict.h"
#include "canfestival.h"


UNS8 canSend(CAN_PORT notused, Message *m)
{
//	UNUSED(notused);
    notused = 0;
	CanSendStandardFrame(m->cob_id,m->m_data);
	return 0;
}

UNS8 canReceive(Message *m)
{
	uint8_t i;
	uint8_t buffer[16];

	CanReadFifo(buffer);
	
	m->cob_id =  ((buffer[1] << 8) + buffer[2]) >> 5;
	m->rtr    =  (buffer[0]>> 6) & 0x01;
	m->len    =  (buffer[0]>> 0) & 0x0F;
	for(i=0;i<8;i++)
	{
		m->m_data[i] = buffer[i+3];
	}
	return 0;
}

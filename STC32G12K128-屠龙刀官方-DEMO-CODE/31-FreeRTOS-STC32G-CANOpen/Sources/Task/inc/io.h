#ifndef IO_H
#define IO_H


/* ��ʾ������ */
portTASK_FUNCTION( vIOTask, pvParameters );

void prvIOInit( void );
void prvIOUpdate( void );

#endif

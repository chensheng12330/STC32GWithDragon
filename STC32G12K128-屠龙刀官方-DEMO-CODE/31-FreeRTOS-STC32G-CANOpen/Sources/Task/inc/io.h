#ifndef IO_H
#define IO_H


/* 显示任务函数 */
portTASK_FUNCTION( vIOTask, pvParameters );

void prvIOInit( void );
void prvIOUpdate( void );

#endif

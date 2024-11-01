
#ifndef ADCKEY_H
#define ADCKEY_H


/* ADC按键扫描任务函数 */
portTASK_FUNCTION( vAdcKeyTask, pvParameters );


extern uint8_t ucKeyCode;                               //给用户使用的键码, 1~16有效

#endif

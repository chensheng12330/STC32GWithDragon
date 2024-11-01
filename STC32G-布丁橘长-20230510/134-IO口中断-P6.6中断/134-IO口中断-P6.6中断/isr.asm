;IO口中断向量地址：
;P0:012BH	P1:0133H	P2:013BH	P3:0143H	
;P4:014BH	P5:0153H	P6:015BH	P7:0163H
;13号中断向量地址：006BH

CSEG AT 015BH	 	; P6 口中断入口地址
JMP 006BH 			; 借用 13 号中断的入口地址
END
#ifndef __ADFLITER_H__
#define __ADFLITER_H__

#include "config.h"

#define Sample 5								// 采样次数

u32 Fliter_range();							// ADC限幅滤波算法
u32 Fliter_middle();						// ADC中位值滤波算法（采样Sample次，冒泡排序，取中间值。采样次数Sample可以按照需求改动,Sample值需为奇数）
u32 Fliter_average();						// ADC算数平均滤波法

#endif

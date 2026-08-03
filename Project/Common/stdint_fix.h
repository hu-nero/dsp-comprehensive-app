/*
 * stdint_fix.h
 *
 *  Created on: 2025年12月16日
 *      Author: 75795
 */

#ifndef COMMON_STDINT_FIX_H_
#define COMMON_STDINT_FIX_H_

// 临界区保护
#define ENTER_CRITICAL() asm(" setc INTM")  // 关中断
#define EXIT_CRITICAL()  asm(" clrc INTM")  // 开中断

// Max宏
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

// 绝对值
#define ABS(x)  ((x) >= 0 ? (x) : -(x))

typedef unsigned char		uint8_t;
typedef char				int8_t;


#endif /* COMMON_STDINT_FIX_H_ */

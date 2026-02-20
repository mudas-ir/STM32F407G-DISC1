/*
 * main.h
 *
 *  Created on: Feb 14, 2026
 *      Author: Mudassir
 */

#ifndef MAIN_H_
#define MAIN_H_

//	MACROS
#define BIT(n) (1U << (n))

//	Memory Mapping
#define RAM_START				0x20000000U
#define	RAM_SIZE				(192U * 1024U)
#define RAM_END					((RAM_START + RAM_SIZE) - 1U)

#define	STACK_SIZE				1024U

#define TASK1_STACK_START     	RAM_END
#define TASK2_STACK_START      	(TASK1_STACK_START - STACK_SIZE)
#define TASK3_STACK_START      	(TASK2_STACK_START - STACK_SIZE)
#define TASK4_STACK_START 		(TASK3_STACK_START - STACK_SIZE)
#define SCHEDULER_STACK_START	(TASK4_STACK_START - STACK_SIZE)

//	SysTick Register Mapping
#define SYST_CSR				(*((volatile uint32_t *)0xE000E010))
#define SYST_RVR				(*((volatile uint32_t *)0xE000E014))
#define SYST_CVR				(*((volatile uint32_t *)0xE000E018))
#define SYST_CALIB				(*((volatile uint32_t *)0xE000E01C))

#define CPU_CLOCK_HZ		 	16000000U
#define TICK_HZ 				1000U

#define MAX_TASK 				4

#define DUMMY_XPSR				0x01000000U
#define EXC_RETURN				0xFFFFFFFDU

#define SHCSR					(*((volatile uint32_t *)0xE000ED24))

#endif /* MAIN_H_ */

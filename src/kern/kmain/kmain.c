#include <sys_init.h>
#include <cm4.h>
#include <kmain.h>
#include <kstdio.h>
#include <kstring.h>
#include <stdint.h>
#include <usart.h>
#include <seven_segment.h>
#include <sys.h>
#include <test_interrupt.h>

void Exti_Init()
{
	RCC->AHB1ENR |= (1 << 0); // Enable GPIOA clock

	GPIOA->PUPDR |= (1 << 0); // Bits (3:2) = 1:0  --> PA1 is in Pull Up mode

	RCC->APB2ENR |= (1 << 14); // Enable SYSCNFG

	SYSCFG->EXTICR[0] &= ~(0xf << 0); // Bits[7:6:5:4] = (0:0:0:0)  -> configure EXTI1 line for PA1

	EXTI->IMR |= (1 << 0); // Bit[1] = 1  --> Disable the Mask on EXTI 1

	EXTI->RTSR &= ~(1 << 0); // Disable Rising Edge Trigger for PA0

	EXTI->FTSR |= (1 << 0); // Enable Falling Edge Trigger for PA0

	__NVIC_SetPriority(EXTI0_IRQn, 5); // Set Priority

	__NVIC_EnableIRQn(EXTI0_IRQn); // Enable Interrupt
}

void kmain(void)
{
	__sys_init();
	__SysTick_init(100000000);

	Exti_Init();

	while (1)
	{
		int x;
		kprintf("\n********************************************\n");
		kprintf("Enter 0 to restart the System\n");
		kprintf("Enter 1 to enable hardfault\n");
		kprintf("Enter 2 to Change Basepri value\n");
		kprintf("Press Button for External Interrupt\n");
		kprintf("********************************************\n\n");

		kscanf("%d", &x);

		switch (x)
		{
		case 0:
			kprintf("System Restarting............");
			SCB->AIRCR = (0x5FA << 16) | (0x4 << 0);
			break;
		case 1:
			__asm__("UDF #1");
			break;
		case 2:
			config_basepri();
			break;

		default:
			break;
		}
	}
}

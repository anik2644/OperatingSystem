#include <test_interrupt.h>
#include <cm4.h>
#include <kstdio.h>
#include <sys.h>

void config_basepri()
{
      kprintf("Current Basepri Value is: %d\n", get_basepri_value());
      kprintf("Enter a new value for BASEPRI: \n");
      int val;
      kscanf("%d\n", &val);
      __set_BASEPRI(val);
      kprintf("Current Basepri Value is: %d\n", get_basepri_value());
}
void change_systick_priority()
{
      int p;
      kprintf("Systick Current priority: %d\n", NVIC_GetPriority(SysTick_IRQn));
      kprintf("Enter a value for set new Systick priority: ");
      kscanf("%d\n", &p);
      NVIC_SetPriority(SysTick_IRQn, p);
      kprintf("Systick priority changed to: %d\n", NVIC_GetPriority(SysTick_IRQn));
}
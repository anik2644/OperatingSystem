/*
 * Copyright (c) 2022
 * Computer Science and Engineering, University of Dhaka
 * Credit: CSE Batch 25 (starter) and Prof. Mosaddek Tushar
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <kmain.h>
#include <schedule.h>

#define O_RDONLY 0
#define O_WRONLY 1
#define O_APPEND 2

TCB_TypeDef task[10], idleTask;
dev_table device_list[64];
int count = 0;
uint32_t device_count = 0;

// Function to initialize the device table
void __init_dev_table(void)
{
    // initialize device_list.tref = 0
    for (int i = 0; i < 64; i++)
    {
        device_list[i].t_ref = 0;
    }

    // init device_list[device_count]
    __strcpy(device_list[device_count].name, "USART2");
    device_list[device_count].t_ref += 1;
    device_list[device_count].t_access = O_RDONLY;
    device_list[device_count].op_addr = USART2;
    device_count++;

    // init device_list[device_count] (usart2)
    __strcpy(device_list[device_count].name, "USART2");
    device_list[device_count].t_ref += 1;
    device_list[device_count].t_access = O_WRONLY;
    device_list[device_count].op_addr = USART2;
    device_count++;

    // init device_list[device_count] (stderr)
    __strcpy(device_list[device_count].name, "STDERR");
    device_list[device_count].t_ref += 1;
    device_list[device_count].t_access = O_WRONLY;
    device_list[device_count].op_addr = 0;
    device_count++;
}

// Task to handle system sleep and potential system reboot
void taskIdle_sleep(void)
{
    int is_Reboot = 0;
    uprintf("\n\r\n\rAll the tasks have been completed.\n\r\n\r\tEnter 1 to reboot the system. \n\r\tEnter any other number to enter sleeping mode.\n\r");
    uscanf("%d", &is_Reboot);
    if (is_Reboot == 1)
        reboot();

    uprintf("Entering sleep mode...\n\r");
    while (1)
        ;
}

// Sample task incrementing a global variable
void task1(void)
{
    uint32_t value;
    uint32_t inc_count = 0;
    while (1)
    {
        // critical region
        value = count;
        value++;

        // check if some other tasks increase the count
        if (value != count + 1)
        {
            uprintf("Error %d from task1 is = %d\n\r", value, count + 1);
        }
        else
        {
            // critical region
            count = value;
            inc_count++;
        }
        if (count >= 10000000)
        {
            uint16_t task_id = getpid(); /* It is an SVC call*/
            // display how many increments it has successfully done
            uprintf("Total increment done by task1 %d is: %d\n\r", task_id, inc_count);
            break;
        }
    }
    exit();
}

// Sample task incrementing a global variable
void task2(void)
{
    uint32_t value;
    uint32_t inc_count = 0;
    while (1)
    {
        // critical region
        value = count;
        value++;

        // check if some other tasks increase the count
        if (value != count + 1)
        {
            uprintf("Error %d from task2 is = %d\n\r", value, count + 1);
        }
        else
        {
            // critical region
            count = value;
            inc_count++;
        }
        if (count >= 10000000)
        {
            uint16_t task_id = getpid(); /* It is an SVC call*/
            // display how many increments it has successfully done
            uprintf("Total increment done by task2 %d is: %d\n\r", task_id, inc_count);
            break;
        }
    }
    exit();
}

// Sample task incrementing a global variable
void task3(void)
{
    uint32_t value;
    uint32_t inc_count = 0;
    while (1)
    {
        // critical region
        value = count;
        value++;

        // check if some other tasks increase the count
        if (value != count + 1)
        {
            uprintf("Error %d from task3 is = %d\n\r", value, count + 1);
        }
        else
        {
            // critical region
            count = value;
            inc_count++;
        }
        if (count >= 10000000)
        {
            uint16_t task_id = getpid(); /* It is an SVC call*/
            // display how many increments it has successfully done
            uprintf("Total increment done by task3 %d is: %d\n\r", task_id, inc_count);
            break;
        }
    }
    exit();
}

// Function to switch to unprivileged mode
void unprivileged_mode(void)
{
    // read operation from special register CPSR and SPSR
    __asm volatile("MRS R0, CONTROL");
    // writing 1 to R0 by doing OR operation
    __asm volatile("ORRS R0, R0, #1");
    // write operation to special register CPSR and SPSR
    __asm volatile("MSR CONTROL, R0");
}

// Function to set interrupt priorities
void __set_interrupt_priorities(void)
{
    __NVIC_SetPriority(SVCall_IRQn, 1);
    __NVIC_SetPriority(SysTick_IRQn, 0x2);
    // lowest priority given to PendSV
    __NVIC_SetPriority(PendSV_IRQn, 0xFF);
}

// Function to print the device list
void print_device_list()
{
    kprintf("\n\n______________________\n\n");
    for (int i = 0; i < device_count; i++)
    {
        kprintf("device name = %s\n", device_list[i].name);
        kprintf("device t_ref = %d\n", device_list[i].t_ref);
        kprintf("device t_access = %d\n", device_list[i].t_access);
        kprintf("device op_addr = %x\n", device_list[i].op_addr);
        kprintf("\n");
    }
}

// Function for SVC testing
void SVCall_Checking_Test(void)
{
    // test my_scanf
    char *data = "Print things by USART-2 to Hercules";
    my_scanf(0, &data, 5);
    kprintf("data = %s\n", data);

    // test fopen and fclose
    char *device_name = "GPIOA";
    uint8_t t_access = O_RDONLY;
    uint32_t *op_addr = (uint32_t *)GPIOA;
    fopen(device_name, t_access, op_addr);
    print_device_list();
    int x = 100;
    kprintf("The Value of x = %d\n", x);
    fclose(op_addr);
    print_device_list();

    // test reboot
    kprintf("Do you want to reboot the OS? (y : 1 /n : 0)\n");
    int if_reboot;
    kscanf("%d", &if_reboot);
    if (if_reboot == 1)
    {
        reboot();
    }
}

// Main function
void kmain(void)
{
    __sys_init();

    // SVC Testing Code
    int inp;
    kprintf("Want to test SVC?\n5: \"Yes\"\nany other number:\"No\"\n");
    kscanf("%d", &inp);
    if (inp == 5)
    {
        __init_dev_table();
        SVCall_Checking_Test();
    }

    // PendSV Testing Code
    __set_interrupt_priorities();

    // Creating three tasks to modify a global variable count and a sleep task to make the system sleep
    task_create(task, task1, (uint32_t *)TASK_STACK_START);
    task_create(task + 1, task2, (uint32_t *)(TASK_STACK_START - TASK_STACK_SIZE));
    task_create(task + 2, task3, (uint32_t *)(TASK_STACK_START - (2 * TASK_STACK_SIZE)));
    task_create(&idleTask, taskIdle_sleep, (uint32_t *)(TASK_STACK_START - (3 * TASK_STACK_SIZE)));

    // initializing queue
    initialize_queue();

    // adding tasks to queue
    add_to_ready_queue(task);
    add_to_ready_queue(task + 1);
    add_to_ready_queue(task + 2);
    set_sleeping_task(&idleTask);

    // going to user mode
    unprivileged_mode();

    // set pendsv before starting task
    set_task_pending(1);

    task_start();
    uprintf("\n\r\tTasks Finished....\n\r");

    while (1)
        ;
}

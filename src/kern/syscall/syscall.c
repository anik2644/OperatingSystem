#include <syscall.h>
#include <syscall_def.h>
#include <errno.h>
#include <errmsg.h>
#include <kstdio.h>
#include <usart.h>
#include <cm4.h>
#include <types.h>

void __sys_start_task(void) {
    // Entry point after an exception
    __asm volatile("POP {LR}");

    // Extract arguments from the stack
    unsigned int *svc_args;
    __asm volatile("MOV %0, R1"
                   : "=r"(svc_args));
    uint32_t task_psp = svc_args[3];
    __asm volatile("MOV R0, %0"
                   :
                   : "r"(task_psp));

    // Load task context from stack
    __asm volatile("LDMIA R0!,{R4-R11}");
    __asm volatile("MSR PSP, R0");
    __asm volatile("ISB");

    // Set LR for exception return and return from exception
    __asm volatile("MOV LR, 0xFFFFFFFD");
    __asm volatile("BX LR");
}

void __sys_getpid(void) {
    // Extract syscall arguments from the stack
    unsigned int *svc_args;
    __asm volatile("MOV %0, R1"
                   : "=r"(svc_args));

    // Get task ID from the Task Control Block (TCB)
    *((unsigned int *)svc_args[0]) = (*((TCB_TypeDef *)svc_args[4])).task_id;
    return;
}

void __sys_exit(void) {
    // Extract syscall arguments from the stack
    unsigned int *svc_args;
    __asm volatile("MOV %0, R1"
                   : "=r"(svc_args));

    // Set task status to 4 (exit)
    (*((TCB_TypeDef *)svc_args[4])).status = 4;
    return;
}

void __sys_read(void) {
    // Extract syscall arguments from the stack
    unsigned int *svc_args;
    __asm volatile("MOV %0, R1"
                   : "=r"(svc_args)
                   :);

    // Initialize pointers and variables
    int *bytes_read = (int *)svc_args[4]; // R12
    int len = (int)svc_args[3];           // R3
    unsigned char *buff = (unsigned char *)svc_args[2];

    // Perform read based on length
    if (len == 1) {
        // Read a single character
        buff[0] = UART_GetChar(USART2);
        *bytes_read = 1;
    } else {
        // Read a string
        *bytes_read = _USART_READ_STR(USART2, buff, 50);
    }
    return;
}

void __sys_write(void) {
    // Extract syscall arguments from the stack
    unsigned char *s;
    unsigned int *svc_args;
    __asm volatile("MOV %0, R1"
                   : "=r"(svc_args)
                   :);
    s = (unsigned char *)svc_args[1]; // R1
    int len = _USART_WRITE(USART2, s);
    *((int *)svc_args[4]) = len;
    return;
}

void __sys_gettime(void) {
    // Extract syscall arguments from the stack
    unsigned int *svc_args;
    __asm volatile("MOV %0, R1"
                   : "=r"(svc_args)
                   :);

    // Get the current time and store it
    *((unsigned int *)svc_args[1]) = __getTime(); // R1
}

void __sys_reboot(void) {
    // Print a message and initiate a reboot
    kprintf("rebooting...");
    SCB->AIRCR = (0x05FA << 16) | (1 << 2);

    // Halt further execution
    while (1);
}

void __sys_yield(void) {
    // Trigger a context switch by setting the PendSV bit
    SCB->ICSR |= (1 << 28);
}

void syscall(uint16_t callno) {
    // Dispatcher for system calls based on syscall number
    switch (callno) {
        case SYS_read:
            __sys_read();
            break;
        case SYS_write:
            __sys_write();
            break;
        case SYS_reboot:
            __sys_reboot();
            break;
        case SYS__exit:
            __sys_exit();
            break;
        case SYS_getpid:
            __sys_getpid();
            break;
        case SYS___time:
            __sys_gettime();
            break;
        case SYS_yield:
            __sys_yield();
            break;
        case SYS_start_task:
            __sys_start_task();
            break;
        default:
            // Handle unknown syscall number (return an error)
            break;
    }

    // Handle SVC return
    __asm volatile("POP {LR}");
}

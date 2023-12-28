#include <schedule.h>
#include <types.h>

ReadyQ_TypeDef rq;
BlockedQ_TypeDef bq;
TCB_TypeDef *current, *__sleep;
uint16_t t1, t2;

uint16_t pri_vals[task_count] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

void set_sleeping_task(TCB_TypeDef *s)
{
    __sleep = s;
    return;
}

void task_start(void)
{
    if (is_ready_queue_empty() == 0)
    {
        kprintf("There is no task in the queue to start\n\r");
        return;
    }

    TCB_TypeDef *qf = ready_queue_front_();
    current = qf;

    t1 = __getTime();
    current->response_time = t1;

    current->status = RUNNING;
    __asm volatile("MOV R12, %0"
                   :
                   : "r"(qf->psp));
    // calls SYS_start_task from syscall
    __asm volatile("SVC #121");
}

void initialize_queue(void)
{
    rq.max = 22;
    rq.size = 0;
    rq.st = 0;
    rq.ed = -1;

    bq.max = 22;
    bq.size = 0;
    bq.st = 0;
    bq.ed = -1;
}

void add_to_ready_queue(TCB_TypeDef *t)
{
    if (rq.max >= rq.size + 1)
    {
        rq.ed = (rq.ed + 1) % rq.max;
        rq.q[rq.ed] = t;
        rq.size++;
    }
    else
    {
        kprintf("The ready queue is full, thus the task could not be added\n\r");
    }
}

void add_to_blocked_queue(TCB_TypeDef *t)
{
    if (bq.max >= bq.size + 1)
    {
        bq.ed = (bq.ed + 1) % bq.max;
        bq.q[bq.ed] = t;
        bq.size++;
    }
    else
    {
        kprintf("The blocked queue is full, thus the task could not be added\n\r");
    }
}

TCB_TypeDef *ready_queue_front_(void)
{
    if (is_ready_queue_empty() == 0)
    {
        add_to_ready_queue(__sleep);
    }
    int front = rq.st;
    rq.st = (rq.st + 1) % rq.max;
    rq.size--;
    return *((rq.q) + front);
}

TCB_TypeDef *blocked_queue_front_(void)
{
    int front = bq.st;
    bq.st = (bq.st + 1) % bq.max;
    bq.size--;
    return *((bq.q) + front);
}

int is_ready_queue_empty(void)
{
    return rq.size;
}

int is_blocked_queue_empty(void)
{
    return bq.size;
}

const uint16_t initial_task_id = 1000;
uint16_t last_assigned = initial_task_id;

// Generate a unique ID for the task
uint16_t generate_task_id(void)
{
    return last_assigned++;
}

void task_create(TCB_TypeDef *tcb, void (*task_func)(void), uint32_t *stack)
{
    uint16_t x = generate_task_id();

    // Initialize the TCB fields
    tcb->magic_number = 0xFECABAA0;

    // Generate a unique ID for the task
    tcb->task_id = x;
    tcb->priority = pri_vals[x - 1000];
    tcb->status = READY;
    tcb->starting_time = __getTime();
    tcb->execution_time = 0;
    tcb->response_time = 0;
    // Initialize the stack for the task
    // Point to the top of the stack
    tcb->psp = stack;

    *(--tcb->psp) = DUMMY_XPSR;          // xPSR
    *(--tcb->psp) = (uint32_t)task_func; // PC (task entry point)
    *(--tcb->psp) = 0xFFFFFFFD;          // LR Exception return table

    *(--tcb->psp) = 0x0000000; // R12 - storing actual object address in here
    *(--tcb->psp) = 0x0000000; // R3
    *(--tcb->psp) = 0x0000000; // R2
    *(--tcb->psp) = 0x0000000; // R1
    *(--tcb->psp) = 0x0000000; // R0

    // initializing registers r11 to r4
    for (int i = 0; i < 8; i++)
    {
        if (i == 0)
        {
            // pushing reference of own task in r11
            *(--tcb->psp) = (uint32_t)tcb;
        }
        else
        {
            *(--tcb->psp) = 0x0000000;
        }
    }
}

void context_switch(void)
{
    int index = current->task_id - 1000;

    condition = 1;

    // int condition = 1 && !semaphore; // RR

    // int condition = (finished[index] || pri_vals[index] > pri_vals[(index + 1) % task_count]); // P

    // int condition = finished[index]; // FCFS

    switch (option)
    {
    case 0: // RR
        condition = 1;
        break;
    case 1: // RR with semaphore
        condition = !semaphore; // 1 means none is on critical section
        break;
    case 2: // FCFS
        condition = finished[index];
        break;
    // case 3: // P
    //     condition = (finished[index] || pri_vals[index] > pri_vals[(index + 1) % task_count]);
    //     break;
    default:
        condition = 1;
        break;
    }

    if (condition)
    {
        if (current->status == RUNNING)
        {
            current->status = READY;
            add_to_ready_queue(current);
        }
    }
    else
    {
        if (option != 3)
        {
            if (is_ready_queue_empty()) //when not empty
            {
                TCB_TypeDef *qf = ready_queue_front_();
                qf->status = BLOCKED;
                add_to_blocked_queue(qf);
            }
        }
    }

    t2 = __getTime();
    current->execution_time += t2 - t1;
    current->completion_time = t2;
    t1 = t2;

    // if (index != task_count)
    //     kprintf("Task %d to ", current->task_id);

    if (condition)
    {
        TCB_TypeDef *qf;

        if (!is_blocked_queue_empty())
            qf = ready_queue_front_();
        else
            qf = blocked_queue_front_();
        current = qf;
        current->status = RUNNING;
    }

    // if (index != task_count)
    //     kprintf("Task %d\n", current->task_id);

    if (current->response_time == 0)
        current->response_time = t2;

    return;
}

void __attribute__((naked)) PendSV_Handler(void)
{
    SCB->ICSR |= (1 << 27);
    /* Save current context */

    // Get current process stack pointer value
    __asm volatile("MRS R0,PSP");
    // Save R4 to R11 in task stack (8 regs)
    __asm volatile("STMDB R0!,{R4-R11}");//push manually
    __asm volatile("PUSH {LR}");
    // Save current psp value
    __asm volatile("MOV %0, R0"
                   : "=r"(current->psp)
                   :);

    context_switch();

    // Get new task psp value
    __asm volatile("MOV R0, %0"
                   :
                   : "r"(current->psp));
    // Load R4 to R11 from taskstack (8 regs)
    __asm volatile("LDMIA R0!,{R4-R11}");//pop manually
    __asm volatile("MSR PSP,R0");
    __asm volatile("POP {LR}");
    __asm volatile("BX LR");
}

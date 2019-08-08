extern void stackcopy(int, int);
extern void Switch();
extern void showchar();


int NEW = 0;
int READY = 1;
int RUNNING = 2;
int BLOCKED = 3;
int EXIT = 4;
int Segment = 0x1000;             /* 用户程序段值与实验六不同的是，将这两个变量其声明为了全局变量 */
int Finite = 0;					  /* 调度次数 */
#define MAX_PCB_NUMBER 8          /*最多同时放这么多个进程 */

typedef struct RegisterImage{
	int SS;
	int GS;
	int FS;
	int ES;
	int DS;
	int DI;
	int SI;
	int BP;
	int SP;
	int BX;
	int DX;
	int CX;
	int AX;
	int IP;
	int CS;
	int FLAGS;
}RegisterImage;

typedef struct PCB{
	RegisterImage regImg;		/* 各个寄存器的值，多个int组成 */
	int Process_Status;			/* 进程状态 */
	int Used;					/* 是否被占用 */
	int FatherID;				/* 父进程ID */
}PCB;


PCB pcb_list[MAX_PCB_NUMBER];
int CurrentPCBno = 0; 		
int Program_Num = 0;



PCB* Current_Process();
void Save_Process(int,int, int, int, int, int, int, int,
		  int,int,int,int, int,int, int,int );
void init(PCB*, int, int);
void Schedule();
void special();

void wakeup(int id){
	pcb_list[id].Process_Status = READY;
}

void blocked(int id){
	pcb_list[id].Process_Status = BLOCKED;
}


void Save_Process(int gs,int fs,int es,int ds,int di,int si,int bp,
		int sp,int dx,int cx,int bx,int ax,int ss,int ip,int cs,int flags)
{
	pcb_list[CurrentPCBno].regImg.AX = ax;
	pcb_list[CurrentPCBno].regImg.BX = bx;
	pcb_list[CurrentPCBno].regImg.CX = cx;
	pcb_list[CurrentPCBno].regImg.DX = dx;

	pcb_list[CurrentPCBno].regImg.DS = ds;
	pcb_list[CurrentPCBno].regImg.ES = es;
	pcb_list[CurrentPCBno].regImg.FS = fs;
	pcb_list[CurrentPCBno].regImg.GS = gs;
	pcb_list[CurrentPCBno].regImg.SS = ss;

	pcb_list[CurrentPCBno].regImg.IP = ip;
	pcb_list[CurrentPCBno].regImg.CS = cs;
	pcb_list[CurrentPCBno].regImg.FLAGS = flags;
	
	pcb_list[CurrentPCBno].regImg.DI = di;
	pcb_list[CurrentPCBno].regImg.SI = si;
	pcb_list[CurrentPCBno].regImg.SP = sp;
	pcb_list[CurrentPCBno].regImg.BP = bp;
}

void Schedule(){

	int temp_count = MAX_PCB_NUMBER;
    if (pcb_list[CurrentPCBno].Process_Status == RUNNING)
        pcb_list[CurrentPCBno].Process_Status = READY;
	
    while (temp_count--)        /* 要么return 要么遍历整个PCB */
    {
		CurrentPCBno++;
        if (CurrentPCBno >= MAX_PCB_NUMBER)
            CurrentPCBno = 1;
        if (pcb_list[CurrentPCBno].Used == 1)    /*如果这个PCB块被占用 */
        {
            if (pcb_list[CurrentPCBno].Process_Status == READY )
            {
                pcb_list[CurrentPCBno].Process_Status = RUNNING;
                return ;
            }
            else if (pcb_list[CurrentPCBno].Process_Status == NEW)  /* 如果是new状态的话要单独讨论一次 */
				{
					if( Finite >= 118 ){         /* 特殊处理了一下，因为多进程几乎结束了，不应该出现new的状态，忽略 */
						continue;
					}
					return ;
				}
        }
    }
	
	/* 如果没有其他运行的进程，就继续运行之前的进程 如果它也结束了就返回内核 */
    if (pcb_list[CurrentPCBno].Used == 1 && pcb_list[CurrentPCBno].Process_Status == READY)
        pcb_list[CurrentPCBno].Process_Status = RUNNING;
    else{
		CurrentPCBno = 0;
		Segment = 0x1000;
		Program_Num = 0;
	} 

}



PCB* Current_Process(){
	return &pcb_list[CurrentPCBno];
}

void init(PCB* pcb,int segement, int offset)
{
	pcb->Used = 0;			/* 初始化为未被占用 */
	pcb->FatherID = 0;		/* 初始父进程为主进程 */
	pcb->regImg.GS = 0xb800;
	pcb->regImg.SS = segement;
	pcb->regImg.ES = segement;
	pcb->regImg.DS = segement;
	pcb->regImg.CS = segement;
	pcb->regImg.FS = segement;
	pcb->regImg.IP = offset;
	pcb->regImg.SP = offset - 4;
	pcb->regImg.AX = 0;
	pcb->regImg.BX = 0;
	pcb->regImg.CX = 0;
	pcb->regImg.DX = 0;
	pcb->regImg.DI = 0;
	pcb->regImg.SI = 0;
	pcb->regImg.BP = 0;
	pcb->regImg.FLAGS = 512;
	pcb->Process_Status = NEW;
}

void special()
{
	if(pcb_list[CurrentPCBno].Process_Status==NEW)
		pcb_list[CurrentPCBno].Process_Status=RUNNING;
}


/* 实验7新增代码 */
int Find_empty_PCB()   /* 用于寻找空闲进程 */
{
    int index = 1;
    while (index < MAX_PCB_NUMBER)
    {
        if (pcb_list[index].Used != 1)
            return index;
        index++;
    }
    return -1;
}

void PCBcopy(PCB* p1, PCB* p2)
{
    p1->regImg.AX = p2->regImg.AX;
    p1->regImg.BX = p2->regImg.BX;
    p1->regImg.CX = p2->regImg.CX;
    p1->regImg.DX = p2->regImg.DX;
    p1->regImg.CS = p2->regImg.CS;
    p1->regImg.IP = p2->regImg.IP;
    p1->regImg.DS = p2->regImg.DS;
    p1->regImg.ES = p2->regImg.ES;
    p1->regImg.GS = p2->regImg.GS;
    p1->regImg.FS = p2->regImg.FS;
    /*p1->regImg.SS = p2->regImg.SS;   栈要做特殊处理，见stackcopy() */
    p1->regImg.DI = p2->regImg.DI;
    p1->regImg.SI = p2->regImg.SI;
    p1->regImg.BP = p2->regImg.BP;
    p1->regImg.SP = p2->regImg.SP;
    p1->regImg.FLAGS = p2->regImg.FLAGS;
    p1->Process_Status = READY;
}

/* Copy PCB */
int do_fork()
{
    int i = Find_empty_PCB();		/* 寻找空闲进程 */
    if (i == -1){				/* 没有空闲的PCB块 ax作为返回值 */
        pcb_list[CurrentPCBno].regImg.AX = -1;
        return ;
    }
    Program_Num++;              /* 创建成功 */
    PCBcopy(&pcb_list[i], &pcb_list[CurrentPCBno]); 			        /* 拷贝PCB */
    stackcopy(pcb_list[i].regImg.SS, pcb_list[CurrentPCBno].regImg.SS);	/* 特殊处理栈 */
    pcb_list[i].FatherID = CurrentPCBno;								/* 标识父进程 */
    pcb_list[i].Used =  1;					/* 标识此进程被占用 */
    pcb_list[i].regImg.AX = 0;  			/* 子进程本身返回0 */
    pcb_list[CurrentPCBno].regImg.AX = i;	/* 父进程返回进程号 */
    pcb_list[CurrentPCBno].Process_Status = READY;  
    Switch();
}


void delay() /* 延时 */
{
    int i;
    int j;
    for(i=0; i<20000; i++)
        for(j=0; j<20000; j++);
    return ;
}

void do_wait()
{
	/* 阻塞进程 */
	blocked(CurrentPCBno);
    Schedule();
    Switch();
	delay();
	delay();
}


int do_exit(int ch)
{
    int k;
    int FatherID = pcb_list[CurrentPCBno].FatherID;
    pcb_list[CurrentPCBno].Process_Status = EXIT;
	/* 结束进程并初始化进程控制块 */
    init(&pcb_list[CurrentPCBno], (CurrentPCBno)*0x1000, 0x100);
	/* 如果当前退出进程的父进程不是内核，解除父进程的阻塞 */
    if (FatherID != 0){
		/* 唤醒父进程 */
        wakeup(FatherID);
		/* 用ax来传递信号 */
        pcb_list[FatherID].regImg.AX = ch;
    }
    Program_Num --;
    Segment=0x1000;
    for (k=0; k<15; ++k)
        delay();
    Schedule();
    Switch();
}


/* 实验8 新增*/
#define max_semaphore 8					/* 最多可用信号量 */
#define max_blocked_pcb 8				/* 阻塞进程队列长度 */

typedef struct semaphoretype {
    int count;							/* 资源数量 */
    int blocked_pcb[max_blocked_pcb];	/* 循环队列 */
    int used;							/* 该信号量是否被使用 */
	int front, tail;					/* 循环队列的头尾指针 */
} semaphoretype;

semaphoretype semaphorequeue[max_semaphore];      /* 定义信号量的数组 */

/* 向内核申请一个内核可用信号量，并将count初始化为value */
int do_getsem(int value) {
    int i = 0;
	/* 找没被用的信号量 */
    while (semaphorequeue[i].used == 1 && i < max_semaphore) { ++i; }
    if (i < max_semaphore) {
        semaphorequeue[i].used = 1;
        semaphorequeue[i].count = value;
        semaphorequeue[i].front = 0;
        semaphorequeue[i].tail = 0;
		pcb_list[CurrentPCBno].regImg.AX = i;      /* 告知父进程 */
		Switch();
		return i;								   /* 返回用于定位信号量的值 */
    }
	else {
		pcb_list[CurrentPCBno].regImg.AX = -1;     /* 为找到可用的信号量 */
		Switch();
		return -1;
	}
}

/* 释放对应的信号量 */
void do_freesem(int s) {
	semaphorequeue[s].used = 0;
}

/* 将当前进程阻塞并放入信号量s的阻塞队列中 */
void semaBlock(int s) {
	pcb_list[CurrentPCBno].Process_Status = BLOCKED;
	/* 如果阻塞队列已满，什么都不做 */
	if ((semaphorequeue[s].tail + 1) % max_blocked_pcb == semaphorequeue[s].front) {
		return;
	}
	/* 将该进程放到队列最后 */
	semaphorequeue[s].blocked_pcb[semaphorequeue[s].tail] = CurrentPCBno;
	semaphorequeue[s].tail = (semaphorequeue[s].tail + 1) % max_blocked_pcb;
}


/* 唤醒信号量s的阻塞队列中的一个进程 */
void semaWakeUp(int s) {
	int t;
	/* 如果没有阻塞进程，什么都不做 */
	if (semaphorequeue[s].tail == semaphorequeue[s].front) {
		return;
	}
	/* 拿出队首进程并唤醒 */
	t = semaphorequeue[s].blocked_pcb[semaphorequeue[s].front];
	pcb_list[t].Process_Status = READY;
	semaphorequeue[s].front = (semaphorequeue[s].front + 1) % max_blocked_pcb;
}

/* 信号量的P操作，申请资源 */
void do_p(int s) {
	/* 可用资源减少 */
	semaphorequeue[s].count--;
	/* 没有可用资源则阻塞当前进程 */
	if (semaphorequeue[s].count < 0) {
		semaBlock(s);
		Schedule();
	}
	Switch();
}

/* 信号量的P操作，释放资源 */
void do_v(int s) {
	semaphorequeue[s].count++;
	/* 如果有被阻塞的进程，则可将其唤醒 */
	if (semaphorequeue[s].count <= 0) {
		semaWakeUp(s);
		Schedule();
	}
	Switch();
}

/*初始化信号量块 在主程序中调用*/
void initsema() {
	int i;
	for (i = 0; i < max_semaphore; ++i) {
		semaphorequeue[i].used = 0;
		semaphorequeue[i].count = 0;
		semaphorequeue[i].front = 0;
		semaphorequeue[i].tail = 0;
	}
}




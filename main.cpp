#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "deque"

using namespace std;

#define MAX_PROCESS 10 // 最大进程数量
#define MAX_INSTRUCTION 10 // 每个进程最大指令数量
#define TIME_SLICE 3 // 时间片长度



// 进程状态
enum PROCESS_STATE {CREATE,READY, RUNNING,SUSPEND, TERMINATED};

// PCB（进程控制块）
struct PCB {
    int pid; // 进程ID
    int priority; // 进程优先级
    int state; // 进程状态
    int pc; // 程序计数器，指向下一条将要执行的指令
    int num_instructions; // 指令数量
    void (*instructions[MAX_INSTRUCTION])(); // 指令集，存储函数指针
};

// 全局变量
int num_processes; // 进程数量

deque<PCB> processes;

//struct PCB processes[MAX_PROCESS]; // PCB 数组
int running_process = -1; // 当前正在运行的进程
int to_run = 0; //要运行的进程
struct PCB* now_process = nullptr; //当前运行的程序
int terminate_num = 0; //已经结束进程的数量


// 定义指令函数
void instr1() {
    printf("进程%d执行指令1\n", running_process);
}

void instr2() {
    printf("进程%d执行指令2\n", running_process);
}

int resource = 3;

//获取资源
void getResource(){

    //等待获取资源，当前进程阻塞，并且pc--;
    if (resource<=0){
        now_process->state = SUSPEND;
        now_process->pc--;

        cout<<"资源不够，进入阻塞态"<<endl;
    } else{
        cout<<"获取资源"<<endl;
        resource--;
    }

}

//释放资源
void releaseResource(){


    resource++;

    // 使用迭代器遍历
    for (auto & processe : processes) {

        //激活第一个挂起的进程，将资源给他
        if (processe.state == SUSPEND){
            processe.state = READY;
        }
    }

}



// 初始化进程
void init_processes(int processes_num, int num_instructions) {
    // 随机生成进程的优先级和指令集合
    for (int i = 0; i < processes_num; i++) {
        struct PCB pcb = {i, rand() % 10, CREATE, 0, num_instructions};
        for (int j = 0; j < num_instructions; j++) {

            if (j == 0){
                pcb.instructions[j] = &getResource;
                continue;
            }

            if (j == num_instructions-1){
                pcb.instructions[j] = &releaseResource;
                continue;
            }

            if (rand() % 2 == 0) {
                pcb.instructions[j] = &instr1;
            } else {
                pcb.instructions[j] = &instr2;
            }
        }
        pcb.state = READY;
        processes.push_back(pcb);
    }
}

// 运行进程
void run_process(struct PCB* pcb) {

    //不是就绪态就不运行
    if (pcb->state != READY){
        return;
    }

    printf("进程%d开始运行\n", pcb->pid);
    pcb->state = RUNNING;
    running_process = pcb->pid;
    // 执行指令集
    for (int i = 0;; i++) {


        pcb->instructions[pcb->pc]();
        pcb->pc++;


        //如果阻塞，主动让出处理机
        if (pcb->state == SUSPEND){
            return;
        }

        if (i>=TIME_SLICE ) { // 时间片结束，切换到下一个进程
            pcb->state = READY;
            break;
        }

        if (pcb->pc >= pcb->num_instructions){
            pcb->state = TERMINATED;
            break;
        }
    }

    //进程没有执行完成，就返回就绪态，否则终止态
    if(pcb->pc >= pcb->num_instructions){
        pcb->state = TERMINATED;
        printf("进程%d结束运行\n\n", pcb->pid);
    } else{
        pcb->state = READY;
        printf("进程%d时间片到，返回就绪态\n\n", pcb->pid);
    }

    running_process = -1;
}


int main() {
    srand(1);
    int num_instructions;
    printf("请输入每个进程需要执行的指令数量：");
    scanf("%d", &num_instructions);
    printf("请输入进程数量：");
    scanf("%d", &num_processes);
    init_processes(num_processes, num_instructions);


    while (true){


        if (processes.empty()){
            break;
        }

        now_process = &processes.front();
        processes.pop_front();


        if(now_process->state == TERMINATED){
            continue;
        }

        run_process(now_process);

        to_run = (to_run+1)%num_processes;

        if(terminate_num >= num_processes){
            break;
        }

        processes.push_back(*now_process);

    }


    return 0;
}
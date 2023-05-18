#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "deque"

using namespace std;

#define MAX_PROCESS 10 // ����������
#define MAX_INSTRUCTION 10 // ÿ���������ָ������
#define TIME_SLICE 3 // ʱ��Ƭ����



// ����״̬
enum PROCESS_STATE {CREATE,READY, RUNNING,SUSPEND, TERMINATED};

// PCB�����̿��ƿ飩
struct PCB {
    int pid; // ����ID
    int priority; // �������ȼ�
    int state; // ����״̬
    int pc; // �����������ָ����һ����Ҫִ�е�ָ��
    int num_instructions; // ָ������
    void (*instructions[MAX_INSTRUCTION])(); // ָ����洢����ָ��
};

// ȫ�ֱ���
int num_processes; // ��������

deque<PCB> processes;

//struct PCB processes[MAX_PROCESS]; // PCB ����
int running_process = -1; // ��ǰ�������еĽ���
int to_run = 0; //Ҫ���еĽ���
struct PCB* now_process = nullptr; //��ǰ���еĳ���
int terminate_num = 0; //�Ѿ��������̵�����


// ����ָ���
void instr1() {
    printf("����%dִ��ָ��1\n", running_process);
}

void instr2() {
    printf("����%dִ��ָ��2\n", running_process);
}

int resource = 3;

//��ȡ��Դ
void getResource(){

    //�ȴ���ȡ��Դ����ǰ��������������pc--;
    if (resource<=0){
        now_process->state = SUSPEND;
        now_process->pc--;

        cout<<"��Դ��������������̬"<<endl;
    } else{
        cout<<"��ȡ��Դ"<<endl;
        resource--;
    }

}

//�ͷ���Դ
void releaseResource(){


    resource++;

    // ʹ�õ���������
    for (auto & processe : processes) {

        //�����һ������Ľ��̣�����Դ����
        if (processe.state == SUSPEND){
            processe.state = READY;
        }
    }

}



// ��ʼ������
void init_processes(int processes_num, int num_instructions) {
    // ������ɽ��̵����ȼ���ָ���
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

// ���н���
void run_process(struct PCB* pcb) {

    //���Ǿ���̬�Ͳ�����
    if (pcb->state != READY){
        return;
    }

    printf("����%d��ʼ����\n", pcb->pid);
    pcb->state = RUNNING;
    running_process = pcb->pid;
    // ִ��ָ�
    for (int i = 0;; i++) {


        pcb->instructions[pcb->pc]();
        pcb->pc++;


        //��������������ó������
        if (pcb->state == SUSPEND){
            return;
        }

        if (i>=TIME_SLICE ) { // ʱ��Ƭ�������л�����һ������
            pcb->state = READY;
            break;
        }

        if (pcb->pc >= pcb->num_instructions){
            pcb->state = TERMINATED;
            break;
        }
    }

    //����û��ִ����ɣ��ͷ��ؾ���̬��������ֹ̬
    if(pcb->pc >= pcb->num_instructions){
        pcb->state = TERMINATED;
        printf("����%d��������\n\n", pcb->pid);
    } else{
        pcb->state = READY;
        printf("����%dʱ��Ƭ�������ؾ���̬\n\n", pcb->pid);
    }

    running_process = -1;
}


int main() {
    srand(1);
    int num_instructions;
    printf("������ÿ��������Ҫִ�е�ָ��������");
    scanf("%d", &num_instructions);
    printf("���������������");
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
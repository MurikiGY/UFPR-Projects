// PingPongOS - PingPong Operating System
// Muriki Gusmao Yamanaka - GRR20203933
// Versão 1.4 -- Janeiro de 2022

// interface do gerente de disco rígido (block device driver)

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "ppos_data.h"
#include "ppos_disk.h"
#include "disk.h"
#include "ppos.h"
#include "queue.h"

//Gerenciamento do disco
task_t DiskManagerTask;             //Gerenciador de discos
task_t *DiskSuspendList = NULL;     //Lista de entreda/saida
semaphore_t s_disk;                 //Semaforo de acesso ao disco

//Sistema operacional
extern task_t *ReadyTaskList;       //Lista de tarefas prontas
extern task_t *CurrentTask;

//Sinal de disco
struct sigaction diskAction;

// Tratador de sinais =============================================================

//Procedimento de sinal SIGUSR1
void sigusr1_handler(int signum){
  //Proxima tarefa pode retornar a prontos
  DiskSuspendList->task_io.op_ready = 1;
  
  //Acorda gerenciador de disco
  task_resume(&DiskManagerTask, NULL);
}


// Gerenciamento de disco ==========================================================

//Gerenciador de sinais gerados pelo disco
//Apenas ativa quando o sinal SIGUSR1 for gerado
void diskDriverBody (void * args){
  while ( 1 ){
    sem_down(&s_disk);

    //Se a operacao de disco estiver pronta
    if ( DiskSuspendList->task_io.op_ready )
      //Acorda tarefa que gerou o sinal pedido
      task_resume(DiskSuspendList, &DiskSuspendList);
 
    //Se disco em standby e tem coisa pra fazer
    if ( (disk_cmd(DISK_CMD_STATUS,0,0) == 1) && (DiskSuspendList) )
      //Solicita ao disco a leitura/escrita da fila
      disk_cmd(DiskSuspendList->task_io.op_type,
               DiskSuspendList->task_io.block,
               DiskSuspendList->task_io.buffer);

    sem_up(&s_disk);
    DiskManagerTask.status = SUSPEND;
    queue_remove((queue_t **) &ReadyTaskList, (queue_t *) &DiskManagerTask);
    task_yield();
  }
}


// Operacoes de disco ==============================================================

// inicializacao do gerente de disco
// retorna -1 em erro ou 0 em sucesso
// numBlocks: tamanho do disco, em blocos
// blockSize: tamanho de cada bloco do disco, em bytes
int disk_mgr_init (int *numBlocks, int *blockSize){
  if ( disk_cmd(DISK_CMD_INIT, 0, 0) < 0 )
    return -1;

  //Carrega numero de blocos e tamanho do bloco
  *numBlocks = disk_cmd(DISK_CMD_DISKSIZE, 0, 0);
  if ( numBlocks < 0 )  return -1;
  *blockSize = disk_cmd(DISK_CMD_BLOCKSIZE, 0, 0);
  if ( blockSize < 0 )  return -1;

  //Inicializa tratador de SIGUSR1
  diskAction.sa_handler = sigusr1_handler;
  sigemptyset(&diskAction.sa_mask);
  diskAction.sa_flags = 0;
  if ( sigaction(SIGUSR1, &diskAction, 0) < 0 ){
    perror("Couldn't initialize diskAction: ");
    exit(1);
  }

  //Inicia tarefa do gerente de disco e bota para dormir
  task_init(&DiskManagerTask, diskDriverBody , NULL);
  DiskManagerTask.owner = SYSTEM;
  queue_remove((queue_t **) &ReadyTaskList, (queue_t *) &DiskManagerTask);
  DiskManagerTask.status = SUSPEND;

  //Inicia semaforo de acesso
  sem_init(&s_disk, 1);
  return 0;
}

// leitura de um bloco, do disco para o buffer
int disk_block_read (int block, void *buffer){
  //Obtem semaforo de disco
  sem_down(&s_disk);

  CurrentTask->task_io.block = block;
  CurrentTask->task_io.buffer = buffer;
  CurrentTask->task_io.op_type = DISK_CMD_READ;
  CurrentTask->task_io.op_ready = 0;

  //Acorda gerente se estiver dormindo
  if ( DiskManagerTask.status == SUSPEND )
    task_resume(&DiskManagerTask, NULL);

  //Libera semaforo
  sem_up(&s_disk);
  //Suspende tarefa atual para lista de disco
  task_suspend(&DiskSuspendList);
  return 0;
}

// escrita de um bloco, do buffer para o disco
int disk_block_write (int block, void *buffer){
  //Obtem semaforo de disco
  sem_down(&s_disk);

  CurrentTask->task_io.block = block;
  CurrentTask->task_io.buffer = buffer;
  CurrentTask->task_io.op_type = DISK_CMD_WRITE;
  CurrentTask->task_io.op_ready = 0;

  //Acorda gerente se estiver dormindo
  if ( DiskManagerTask.status == SUSPEND )
    task_resume(&DiskManagerTask, NULL);

  //Libera semaforo
  sem_up(&s_disk);
  //Suspende tarefa atual para lista de disco
  task_suspend(&DiskSuspendList);
  return 0;
}


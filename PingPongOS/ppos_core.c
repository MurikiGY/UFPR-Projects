// PingPongOS - PingPong Operating System
// GRR2020 - Muriki Gusmão Yamanaka
// Versão 1.5 -- Março de 2023

// Interface do núcleo para as aplicações
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/ucontext.h>

#include "ppos.h"
#include "ppos_data.h"
#include "ppos_disk.h"
#include "queue.h"

#define STACKSIZE 64*1024         //Tamanho da pilha da task
#define QUANTUM_TIME 1000         //Tempo em microsegundos de preempcao 1000 = 1ms
#define QUANTUM_TASK 20           //19 para 20 quantum de cada tarefa

task_t MainTask;                  //Contexto da main
task_t *CurrentTask;              //Contexto atual
task_t DispatcherTask;            //Gerenciador de tasks

//Listas de gerencia
task_t *ReadyTaskList = NULL;     //Lista de tarefas prontas
task_t *SleepTaskList = NULL;     //Lista de tarefas dormindo

//Disco
extern task_t *DiskSuspendList;

//Sinais de sistema
struct sigaction action;          //Tratador de sinais
struct itimerval timer;           //Temporizador de sinais

//Relógio do sistema
unsigned int exec_time;           //Variavel de tempo do sistema
int ID = 0;


// funções gerais ==============================================================

//Verifica se ha tarefa na lista
int queue_contain(task_t *task, task_t *queue){
  task_t *aux = queue;

  if ( !aux )  return 0;
  else do {
     if ( aux == task )
       return 1;
     aux = aux->next;
   } while (aux != queue);
  return 0;
}


// funções de sistema ==============================================================

// Sinais de sistema ===============================================================

//Procedimento de sinal SIGALRM
void sigalrm_handler(int signum){
  //Incrementa tempo de execução do sistema e da tarefa
  exec_time++;
  CurrentTask->process_time++;

  if ( CurrentTask->owner == USER ){
    if ( CurrentTask->quantum > 0)
      (CurrentTask->quantum)--;
    else 
      task_yield();
  }
}


// Gerenciamento de tarefas ========================================================

//Retorna a proxima tarefa a ser executata com envelhecimento
task_t *scheduler(){
  task_t *startTask = ReadyTaskList;
  task_t *returnTask = startTask;

  //Se fila nao vazia
  if ( startTask ){
    //Busca proxima tarefa de menor prioridade dinamica
    do {
      ReadyTaskList = ReadyTaskList->next;
      if ( ReadyTaskList->din_priority < returnTask->din_priority )
        returnTask = ReadyTaskList;
    } while ( ReadyTaskList != startTask );

    //Aumenta prioridade dinamica
    do {
      if ( ReadyTaskList != returnTask ) //Com excessao da que vai retornar
        ReadyTaskList->din_priority--;
      ReadyTaskList = ReadyTaskList->next;
    } while ( ReadyTaskList != startTask );

    //Restore the task priority
    returnTask->din_priority = returnTask->stat_priority;
  }

  return returnTask;
}


//Gerenciador das tarefas criadas pelo ususario
void dispatcher(void *arg){
  task_t *NextTask;

  //Equanto existirem tarefas
  while ( ReadyTaskList || SleepTaskList || DiskSuspendList ){
    //Verifica se ha tarefas dormindo
    if ( SleepTaskList ){
      int count = 0, list_size = queue_size((queue_t*)SleepTaskList);
      task_t *aux = SleepTaskList;
      while ( count < list_size ){
        if ( aux->next->sleepTime == exec_time )
          task_resume(aux->next, &SleepTaskList);
        aux = aux->next;
        count++; } }

    //Busca tarefa
    NextTask = scheduler();
    if ( NextTask ){
      NextTask->quantum = QUANTUM_TASK;
      task_switch( NextTask );

      //Tratamento de status do retorno da tarefa
      switch ( NextTask->status ){
        case READY: //Tarefa criada
          break;
        case SUSPEND: //Tarefa em execução
          break;
        case TERMINATED: //Tarefa terminada
          free(NextTask->context.uc_stack.ss_sp);
          break; }
    } }

  task_exit(0);
}


// Inicializacao do sistema ========================================================

//Procedimento de inicio da main
void main_init(task_t *task){
    getcontext(&task->context);
    task->next = NULL;
    task->prev = NULL;

    //Configura parametros
    task->id = ID++;
    task->status = READY;
    task->owner = USER;
    task->stat_priority = 0;
    task->din_priority = 0;
    task->quantum = QUANTUM_TASK;
    task->create_time = exec_time;
    task->end_time = 0;
    task->process_time = 0;
    task->activations = 0;
    task->SuspendTaskList = NULL;
    task->sleepTime = 0;
    
    //Insere main na lista de prontos
    queue_append((queue_t **) &ReadyTaskList, (queue_t *) task);
}


// Inicializa o sistema operacional; deve ser chamada no inicio do main()
void ppos_init (){
  // desativa o buffer da saida padrao (stdout), usado pela função printf
  setvbuf(stdout, 0, _IONBF, 0);

  //Inicializa Temporizador
  exec_time = 0;

  //Inicializa action do SIGALRM
  action.sa_handler = sigalrm_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  if ( sigaction(SIGALRM, &action, 0) < 0 ){
    perror("Couldn't initialize action: ");
    exit(1);
  }

  //Inicializa timer
  timer.it_value.tv_usec = QUANTUM_TIME;    //Primeiro disparo
  timer.it_value.tv_sec = 0;
  timer.it_interval.tv_usec = QUANTUM_TIME; //Proximo disparo
  timer.it_interval.tv_sec = 0;
  if (setitimer(ITIMER_REAL, &timer, 0) < 0 ){
    perror("Couldn't initialize timer: ");
    exit(1);
  }

  //Cria main
  main_init(&MainTask);

  //Cria dispatcher
  task_init(&DispatcherTask, dispatcher, NULL);
  DispatcherTask.owner = SYSTEM;
  queue_remove((queue_t **) &ReadyTaskList, (queue_t *) &DispatcherTask);

  CurrentTask = &MainTask;
  CurrentTask->activations++;
}


// gerência de tarefas =========================================================

// Inicializa uma nova tarefa. Retorna um ID> 0 ou erro.
int task_init (task_t *task, void  (*start_func)(void *), void   *arg){
  char *stack;

  getcontext(&task->context);

  stack = malloc(STACKSIZE);
  //Stack de funções e variaveis locais
  if ( stack ){
    //Salva contexto da tarefa
    task->context.uc_stack.ss_sp = stack;
    task->context.uc_stack.ss_size = STACKSIZE;
    task->context.uc_stack.ss_flags = 0;
    task->context.uc_link = 0;
    makecontext(&task->context, (void (*)(void))start_func, 1, arg);

    //Configura parametros
    task->id = ID++;
    task->status = READY;
    task->owner = USER;
    task->stat_priority = 0;
    task->din_priority = 0;
    task->quantum = QUANTUM_TASK;
    task->create_time = exec_time;
    task->end_time = 0;
    task->process_time = 0;
    task->activations = 0;
    task->SuspendTaskList = NULL;
    task->sleepTime = 0;

    //Insere na fila de prontos
    queue_append((queue_t **) &ReadyTaskList, (queue_t *) task);

    return task->id;
  } else
    return -1;
}


// retorna o identificador da tarefa corrente (main deve ser 0)
int task_id () {
  return CurrentTask->id;
}


// Termina a tarefa corrente com um valor de status de encerramento
void task_exit (int exit_code){
  //Atribui exit code
  CurrentTask->exit_code = exit_code;

  //Remove task atual da lista
  queue_remove((queue_t **) &ReadyTaskList, (queue_t *) CurrentTask);

  //Restaura tarefas dependentes
  while ( CurrentTask->SuspendTaskList )
    task_resume(CurrentTask->SuspendTaskList, &CurrentTask->SuspendTaskList);

  //Estatisticas
  CurrentTask->end_time = exec_time;
  printf("Task %d exit: execution time %5d ms, processor time %5d ms, %d activations\n",
        CurrentTask->id, CurrentTask->end_time - CurrentTask->create_time,
        CurrentTask->process_time, CurrentTask->activations);

  if ( CurrentTask == &DispatcherTask )
    exit(0);
  else
    task_switch(&DispatcherTask);
}


// alterna a execução para a tarefa indicada
int task_switch (task_t *task){
  ucontext_t *aux = &CurrentTask->context;
  
  task->activations++;
  if ( task ){
    CurrentTask = task;                 //Atualiza variavel de contexto atual
    swapcontext(aux, &task->context);   //Salva contexto atual e muda de context
    return 0;
  }
  return -1;
}


// suspende a tarefa atual,
// transferindo-a da fila de prontas para a fila "queue"
void task_suspend (task_t **queue){
  //Se a fila existe
  if ( queue ){
    //Remove de prontas, ajusta status, insere na suspensa
    queue_remove((queue_t **) &ReadyTaskList, (queue_t *) CurrentTask);
    CurrentTask->status = SUSPEND;
    queue_append((queue_t **) queue, (queue_t *) CurrentTask);
  } else  return;

  task_yield();
}


// acorda a tarefa indicada,
// trasferindo-a da fila "queue" para a fila de prontas
void task_resume (task_t *task, task_t **queue){
  //Remove de suspensa, ajusta status, insere lista de prontos
  queue_remove((queue_t **) queue, (queue_t *) task);
  task->status = READY;
  queue_append((queue_t **) &ReadyTaskList, (queue_t *) task);
}


// operações de escalonamento ==================================================


// a tarefa atual libera o processador para outra tarefa
void task_yield (){
  //Muda o contexto para o dispatcher
  task_switch(&DispatcherTask);
}


// define a prioridade estática de uma tarefa (ou a tarefa atual)
void task_setprio (task_t *task, int prio){
  if ( task ){
    task->stat_priority = prio;
    task->din_priority = prio;
  } else {
    CurrentTask->stat_priority = prio;
    CurrentTask->din_priority = prio;
  }
}


// retorna a prioridade estática de uma tarefa (ou a tarefa atual)
int task_getprio (task_t *task){
  if ( task )
    return task->stat_priority;
  else
    return CurrentTask->stat_priority;
}


// operações de gestão do tempo ================================================

// retorna o relógio atual (em milisegundos)
unsigned int systime (){
  return exec_time;
}

// suspende a tarefa corrente por t milissegundos
void task_sleep (int t){
  if ( t < 1)
    return;
  //Tira da fila de pronta e insere na fila de dormindo
  CurrentTask->sleepTime = exec_time + t;
  task_suspend(&SleepTaskList);
}

// operações de sincronização ==================================================

// a tarefa corrente aguarda o encerramento de outra task
int task_wait (task_t *task){
  //Suspende tarefa atual joga na lista de suspensas de "task"
  //Verifica se task existe em ReadyTaskList ou SleepTaskList
  if ( task && (queue_contain(task, ReadyTaskList) || 
                queue_contain(task, SleepTaskList) ||
                queue_contain(task, DiskSuspendList)))
    task_suspend(&task->SuspendTaskList);
  else
    return -1;
  return task->exit_code;
}

// inicializa um semáforo com valor inicial "value"
int sem_init (semaphore_t *s, int value){
  //Testa se endereco do ponteiro existe
  if ( !s )  return -1;
  else {
    s->counter = value;
    s->sem_lock = 0;
    s->sem_queue = NULL;
    s->destroyed = 0;
  }
  return 0;
}

// enter critical section
void enter_cs (int *lock){
  while (__sync_fetch_and_or (lock, 1)) ;   // busy waiting
}
 
// leave critical section
void leave_cs (int *lock){
  (*lock) = 0 ;
}

// requisita o semáforo
int sem_down (semaphore_t *s){
  if ( !s || s->destroyed )  return -1;

  enter_cs(&s->sem_lock);
  (s->counter)--;
  if ( s->counter < 0 ){
    leave_cs(&s->sem_lock);
    task_suspend(&s->sem_queue);
  }
  leave_cs(&s->sem_lock);

  return 0;
}

// libera o semáforo
int sem_up (semaphore_t *s){
  //Se semaforo existe
  if ( !s || s->destroyed )  return -1;

  enter_cs(&s->sem_lock);
  (s->counter)++;
  if ( s->counter <= 0 ){
    leave_cs(&s->sem_lock);
    task_resume(s->sem_queue, &s->sem_queue);  //Retorna prim task da fila
  }
  leave_cs(&s->sem_lock);

  return 0;
}

// "destroi" o semáforo, liberando as tarefas bloqueadas
int sem_destroy (semaphore_t *s){
  if ( !s )  return -1;
  else {
    enter_cs(&s->sem_lock);
    while ( s->sem_queue )
      task_resume(s->sem_queue, &s->sem_queue);
    s->counter = 0;
    s->destroyed = 1;
    leave_cs(&s->sem_lock);
  }

  return 0;
}

//// inicializa um mutex (sempre inicialmente livre)
//int mutex_init (mutex_t *m) ;
//
//// requisita o mutex
//int mutex_lock (mutex_t *m) ;
//
//// libera o mutex
//int mutex_unlock (mutex_t *m) ;
//
//// "destroi" o mutex, liberando as tarefas bloqueadas
//int mutex_destroy (mutex_t *m) ;
//
//// inicializa uma barreira para N tarefas
//int barrier_init (barrier_t *b, int N) ;
//
//// espera na barreira
//int barrier_wait (barrier_t *b) ;
//
//// destrói a barreira, liberando as tarefas
//int barrier_destroy (barrier_t *b) ;
//
//// operações de comunicação ====================================================

// inicializa uma fila para até max mensagens de size bytes cada
int mqueue_init (mqueue_t *queue, int max, int size){
  
  if ( !(queue->buffer = malloc( max * size )) )
    return -1;
  queue->b_size = max;    //tamanho maximo do buffer
  queue->t_size = size;   //Tamanho em bytes da mensagem
  queue->writeInd = 0;
  queue->readInd = 0;
    
  // cria semaforos
  sem_init (&queue->s_vaga, max);   //Buffer nao cheio
  sem_init (&queue->s_buffer, 1);   //Acesso ao buffer
  sem_init (&queue->s_item, 0);     //Tem algo pra consumir
  return 0;
}

// envia uma mensagem para a fila
int mqueue_send (mqueue_t *queue, void *msg){
  if ( !queue )
    return -1;

  if ( sem_down(&queue->s_vaga) < 0 )    return -1;
  if ( sem_down(&queue->s_buffer) < 0 )  return -1;

  //Insere no buffer circular
  memcpy(queue->buffer+(queue->writeInd*queue->t_size), msg, queue->t_size);
  queue->writeInd = (queue->writeInd+1) % queue->b_size;

  if ( sem_up(&queue->s_buffer) < 0 )    return -1;
  if ( sem_up(&queue->s_item) < 0 )      return -1;

  return 0;
}

// recebe uma mensagem da fila
int mqueue_recv (mqueue_t *queue, void *msg){
  if ( !queue )
    return -1;

  if ( sem_down(&queue->s_item) < 0 )    return -1;
  if ( sem_down(&queue->s_buffer) < 0 )  return -1;

  //Le do buffer circular
  memcpy(msg, queue->buffer+(queue->readInd*queue->t_size), queue->t_size);
  queue->readInd = (queue->readInd+1) % queue->b_size;

  if ( sem_up(&queue->s_buffer) < 0 )    return -1;
  if ( sem_up(&queue->s_vaga) < 0 )      return -1;

  return 0;
}

// destroi a fila, liberando as tarefas bloqueadas
int mqueue_destroy (mqueue_t *queue){
  if ( !queue )
    return -1;

  sem_destroy(&queue->s_item);
  sem_destroy(&queue->s_vaga);
  sem_destroy(&queue->s_buffer);

  free(queue->buffer);

  return 0;
}

// informa o número de mensagens atualmente na fila
int mqueue_msgs (mqueue_t *queue){
  if ( !queue )
    return -1;
  return queue->b_size;
}


// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DINF UFPR
// Versão 1.5 -- Março de 2023

// Estruturas de dados internas do sistema operacional

#ifndef __PPOS_DATA__
#define __PPOS_DATA__

#include <ucontext.h>		// biblioteca POSIX de trocas de contexto
#include "ppos_disk.h"

#define READY 1
#define SUSPEND 2
#define TERMINATED 3

#define SYSTEM 1
#define USER 2

// Estrutura que define um Task Control Block (TCB)
typedef struct task_t
{
  struct task_t *prev, *next;  // ponteiros para usar em filas
  int id;				               // identificador da tarefa
  ucontext_t context;		  	   // contexto armazenado da tarefa
  short status;			           // pronta, rodando, suspensa
  short owner;                 // Tarefa de sistema ou usuario
  short stat_priority;         // prioridade estatica
  short din_priority;          // prioridade dinamica
  short quantum;               // quantum de execucao
  unsigned int create_time;    // tempo de criacao
  unsigned int end_time;       // tempo de finalizacao
  unsigned int process_time;   // tempo usando processador
  unsigned int activations;    // numero de ativacoes
  unsigned int sleepTime;      // tempo da chamada sleep
  int exit_code;               // Codigo de termino da tarefa
  struct task_t *SuspendTaskList;
  disk_t task_io;              //Entrada e saida da tarefa
  // ... (outros campos serão adicionados mais tarde)
} task_t ;

// estrutura que define um semáforo
typedef struct
{
  int counter;                 //Contador do semaforo
  int sem_lock;                //Controle a area critica
  task_t *sem_queue;           //Fila de suspensos
  int destroyed;               //Booleano de semaforo destruido
} semaphore_t ;

// estrutura que define um mutex
typedef struct
{
  // preencher quando necessário
} mutex_t ;

// estrutura que define uma barreira
typedef struct
{
  // preencher quando necessário
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct
{
  void *buffer;               //Buffer de dados
  int b_size;                 //Tamanho do Buffer
  int t_size;                 //Tamanho da mensagem
  int writeInd;               //Index de escrita
  int readInd;                //Index de leitura
  semaphore_t s_vaga, s_buffer, s_item;
} mqueue_t ;

#endif


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ucontext.h>

#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/cthread.h"


int firstExec = 1;
int nroTID = 1;
int retThread = 0;


TCB_t *exec;
TCB_t *threadMain;

ucontext_t endThread, dispatch_ctx;

void initMain(){

	threadMain = (TCB_t*) malloc(sizeof(TCB_t));
		
	threadMain->tid = 0;	
	threadMain->prio = 0;	
	threadMain->state = PROCST_EXEC;	
	threadMain->tidCjoin = defaultTID;
	
	getcontext(&threadMain->context);
	exec = threadMain;
}

void initDispatch(){
	
	getcontext(&dispatch_ctx);

	dispatch_ctx.uc_link = 0;
	dispatch_ctx.uc_stack.ss_sp = (char*) malloc(stackSize);
	dispatch_ctx.uc_stack.ss_size = stackSize;

	makecontext(&dispatch_ctx, (void(*)(void))dispatch, 0);
}


void initFimThread(){

	getcontext(&endThread);

	endThread.uc_link = 0;
	endThread.uc_stack.ss_sp = (char*) malloc(stackSize);
	endThread.uc_stack.ss_size = stackSize;

	makecontext(&endThread, (void(*)(void))fimThread, 0);
}


int ccreate (void *(*start) (void*), void *arg, int zero){
	
	//primeira execucao cria contexto da main, do dispatcher;
	//inicializa filas e funcao que testa fim de thread 
	
	if(firstExec){
		
		initMain();
		initDispatch();
		initFimThread();
		

		//criando fila de aptos e bloqueados 

		if(CreateFila2(&aptos) != 0){
			printf("Nao criou a fila de aptos\n");
			return -1;
		}
		
		if(CreateFila2(&bloqueados) != 0){
			printf("Nao criou a fila de bloqueados\n");
			return -1;
		}	
		
		firstExec = 0;	//variavel que indica que nao eh mais a primeira execucao
	}

	
	//criando nova thread

	TCB_t *novaThread;
	novaThread = (TCB_t*) malloc(sizeof(TCB_t));

	novaThread->prio = zero;
	novaThread->tid = nroTID;
	novaThread->state = PROCST_APTO;
	novaThread->tidCjoin = defaultTID;
		
	getcontext(&(novaThread->context));

	novaThread->context.uc_link = &endThread;
	novaThread->context.uc_stack.ss_sp = (char*) malloc(stackSize);
	novaThread->context.uc_stack.ss_size = stackSize;
	
	makecontext(&(novaThread->context), (void (*) (void))start, 1, arg);

	
	//Inserindo na Fila de Aptos;

	if (Insert(&aptos, novaThread) == 0)
		printf("Nova Thread inserida na fila de Aptos\n");
	else{
		printf("Erro na insercao na fila de Aptos\n");
		return -1;
	}

	nroTID++;
	
	return novaThread->tid;

}

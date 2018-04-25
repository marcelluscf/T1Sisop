

/********************************************************************
	Vers. 17.2 - 11/09/2017
********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/cthread.h"

int	Insert(PFILA2 pfila, TCB_t *tcb) {
	// pfile vazia?
	if (FirstFila2(pfila)==0) {
		LastFila2(pfila);
		return InsertAfterIteratorFila2(pfila, tcb);
	}	
	return AppendFila2(pfila, (void *)tcb);
}



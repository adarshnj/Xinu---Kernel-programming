/* receive.c - receive */

#include <xinu.h>

/*------------------------------------------------------------------------
*  receive  -  Wait for a message and return the message to the caller
*------------------------------------------------------------------------
*/

umsg32	receive(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	umsg32	msg;			/* Message to return		*/

	mask = disable();
	prptr = &proctab[currpid];
	if (prptr->prhasmsg == FALSE) {
		prptr->prstate = PR_RECV;
		resched();		/* Block until message arrives	*/
	}
	msg = prptr->prmsg;		/* Retrieve message		*/
	prptr->prhasmsg = FALSE;	/* Reset message flag		*/
	restore(mask);
	return msg;
}


umsg32 receiveMsg(void)
{
	intmask mask;
	struct procent *proc;
	umsg32 msg;
	int32 i = 0;

	mask = disable();
	proc = &proctab[currpid];
	if (proc->count < 0) {
		return SYSERR;
		//	resched();
	}
	msg = proc->msgQ[0];
	proc->count = proc->count - 1;
	if (proc->count > 0)
	{
		for (i = 0; i < proc->count; i++)
		{
			proc->msgQ[i] = proc->msgQ[i + 1];
		}
	}

	restore(mask);
	return msg;
}

syscall receiveMsgs(umsg32* msgs, uint32 msg_count)
{
	int32 i = 0;

	for (i = 0; i < msg_count; i++)
	{
		msgs[i] = receiveMsg();
	}
}
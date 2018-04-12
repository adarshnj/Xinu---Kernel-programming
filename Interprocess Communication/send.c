/* send.c - send */

#include <xinu.h>

/*------------------------------------------------------------------------
*  send  -  Pass a message to a process and start recipient if waiting
*------------------------------------------------------------------------
*/
syscall	send(
	pid32		pid,		/* ID of recipient process	*/
	umsg32	msg		/* Contents of message		*/
)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	prptr = &proctab[pid];
	if (prptr->prhasmsg) {
		restore(mask);
		return SYSERR;
	}
	prptr->prmsg = msg;		/* Deliver message		*/
	prptr->prhasmsg = TRUE;		/* Indicate message is waiting	*/

								/* If recipient waiting or in timed-wait make it ready */

	if (prptr->prstate == PR_RECV) {
		ready(pid);
	}
	else if (prptr->prstate == PR_RECTIM) {
		unsleep(pid);
		ready(pid);
	}
	restore(mask);		/* Restore interrupts */
	return OK;
}


syscall sendMsg(pid32 pid, umsg32 msg)
{
	intmask mask;
	struct procent *proc;

	mask = disable();
	proc = &proctab[pid];
	if (!proc->count)
		proc->count = 0;
	if (proc->count >= 10)
	{
		restore(mask);
		return SYSERR;
	}

	proc->msgQ[proc->count] = msg;
	kprintf("process %d sent %s \n", currpid, proc->msgQ[proc->count]);
	proc->count = proc->count + 1;

	restore(mask);
	return OK;

}


uint32 sendMsgs(pid32 pid, umsg32* msgs, uint32 msg_count)
{
	int32 i, success = 0;
	umsg32 amsg;
	for (i = 0; i < msg_count; i++)
	{
		amsg = sendMsg(pid, msgs[i]);
		if (amsg == OK)
			success++;
		else
			return SYSERR;
	}

	return success;
}

uint32 sendnMsg(uint32 pid_count, pid32* pids, umsg32 msg)
{
	int i = 0, acount = 0;
	umsg32 pamsg;

	for (i = 0; i < pid_count; i++)
	{
		pamsg = sendMsg(pids[i], msg);
		if (pamsg == OK)
			acount++;
		else
			return SYSERR;
	}
	return acount;
}
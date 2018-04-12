#include<xinu.h>


pid32 sndr_1;
pid32 rcvr_1;
pid32 sndr_2;
pid32 rcvr_2;
pid32 sndr_3;
pid32 rcvr_3;

sid32 sem;


process sender_1()
{
	int32 sent;
	umsg32 msg;
	wait(sem);
	msg = sendMsg(rcvr_1, "Msg1");
	signal(sem);
}

process receiver_1()
{
	umsg32 msg;
	wait(sem);
	msg = receiveMsg();
	kprintf("PID (%d) received %s\n", currpid, msg);
	signal(sem);
}

process sender_2()
{
	int32 sent;
	int32 n = 2;
	umsg32 *msg[n];
	msg[0] = "Sender1_msg1";
	msg[1] = "Sender1_msg2";
	wait(sem);
	sent = sendMsgs(rcvr_2, msg, n);
	kprintf("sender_2 sent %d messages\n", sent);
	signal(sem);
}

process receiver_2()
{
	umsg32 *msgs[10];
	int32 N = 2, i = 0;

	wait(sem);
	receiveMsgs(msgs, 2);

	for (i = 0; i< N; i++)
		kprintf("PID (%d) received %s\n", currpid, msgs[i]);

	signal(sem);

}

process sender_3()
{
	umsg32 msg = "Common Message";
	int32 N;
	pid32 *pids[3];
	int32 pcount = 3;
	pids[0] = rcvr_1;
	pids[1] = rcvr_2;
	pids[2] = rcvr_3;

	wait(sem);
	N = sendnMsg(pcount, pids, msg);
	kprintf("sender_3 sent to %d pids\n", N);
	signal(sem);
}

process receiver_3()
{
	//umsg32 msgs[10];
	umsg32 msg;
	int32 N = 2, i = 0;

	wait(sem);

	msg = receiveMsg();

	kprintf("PID (%d) received %s\n", currpid, msg);

	signal(sem);
}

process main()
{
	recvclr();

	sem = semcreate(1);

	rcvr_1 = create(receiver_1, 1024, 20, "receiver_1", 0);
	rcvr_2 = create(receiver_2, 1024, 20, "receiver_2", 0);
	rcvr_3 = create(receiver_3, 1024, 20, "receiver_3", 0);

	sndr_1 = resume(create(sender_1, 1024, 20, "sender_1", 0));
	sndr_2 = resume(create(sender_2, 1024, 20, "sender_2", 0));
	sndr_3 = resume(create(sender_3, 1024, 20, "sender_3", 0));
	resume(rcvr_1);
	resume(rcvr_2);
	resume(rcvr_3);
}
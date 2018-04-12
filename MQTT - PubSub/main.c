#include<xinu.h>

struct Queue {
	int32 data;
	topic16 topic;
	struct Queue* prev;
	struct Queue* next;
};


struct tables
{
	int32 gcount;  // Number of Groups that exist
	int16 group[8]; // For translating topicid to internal group index
	int16 gdata[8];

	pid32* subscribers[8];
	//int32 scount;
	void(*handler[8])(topic16, uint32);

};

int32 qcount = 0;
struct Queue* curr;
struct Queue* first;
struct tables table[256];


syscall subscribe(topic16 topicd, void(*handler)(topic16, uint32)) {

	uint16 group = (int)topicd >> 8;
	uint16 topic = (int)topicd & 0xFF;

	printf("\nProcess %d subscribes to topic %d \n", currpid, topic);

	if (table[topic].gcount <0 && table[topic].gcount > 8)
	{
		table[topic].gcount = 0;
	}

	if (table[topic].gcount < 8) {
		int32 count = table[topic].gcount;
		table[topic].group[count] = group;
		table[topic].subscribers[count] = &currpid;
		table[topic].handler[count] = handler;

		table[topic].gcount++;

	}
	else {
		return SYSERR;
	}
	return OK;
}

syscall unsubscribe(topic16 topicd) {
	//uint16 group = (int)topicd >> 8;
	uint16 topic = (int)topicd & 0xFF;

	int32 i = 0;

	while (*table[topic].subscribers[i] != currpid) {
		i++;
	};

	if (i != table[topic].gcount) {
		for (; i < table[topic].gcount - 1; i++) {
			*table[topic].subscribers[i] = *table[topic].subscribers[i + 1];
			table[topic].handler[i] = table[topic].handler[i + 1];
			table[topic].group[i] = table[topic].group[i + 1];
			table[topic].gdata[i] = table[topic].gdata[i + 1];
		}
		table[topic].gcount--;
		printf("\nProcess %d unsubscribed from topic %d\n", currpid, topic);
	}
	else {
		return SYSERR;
	}
	return OK;
}


syscall publisher(topic16 topicd, uint32 data) {
	//uint16 group = (int)topicd >> 8;
	uint16 topic = (int)topicd & 0xFF;

	struct Queue *dup;
	struct Queue *pubs = getmem(sizeof(struct Queue));

	if (qcount == 0) {
		curr = &pubs;
		first = curr;
		curr->prev = NULL;
		curr->next = NULL;

		curr->data = data;
		curr->topic = topicd;

		qcount++;
	}
	else {
		curr->next = &pubs;
		dup = curr;
		curr = &pubs;

		curr->prev = dup;
		curr->next = NULL;

		curr->data = data;
		curr->topic = topicd;
		qcount++;
	}

	printf("\nProcess %d publishes %d to topic %d\n", currpid, data, topic);

	return OK;
}

sid32 sem;
pid32 publishe1;
pid32 publishe2;
pid32 subscribe1;
pid32 subscribe2;
pid32 Broke;

process Broker()  //topic16 topicd, uint32 data
{
	
	while (1)
	{
		wait(sem);
	
		int32 i;
		if (qcount > 0)
		{
			topic16 topicd = first->topic;

			uint16 group = (int)topicd >> 8;
			uint16 topic = (int)topicd & 0xFF;

			if (group == 0)
			{
				for (i = 0; i<table[topic].gcount; i++)
				{
					table[topic].gdata[i] = first->data;
					(table[topic].handler[i])(topic, first->data);
				}

				struct Queue *dup = first;
				first = first->next;
				first->prev = NULL;

				freemem(dup, sizeof(struct Queue));
				qcount--;

			}
			else
			{
				table[topic].gdata[group] = first->data;
				(table[topic].handler[group])(topic, first->data);
				printf("else not group0\n");
				struct Queue *dup = first;
				first = first->next;
				first->prev = NULL;

				freemem(dup, sizeof(struct Queue));
				qcount--;

			}

		}
		else
		{
			//printf("qcount = 0");
		}
		signal(sem);
	}
	
}

process publisher1()
{
	wait(sem);
	topic16 topic = 5, topic2 = 8;
	uint32 data = 70;
	publisher(topic, data);
	publisher(topic2, data);
	signal(sem);
}

process publisher2()
{
	wait(sem);
	printf("here");
	topic16 topic = 5, topic2 = 8;
	uint32 data = 98;
	publisher(topic, data);
	publisher(topic2, data);
	signal(sem);
}

void foo(topic16 topic, uint32 data)
{
	printf("\nfoo: topic %d data &d\n", topic, data);
	//return OK;
}

void bar(topic16 topic, uint32 data)
{
	printf("\nbar: topic %d data %d\n", topic, data);
	//return OK;
}


process subscriber1()
{
	wait(sem);
	topic16 topic = 5;
	subscribe(topic, *foo);
	unsubscribe(topic);
	signal(sem);

}

process subscriber2()
{
	wait(sem);
	topic16 topic = 5;
	subscribe(topic, *bar);
	signal(sem);
}

process main()
{
	recvclr();
	sem = semcreate(1);
	subscribe1 = create(subscriber1, 1024, 20, "subscriber1", 0);
	subscribe2 = create(subscriber2, 1024, 20, "subscriber2", 0);
	publishe1 = create(publisher1, 1024, 20, "publisher1", 0);
	publishe2 = create(publisher2, 1024, 20, "publisher2", 0);
	Broke = create(Broker, 1024, 20, "Broker", 0);
	resume(subscribe1);
	resume(subscribe2);
	resume(publishe1);
	resume(publishe2);
	resume(Broke);

}
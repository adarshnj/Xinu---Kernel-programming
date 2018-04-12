/*  main.c  - main */

#include <xinu.h>

pid32 producer_id;
pid32 consumer_id;

int32 consumed_count = 0;
const int32 CONSUMED_MAX = 100;

/* Define your circular buffer structure and semaphore variables here */
/* */
int32 queue[CONSUMED_MAX];
int32 count = 0;
sid32 sem;

/* Place your code for entering a critical section here */
void mutex_acquire(sid32 mutex)
{
	/* */
	if(count > 0 && count < CONSUMED_MAX)
	{
		wait(mutex);	
	}
	if (count < 0)
		kprintf("Empty Queue!\n");
	else
	{
		kprintf("Queue Full, Wait for Consumer");
		wait(mutex);
	}
}

/* Place your code for leaving a critical section here */
void mutex_release(sid32 mutex)
{
	/* */
	if(count < CONSUMED_MAX)
	{
		signal(mutex);
	}
	else
		kprintf("Queue Overflow!!\n");	
}

/* Place the code for the buffer producer here */
process producer(void)
{
	/* */
	while(consumed_count < 1000)
	{
		 if(count >= 0 && count < CONSUMED_MAX)
		{
			queue[count] = consumed_count;		
			kprintf("Produced %d\n",consumed_count);
			count++;
		}
		else
		{
			mutex_acquire(sem);
		}
		//mutex_release(sem);				
	}
	return OK;
}

/* Place the code for the buffer consumer here */
process consumer(void)
{
	/* Every time your consumer consumes another buffer element,
	 * make sure to include the statement:
	 *   consumed_count += 1;
	 * this will allow the timing function to record performance */
	/* */
	while(consumed_count < 1000)
	{
		if(count == 0)
		{
			mutex_acquire(sem);
		}
				
		kprintf("Consumed %d\n",queue[0]);
		consumed_count++;
		for(int i=0;i<count-1;i++)
		{
			queue[i]= queue[i+1];
		}
		queue[count--] = null;		
		if(count == CONSUMED_MAX)
		{
			mutex_release(sem);		
		}		
	}
	return OK;
}


/* Timing utility function - please ignore */
void time_and_end(void)
{
	int32 times[5];
	int32 i;

	for (i = 0; i < 5; ++i)
	{
		times[i] = clktime;
		while (consumed_count < CONSUMED_MAX * (i+1));
		times[i] = clktime - times[i];

		consumed_count = 0;
	}

	kill(producer_id);
	kill(consumer_id);

	for (i = 0; i < 5; ++i)
	{
		kprintf("TIME ELAPSED (%i): %i\n", (i+1) * CONSUMED_MAX, times[i]);
	}
}

process	main(void)
{
	recvclr();

	/* Create the shared circular buffer and semaphores here */
	/* */
	sem = semcreate(1);

	producer_id = create(producer, 4096, 50, "producer", 0);
	consumer_id = create(consumer, 4096, 50, "consumer", 0);
	resume(producer_id);
	resume(consumer_id);

	/* Uncomment the following line for part 3 to see timing results */
	/* time_and_end(); */

	return OK;
}

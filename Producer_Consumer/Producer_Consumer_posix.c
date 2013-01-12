#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define N 5
#define PN 2
#define CN 3

pthread_mutex_t mutex_count;
pthread_cond_t cond_produce;
pthread_cond_t cond_consume;

int count = 0;

void *produce(void *arg)
{
	srand((int)time(0));
	while (1)
	{
		pthread_mutex_lock(&mutex_count);
		while (N == count)
		{
			pthread_cond_wait(&cond_produce, &mutex_count);
		}
		count++;
		printf("producer %d: items remain %d\n", (int)arg, count);
		pthread_cond_signal(&cond_consume);
		pthread_mutex_unlock(&mutex_count);
		
		if (rand() % PN == (int)arg)
		{
			sleep(1);
		}
	}

	pthread_exit(EXIT_SUCCESS);
}

void *consume(void *arg)
{
	srand((int)time(0));
	while (1)
	{
		pthread_mutex_lock(&mutex_count);
		while (0 == count)
		{
			pthread_cond_wait(&cond_consume, &mutex_count);
		}
		count--;
		printf("consumer %d: items remain %d\n", (int)arg, count);
		pthread_cond_signal(&cond_produce);
		pthread_mutex_unlock(&mutex_count);

		if (rand() % CN == (int)arg)
		{
			sleep(1);
		}
	}

	pthread_exit(EXIT_SUCCESS);
}

int main()
{
	pthread_t producer[PN];
	pthread_t consumer[CN];
	pthread_mutex_init(&mutex_count, NULL);
	pthread_cond_init(&cond_produce, NULL);
	pthread_cond_init(&cond_consume, NULL);

	int i;
	for (i = 0; i != PN; ++i)
	{
		pthread_create(&producer[i], NULL, produce, (void *)i);
	}
	for (i = 0; i != CN; ++i)
	{
		pthread_create(&consumer[i], NULL, consume, (void *)i);
	}

	for (i = 0; i != PN; ++i)
	{
		pthread_join(producer[i], NULL);
	}
	for (i = 0; i != CN; ++i)
	{
		pthread_join(consumer[i], NULL);
	}

	pthread_mutex_destroy(&mutex_count);
	pthread_cond_destroy(&cond_produce);
	pthread_cond_destroy(&cond_consume);

	exit(EXIT_SUCCESS);
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N 3
int count = 0;
sem_t mutex;
sem_t full;
sem_t empty;
int flag = 1;

void *produce(void *arg)
{
	while (flag)
	{
		sem_wait(&empty);
		sem_wait(&mutex);
		count++;
		printf("produce--remain %d\n", count);
		sem_post(&mutex);
		sem_post(&full);
	}
	
	pthread_exit(EXIT_SUCCESS);
}

void *consume(void *arg)
{
	while (flag)
	{
		sem_wait(&full);
		sem_wait(&mutex);
		count--;
		printf("consumer--remain %d\n", count);
		sem_post(&mutex);
		sem_post(&empty);
	}

	pthread_exit(EXIT_SUCCESS);
}

int main()
{
	int res;
	pthread_t producer, consumer;
	void *thread_res;

	res = sem_init(&mutex, 0, 1);
	if (res != 0)
	{
		perror("sem_init() failed");
		exit(EXIT_FAILURE);
	}
	res = sem_init(&full, 0, 0);
	if (res != 0)
	{
		perror("sem_init() failed");
		exit(EXIT_FAILURE);
	}
	res = sem_init(&empty, 0, N);
	if (res != 0)
	{
		perror("sem_init() failed");
		exit(EXIT_FAILURE);
	}

	res = pthread_create(&producer, NULL, produce, thread_res);
	if (res != 0)
	{
		perror("pthread_create() failed");
		exit(EXIT_FAILURE);
	}
	res = pthread_create(&consumer, NULL, consume, thread_res);
	if (res != 0)
	{
		perror("pthread_create() failed");
		exit(EXIT_FAILURE);
	}

	sleep(1);
	flag = 0;

	pthread_join(producer, &thread_res);
	pthread_join(consumer, &thread_res);

	sem_destroy(&mutex);
	sem_destroy(&full);
	sem_destroy(&empty);

	exit(EXIT_SUCCESS);
}

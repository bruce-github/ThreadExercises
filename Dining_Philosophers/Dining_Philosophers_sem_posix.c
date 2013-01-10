#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define N 5
#define left(i) ((i) + N - 1) % N
#define right(i) ((i) + 1) % N
#define THINKING 0
#define HUNGRY 1
#define EATING 2

int state[N];
sem_t s[N];
sem_t mutex;
int flag = 1;

void test(int i)
{
	if (state[i] == HUNGRY && state[left(i)] != EATING && state[right(i)] != EATING)
	{
		state[i] = EATING;
		sem_post(&s[i]);
	}
}

void take_forks(int i)
{
	sem_wait(&mutex);
	state[i] = HUNGRY;
	test(i);
	sem_post(&mutex);
	sem_wait(&s[i]);
}

void put_forks(int i)
{
	sem_wait(&mutex);
	state[i] = THINKING;
	test(left(i));
	test(right(i));
	sem_post(&mutex);
}

void think(int i)
{
	printf("philosopher %d is thinking\n", i);
}

void eat(int i)
{
	sleep(1);
	printf("philosopher %d is eating\n", i);
}

void *philosopher(void *arg)
{
	int i = (int)arg;
	while (flag)
	{
	//	think(i);
		take_forks(i);
		eat(i);
		put_forks(i);
	}
	
	pthread_exit(EXIT_SUCCESS);
}

int main()
{
	if (sem_init(&mutex, 0, 1) != 0)
	{
		perror("sem_init failed");
		exit(EXIT_FAILURE);
	}
	int i;
	for (i = 0; i != N; ++i)
	{
		if (sem_init(&s[i], 0, 0) != 0)
		{
			perror("sem_init failed");
			exit(EXIT_FAILURE);
		}
	}

	pthread_t p[N];
	for (i = 0; i != N; ++i)
	{
		int res = pthread_create(&p[i], NULL, philosopher, (void *)i);
		if (res != 0)
		{
			perror("pthread_create failed");
			exit(EXIT_FAILURE);
		}
	}

	//sleep(1);
	//flag = 0;

	for (i = 0; i != N; ++i)
	{
		int res = pthread_join(p[i], NULL);
		if (res != 0)
		{
			perror("pthread_join failed");
			exit(EXIT_FAILURE);
		}
	}
	
	sem_destroy(&mutex);
	for (i = 0; i != N; ++i)
	{
		if (sem_destroy(&s[i]) != 0)
		{
			perror("sem_destroy failed");
			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}


















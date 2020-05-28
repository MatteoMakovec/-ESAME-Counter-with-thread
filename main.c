#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <errno.h>
#include <semaphore.h>
#include <pthread.h>


sem_t * process_semaphore;
int shared_counter;

// esercizio:
// nei due thread t1 e t2, tenere traccia del valore max e valore minimo assunto da shared_counter
// e scriverlo prima di terminare

#define CHECK_ERR(a,msg) {if ((a) == -1) { perror((msg)); exit(EXIT_FAILURE); } }
#define NUMBER_OF_CYCLES 10000000

void * thread_function_1(void * arg) {
	for (int i = 0; i < NUMBER_OF_CYCLES; i++) {
		if (sem_wait(process_semaphore) == -1) {
			perror("sem_wait");
			exit(EXIT_FAILURE);
		}

		shared_counter++;

		if (sem_post(process_semaphore) == -1) {
			perror("sem_post");
			exit(EXIT_FAILURE);
		}
	}

	return NULL;
}


void * thread_function_2(void * arg) {
	for (int i = 0; i < NUMBER_OF_CYCLES; i++) {
		if (sem_wait(process_semaphore) == -1) {
			perror("sem_wait");
			exit(EXIT_FAILURE);
		}

		shared_counter--;

		if (sem_post(process_semaphore) == -1) {
			perror("sem_post");
			exit(EXIT_FAILURE);
		}
	}

	return NULL;
}


int main(int argc, char * argv[]) {
	pthread_t t1;
	pthread_t t2;
	void * res;
	int s;

	printf("initial value of shared_counter = %d\nNUMBER_OF_CYCLES = %d\n\n", shared_counter, NUMBER_OF_CYCLES);

	process_semaphore = malloc(sizeof(sem_t));
	s = sem_init(process_semaphore,
					0, // 1 => il semaforo è condiviso tra processi,
					   // 0 => il semaforo è condiviso tra threads del processo
					1 // valore iniziale del semaforo (se mettiamo 0 che succede?)
				  );
	CHECK_ERR(s,"sem_init")

	s = pthread_create(&t1, NULL, thread_function_1, NULL);
	if (s != 0) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	s = pthread_create(&t2, NULL, thread_function_2, NULL);
	if (s != 0) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}


	s = pthread_join(t1, &res);
	if (s != 0) {
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}

	s = pthread_join(t2, &res);
	if (s != 0) {
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}

	printf("final value of shared_counter = %d\n", shared_counter);

	s = sem_destroy(process_semaphore);
	CHECK_ERR(s,"sem_destroy")

	return 0;
}
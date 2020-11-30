#include <pthread.h>
#include <stdio.h>

struct station {
	int waiting_queue;
	int number_of_passengers;
	int availabe_seats;
	pthread_mutex_t mutex;
	pthread_cond_t train_available;
	pthread_cond_t train_full;
	pthread_cond_t control;
	pthread_mutexattr_t mutex_attr;
	pthread_condattr_t cond_attr;
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);
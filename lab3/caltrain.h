#include <pthread.h>
#include <stdio.h>

struct station {
	int passengers_on_train; //Number of passengers on the train
	int number_of_passengers; //number of passengers in the station and not on the train
	int availabe_seats; //capacity of train
	int train_state; //Flag to indicate if train empty or full
	int tickets; //Flag to indicate if there is more tickets or all sold out
	pthread_mutex_t mutex;
	pthread_cond_t train_available;
	pthread_cond_t train_full;
	pthread_cond_t sold; 
	pthread_mutexattr_t mutex_attr;
	pthread_condattr_t cond_attr;
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);
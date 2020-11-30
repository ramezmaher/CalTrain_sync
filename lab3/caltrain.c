#include <pthread.h>
#include <stdio.h>
#include "caltrain.h"



void
station_init(struct station *station)
{
	station->availabe_seats = 0;  //Initialize number of available seats of the train, currently 0 because there is no train
	station->number_of_passengers = 0;  //Initialize counter of the number of passengers waiting for the train
	station->passengers_on_train=0;
	pthread_mutex_init(&(station->mutex),NULL);
	pthread_cond_init(&(station->train_accessible),NULL);
	pthread_cond_init(&(station->train_full),NULL);
}

void
station_load_train(struct station *station, int count)
{
	pthread_mutex_lock(&(station->mutex));
	if(count == 0 || station->number_of_passengers == 0){
		pthread_mutex_unlock(&(station->mutex));
		return;
	}
	station->availabe_seats = count;
	pthread_cond_broadcast(&(station->train_accessible));
	while(station->availabe_seats != 0){
		pthread_cond_wait(&(station->train_full),&(station->mutex));
	}
	return;
}
void
station_wait_for_train(struct station *station)
{   
	pthread_mutex_lock(&(station->mutex));
	station->number_of_passengers++;
	printf("passenger %d arrived\n",station->number_of_passengers);
	while(station->availabe_seats == 0){
		pthread_cond_wait(&(station->train_accessible),&(station->mutex));
	}
	station->number_of_passengers--;
	station->passengers_on_train++;
	pthread_mutex_unlock(&(station->mutex));
	return;
}

void
station_on_board(struct station *station)
{
	pthread_mutex_lock(&(station->mutex));
	station->passengers_on_train--;
	printf("%d passengers left\n",station->passengers_on_train);
	if(station->passengers_on_train == 0){
		station->availabe_seats=0;
		pthread_cond_signal(&(station->train_full));
	}
	pthread_mutex_unlock(&(station->mutex));
	return;
}

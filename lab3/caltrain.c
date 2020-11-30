#include <pthread.h>
#include <stdio.h>
#include "caltrain.h"



void
station_init(struct station *station)
{
	station->availabe_seats = 0;  //Initialize number of available seats of the train, currently 0 because there is no train
	station->number_of_passengers = 0;  //Initialize counter of the number of passengers waiting for the train
	station->passengers_on_train=0; //Initialize the number of passenger on the train to 0
	station->train_state=1; //Initialize train capacity to full (because if their is no train yet then the last one was full)
	station->passengers_state=0;
	pthread_mutex_init(&(station->mutex),NULL);
	pthread_cond_init(&(station->train_available),NULL);
	pthread_cond_init(&(station->sold),NULL);
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
	station->train_state=0;
	pthread_cond_broadcast(&(station->train_available));
	while(station->availabe_seats != 0){
		pthread_cond_wait(&(station->train_full),&(station->mutex));
	}
	pthread_mutex_unlock(&(station->mutex));
}

void
station_wait_for_train(struct station *station)
{   
	pthread_mutex_lock(&(station->mutex));
	station->number_of_passengers++;
	printf("passenger %d arrived\n",station->number_of_passengers);
	while(station->train_state){
		pthread_cond_wait(&(station->train_available),&(station->mutex));
	}
	station->number_of_passengers--;
	station->passengers_on_train++;
	if(station->availabe_seats == station->passengers_on_train){
		station->train_state=1;
		station->passengers_state=1;
		pthread_cond_broadcast(&(station->sold));
	}
	pthread_mutex_unlock(&(station->mutex));
}

void
station_on_board(struct station *station)
{
	pthread_mutex_lock(&(station->mutex));
	while(!station->train_state){
		pthread_cond_wait(&(station->sold),&(station->mutex));
	}
	station->availabe_seats--;
	printf("%d passengers left\n",station->availabe_seats);
	if(station->availabe_seats == 0){
		pthread_cond_signal(&(station->train_full));
	}
	pthread_mutex_unlock(&(station->mutex));
}

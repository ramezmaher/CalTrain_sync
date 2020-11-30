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
	station->tickets=0;
	pthread_mutex_init(&(station->mutex),NULL);
	pthread_cond_init(&(station->train_available),NULL);
	pthread_cond_init(&(station->sold),NULL);
	pthread_cond_init(&(station->train_full),NULL);
}

void
station_load_train(struct station *station, int count)
{	
	//Train enter station
	pthread_mutex_lock(&(station->mutex));
	if(count == 0 || station->number_of_passengers == 0){
		pthread_mutex_unlock(&(station->mutex));
		return;
	}
	station->availabe_seats = count;  
	station->train_state=0; 
	station->passengers_on_train=0; 
	station->tickets=0; 
	pthread_cond_broadcast(&(station->train_available)); //Notify all waiting passengers that train has arrived
	while(station->availabe_seats != 0){
		pthread_cond_wait(&(station->train_full),&(station->mutex)); //wait for everyone to get aboard
	}
	pthread_mutex_unlock(&(station->mutex)); //Train leaves
}

void
station_wait_for_train(struct station *station)
{   
	//passenger enters the station
	pthread_mutex_lock(&(station->mutex));
	station->number_of_passengers++;
	while(station->train_state){
		//he || she waits for the train 
		pthread_cond_wait(&(station->train_available),&(station->mutex));
	}
	//train arrived so they go to get their tickets
	station->number_of_passengers--;
	station->passengers_on_train++;
	if( station->availabe_seats == station->passengers_on_train || station->number_of_passengers == 0 ){
		//Their is no more tickets or no more passengers,hence train is full, and all possible tickets sold
		station->train_state=1;
		station->tickets=1;
		pthread_cond_broadcast(&(station->sold)); //Tell everyone who has ticket to get aboard
	}
	pthread_mutex_unlock(&(station->mutex)); //passneger leaves waiting hall
}

void
station_on_board(struct station *station)
{
	//passenger waiting for all possible tickets to be sold to get aboard
	pthread_mutex_lock(&(station->mutex));
	while(!station->train_state){
		pthread_cond_wait(&(station->sold),&(station->mutex));
	}
	//passenger gets on the train
	station->passengers_on_train--;
	if(station->passengers_on_train == 0){
		//no more passengers with tickets
		station->availabe_seats = 0;
		pthread_cond_signal(&(station->train_full)); // tell train to leave
	}
	pthread_mutex_unlock(&(station->mutex));
}

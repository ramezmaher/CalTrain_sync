#include <pthread.h>
#include <stdio.h>
#include "caltrain.h"



void
station_init(struct station *station)
{
	station->availabe_seats = 0;  //Initialize number of available seats of the train, currently 0 because there is no train
	station->number_of_passengers = 0;  //Initialize counter of the number of passengers waiting for the train
	station->waiting_queue = 0;  //Initialize the queue of waiting passengers to enter train

	//Set the attributes of the mutex to be shared between processes so that multiple cores could work simultaniously
	pthread_mutexattr_init(&station->mutex_attr);
	pthread_mutexattr_setpshared(&station->mutex_attr,PTHREAD_PROCESS_SHARED);
	//Initialize the mutex
	pthread_condattr_init(&station->cond_attr);

	//Set the attributes of the condition variables to be also shared between processes
	pthread_condattr_setpshared(&station->cond_attr,PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&station->mutex,&station->mutex_attr);
	//Initialize the condition variables
	pthread_cond_init(&station->train_available,&station->cond_attr);
	pthread_cond_init(&station->train_full,&station->cond_attr);
	pthread_cond_init(&station->control,&station->cond_attr);
}

void
station_load_train(struct station *station, int count)
{
	if(count == 0){
		printf("No seats\n");
		return;
	}
	station->availabe_seats = count;
	printf("Train arrived with %d seats\n",station->availabe_seats);
	//Critical section to check if their is any passengers
	//pthread_cond_wait(&station->control,&station->mutex);
	if(station->number_of_passengers == 0){
		printf("No passangers\n");
		pthread_cond_signal(&station->control);
		return;
	}

	else{
		//Train brodcast his arrival
		pthread_cond_broadcast(&station->train_available);
		pthread_cond_signal(&station->control);
	}
	//Critical section ended

	//Train waits until no seat left on it
	while(1){
		pthread_cond_wait(&station->train_full,&station->mutex);
		break;
	}
	return;
}

void
station_wait_for_train(struct station *station)
{
	//Passenger enters critical section to increment number of passengers
	station->number_of_passengers++;
	printf("Passenger arrived, %d passengers waiting\n",station->number_of_passengers);
	//Here he waits for the train to arrive, when it does all passnegers will wake up due to brodcast but only one will go at a time, he who gains control first
	pthread_cond_wait(&station->train_available,&station->mutex);

	//Now he will wait to gain control and enters the train,this is a critical section because the number of passengers should be decremented and the waiting queue to enter the train will increment
	pthread_cond_wait(&station->control,&station->mutex);
	station->number_of_passengers--;
	station->waiting_queue++;
	printf("Passenger left, %d seats left, %d passengers left\n",station->availabe_seats,station->number_of_passengers);
	pthread_cond_signal(&station->control);
	return;
}

void
station_on_board(struct station *station)
{
	pthread_cond_wait(&station->control,&station->mutex);

	while(station->availabe_seats == 0){
		pthread_cond_signal(&station->train_full);
		pthread_cond_signal(&station->control);
	}

	station->waiting_queue--;
	station->availabe_seats--;
	printf("Passenger on train, %d waiting queue, %d available seats\n", station->waiting_queue,station->availabe_seats);
	pthread_cond_signal(&station->control);	


}

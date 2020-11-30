#include <pthread.h>
#include "caltrain.h"



void
station_init(struct station *station)
{

	station->availabe_seats=0;  //Initialize number of available seats of the train, currently 0 because there is no train
	station->number_of_passengers=0;  //Initialize counter of the number of passengers waiting for the train

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
	
}

void
station_load_train(struct station *station, int count)
{
	// FILL ME IN
}

void
station_wait_for_train(struct station *station)
{
	// FILL ME IN
}

void
station_on_board(struct station *station)
{
	// FILL ME IN
}

/*
Code by: Or Yamin
Project: UID data structure
Date: 28/05/2024
Review by: Oded
Review date: 29/05/2024
Approved by: Oded
Approved date: 29/05/2024
*/

#include <unistd.h>  /* getpid */
#include <time.h>    /* time */
#include <stddef.h>  /* size_t */
#include <pthread.h> /* pthread_mutex_t pthread_mutex_lock pthread_mutex_unlock */

#include "uid.h"

const ilrd_uid_t BadUID = {0, 0, 0}; 

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static size_t counter = 0; 

ilrd_uid_t UIDCreate(void) 
{
	ilrd_uid_t new_uid = {0};
        
	new_uid.timestamp = time(NULL); 
	if (new_uid.timestamp == ((time_t)-1)) 
	{
		return BadUID;
    }
    
    pthread_mutex_lock(&mutex);
	new_uid.counter = ++counter;  
	pthread_mutex_unlock(&mutex);
	
	new_uid.pid = getpid();
	
	
	return new_uid;
}

int IsSameUID(const ilrd_uid_t uid1, const ilrd_uid_t uid2) 
{
    return (uid1.timestamp == uid2.timestamp &&
            uid1.counter == uid2.counter &&
            uid1.pid == uid2.pid);
}


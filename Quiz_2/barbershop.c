#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>

static pthread_mutex_t waitChairs[] = {
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER
};
static pthread_mutex_t barberChair = PTHREAD_MUTEX_INITIALIZER;

int chairs[3];
int queue[1000];
int queueCnt = 0;
int nowHiarcuttingNum = 0;

void *barberAction () {
	printf("Barber is sleeping\n");

	while (1 > 0) {
		int m = pthread_mutex_trylock(&barberChair);
		if (m == 0) {
			m = pthread_mutex_unlock(&barberChair);
		} else {
			printf("Customer wake up barber\n");
			printf("Barber is working\n\n");

			int r = rand() % 5 + 1;
			sleep(r);

			m = pthread_mutex_unlock(&barberChair);
			printf("Barber is sleeping\n\n");
		}
	}
	 
    pthread_exit(0);
}

void *newCustomerCome () {
    
	while (1 > 0) {
		
		printf("New customer has come\n");

		int cnt = 0;
		for (int i = 0; i < 3; i++) {
			int s = pthread_mutex_trylock(&waitChairs[i]);
			if (s == 0) {
				printf("New customer sat down on chair %d\n\n", i + 1);
				queue[queueCnt++] = i;
				break;
			} else {
				cnt++;
			}
		}

		if (cnt == 3) {
			printf("All chair are not available. Customer leaving\n\n");
		}

		sleep(1);
	}

    pthread_exit(0);
}

void *customerAction () {
    
	while (1 > 0) {
		
		if (nowHiarcuttingNum != queueCnt) {
			int s = pthread_mutex_trylock(&barberChair);
			if (s == 0) {
				int unlck = pthread_mutex_unlock(&waitChairs[queue[nowHiarcuttingNum]]);
				nowHiarcuttingNum++;
			}
		}
	}

    pthread_exit(0);
}

int main () {
    pthread_t barberThread, customerThread, newCustomerThread;

    for (int i = 0; i < 3; i++) {
    	chairs[i] = 0;
    }

   	if(pthread_create(&barberThread, 0, barberAction, NULL)) {
                fprintf(stderr, "Error creating barber thread\n");
                return 1;
    }

    if(pthread_create(&newCustomerThread, 0, newCustomerCome, NULL)) {
                fprintf(stderr, "Error creating newCustomerThread thread\n");
                return 1;
    }

    if(pthread_create(&customerThread, 0, customerAction, NULL)) {
                fprintf(stderr, "Error creating customerThread thread\n");
                return 1;
    }

     pthread_join(barberThread, NULL);
     pthread_join(customerThread, NULL);
     pthread_join(newCustomerThread, NULL);
}
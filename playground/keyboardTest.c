#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <linux/input.h>
#include <errno.h>
#include <stdbool.h> //bool is not a primivite data type in C
#include <pthread.h>

enum Key{UP,DOWN,LEFT,RIGHT,ESC};

static struct HWButton
{
	bool bPressed,bReleased,bHeld;
}keyboardState[5];

bool isLongPressed(struct timeval *_escapeDown, struct timeval *_evtime, float time)
{
	//printf("Calculated time difference: %ld\n", _evtime->tv_sec - _escapeDown->tv_sec );
	if( (_evtime->tv_sec - _escapeDown->tv_sec) > time )
		return true;
	else
		return false;
}

void *readKeyboard()
{
	int fd;
	const char *ptr = "/dev/input/event2";
	
	//struct HWButton keyboardState[5];

	//populate HWButton with keyboardState
	for(int i=0; 5 > i;i++)
	{
		keyboardState[i].bPressed = false;	
		keyboardState[i].bReleased = true;
		keyboardState[i].bHeld = false;
	}
	
	//printf("What is keyboardstate?: %d %d %d\n", keyboardState[0].bPressed,keyboardState[0].bPressed,keyboardState[0].bPressed);

	// (const char *)ptr - pass your device like "/dev/input/event2" here
	fd = open((const char *)ptr, O_RDONLY);

	if (fd < 0)
	{
		fprintf(stderr, "failed to open input device %s: %s\n", (const char *)ptr);
		return NULL;
	}

	struct timeval buttonTime[5] = { 0, 0};
	int code;
	struct input_event ev;
	while (1)
	{
		if (read(fd, &ev, sizeof(struct input_event)) < 0)
		{
		    fprintf(stderr, "failed to read input event from input device %s: %s\n", (const char *)ptr);
		    if (errno == EINTR)
		        continue;
		    break;
		}

		code = -1;
		if (ev.type == EV_KEY)
		{
		    switch (ev.code)
		    {
		    case 1: // KEY_ESC
		        if (ev.value == 1)
		        {
		            buttonTime[ESC] = ev.time;
		            keyboardState[ESC].bPressed = true;
					keyboardState[ESC].bHeld = false;
					keyboardState[ESC].bReleased = false;
					printf("ESC pressed : %d\n", keyboardState[ESC].bPressed);
		        }
		        else if (ev.value == 0 && buttonTime[ESC].tv_sec)
		        {
					keyboardState[ESC].bPressed = false;
					keyboardState[ESC].bHeld = false;
					keyboardState[ESC].bReleased = true;
		            printf("ESC released: %d\n", keyboardState[ESC].bReleased);
           			
					if (isLongPressed(&buttonTime[ESC], &ev.time,1))
		                printf("Long press of ESC\n");
					
		        }
				if (isLongPressed(&buttonTime[ESC], &ev.time, 0.5) && !keyboardState[ESC].bHeld && !keyboardState[ESC].bReleased)
				{				
					keyboardState[ESC].bPressed = false;
					keyboardState[ESC].bHeld = true;
					keyboardState[ESC].bReleased = false;	                
					printf("Holding of ESC\n");
				}
				
		        break;
		    //case eOk:

		    case 103: // KEY_UP
				if (ev.value == 1)
		        {
		            buttonTime[UP] = ev.time;
		            keyboardState[UP].bPressed = true;
					keyboardState[UP].bHeld = false;
					keyboardState[UP].bReleased = false;
					printf("UP pressed : %d\n", keyboardState[UP].bPressed);
		        }
		        else if (ev.value == 0 && buttonTime[UP].tv_sec)
		        {
					keyboardState[UP].bPressed = false;
					keyboardState[UP].bHeld = false;
					keyboardState[UP].bReleased = true;
		            printf("UP released: %d\n", keyboardState[UP].bReleased);
           			
					if (isLongPressed(&buttonTime[UP], &ev.time,1))
		                printf("Long press of UP\n");
					
		        }
				if (isLongPressed(&buttonTime[UP], &ev.time, 0.5) && !keyboardState[UP].bHeld && !keyboardState[UP].bReleased)
				{				
					keyboardState[UP].bPressed = false;
					keyboardState[UP].bHeld = true;
					keyboardState[UP].bReleased = false;	                
					printf("Holding of UP\n");
				}
				
		        break;

		    case 105: // KEY_LEFT
				if (ev.value == 1)
		        {
		            buttonTime[LEFT] = ev.time;
		            keyboardState[LEFT].bPressed = true;
					keyboardState[LEFT].bHeld = false;
					keyboardState[LEFT].bReleased = false;
					printf("LEFT pressed : %d\n", keyboardState[LEFT].bPressed);
		        }
		        else if (ev.value == 0 && buttonTime[LEFT].tv_sec)
		        {
					keyboardState[LEFT].bPressed = false;
					keyboardState[LEFT].bHeld = false;
					keyboardState[LEFT].bReleased = true;
		            printf("LEFT released: %d\n", keyboardState[LEFT].bReleased);
           			
					if (isLongPressed(&buttonTime[LEFT], &ev.time,1))
		                printf("Long press of LEFT\n");
					
		        }
				if (isLongPressed(&buttonTime[LEFT], &ev.time, 0.5) && !keyboardState[LEFT].bHeld && !keyboardState[LEFT].bReleased)
				{				
					keyboardState[LEFT].bPressed = false;
					keyboardState[LEFT].bHeld = true;
					keyboardState[LEFT].bReleased = false;	                
					printf("Holding of LEFT\n");
				}
				
		        break;

		    case 106: // KEY_RIGHT
				if (ev.value == 1)
		        {
		            buttonTime[RIGHT] = ev.time;
		            keyboardState[RIGHT].bPressed = true;
					keyboardState[RIGHT].bHeld = false;
					keyboardState[RIGHT].bReleased = false;
					printf("RIGHT pressed : %d\n", keyboardState[RIGHT].bPressed);
		        }
		        else if (ev.value == 0 && buttonTime[RIGHT].tv_sec)
		        {
					keyboardState[RIGHT].bPressed = false;
					keyboardState[RIGHT].bHeld = false;
					keyboardState[RIGHT].bReleased = true;
		            printf("RIGHT released: %d\n", keyboardState[RIGHT].bReleased);
           			
					if (isLongPressed(&buttonTime[RIGHT], &ev.time,1))
		                printf("Long press of RIGHT\n");
					
		        }
				if (isLongPressed(&buttonTime[RIGHT], &ev.time, 0.5) && !keyboardState[RIGHT].bHeld && !keyboardState[RIGHT].bReleased)
				{				
					keyboardState[RIGHT].bPressed = false;
					keyboardState[RIGHT].bHeld = true;
					keyboardState[RIGHT].bReleased = false;	                
					printf("Holding of RIGHT\n");
				}
				
		        break;
		    case 108: // KEY_DOWN
		        if (ev.value == 1)
		        {
		            buttonTime[DOWN] = ev.time;
		            keyboardState[DOWN].bPressed = true;
					keyboardState[DOWN].bHeld = false;
					keyboardState[DOWN].bReleased = false;
					printf("DOWN pressed : %d\n", keyboardState[DOWN].bPressed);
		        }
		        else if (ev.value == 0 && buttonTime[DOWN].tv_sec)
		        {
					keyboardState[DOWN].bPressed = false;
					keyboardState[DOWN].bHeld = false;
					keyboardState[DOWN].bReleased = true;
		            printf("DOWN released: %d\n", keyboardState[DOWN].bReleased);
           			
					if (isLongPressed(&buttonTime[DOWN], &ev.time,1))
		                printf("Long press of DOWN\n");
					
		        }
				if (isLongPressed(&buttonTime[DOWN], &ev.time, 0.5) && !keyboardState[DOWN].bHeld && !keyboardState[DOWN].bReleased)
				{				
					keyboardState[DOWN].bPressed = false;
					keyboardState[DOWN].bHeld = true;
					keyboardState[DOWN].bReleased = false;	                
					printf("Holding of DOWN\n");
				}
				
		        break;
		    }
		}
		    usleep(100);
	}

	close(fd);
}

int main(int argc, char** argv)
{
	int thread;
	pthread_t keyboardEventThread;

	if(thread=pthread_create( &keyboardEventThread, NULL, &readKeyboard, NULL))
		printf("Thread creation failed\n");	

	while(1)
	{
		//printf("Main is sleepy\n");
		sleep(2);
		for(int i=0; 5 > i;i++)
		{
			printf("Keyboard state is: %d %d %d \n",keyboardState[i].bPressed, keyboardState[i].bReleased, keyboardState[i].bHeld);
		}		
		if(keyboardState[ESC].bHeld == true)
		{
			printf("Closing\n");
			break;
		}
	}	

	return 0;
}

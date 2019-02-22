#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <drm/drm.h>
#include <drm/drm_mode.h>
#include <sys/time.h>
#include <linux/input.h>
#include <errno.h>
#include <stdbool.h> //bool is not a primivite data type in C
#include <pthread.h>

static struct drm_mode_card_res res={0};

static void *fb_base[10];
static long fb_w[10];
static long fb_h[10];

enum Key{UP,DOWN,LEFT,RIGHT,ESC};

static struct HWButton
{
	bool bPressed,bReleased,bHeld;
}keyboardState[5];


static struct point2D
{
	int x,y;
}point;

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
	const char *ptr = "/dev/input/event3";
	
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

    struct timeval buttonTime[5] = {{0, 0}};
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
					//printf("ESC pressed : %d\n", keyboardState[ESC].bPressed);
		        }
		        else if (ev.value == 0 && buttonTime[ESC].tv_sec)
		        {
					keyboardState[ESC].bPressed = false;
					keyboardState[ESC].bHeld = false;
					keyboardState[ESC].bReleased = true;
		            //printf("ESC released: %d\n", keyboardState[ESC].bReleased);
           			
					//if (isLongPressed(&buttonTime[ESC], &ev.time,1))
		                //printf("Long press of ESC\n");
					
		        }
				if (isLongPressed(&buttonTime[ESC], &ev.time, 0.5) && !keyboardState[ESC].bHeld && !keyboardState[ESC].bReleased)
				{				
					keyboardState[ESC].bPressed = false;
					keyboardState[ESC].bHeld = true;
					keyboardState[ESC].bReleased = false;	                
					//printf("Holding of ESC\n");
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
					//printf("UP pressed : %d\n", keyboardState[UP].bPressed);
		        }
		        else if (ev.value == 0 && buttonTime[UP].tv_sec)
		        {
					keyboardState[UP].bPressed = false;
					keyboardState[UP].bHeld = false;
					keyboardState[UP].bReleased = true;
		            //printf("UP released: %d\n", keyboardState[UP].bReleased);
           			
					//if (isLongPressed(&buttonTime[UP], &ev.time,1))
		                //printf("Long press of UP\n");
					
		        }
				if (isLongPressed(&buttonTime[UP], &ev.time, 0.5) && !keyboardState[UP].bHeld && !keyboardState[UP].bReleased)
				{				
					keyboardState[UP].bPressed = false;
					keyboardState[UP].bHeld = true;
					keyboardState[UP].bReleased = false;	                
					//printf("Holding of UP\n");
				}
				
		        break;

		    case 105: // KEY_LEFT
				if (ev.value == 1)
		        {
		            buttonTime[LEFT] = ev.time;
		            keyboardState[LEFT].bPressed = true;
					keyboardState[LEFT].bHeld = false;
					keyboardState[LEFT].bReleased = false;
					//printf("LEFT pressed : %d\n", keyboardState[LEFT].bPressed);
		        }
		        else if (ev.value == 0 && buttonTime[LEFT].tv_sec)
		        {
					keyboardState[LEFT].bPressed = false;
					keyboardState[LEFT].bHeld = false;
					keyboardState[LEFT].bReleased = true;
		            //printf("LEFT released: %d\n", keyboardState[LEFT].bReleased);
           			
					//if (isLongPressed(&buttonTime[LEFT], &ev.time,1))
		                //printf("Long press of LEFT\n");
					
		        }
				if (isLongPressed(&buttonTime[LEFT], &ev.time, 0.5) && !keyboardState[LEFT].bHeld && !keyboardState[LEFT].bReleased)
				{				
					keyboardState[LEFT].bPressed = false;
					keyboardState[LEFT].bHeld = true;
					keyboardState[LEFT].bReleased = false;	                
					//printf("Holding of LEFT\n");
				}
				
		        break;

		    case 106: // KEY_RIGHT
				if (ev.value == 1)
		        {
		            buttonTime[RIGHT] = ev.time;
		            keyboardState[RIGHT].bPressed = true;
					keyboardState[RIGHT].bHeld = false;
					keyboardState[RIGHT].bReleased = false;
					//printf("RIGHT pressed : %d\n", keyboardState[RIGHT].bPressed);
		        }
		        else if (ev.value == 0 && buttonTime[RIGHT].tv_sec)
		        {
					keyboardState[RIGHT].bPressed = false;
					keyboardState[RIGHT].bHeld = false;
					keyboardState[RIGHT].bReleased = true;
		            //printf("RIGHT released: %d\n", keyboardState[RIGHT].bReleased);
           			
					//if (isLongPressed(&buttonTime[RIGHT], &ev.time,1))
		                //printf("Long press of RIGHT\n");
					
		        }
				if (isLongPressed(&buttonTime[RIGHT], &ev.time, 0.5) && !keyboardState[RIGHT].bHeld && !keyboardState[RIGHT].bReleased)
				{				
					keyboardState[RIGHT].bPressed = false;
					keyboardState[RIGHT].bHeld = true;
					keyboardState[RIGHT].bReleased = false;	                
					//printf("Holding of RIGHT\n");
				}
				
		        break;
		    case 108: // KEY_DOWN
		        if (ev.value == 1)
		        {
		            buttonTime[DOWN] = ev.time;
		            keyboardState[DOWN].bPressed = true;
					keyboardState[DOWN].bHeld = false;
					keyboardState[DOWN].bReleased = false;
					//printf("DOWN pressed : %d\n", keyboardState[DOWN].bPressed);
		        }
		        else if (ev.value == 0 && buttonTime[DOWN].tv_sec)
		        {
					keyboardState[DOWN].bPressed = false;
					keyboardState[DOWN].bHeld = false;
					keyboardState[DOWN].bReleased = true;
		            //printf("DOWN released: %d\n", keyboardState[DOWN].bReleased);
           			
					//if (isLongPressed(&buttonTime[DOWN], &ev.time,1))
		                //printf("Long press of DOWN\n");
					
		        }
				if (isLongPressed(&buttonTime[DOWN], &ev.time, 0.5) && !keyboardState[DOWN].bHeld && !keyboardState[DOWN].bReleased)
				{				
					keyboardState[DOWN].bPressed = false;
					keyboardState[DOWN].bHeld = true;
					keyboardState[DOWN].bReleased = false;	                
					//printf("Holding of DOWN\n");
				}
				
		        break;
		    }
		}
		    usleep(10000);
	}

	close(fd);
}


void draw(int x, int y, int col){
	int j;
	for (j=0;j<res.count_connectors;j++)
	{
		int location=y*(fb_w[0]) + x;
		*(((uint32_t*)fb_base[0])+location)=col;	
	}
}

//Draw a circle at (cx,cy)
void draw_circle(double cx, double cy, int radius, uint32_t pixel)
{
	inline void plot4points(double cx, double cy, double x, double y, uint32_t pixel)
	{
		draw(cx + x, cy + y,pixel);
		draw(cx - x, cy + y,pixel);
		draw(cx + x, cy - y,pixel);
		draw(cx - x, cy - y,pixel);
	}

	inline void plot8points(double cx, double cy, double x, double y, uint32_t pixel)
	{
		plot4points(cx, cy, x, y,pixel);
		plot4points(cx, cy, y, x,pixel);
	}

	int error = -radius;
	double x = radius;
	double y = 0;

	while (x >= y)
	{
		plot8points(cx, cy, x, y, pixel);

		error += y;
		y++;
		error += y;

		if (error >= 0)
		{
			error += -x;
			x--;
			error += -x;
		}
	}
}

void refreshScreen()
{
	int x,y;
	int col=0x00000000;
		for (y=0;y<fb_h[0];y++)
			for (x=0;x<fb_w[0];x++)
				draw(x,y,col);
	col=0x00ffffff;
	draw_circle(point.x,point.y,40,col);
}

void initDRM()
{
//------------------------------------------------------------------------------
//Opening the DRI device
//------------------------------------------------------------------------------

	int dri_fd  = open("/dev/dri/card0",O_RDWR | O_CLOEXEC);

//------------------------------------------------------------------------------
//Kernel Mode Setting (KMS)
//------------------------------------------------------------------------------

	uint64_t res_fb_buf[10]={0},
			res_crtc_buf[10]={0},
			res_conn_buf[10]={0},
			res_enc_buf[10]={0};

	

	//Become the "master" of the DRI device
	ioctl(dri_fd, DRM_IOCTL_SET_MASTER, 0);

	//Get resource counts
	ioctl(dri_fd, DRM_IOCTL_MODE_GETRESOURCES, &res);
	res.fb_id_ptr=(uint64_t)res_fb_buf;
	res.crtc_id_ptr=(uint64_t)res_crtc_buf;
	res.connector_id_ptr=(uint64_t)res_conn_buf;
	res.encoder_id_ptr=(uint64_t)res_enc_buf;
	//Get resource IDs
	ioctl(dri_fd, DRM_IOCTL_MODE_GETRESOURCES, &res);

	printf("fb: %d, crtc: %d, conn: %d, enc: %d\n",res.count_fbs,res.count_crtcs,res.count_connectors,res.count_encoders);

	

	//Loop though all available connectors
	int i;
	for (i=0;i<res.count_connectors;i++)
	{
        struct drm_mode_modeinfo conn_mode_buf[20]={{0}};
		uint64_t	conn_prop_buf[20]={0},
					conn_propval_buf[20]={0},
					conn_enc_buf[20]={0};

		struct drm_mode_get_connector conn={0};

		conn.connector_id=res_conn_buf[i];

		ioctl(dri_fd, DRM_IOCTL_MODE_GETCONNECTOR, &conn);	//get connector resource counts
		conn.modes_ptr=(uint64_t)conn_mode_buf;
		conn.props_ptr=(uint64_t)conn_prop_buf;
		conn.prop_values_ptr=(uint64_t)conn_propval_buf;
		conn.encoders_ptr=(uint64_t)conn_enc_buf;
		ioctl(dri_fd, DRM_IOCTL_MODE_GETCONNECTOR, &conn);	//get connector resources

		//Check if the connector is OK to use (connected to something)
		if (conn.count_encoders<1 || conn.count_modes<1 || !conn.encoder_id || !conn.connection)
		{
			printf("Not connected\n");
			continue;
		}

//------------------------------------------------------------------------------
//Creating a dumb buffer
//------------------------------------------------------------------------------
		struct drm_mode_create_dumb create_dumb={0};
		struct drm_mode_map_dumb map_dumb={0};
		struct drm_mode_fb_cmd cmd_dumb={0};

		//If we create the buffer later, we can get the size of the screen first.
		//This must be a valid mode, so it's probably best to do this after we find
		//a valid crtc with modes.
		create_dumb.width = conn_mode_buf[0].hdisplay;
		create_dumb.height = conn_mode_buf[0].vdisplay;
		create_dumb.bpp = 32;
		create_dumb.flags = 0;
		create_dumb.pitch = 0;
		create_dumb.size = 0;
		create_dumb.handle = 0;
		ioctl(dri_fd, DRM_IOCTL_MODE_CREATE_DUMB, &create_dumb);

		cmd_dumb.width=create_dumb.width;
		cmd_dumb.height=create_dumb.height;
		cmd_dumb.bpp=create_dumb.bpp;
		cmd_dumb.pitch=create_dumb.pitch;
		cmd_dumb.depth=24;
		cmd_dumb.handle=create_dumb.handle;
		ioctl(dri_fd,DRM_IOCTL_MODE_ADDFB,&cmd_dumb);

		map_dumb.handle=create_dumb.handle;
		ioctl(dri_fd,DRM_IOCTL_MODE_MAP_DUMB,&map_dumb);

		fb_base[i] = mmap(0, create_dumb.size, PROT_READ | PROT_WRITE, MAP_SHARED, dri_fd, map_dumb.offset);
		fb_w[i]=create_dumb.width;
		fb_h[i]=create_dumb.height;

//------------------------------------------------------------------------------
//Kernel Mode Setting (KMS)
//------------------------------------------------------------------------------

		printf("%d : mode: %d, prop: %d, enc: %d\n",conn.connection,conn.count_modes,conn.count_props,conn.count_encoders);
		printf("modes: %dx%d FB: %d\n",conn_mode_buf[0].hdisplay,conn_mode_buf[0].vdisplay,fb_base[i]);

		struct drm_mode_get_encoder enc={0};

		enc.encoder_id=conn.encoder_id;
		ioctl(dri_fd, DRM_IOCTL_MODE_GETENCODER, &enc);	//get encoder

		struct drm_mode_crtc crtc={0};

		crtc.crtc_id=enc.crtc_id;
		ioctl(dri_fd, DRM_IOCTL_MODE_GETCRTC, &crtc);

		crtc.fb_id=cmd_dumb.fb_id;
		crtc.set_connectors_ptr=(uint64_t)&res_conn_buf[i];
		crtc.count_connectors=1;
		crtc.mode=conn_mode_buf[0];
		crtc.mode_valid=1;
		ioctl(dri_fd, DRM_IOCTL_MODE_SETCRTC, &crtc);
	}

	//Stop being the "master" of the DRI device
	ioctl(dri_fd, DRM_IOCTL_DROP_MASTER, 0);
}


int main()
{
	initDRM();

    pthread_t keyboardEventThread;
    int thread = pthread_create( &keyboardEventThread, NULL, &readKeyboard, NULL);
    if(thread)
		printf("Thread creation failed\n");	
	int x = fb_w[0]/2;
	int y = fb_h[0]/2;

	point.x = x;
	point.y = y;
	
	while(1)
	{
		if(keyboardState[LEFT].bHeld == true)
		{
			x--;
			//int col=(rand()%0x00ffffff)&0x00ff00ff;
			//draw_circle(x,y,40,col);
		}
		else if(keyboardState[RIGHT].bHeld == true)
		{
			x++;
			//int col=(rand()%0x00ffffff)&0x00ff00ff;
			//draw_circle(x,y,40,col);
		}
		else if(keyboardState[UP].bHeld == true)
		{
			y--;	
			//int col=(rand()%0x00ffffff)&0x00ff00ff;
			//draw_circle(x,y,40,col);
		}
		else if(keyboardState[DOWN].bHeld == true)
		{
			y++;
			//int col=(rand()%0x00ffffff)&0x00ff00ff;
			//draw_circle(x,y,40,col);
		}
		else if(keyboardState[ESC].bHeld == true)
		{
			break;
		}
		point.x = x;
		point.y = y;
		refreshScreen();
		//usleep(500);
	};	

	/*int fd, bytes;
    unsigned char data[3];

    const char *pDevice = "/dev/input/mice";

    // Open Mouse
    fd = open(pDevice, O_RDWR);
    if(fd == -1)
    {
        printf("ERROR Opening %s\n", pDevice);
        return -1;
    }

    int left, middle, right;
	int counter = 0;
    signed char xInput, yInput;
	int x,y;
    while(counter < 10)
    {
        // Read Mouse     
        bytes = read(fd, data, sizeof(data));

        if(bytes > 0)
        {
            left = data[0] & 0x1;
            right = data[0] & 0x2;
            middle = data[0] & 0x4;

            xInput = data[1];
            yInput = data[2];
            //printf("x=%d, y=%d, left=%d, middle=%d, right=%d\n", x, y, left, middle, right);
			if(left == 1) {
				counter++; 
				printf("Counter =%d\n",counter);
				int col=(rand()%0x00ffffff)&0x00ff00ff;
				draw_circle(fb_w[0]/2,fb_h[0]/2,40,col);
			}

        }   
	}
	
	/*
	for (int i=0;i<100;i++)
	{
		//int j;
		//for (j=0;j<res.count_connectors;j++)
		//{
		int col=(rand()%0x00ffffff)&0x00ff00ff;
			/*for (y=fb_h[j]/2;y<fb_h[j];y++)
				for (x=0;x<fb_w[j];x++)
					draw(x,y,col);
		draw_circle(fb_w[0]/2,fb_h[0]/2,40,col);
		//}
		usleep(100000);
	}
	*/

	return 0;
}

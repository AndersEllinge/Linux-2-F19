#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <drm/drm.h>
#include <drm/drm_mode.h>
#include <sys/time.h>
#include <linux/input.h>
#include <errno.h>
#include <stdbool.h> //bool is not a primivite data type in C
#include <pthread.h>

struct modeset_dev;
static int modeset_open(void);
static int modeset_prepare(int dri_fd);
static int modeset_create_fb(int dri_fd, struct modeset_dev *dev, struct drm_mode_modeinfo *conn_mode_buf);
static int modeset_setup_dev(int dri_fd, struct modeset_dev *dev, struct drm_mode_get_connector conn, struct drm_mode_card_res res, struct drm_mode_modeinfo *conn_mode_buf, uint64_t *conn_enc_buf, uint64_t *res_crtc_buf);
static int modeset_find_crtc(int dri_fd, struct modeset_dev *dev, struct drm_mode_get_connector conn, struct drm_mode_card_res res, uint64_t *conn_enc_buf, uint64_t *res_crtc_buf);


struct modeset_dev {
        struct modeset_dev *next;

        uint32_t width;
        uint32_t height;
        uint32_t stride;
        uint32_t size;
        uint32_t handle;
        uint8_t *map;

        struct drm_mode_modeinfo mode;
        uint32_t fb;
        uint32_t conn;
        uint32_t crtc;
        struct drm_mode_crtc *saved_crtc;
};

static struct modeset_dev *modeset_list = NULL;

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


/*void draw(int x, int y, int col){
    int j;
    for (j=0;j<res.count_connectors;j++)
    {
        int location=y*(fb_w[0]) + x;
        *(((uint32_t*)fb_base[0])+location)=col;
    }
}
*/

static uint8_t next_color(bool *up, uint8_t cur, unsigned int mod)
{
    uint8_t next;

    next = cur + (*up ? 1 : -1) * (rand() % mod);
    if ((*up && next < cur) || (!*up && next > cur)) {
        *up = !*up;
        next = cur;
    }

    return next;
}

static void draw2(uint8_t r, uint8_t g, uint8_t b, unsigned int x, unsigned int y)
{
    unsigned int off;
    struct modeset_dev *iter;

    for (iter = modeset_list; iter; iter = iter->next) {
        off = iter->stride * y + x * 4;
        *(uint32_t*)&iter->map[off] = (r << 16) | (g << 8) | b;
    }
    //usleep(100000);
}

/*
void draw2(int x, int y, int col){
    struct modeset_dev *iter;
    unsigned int off;
    printf("Starting to print\n");
    for (iter = modeset_list; iter ; iter = iter->next) {
        for (int j = 0; j < iter->height; ++j) {
            for (int k = 0; k < iter->width; ++k) {
                off = iter->stride * y + x *4;
                *(uint32_t*)&iter->map[off] = col;
            }
        }
    }
    printf("Printed once\n");
}
*/

//Draw a circle at (cx,cy)
void draw_circle(double cx, double cy, int radius, uint8_t r, uint8_t g, uint8_t b)
{
    inline void plot4points(double cx, double cy, double x, double y, uint8_t r, uint8_t g, uint8_t b)
    {
        draw2(r,g,b,cx + x, cy + y);
        draw2(r,g,b,cx - x, cy + y);
        draw2(r,g,b,cx + x, cy - y);
        draw2(r,g,b,cx - x, cy - y);
    }

    inline void plot8points(double cx, double cy, double x, double y, uint8_t r, uint8_t g, uint8_t b)
    {
        plot4points(cx, cy, x, y, r, g, b);
        plot4points(cx, cy, y, x, r, g, b);
    }

    int error = -radius;
    double x = radius;
    double y = 0;

    while (x >= y)
    {
        plot8points(cx, cy, x, y, r, g, b);

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
        for (y=0;y<modeset_list->height;y++)
            for (x=0;x<modeset_list->width;x++)
                draw2(0x00,0x00,0x00,x,y);
    draw_circle(point.x,point.y,40,0xff,0xff,0xff);
}


static int modeset_open(void){
    /* Linux Kernel detects a graphics-card on the machine with device driver ./drivers/gpu/drm/<xy>.
     * They are then controlled with /dev/dri/cardX
     * Make more fancy opening function, atm we hardcode it.
    */
    int fd = open("/dev/dri/card0",O_RDWR | O_CLOEXEC);

    return fd;
}

static int modeset_prepare(int dri_fd){

    /* We make more than enough space and assume it is more than enough for the the resources */
    uint64_t res_fb_buf[10]={0},
             res_crtc_buf[10]={0},
             res_conn_buf[10]={0},
             res_enc_buf[10]={0};
    int ret;
    /* res and conn are structs that hold information about SOMETHING */
    struct drm_mode_card_res res={0};

    //Construct the first modeset device
    struct modeset_dev *dev;

    //Become the "master" of the DRI device
    ioctl(dri_fd, DRM_IOCTL_SET_MASTER, 0);

    /* Get resource counts - AKA popularize the res struct */
    ret = ioctl(dri_fd, DRM_IOCTL_MODE_GETRESOURCES, &res);
    if(ret)
        printf("something went wrong\n");
    res.fb_id_ptr=(uint64_t)res_fb_buf; // framebuffer buffer
    res.crtc_id_ptr=(uint64_t)res_crtc_buf; // crtc buffer
    res.connector_id_ptr=(uint64_t)res_conn_buf; // connectors buffer
    res.encoder_id_ptr=(uint64_t)res_enc_buf; // encoder buffer
    //Get resource IDs
    ioctl(dri_fd, DRM_IOCTL_MODE_GETRESOURCES, &res);

    printf("fb: %d, crtc: %d, conn: %d, enc: %d\n",res.count_fbs,res.count_crtcs,res.count_connectors,res.count_encoders);

    /* Go through all connectors */
    for (int i = 0; i < res.count_connectors; ++i) {
        printf("trying connector %d out of %d\n",i+1,res.count_connectors);
        /* Getting information about each connector */
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

        /* create a device structure */
        dev = malloc(sizeof(*dev));
        memset(dev, 0, sizeof(*dev));
        dev->conn = conn.connector_id;

        /* prepare the connector */


        ret = modeset_setup_dev(dri_fd, dev, conn, res, conn_mode_buf, conn_enc_buf, res_crtc_buf);
        if(ret){
            free(dev);
            continue;
        }
        dev->next = modeset_list;
        modeset_list = dev;
    }

    return 0;
}

int modeset_setup_dev(int dri_fd,struct modeset_dev *dev, struct drm_mode_get_connector conn, struct drm_mode_card_res res, struct drm_mode_modeinfo *conn_mode_buf, uint64_t *conn_enc_buf, uint64_t *res_crtc_buf){

    int ret;
    /* check if monitor is connected */
    if(!conn.connection){ // returns 1 for connected
        printf("Ignoring unused connector: %u\n",conn.connector_id);
        return 1;
    }

    /* check for valid modes */
    if(conn.count_modes < 1){
        printf("No valid connector for: %u\n",conn.connector_id);
        return 1;
    }

    /* retrive information into the device structure */
    memcpy(&dev->mode, &conn_mode_buf[0], sizeof(dev->mode));
    dev->width = conn_mode_buf[0].hdisplay;
    dev->height = conn_mode_buf[0].vdisplay;
    printf("Mode for connector %u is %ux%u\n",conn.connector_id, dev->width,dev->height);
    /* Find a CRTC for this connector */
    ret = modeset_find_crtc(dri_fd, dev, conn, res,conn_enc_buf,res_crtc_buf);
    if (ret){
        printf("No valid crtc for connector %u\n",conn.connector_id);
        return ret;
    }

    /* create a framebuffer for this CRTC */
    ret = modeset_create_fb(dri_fd, dev, conn_mode_buf);
    if(ret){
        printf("Cannot create framebuffer for connector %u\n",conn.connector_id);
        return ret;
    }

    return 0;

}

int modeset_find_crtc(int dri_fd, struct modeset_dev *dev, struct drm_mode_get_connector conn, struct drm_mode_card_res res, uint64_t *conn_enc_buf, uint64_t *res_crtc_buf){
    /*
     * Encoders help the CRTC to convert data from a framebuffer
     * into the right fromat that can be used for the connector
     * -- REMEMBER -- we can only drive one connector pr. CRTC!
    */

    struct drm_mode_get_encoder enc={0};
    int32_t crtc;
    struct modeset_dev *iter;
    int ret;


    /* Frist try the current connected encoder and crtc */
    if(conn.encoder_id){
        enc.encoder_id=conn.encoder_id;
        ret = ioctl(dri_fd, DRM_IOCTL_MODE_GETENCODER, &enc);	//get encoder
        if (!ret){ // if succesfull
            if(enc.crtc_id){
                crtc = enc.crtc_id;
                for (iter = modeset_list; iter ; iter=iter->next) {
                    if(iter->crtc == crtc){
                        crtc = -1;
                        break;
                    }
                }
                if(crtc >= 0) {
                    dev->crtc = crtc;
                    return 0;
                }
            }
        }
    }


    else {
        for (int i = 0; i < conn.count_encoders; ++i) {
            enc.encoder_id=conn_enc_buf[i];
            ret = ioctl(dri_fd, DRM_IOCTL_MODE_GETENCODER, &enc);	//get encoder
            if(!ret){ // if not succesfull
                printf("Cannot retrive encoder %u:%u\n",i,conn_enc_buf[i]);
                continue;
            }


            for (int j = 0; res.count_crtcs ; ++j) {
                /* check if CRTC work with encoder */
                if (!(enc.possible_crtcs & (1 << j)))
                    continue;

                /* check that no other decive uses this CRTC */
                crtc = res_crtc_buf[j];
                for (iter = modeset_list; iter ; iter->next) {
                    if (iter->crtc == crtc){
                        crtc = -1;
                        break;
                    }
                }

                /* We found a CRTC, save and return */
                if(crtc >= 0){
                    dev->crtc =crtc;
                    return 0;
                }
            }
        }

    }
    //printf("Cannot find suitable CRTC for connector: %u\n",conn.connector_id);
    return 1;
}

int modeset_create_fb(int dri_fd, struct modeset_dev *dev, struct drm_mode_modeinfo *conn_mode_buf){
    //------------------------------------------------------------------------------
    //Creating a dumb buffer
    //------------------------------------------------------------------------------
    struct drm_mode_create_dumb create_dumb={0};
    struct drm_mode_map_dumb map_dumb={0};
    struct drm_mode_fb_cmd cmd_dumb={0};
    struct drm_mode_destroy_dumb destroy_dumb={0};
    int ret;
    create_dumb.width = conn_mode_buf[0].hdisplay;
    create_dumb.height = conn_mode_buf[0].vdisplay;
    create_dumb.bpp = 32;
    create_dumb.flags = 0;
    create_dumb.pitch = 0;
    create_dumb.size = 0;
    create_dumb.handle = 0;
    ret = ioctl(dri_fd, DRM_IOCTL_MODE_CREATE_DUMB, &create_dumb);
    if(ret){
        printf("Cannot create dumb buffer\n");
        return ret;
    }
    dev->stride = create_dumb.pitch;
    dev->size = create_dumb.size;
    dev->handle = create_dumb.handle;

    /* Create FB object for the dumb buffer */
    cmd_dumb.width=create_dumb.width;
    cmd_dumb.height=create_dumb.height;
    cmd_dumb.bpp=create_dumb.bpp;
    cmd_dumb.pitch=create_dumb.pitch;
    cmd_dumb.depth=24;
    cmd_dumb.handle=create_dumb.handle;
    ret = ioctl(dri_fd,DRM_IOCTL_MODE_ADDFB,&cmd_dumb);
    if(ret){
        printf("Cannot create framebuffer\n");
        memset(&destroy_dumb, 0, sizeof (destroy_dumb));
        destroy_dumb.handle = dev->handle;
        ioctl(dri_fd,DRM_IOCTL_MODE_DESTROY_DUMB, &destroy_dumb);
        return ret;
    }
    dev->fb = cmd_dumb.fb_id;

    /* Prepare buffer for memory map */
    map_dumb.handle=dev->handle;
    ret = ioctl(dri_fd,DRM_IOCTL_MODE_MAP_DUMB,&map_dumb);
    if(ret){
        printf("Cannot map dumb buffer\n");
        return ret;
    }

    /* Do mapping */
    dev->map = mmap(0, create_dumb.size, PROT_READ | PROT_WRITE, MAP_SHARED, dri_fd, map_dumb.offset);
    if(dev->map == MAP_FAILED){
        printf("Cannot mmap dumb buffer\n");
        ioctl(dri_fd,DRM_IOCTL_MODE_RMFB, dev->fb);
        return ret;
    }
    //fb_base[i] = mmap(0, create_dumb.size, PROT_READ | PROT_WRITE, MAP_SHARED, dri_fd, map_dumb.offset);
    //fb_w[i]=create_dumb.width;
    //fb_h[i]=create_dumb.height;

    /* clear the FB to 0 */
    memset(dev->map, 0, dev->size);

    return 0;
}

int main()
{


    int ret, dri_fd;
    struct modeset_dev *iter;

    dri_fd = modeset_open();

    ret = modeset_prepare(dri_fd);

    if(ret){
        close(ret);
        return 1;
    }

    for (iter=modeset_list; iter!=NULL ; iter = iter->next) {
        struct drm_mode_crtc crtc={0};
        crtc.crtc_id=iter->crtc;
        ioctl(dri_fd, DRM_IOCTL_MODE_GETCRTC, &crtc);
        iter->saved_crtc = &crtc;
        crtc.fb_id=iter->fb;
        crtc.set_connectors_ptr=(uint64_t)&iter->conn;
        crtc.count_connectors=1;
        crtc.mode=iter->mode;
        crtc.mode_valid=1;
        ret = ioctl(dri_fd, DRM_IOCTL_MODE_SETCRTC, &crtc);
        if (ret){
            printf("Cannot set CRTC for connector %u \n",iter->conn);
        }
    }

    pthread_t keyboardEventThread;
    int thread = pthread_create( &keyboardEventThread, NULL, &readKeyboard, NULL);
    if(thread)
        printf("Thread creation failed\n");
    int x = modeset_list->width/2;
    int y = modeset_list->height/2;

    point.x = x;
    point.y = y;
    printf("We are starting to show stuff now\n");
    uint8_t r, g, b;

    while(1)
    {
        srand(time(NULL));
        r = rand() % 0xff;
        g = rand() % 0xff;
        b = rand() % 0xff;
        if(keyboardState[LEFT].bHeld == true)
        {
            x--;
            //draw_circle(x,y,40,r,g,b);
        }
        else if(keyboardState[RIGHT].bHeld == true)
        {
            x++;
            //draw_circle(x,y,40,r,g,b);
        }
        else if(keyboardState[UP].bHeld == true)
        {
            y--;
            //draw_circle(x,y,40,r,g,b);
        }
        else if(keyboardState[DOWN].bHeld == true)
        {
            y++;
            //draw_circle(x,y,40,r,g,b);
        }
        else if(keyboardState[ESC].bHeld == true)
        {
            break;
        }
        point.x = x;
        point.y = y;
        refreshScreen();
        //usleep(5000);
    };

    //Stop being the "master" of the DRI device
    ioctl(dri_fd, DRM_IOCTL_DROP_MASTER, 0);


    return 0;
}

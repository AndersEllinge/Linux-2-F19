#include "drmImpl.h"


extern uint8_t next_color(bool *up, uint8_t cur, unsigned int mod)
{
    uint8_t next;

    next = cur + (*up ? 1 : -1) * (rand() % mod);
    if ((*up && next < cur) || (!*up && next > cur)) {
        *up = !*up;
        next = cur;
    }

    return next;
}

extern void draw2(int dri_fd, struct modeset_buf *buf, uint8_t r, uint8_t g, uint8_t b, unsigned int x, unsigned int y)
{
    *(uint32_t*)&buf->map[buf->stride * y + x * 4] = (r << 16) | (g << 8) | b;
}


//Draw a circle at (cx,cy)
void draw_circle(int dri_fd, struct modeset_buf *buf,double cx, double cy, int radius, uint8_t r, uint8_t g, uint8_t b)
{
    inline void plot4points(double cx, double cy, double x, double y, uint8_t r, uint8_t g, uint8_t b)
    {
        draw2(dri_fd,buf, r,g,b,cx + x, cy + y);
        draw2(dri_fd,buf, r,g,b,cx - x, cy + y);
        draw2(dri_fd,buf, r,g,b,cx + x, cy - y);
        draw2(dri_fd,buf, r,g,b,cx - x, cy - y);
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

extern void writeToBuf(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    struct modeset_dev *iter = modeset_list;
    *(uint32_t*)&iter->bufs[iter->front_buf ^ 1].map[iter->bufs[iter->front_buf ^ 1].stride * y + x*4] = (r << 16) | (g << 8) | b;
}

extern void refresh(int dri_fd)
{
    int ret;
    struct modeset_dev *iter = modeset_list;
    struct modeset_buf *buf;

    buf = &iter->bufs[iter->front_buf ^ 1];

    //draw2(dri_fd,buf,r,g,b,x,y);

    struct drm_mode_crtc crtc={0};
    crtc.crtc_id=iter->crtc;
    ioctl(dri_fd, DRM_IOCTL_MODE_GETCRTC, &crtc);
    crtc.fb_id=buf->fb;
    crtc.set_connectors_ptr=(uint64_t)&iter->conn;
    crtc.count_connectors=1;
    crtc.mode=iter->mode;
    crtc.mode_valid=1;
    ret = ioctl(dri_fd, DRM_IOCTL_MODE_SETCRTC, &crtc);
    if (ret){
        printf("DRAW:Cannot set CRTC for connector %u \n",iter->conn);
    }
    else {
        iter->front_buf ^= 1;
    }
}

extern void refreshScreenCircle(int dri_fd,uint8_t r, uint8_t g,uint8_t b)
{
    int ret;
    struct modeset_dev *iter = modeset_list;
    struct modeset_buf *buf;
    int x,y;
    buf = &iter->bufs[iter->front_buf ^ 1];

    draw_circle(dri_fd,buf,iter->bufs->width/2,iter->bufs->height/2,40,r,g,b);

    struct drm_mode_crtc crtc={0};
    crtc.crtc_id=iter->crtc;
    ioctl(dri_fd, DRM_IOCTL_MODE_GETCRTC, &crtc);
    crtc.fb_id=buf->fb;
    crtc.set_connectors_ptr=(uint64_t)&iter->conn;
    crtc.count_connectors=1;
    crtc.mode=iter->mode;
    crtc.mode_valid=1;
    ret = ioctl(dri_fd, DRM_IOCTL_MODE_SETCRTC, &crtc);
    if (ret){
        printf("DRAW:Cannot set CRTC for connector %u \n",iter->conn);
    }
    else {
        iter->front_buf ^= 1;
    }
}

extern void refreshScreenColor(int dri_fd,uint8_t r, uint8_t g,uint8_t b, bool r_up, bool g_up, bool b_up)
{

    r = next_color(&r_up, r, 20);
    g = next_color(&g_up, g, 10);
    b = next_color(&b_up, b, 5);

    int ret;
    struct modeset_dev *iter = modeset_list;
    struct modeset_buf *buf;
    int x,y;

    //for (iter = modeset_list; iter!=NULL; iter = iter->next) {
        //printf("count %\n",modeset_list->bufs[iter->front_buf ^ 1].height);
        buf = &iter->bufs[iter->front_buf ^ 1];

        for ( y = 0; y < modeset_list->bufs[iter->front_buf ^ 1].height; y++)
            for ( x = 0; x < modeset_list->bufs[iter->front_buf ^ 1].width; x++)
                draw2(dri_fd,buf,r,g,b,x,y);

        //draw_circle(dri_fd,point.x,point.y,40,0xff,0xff,0xff);

        struct drm_mode_crtc crtc={0};
        crtc.crtc_id=iter->crtc;
        ioctl(dri_fd, DRM_IOCTL_MODE_GETCRTC, &crtc);
        crtc.fb_id=buf->fb;
        crtc.set_connectors_ptr=(uint64_t)&iter->conn;
        crtc.count_connectors=1;
        crtc.mode=iter->mode;
        crtc.mode_valid=1;
        ret = ioctl(dri_fd, DRM_IOCTL_MODE_SETCRTC, &crtc);
        if (ret){
            printf("DRAW:Cannot set CRTC for connector %u \n",iter->conn);
        }
        else {
            iter->front_buf ^= 1;
        }

   //}
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

static int modeset_setup_dev(int dri_fd,struct modeset_dev *dev, struct drm_mode_get_connector conn, struct drm_mode_card_res res, struct drm_mode_modeinfo *conn_mode_buf, uint64_t *conn_enc_buf, uint64_t *res_crtc_buf){

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
    dev->bufs[0].width = conn_mode_buf[0].hdisplay;
    dev->bufs[0].height = conn_mode_buf[0].vdisplay;
    dev->bufs[1].width = conn_mode_buf[0].hdisplay;
    dev->bufs[1].height = conn_mode_buf[0].vdisplay;
    printf("Mode for connector %u is %ux%u@%u\n",conn.connector_id, dev->bufs[0].width,dev->bufs[0].height,conn_mode_buf->vrefresh);
    /* Find a CRTC for this connector */
    ret = modeset_find_crtc(dri_fd, dev, conn, res,conn_enc_buf,res_crtc_buf);
    if (ret){
        printf("No valid crtc for connector %u\n",conn.connector_id);
        return ret;
    }

    /* create a framebuffer for this CRTC */
    ret = modeset_create_fb(dri_fd, &dev->bufs[0]);
    if(ret){
        printf("Cannot create framebuffer for connector %u\n",conn.connector_id);
        return ret;
    }
    ret = modeset_create_fb(dri_fd, &dev->bufs[1]);
    if(ret){
        printf("Cannot create framebuffer for connector %u\n",conn.connector_id);
        return ret;
    }

    return 0;

}

static int modeset_find_crtc(int dri_fd, struct modeset_dev *dev, struct drm_mode_get_connector conn, struct drm_mode_card_res res, uint64_t *conn_enc_buf, uint64_t *res_crtc_buf){
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

static int modeset_create_fb(int dri_fd, struct modeset_buf *buf){
    //------------------------------------------------------------------------------
    //Creating a dumb buffer
    //------------------------------------------------------------------------------
    struct drm_mode_create_dumb create_dumb={0};
    struct drm_mode_map_dumb map_dumb={0};
    struct drm_mode_fb_cmd cmd_dumb={0};
    struct drm_mode_destroy_dumb destroy_dumb={0};
    int ret;
    create_dumb.width = buf->width;
    create_dumb.height = buf->height;
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
    buf->stride = create_dumb.pitch;
    buf->size = create_dumb.size;
    buf->handle = create_dumb.handle;

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
        destroy_dumb.handle = buf->handle;
        ioctl(dri_fd,DRM_IOCTL_MODE_DESTROY_DUMB, &destroy_dumb);
        return ret;
    }
    buf->fb = cmd_dumb.fb_id;

    /* Prepare buffer for memory map */
    map_dumb.handle=buf->handle;
    ret = ioctl(dri_fd,DRM_IOCTL_MODE_MAP_DUMB,&map_dumb);
    if(ret){
        printf("Cannot map dumb buffer\n");
        return ret;
    }

    /* Do mapping */
    buf->map = mmap(0, create_dumb.size, PROT_READ | PROT_WRITE, MAP_SHARED, dri_fd, map_dumb.offset);
    if(buf->map == MAP_FAILED){
        printf("Cannot mmap dumb buffer\n");
        ioctl(dri_fd,DRM_IOCTL_MODE_RMFB, buf->fb);
        return ret;
    }

    /* clear the FB to 0 */
    memset(buf->map, 0, buf->size);

    return 0;
}

extern int startGraphic(void){
    int ret, dri_fd;
    struct modeset_dev *iter;
    struct modeset_buf *buf;

    dri_fd = modeset_open();

    ret = modeset_prepare(dri_fd);

    if(ret){
        close(ret);
        return 0;
    }

    for (iter=modeset_list; iter!=NULL ; iter = iter->next) {
        struct drm_mode_crtc crtc={0};
        crtc.crtc_id=iter->crtc;
        ioctl(dri_fd, DRM_IOCTL_MODE_GETCRTC, &crtc);
        iter->saved_crtc = &crtc;
        buf = &iter->bufs[iter->front_buf];
        crtc.fb_id=buf->fb;
        crtc.set_connectors_ptr=(uint64_t)&iter->conn;
        crtc.count_connectors=1;
        crtc.mode=iter->mode;
        crtc.mode_valid=1;
        ret = ioctl(dri_fd, DRM_IOCTL_MODE_SETCRTC, &crtc);
        if (ret){
            printf("Cannot set CRTC for connector %u \n",iter->conn);
        }
    }
    printf("vrefresh: %d\n",modeset_list->mode.vrefresh);
    printf("vdisplay: %d\n",modeset_list->mode.vdisplay);
    printf("vsync_start: %d\n",modeset_list->mode.vsync_start);
    printf("vsync_end: %d\n",modeset_list->mode.vsync_end);
    printf("We did it?\n");
    return dri_fd;
}

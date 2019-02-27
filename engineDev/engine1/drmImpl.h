#ifndef FOO_H
#define FOO_H

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


struct modeset_buf
{
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t size;
    uint32_t handle;
    uint8_t *map;
    uint32_t fb;
};

struct modeset_dev {
    struct modeset_dev *next;

    unsigned int front_buf;
    struct modeset_buf bufs[2];

    struct drm_mode_modeinfo mode;
    uint32_t conn;
    uint32_t crtc;
    struct drm_mode_crtc *saved_crtc;
};

static struct modeset_dev *modeset_list = NULL;


static struct point2D
{
    int x,y;
}point;

static int modeset_open(void);
static int modeset_prepare(int dri_fd);
static int modeset_create_fb(int dri_fd, struct modeset_buf *buf);
static int modeset_setup_dev(int dri_fd, struct modeset_dev *dev, struct drm_mode_get_connector conn, struct drm_mode_card_res res, struct drm_mode_modeinfo *conn_mode_buf, uint64_t *conn_enc_buf, uint64_t *res_crtc_buf);
static int modeset_find_crtc(int dri_fd, struct modeset_dev *dev, struct drm_mode_get_connector conn, struct drm_mode_card_res res, uint64_t *conn_enc_buf, uint64_t *res_crtc_buf);

extern uint8_t next_color(bool *up, uint8_t cur, unsigned int mod);
extern int startGraphic(void);
extern void writeToBuf(int x, int y, uint8_t r, uint8_t g, uint8_t b);
extern void refresh(int dri_fd);
extern void refreshScreenCircle(int dri_fd,uint8_t r, uint8_t g,uint8_t b);
extern void refreshScreenColor(int dri_fd,uint8_t r, uint8_t g,uint8_t b, bool r_up, bool g_up, bool b_up);

#endif

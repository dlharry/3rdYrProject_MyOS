#include "video.h"
#include "mailbox.h"
#include "printf.h"
#include "timer.h"
#include "dma.h"
#include "mm.h"

typedef struct
{
    mailbox_tag tag;
    u32 xres;
    u32 yres;
} mailbox_fb_size;

typedef struct
{
    mailbox_tag tag;
    u32 bpp; // bits per pixel
} mailbox_fb_depth;

typedef struct
{
    mailbox_tag tag;
    u32 pitch;
} mailbox_fb_pitch;

typedef struct
{
    mailbox_tag tag;
    u32 base;
    u32 screen_size;
} mailbox_fb_buffer;

typedef struct
{
    mailbox_fb_size res;
    mailbox_fb_size vres;
    mailbox_fb_depth depth;
    mailbox_fb_buffer buff;
    mailbox_fb_pitch pitch;
} mailbox_fb_request;

static mailbox_fb_request fb_req; // static for reference

static dma_channel *dma;
static u8 *vid_buffer; // using for dma

static u32 *bg32_buffer; // background buffer to see how fast the background is transferred
static u32 *bg8_buffer;

#define MB (1024 * 1024)

// no allocate function yet...
// revisit when MMU implemented
#define BG32_MEM_LOCATION (LOW_MEMORY + (10 * MB))
#define BG8_MEM_LOCATION (BG32_MEM_LOCATION + (10 * MB))
#define VB_MEM_LOCATION (BG8_MEM_LOCATION + (4 * MB))
#define TEXT_COLOR 0xFFFFFFFF
#define BACK_COLOR 0x0055BBFF

// for initiaing dma
void video_init() {
    dma = dma_open_channel(CT_NORMAL);
    vid_buffer = (u8 *)VB_MEM_LOCATION;

    printf("DMA CHANNEL: %d\n", dma->channel);
    printf("VID BUFF: %X\n", vid_buffer);

    bg32_buffer = (u32 *)BG32_MEM_LOCATION;
    bg8_buffer = (u32 *)BG8_MEM_LOCATION;

    for (int i=0; i<(10 * MB) / 4; i++) {
        bg32_buffer[i] = BACK_COLOR;
    }

    for (int i=0; i<(4 * MB) / 4; i++) {
        bg8_buffer[i] = 0x01010101; // palette index
    }
}

static bool use_dma = false;

// might have another one?
#define BUS_ADDR(x) (((u64)x | 0x40000000) & ~0xC0000000)

#define FRAMEBUFFER ((volatile u8 *)BUS_ADDR(fb_req.buff.base))
#define DMABUFFER ((volatile u8 *)vid_buffer)
#define DRAWBUFFER (use_dma ? DMABUFFER : FRAMEBUFFER)
// volatile u8 *frame_buffer = (u8 *)((fb_req.buff.base | 0x40000000) & ~0xC0000000);

void video_set_dma(bool b) {
    use_dma = b;
}

void do_dma(void *dest, void *src, u32 total) {
    u32 ms_start = timer_get_ticks() / 1000;

    u32 start = 0;
    while(total > 0) {
        int num_bytes = total;

        if (num_bytes > 0xFFFFFF) {
            num_bytes = 0xFFFFFF;
        }
        dma_setup_mem_copy(dma, dest + start, src + start, num_bytes, 2); // sending the number of bytes maximizing it to ffffff
        dma_start(dma);
        dma_wait(dma);

        start += num_bytes;
        total -= num_bytes;
    }

    u32 ms_end = timer_get_ticks() / 1000;
    //when done

    printf("DMA took %d ms\n", (ms_end - ms_start));
}

void video_dma() {
    do_dma(FRAMEBUFFER, DMABUFFER, fb_req.buff.screen_size);
}

typedef struct  {
    mailbox_tag tag;
    u32 offset; // multiple palettes
    u32 num_entries;
    u32 entries[8];
} mailbox_set_palette;


bool video_set_resolution(u32 xres, u32 yres, u32 bpp){
    mailbox_fb_request req;

    fb_req.res.tag.id = RPI_FIRMWARE_FRAMEBUFFER_SET_PHYSICAL_WIDTH_HEIGHT;
    fb_req.res.tag.buffer_size = 8;
    fb_req.res.tag.value_length = 8;
    fb_req.res.xres = xres;
    fb_req.res.yres = yres;

    fb_req.vres.tag.id = RPI_FIRMWARE_FRAMEBUFFER_SET_VIRTUAL_WIDTH_HEIGHT;
    fb_req.vres.tag.buffer_size = 8;
    fb_req.vres.tag.value_length = 8;
    fb_req.vres.xres = xres;
    fb_req.vres.yres = yres;

    fb_req.depth.tag.id = RPI_FIRMWARE_FRAMEBUFFER_SET_DEPTH;
    fb_req.depth.tag.buffer_size = 4; // 32 bit
    fb_req.depth.tag.value_length = 4;
    fb_req.depth.bpp = bpp;

    fb_req.buff.tag.id = RPI_FIRMWARE_FRAMEBUFFER_ALLOCATE;
    fb_req.buff.tag.buffer_size = 8;
    fb_req.buff.tag.value_length = 4;
    fb_req.buff.base = 16;
    fb_req.buff.screen_size = 0; // will comeback

    fb_req.pitch.tag.id = RPI_FIRMWARE_FRAMEBUFFER_GET_PITCH;
    fb_req.pitch.tag.buffer_size = 4;
    fb_req.pitch.tag.value_length = 4;
    fb_req.pitch.pitch = 0;

    // PALETTE area
    mailbox_set_palette palette;
    palette.tag.id = RPI_FIRMWARE_FRAMEBUFFER_SET_PALETTE;
    palette.tag.buffer_size = 40;
    palette.tag.value_length = 0;
    palette.offset = 0;
    palette.num_entries = 8;
    palette.entries[0] = 0x00000000; // black
    palette.entries[1] = 0xFFBB5500;
    palette.entries[2] = 0xFFFFFFFF; // white
    palette.entries[3] = 0xFFFF0000; // blue
    palette.entries[4] = 0xFF00FF00; // green
    palette.entries[5] = 0xFF0000FF; // red
    palette.entries[6] = 0x55555555; // shades of grey
    palette.entries[7] = 0xCCCCCCCC;

    // set real resolution
    mailbox_process((mailbox_tag *)&fb_req, sizeof(fb_req));
    printf("Allocated Buffer: %X - %d\n", fb_req.buff.base, fb_req.buff.screen_size);
    if (bpp == 8) mailbox_process((mailbox_tag *)&palette, sizeof(palette));
    timer_sleep(2000);
    char res[64];
    sprintf(res, "Resolution: %d x %d\n", xres, yres);
    video_draw_string(res, 20, 20);// up left corner

    // red square with blue bound
    // u32 square_margin = 100;
    // u64 ms_start = timer_get_ticks() / 1000;
    // for(int y=0; y<yres; ++y){
    //     if(y == 40){
    //         // redraw the resolution string
    //         video_draw_string(res, 20, 20);
    //     }

    //     for(int x=0; x<xres; x++){
    //         // check if we are on boundary or square
    //         bool draw_square = (y > square_margin && (yres - y)>square_margin) && (x > square_margin && (xres - x)>square_margin);

    //         video_draw_pixel(x, y, draw_square ? 0xAA0000FF : 0X0055BBFF);
    //     }
    // }

    // u64 ms_end = timer_get_ticks() / 1000;
    // sprintf(res, "Screen took %d ms to draw\n", (ms_end - ms_start));
    // video_draw_string(res, 20, 40);

    // printf("Drawing done\n");
    // timer_sleep(2000);

    for (int i=0; i<4; i++) {
        u64 ms_start = timer_get_ticks() / 1000; //get ticks in ms before drawing on screen...

        if (fb_req.depth.bpp == 32) {
            if (!use_dma) { // 32 bit not using dma
                // memcpy too slow
                // memcpy(FRAMEBUFFER, bg32_buffer, fb_req.buff.screen_size);
                u32 *buff = (u32 *)FRAMEBUFFER;
                for (int i=0; i<fb_req.buff.screen_size / 4; i++) { // take each byte and copy
                    buff[i] = bg32_buffer[i];
                }
            } else {
                do_dma(BUS_ADDR(vid_buffer), bg32_buffer, fb_req.buff.screen_size);
            }
        }
        else if (fb_req.depth.bpp == 8) {

            if (!use_dma) { // 8 bit not using dma
                // memcpy(FRAMEBUFFER, bg8_buffer, fb_req.buff.screen_size);
                u32 *buff = (u32 *)FRAMEBUFFER;
                for (int i=0; i<fb_req.buff.screen_size / 4; i++) {
                    buff[i] = bg8_buffer[i];
                }
            } else {
                do_dma(BUS_ADDR(vid_buffer), bg8_buffer, fb_req.buff.screen_size);
            }
        }

        u64 ms_end = timer_get_ticks() / 1000;
        u32 ms_buff = ms_end - ms_start;
        u64 ms_before_draw = ms_end;
        
        if (use_dma) video_dma();

        ms_end = timer_get_ticks() / 1000;

        sprintf(res, "Resolution: %d x %d x %d", xres, yres, bpp);
        video_draw_string(res, 20, 20 + (i * 20));
        sprintf(res, "BG write took: %d ms\n", ms_buff);
        video_draw_string(res, 20, 40 + (i * 20));
        sprintf(res, "DMA BG draw took: %d ms\n", ms_end - ms_before_draw);
        video_draw_string(res, 20, 60 + (i * 20));
        sprintf(res, "Video Drawing Done!");
        video_draw_string(res, 20, 80 + (i * 20));
        if (use_dma) video_dma();

        ms_end = timer_get_ticks() / 1000;

        sprintf(res, "FRAME DRAW TIME: %d ms\n", ms_end - ms_start);
        video_draw_string(res, 20, 100 + (i * 20));

        if (use_dma) video_dma();

        timer_sleep(2000);
    }

    return true;
}

void video_draw_pixel(u32 x, u32 y, u32 color) {
    // volatile u8 *frame_buffer = (u8 *)((fb_req.buff.base | 0x40000000) & ~0xC0000000);

    u32 pixel_offset = (x*(32>>3)) + (y*fb_req.pitch.pitch);
    if(fb_req.depth.bpp == 32){
        // u8 r = (color & 0xFF000000) >> 24;
        // u8 g = (color & 0xFF0000) >> 16;
        // u8 b = (color & 0xFF00) >> 8;
        // u8 a = color & 0xFF;

        // frame_buffer[pixel_offset++] = b;
        // frame_buffer[pixel_offset++] = g;
        // frame_buffer[pixel_offset++] = r;
        // frame_buffer[pixel_offset++] = a;
        u32 *buff = (u32 *)DRAWBUFFER;
        buff[pixel_offset / 4] = color;
    }else if (fb_req.depth.bpp == 16){
        // frame_buffer[pixel_offset++] = (color >> 8) & 0xFF;
        // frame_buffer[pixel_offset++] = (color & 0xFF);
        u16 *buff = (u16 *)DRAWBUFFER;
        buff[pixel_offset / 2] = color & 0xFFFF;
    }else{
        DRAWBUFFER[pixel_offset++] = (color & 0xFF);
    }
}



void video_draw_char(char c, u32 pos_x, u32 pos_y){
    // support 8 bit
    u32 tc = TEXT_COLOR, bc = BACK_COLOR;
    if(fb_req.depth.bpp == 8){
        tc = 2;
        bc = 1;
    }
    for(int y=0; y<font_get_height(); ++y){
        for(int x=0; x<font_get_width(); ++x){
            bool yes = font_get_pixel(c, x, y); // if there is a pixel for the font at here
            video_draw_pixel(pos_x + x, pos_y + y, yes ? tc : bc);
        }
    }

}

void video_draw_string(char *s, u32 pos_x, u32 pos_y){
    for(int i=0; s[i] != 0; pos_x += (font_get_width( + 2))){ // 2 for space between chars
        video_draw_char(s[i], pos_x, pos_y);
        ++i;
    }
}


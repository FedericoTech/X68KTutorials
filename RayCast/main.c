//#define __MARIKO_CC__

#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

#include <stdint.h>
#include <signal.h>
#include <math.h>
//#include <sys/xmath.h>

#define TRUE 1
#define FALSE 0
#define MAPS 32 // 64
#define PXY 128.0f //128.0f

typedef uint8_t bool;

int8_t last_mode;

bool buffer = 0;

float PI = M_PI;            //PI
float PI2 = 2.0f * M_PI;    //two times PI

float P2 = M_PI / 2;        //half a PI
float P3 = 3.0f * M_PI / 2;

float DR = 0.0174533f;      //one degree in radians

float px, py, pdx = 0.0f, pdy = 0.0f, pa = 0.0f; //player position

int8_t mapX = 8, mapY = 8, mapS = MAPS;

int8_t map[] =
{
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
};

float dist(float ax, float ay, float bx, float by, float ang)
{
    float A = bx - ax;
    float B = by - ay;
    float ret2; //  ((bx - ax) * (bx - ay)) + ((by - ay) * (by - ay));
    float ret;

    A *= A;
    B *= B;


    ret2 = A + B;
    ret = sqrt(ret2);

    if(isnan(ret) || ret != ret){
        printf(
           "(%f - %f) (%f - %f) %f %f %f\n",
           bx,
           ax,

           by,
           ay,

           ang,
           ret2,
           ret
       );
    }



    return ret;
}

void drawRays2D()
{
    int16_t r, mx, my, mp, dof;
    uint8_t color;
    float rx, ry, xo, yo, ra, disT, ca, lineH, lineO;
    struct _lineptr linebuf;
    struct _fillptr fillbuf;

    ra = pa - DR * 30.0f;
    if(ra < 0){ ra += PI2; }
    if(ra > PI2){ ra -= PI2; }

    _iocs_apage(2);
    _iocs_wipe();
    _iocs_apage(buffer);

    for(r = 0; r < 60; r++){
        float Tan = tan(ra);
        float aTan = -1 / Tan;
        float nTan = -Tan;
        float disH = 1000000, hx = px, hy = py;
        float disV = 1000000, vx = px, vy = py;

        //---Check Horizontal Lines ---
        dof = 0;

        //printf("%f %f\n", ra, PI);

        if(ra > PI){ // looking up
            ry = (((int16_t) py >> 5) << 5) - 0.0001f;

            //printf("up %f %f \n", py, ry);

            rx = (py-ry) * aTan + px;
            yo = -MAPS;
            xo = -yo * aTan;
        }

        if(ra < PI){ // looking down
            ry = (((int16_t) py >> 5) << 5) + MAPS;

            //printf("down %f %f\n", py, ry);

            rx = (py-ry) * aTan + px;
            yo = MAPS;
            xo = -yo * aTan;
        }

        if(ra == 0 || ra == PI){rx = px; ry = py; dof = 8; } //looking straight left or right

        //printf("--\n");
        while(dof < 8){
            mx = ((int16_t)(rx)) >> 5;
            my = ((int16_t)(ry)) >> 5;

            mp = my * mapX + mx;

            if(mp > 0
               && mp < mapX * mapY
               && map[mp] == 1
            ){ //hit
                hx = rx;
                hy = ry;
                disH = dist(px, py, hx, hy, ra);
                dof = 8;
            } else {
                rx += xo;
                ry += yo;
                dof += 1;
            } // next line
        }

        //---Check Vertical Lines ---

        dof = 0;

        //printf("%f %f %f\n", P2, ra, P3);

        if(ra > P2 && ra < P3){ // looking left
            rx = (((int16_t) px >> 5) << 5) - 0.0001f;

            //printf("left %f %f \n", py, ry);

            ry = (px-rx) * nTan + py;
            xo = -MAPS;
            yo = -xo * nTan;
        }

        if(ra < P2 || ra > P3){ // looking right
            rx = (((int16_t) px >> 5) << 5) + MAPS;

            //printf("right %f %f\n", py, ry);

            ry = (px-rx) * nTan + py;
            xo = MAPS;
            yo = -xo * nTan;
        }

        if(ra == 0 || ra == PI){rx = px; ry = py; dof = 8; } //looking straight left or right

        //printf("--\n");
        while(dof < 8){
            mx = ((int16_t)(rx)) >> 5;
            my = ((int16_t)(ry)) >> 5;

            //printf("%f %d %f %d %f\n", ra, mx, rx, my, ry);

            mp = my * mapX + mx;

            if(mp > 0
               && mp < mapX * mapY
               && map[mp] == 1
            ){ //hit
                vx = rx;
                vy = ry;
                disV = dist(px, py, vx, vy, ra);
                dof = 8;
            } else {
                rx += xo;
                ry += yo;
                dof += 1;
            } // next line
        }

        if(disV < disH){ rx = vx; ry = vy; disT = disV; color = 10;}
        if(disH < disV){ rx = hx; ry = hy; disT = disH; color = 11;}

        if(r % 60 == 0){
            //printf("%f %f %f \n", disV, disH, disT);
        }

        linebuf.x1 = (int16_t)px;
        linebuf.y1 = (int16_t)py;
        linebuf.x2 = (int16_t)rx;
        linebuf.y2 = (int16_t)ry;
        linebuf.color = 9; //green
        linebuf.linestyle = 0xFFFF;

        _iocs_line (&linebuf);

        ca = pa - ra;
        if(ca < 0){
            ca += PI2;
        }
        if(ca > PI){
            ca -= PI;
        }

        disT = disT * cos(ca); //fix fisheye

        //---Draw 3D Walls---
        lineH = mapS * 256.0f;
        lineH /= disT;

        if(lineH > 256.0f){lineH = 256.0f;} //line height

        lineO = lineH  / 2.0f;
        lineO = 128 - lineO;  //line offset

        //_iocs_apage(2);

        fillbuf.x1 = r * 4 + 256;
        fillbuf.y1 = (int16_t)lineO;
        fillbuf.x2 = r * 4 + 4 + 256;
        fillbuf.y2 = (int16_t)(lineH + lineO);
        fillbuf.color = color;
/*
        printf(
           "%d %d %d %d\n",
           fillbuf.x1,
           fillbuf.y1,
           fillbuf.x2,
           fillbuf.y2
        );
*/
        _iocs_fill (&fillbuf);

        //_iocs_apage(buffer);

        ra += DR;

        if(ra < 0){ ra += PI2; }
        if(ra > PI2){ ra -= PI2; }

    } //for...END
}

void drawPlayer()
{
    struct _fillptr fillbuf;
    struct _lineptr linebuf;

    int16_t ipx = (int16_t) px, ipy = (int16_t) py, ipdx = (int16_t) pdx, ipdy = (int16_t) pdy;

    fillbuf.x1 = ipx;
    fillbuf.y1 = ipy;
    fillbuf.x2 = ipx + 4;
    fillbuf.y2 = ipy + 4;
    fillbuf.color = 0xD;

    _iocs_fill (&fillbuf);

    linebuf.x1 = ipx;
    linebuf.y1 = ipy;
    linebuf.x2 = ipx + ipdx * 2;
    linebuf.y2 = ipy + ipdy * 2;
    linebuf.color = 0xD;
    linebuf.linestyle = 0xFFFF;

    _iocs_line (&linebuf);
}



void drawMap()
{
    int x, y, xo, yo;

    uint16_t color;

    struct _fillptr fillbuf;

    for(y = 0; y < mapY; y++){
        for(x = 0; x < mapX; x++){

            //only if occupied
            if(map[y * mapX + x] == 1){

                xo = x * mapS; yo = y * mapS;

                fillbuf.x1 = xo + 1;
                fillbuf.y1 = yo + 1;
                fillbuf.x2 = xo + mapS - 1;
                fillbuf.y2 = yo + mapS - 1;
                fillbuf.color = 15; //last colour of the 4 bit palette

                _iocs_fill (&fillbuf);
            }
        }
    }
}



void display()
{
    //_iocs_vpage(3);

    buffer = !buffer;
    _iocs_apage(buffer);

    _iocs_wipe();

    drawMap();
    drawRays2D();
    drawPlayer();

    _iocs_vpage(buffer + 1);
}

void terminate()
{
    /* Un interrupt */
	_iocs_vdispst(
        (void *)0,
        0,//0: vertical blanking interval 1: vertical display period
        0
    );

    //we activate the console cursor
    _iocs_b_curon();

    //we restore the video mode
    _iocs_crtmod(last_mode);
    _dos_exit();
}

void buttons()
{
    int8_t key = _dos_inkey();

    //escape
    if(key == 27){ terminate(); }

    //left
    if(key == 19){
        pa -= 0.1f;

        if(pa < 0.0f){ pa += PI2;}

        pdx = cos(pa);
        pdx *= 5.0f;

        pdy = sin(pa);
        pdy *= 5.0f;
    }

    //right
    if(key == 4){

        pa += 0.1f;

        if(pa > PI2){ pa -= PI2;}

        pdx = cos(pa);
        pdx *= 5.0f;

        pdy = sin(pa);
        pdy *= 5.0f;
    }

    //up
    if(key == 1){ px += pdx; py += pdy; }
    //down
    if(key == 6){ px -= pdx; py -= pdy; }

    //printf("%d %f %f %f %f %f\n", key, px, py, pdx, pdy, pa);

    display();
}

void interrupt vsync_disp()
{

}

void init()
{
    last_mode = _iocs_crtmod(-1);

    _iocs_crtmod(4); //this mode is 512 x 512 16 colours
    //_iocs_crtmod(6); //this mode is 256 x 256 16 colours

    _iocs_b_curoff(); //disable the cursor

    _iocs_g_clr_on(); //start graphic system

    signal(SIGINT, terminate); //for the Control + C

    px = PXY; py = PXY;

    pdx = cos(pa);
    pdx *= 5.0f;

    pdy = sin(pa);
    pdy *= 5.0f;
}

int main(void)
{

    init();

    display();
    display();

    while(TRUE){
        buttons();
        //display();
    }
/*
    for(py = 0; py < 200; py ++){
        ++px;
        display();
    }
*/
    _dos_c_print("Press a key.\r\n");

    //waiting for a keystroke.
    _dos_getchar();

    terminate();
}

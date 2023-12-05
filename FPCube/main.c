#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#include <stdio.h>
	#define interrupt __attribute__ ((interrupt_handler))
	#define _putptr iocs_putptr
	#define _lineptr iocs_lineptr
	#define _paintptr iocs_paintptr

#endif

#include <stdint.h>
#include <signal.h>


#define TRUE 1
#define FALSE 0

//#define SHADOW

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 256
#define SCREEN_SCALE 2
#define SIN_TABLE_SIZE 256
#define CUBE_POINTS_SIZE 8
#define CUBE_FACES 12

typedef uint8_t bool;

int8_t last_mode;

bool buffer = 0;

int16_t const d = 277;
int16_t const floorCameraY = 100;

int16_t floorCameraX = 0;
int16_t floorCameraZ = 0;

int16_t drumIntensity = 0;

int16_t angle = 0;
int16_t angle2 = 0;

const int8_t cubeLocalPoints[][3] = {   //it seems that we don't need to set the first size but the second only
    {-100,  100,  100}, //0
    { 100,  100,  100}, //1
    { 100, -100,  100}, //2
    {-100, -100,  100}, //3
    {-100,  100, -100}, //4
    { 100,  100, -100}, //5
    { 100, -100, -100}, //6
    {-100, -100, -100}  //7
};

int16_t cubeScreenPoints[CUBE_POINTS_SIZE][2];
int16_t cubeShadowScreenPoints[CUBE_POINTS_SIZE][2];

// triangles {vertex_0, vertex_1, vertex_2, min_face_color}
const int8_t cubeFaces[][4] = {     //it seems that we don't need to set the first size but the second only
    {3, 2, 1, 5}, {3, 1, 0, 5},
    {5, 6, 7, 7}, {4, 5, 7, 7},
    {1, 2, 6, 9}, {1, 6, 5, 9},
    {3, 0, 4, 11}, {3, 4, 7, 11},
    {0, 1, 5, 13}, {0, 5, 4, 13},
    {2, 3, 7, 15}, {2, 7, 6, 15}
};

const int8_t sinTable[] = {
    0, 3, 6, 9, 12, 15, 18, 21, 24, 28, 31, 34, 37, 40, 43, 46, 48, 51, 54,
    57, 60, 63, 65, 68, 71, 73, 76, 78, 81, 83, 85, 88, 90, 92, 94, 96, 98,
    100, 102, 104, 106, 108, 109, 111, 112, 114, 115, 117, 118, 119, 120,
    121, 122, 123, 124, 124, 125, 126, 126, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 126, 126, 125, 124, 124, 123, 122, 121, 120,
    119, 118, 117, 115, 114, 112, 111, 109, 108, 106, 104, 102, 100, 98,
    96, 94, 92, 90, 88, 85, 83, 81, 78, 76, 73, 71, 68, 65, 63, 60, 57, 54,
    51, 48, 46, 43, 40, 37, 34, 31, 28, 24, 21, 18, 15, 12, 9, 6, 3, 0, -3,
    -6, -9, -12, -15, -18, -21, -24, -28, -31, -34, -37, -40, -43, -46, -48,
    -51, -54, -57, -60, -63, -65, -68, -71, -73, -76, -78, -81, -83, -85,
    -88, -90, -92, -94, -96, -98, -100, -102, -104, -106, -108, -109, -111,
    -112, -114, -115, -117, -118, -119, -120, -121, -122, -123, -124, -124,
    -125, -126, -126, -127, -127, -127, -127, -127, -128, -127, -127, -127,
    -127, -127, -126, -126, -125, -124, -124, -123, -122, -121, -120, -119,
    -118, -117, -115, -114, -112, -111, -109, -108, -106, -104, -102, -100,
    -98, -96, -94, -92, -90, -88, -85, -83, -81, -78, -76, -73, -71, -68,
    -65, -63, -60, -57, -54, -51, -48, -46, -43, -40, -37, -34, -31, -28,
    -24, -21, -18, -15, -12, -9, -6, -3
};

int16_t normalizeAngle(int16_t a) {

    if(a > SIN_TABLE_SIZE - 1) {
        a %= SIN_TABLE_SIZE;
    }

    if (a < 0) {
        a += SIN_TABLE_SIZE;
    }
    return a;
};

int16_t getSin(const int16_t a) {
    return sinTable[normalizeAngle(a)];
}

int16_t getCos(const int16_t a) {
    return sinTable[normalizeAngle(a - 64)]; // 64 in this case = 90 degrees
}

void update()
{
    angle += 1;
    if (angle >= SIN_TABLE_SIZE) {
        angle -= SIN_TABLE_SIZE;
    }

    angle2 += 3;
    if (angle2 >= SIN_TABLE_SIZE) {
        angle2 -= SIN_TABLE_SIZE;
    }

    floorCameraX = 2000 + (getSin(angle) << 2);
    floorCameraZ = 2000 + (getCos(angle) << 3);

    drumIntensity -= 20;

    if (drumIntensity < 1) drumIntensity = 1;

    //_dos_c_print("update()\r\n");
}

void drawFloor()
{
    uint8_t screen[SCREEN_WIDTH * 90];
    uint8_t screenY;
    uint16_t screenX;
    int16_t status;

    for (screenY = 0; screenY < 90; screenY++) {

        uint16_t z = (floorCameraY * d) / (floorCameraY - screenY);

        for (screenX = 0; screenX < SCREEN_WIDTH / 2; screenX++) {

            int16_t x = ((screenX - 63) * z) / d;
            int16_t tx = x + floorCameraX;
            int16_t tz = z + floorCameraZ;

            int16_t testX = (tx >> 7) & 1;
            int16_t testZ = (tz >> 7) & 1;

            uint8_t color = (testX == 0 && testZ == 1)
                || (testX == 1 && testZ == 0) ? 3 : 15;

            //color = color | 0xff000000;

            screen[screenX + (/*199*//*166 -*/90 - screenY) * (SCREEN_WIDTH / 2) ] = color << 4 | color;
        }
    }

    {
        struct _putptr putbuf;

        putbuf.x1 = 0;
        putbuf.y1 = 165;
        putbuf.x2 = 255;
        putbuf.y2 = 255;
        putbuf.buf_start = screen;
        putbuf.buf_end = (void *) (screen + sizeof screen);

        status = _iocs_putgrm (&putbuf);

        switch(status){
            case -1: _dos_c_print("Graphics not available\r\n"); break;
            case -2: _dos_c_print("Coordinate specification is abnormal\r\n"); break;
            case -3: _dos_c_print("Buffer capacity is too small\r\n"); break;
        }

        //printf("%d\n\r", status);
    }
} //drawFloor

void drawCube()
{
    int8_t i;

    for(i = 0; i < CUBE_POINTS_SIZE; i++){
        int16_t x = cubeLocalPoints[i][0];
        int16_t y = cubeLocalPoints[i][1];
        int16_t z = cubeLocalPoints[i][2];

        int16_t s1 = getSin(angle);
        int16_t c1 = getCos(angle);
        int16_t s2 = getSin(angle2);
        int16_t c2 = getCos(angle2);

        int16_t cubeScreenX;
        int16_t cubeScreenY;
        int16_t cubeShadowScreenX;
        int16_t cubeShadowScreenY;

        // rotate y
        int16_t nx = (x * c2 - z * s2) >> 7;
        int16_t nz = (x * s2 + z * c2) >> 7;
        int16_t ny;
        int32_t aux;

        x = nx;
        z = nz;

        // rotate x

        ny = (z * s1 + y * c1) >> 7;
        nz = (z * c1 - y * s1) >> 7;

        z = nz;
        y = ny;



        // translate
        z = z - 1200 + (c1 << 1);
        x = x + (s2 << 1);
        y = y - 32 - (drumIntensity >> 1) + (s1 >> 2);

        // cube perspective transformation

        cubeScreenX = d * x / -z;
        cubeScreenY = d * y / -z;

        // cube perspective transformation
        //cubeShadowScreenX = cubeScreenX; //d * x / -z;
        cubeShadowScreenY = d * (200) / -z;

        // screen space adjust
        cubeScreenX += 130;
        cubeScreenY += 100;

        //cubeShadowScreenX += 130;
        cubeShadowScreenY += 200;

        cubeScreenPoints[i][0] =  cubeScreenX;
        cubeScreenPoints[i][1] =  cubeScreenY;

        cubeShadowScreenPoints[i][0] = cubeScreenX; //cubeShadowScreenX;
        cubeShadowScreenPoints[i][1] = cubeShadowScreenY;
    }


#ifdef SHADOW
    // draw cube shadow
    for(i = 0; i < CUBE_FACES; i++){
        int8_t triangle[4];
        int16_t px1, py1, px2, py2, px3, py3;

        _iocs_b_memset(triangle, &cubeFaces[i], sizeof triangle);

        px1 = cubeShadowScreenPoints[triangle[0]][0];
        py1 = cubeShadowScreenPoints[triangle[0]][1];

        px2 = cubeShadowScreenPoints[triangle[1]][0];
        py2 = cubeShadowScreenPoints[triangle[1]][1];

        px3 = cubeShadowScreenPoints[triangle[2]][0];
        py3 = cubeShadowScreenPoints[triangle[2]][1];

        {
            struct _lineptr linebuf;

            linebuf.color = 0; //black
            linebuf.linestyle = 0xFFFF;

            linebuf.x1 = px1;
            linebuf.y1 = py1;
            linebuf.x2 = px2;
            linebuf.y2 = py2;

            _iocs_line (&linebuf);

            linebuf.x1 = px2;
            linebuf.y1 = py2;
            linebuf.x2 = px3;
            linebuf.y2 = py3;

            _iocs_line (&linebuf);

            linebuf.x1 = px3;
            linebuf.y1 = py3;
            linebuf.x2 = px1;
            linebuf.y2 = py1;

            _iocs_line (&linebuf);
        }

        {
            unsigned char workbuf[4096];
            struct _paintptr paintbuf;
            int16_t centerX = (px1 + px2 + px3) / 3;
            int16_t centerY = (py1 + py2 + py3) / 3;

            paintbuf.x = centerX;
            paintbuf.y = centerY;
            paintbuf.color = 0;
            paintbuf.buf_start = workbuf;
            paintbuf.buf_end = workbuf + sizeof (workbuf);

            _iocs_paint (&paintbuf);
        }


        //fillTriangle(px1, py1, px2, py2, px3, py3, 0xff000000);

    }
#endif

    // draw cube
    for(i = 0; i < CUBE_FACES; i++){
        int8_t triangle[4];
        int16_t px1, py1, px2, py2, px3, py3;
        int16_t vx1, vy1, vx2, vy2;
        int8_t minFaceColor;
        int8_t faceColor;

        _iocs_b_memstr(&cubeFaces[i], triangle, sizeof triangle);


        px1 = cubeScreenPoints[triangle[0]][0];
        py1 = cubeScreenPoints[triangle[0]][1];

        px2 = cubeScreenPoints[triangle[1]][0];
        py2 = cubeScreenPoints[triangle[1]][1];

        px3 = cubeScreenPoints[triangle[2]][0];
        py3 = cubeScreenPoints[triangle[2]][1];

        // back face culling
        vx1 = (px1 - px2);
        vy1 = (py1 - py2);
        vx2 = (px3 - px2);
        vy2 = (py3 - py2);

        if (vx1 * vy2 >= vy1 * vx2) {
            continue;
        }
        // drum music color effect
        minFaceColor = triangle[3];
        faceColor = minFaceColor;

        /*
        faceColor = (drumIntensity < minFaceColor)
                ? minFaceColor : drumIntensity;
                */

        faceColor = (faceColor << 16) + (faceColor << 8) + faceColor;

        {
            struct _lineptr linebuf;
            linebuf.linestyle = 0xFFFF;

            linebuf.color = triangle[3]; //faceColor;

            linebuf.x1 = px1;
            linebuf.y1 = py1;
            linebuf.x2 = px2;
            linebuf.y2 = py2;

            _iocs_line (&linebuf);

            linebuf.x1 = px2;
            linebuf.y1 = py2;
            linebuf.x2 = px3;
            linebuf.y2 = py3;

            _iocs_line (&linebuf);

            linebuf.x1 = px3;
            linebuf.y1 = py3;
            linebuf.x2 = px1;
            linebuf.y2 = py1;

            _iocs_line (&linebuf);
        }

        {
            int16_t centerX = (px1 + px2 + px3) / 3;
            int16_t centerY = (py1 + py2 + py3) / 3;

            unsigned char workbuf[4096];
            struct _paintptr paintbuf;

            paintbuf.x = centerX;
            paintbuf.y = centerY;
            paintbuf.color = triangle[3];
            paintbuf.buf_start = workbuf;
            paintbuf.buf_end = workbuf + sizeof (workbuf);

            _iocs_paint (&paintbuf);
        }

        //fillTriangle(px1, py1, px2, py2, px3, py3, 0xff000000 | faceColor);
    }
    //_dos_c_print("drawCube()\r\n");
} //drawCube


void display()
{
    //_iocs_vpage(3);

    buffer = !buffer;
    _iocs_apage(buffer);

    _iocs_wipe();

#ifdef FLOOR
    drawFloor();
#endif

    drawCube();
    //drawWaveText(g);

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

void init()
{
    last_mode = _iocs_crtmod(-1);

    //_iocs_crtmod(4); //this mode is 512 x 512 16 colours
    _iocs_crtmod(6); //this mode is 256 x 256 16 colours

    _iocs_b_curoff(); //disable the cursor

    _iocs_g_clr_on(); //start graphic system

    signal(SIGINT, terminate); //for the Control + C

}

int main(void)
{
    int8_t count = 0;
    init();


    while(TRUE){
        if(_dos_inpout(0xFF) != 0){
            break;
        }
        update();
        display();
    }



    _dos_c_print("Press a key.\r\n");

    //waiting for a keystroke.
    _dos_getchar();

    terminate();
}

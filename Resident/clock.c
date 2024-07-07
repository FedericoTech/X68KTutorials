

#ifdef __MARIKO_CC__
	#include <iocslib.h>

	#include <math.h>

#else
	#include <iocs.h>
	#include <stdio.h>

	//#include "my_math.h"
	#include <math.h>

	#define REMOVE_AMPERSAND(ptr) (*ptr)

	#define _txfillptr iocs_txfillptr
	#define _tboxptr iocs_tboxptr
	#define _tlineptr iocs_tlineptr
	#define _ylineptr iocs_ylineptr
	#define _xlineptr iocs_xlineptr
#endif




#include "clock.h"
#include "utils.h"

#define TEXT_PLANE 2

int counter;
int currentime = 0;
volatile char mutex = 1;

const int centerX = 500;
const int centerY = 100;
const float PI = M_PI;
const float radians = (float)M_PI / 180.0f;

const float hour_arm_length = 32.0f;
const float minute_arm_length = 40.0f;
const float second_arm_length = 50.0f;

static inline void calculate_arms(const float tick, const float armLength, short* endX, short* endY)
{
    float angle = tick * 6.0f - 90.0f; // Angle of rotation in degrees (adjust as needed)

    // Convert the angle to radians
    float angleRadians = radians * angle;

    // Calculate the endpoint coordinates based on the angle
    *endX = (short) armLength * cos(angleRadians);
    *endY = (short) armLength * sin(angleRadians); // Subtract because Y-axis is inverted
}

//the resident program is this interrupt
void interrupt process_start() //it has to be an interrupt
{
    //we check whether the interrupt is still being processed
    if(mutex){
        mutex = 0; //we hold the mutex
        //only do things every 100 cycles
        if(++counter % 100 == 0){

            int auxtime = _dos_gettim2();

            //if time has changed...
            if(auxtime != currentime){
                currentime = auxtime;

                //clear clock
                {
                    struct _txfillptr txfillptr = {
                        /* unsigned short vram_page */  TEXT_PLANE, //0, 1, 2, 4. Plane 2 is fixed. It won't move up with the scroll
                        /* short x */                   centerX - 50,
                        /* short y */                   centerY - 50,
                        /* short x1 */                  100,
                        /* short y1 */                  100,
                        /* unsigned short fill_patn */  0x0000
                    };

                    _iocs_txfill(&txfillptr);
                }

                //we draw the box
                {
                    struct _tboxptr tboxptr = {
                        /* unsigned short vram_page */  TEXT_PLANE, //0, 1, 2, 4. Plane 2 is fixed. It won't move up with the scroll
                        /* short x */                   centerX - 50,
                        /* short y */                   centerY - 50,
                        /* short x1 */                  100,
                        /* short y1 */                  100,
                        /* unsigned short line_style */ (0xf0 << (counter % 8)) | (0xf0 >> (8 - (counter % 8))) // dashed
                    };

                    _iocs_txbox(&tboxptr);
                }


                {
                    //perpendicular line
                    struct _tlineptr tlptr = {
                        /* unsigned short vram_page */  TEXT_PLANE, //0, 1, 2, 4. Plane 2 is fixed. It won't move up with the scroll
                        /* short x */                   centerX,
                        /* short y */                   centerY,
                        /* short x1 (length) */         0,  //for initialization purposes
                        /* short y1 (length) */         0,  //for initialization purposes
                        /* unsigned short line_style */ 0xff // solid
                    };

                    //vertical line
                    struct _ylineptr ylineptr = {
                        /* unsigned short vram_page */  TEXT_PLANE,
                        /* short x */                   centerX,
                        /* short y */                   centerY,
                        /* short y1 */                  0,  //for initialization purposes,
                        /* unsigned short line_style */ 0xff // solid
                    };

                    //horizontal line
                    struct _xlineptr xlineptr = {
                        /* unsigned short vram_page */  TEXT_PLANE,
                        /* short x */                   centerX,
                        /* short y */                   centerY,
                        /* short x1 */                  0,  //for initialization purposes,
                        /* unsigned short line_style */ 0xff // solid
                    };

                    char hour = (currentime >> 16);
                    char minute = ((currentime >> 8) & 0x3F);
                    char second = (currentime & 0x3F);


                    float aux_hour = ((float) hour) + (1.0f / (float)minute);

                    //hour
                    //if the hour is in a vertical position...
                    if(aux_hour == 12.0f || aux_hour == 24.0f || aux_hour == 6.0f || aux_hour == 18.0f){
                        ylineptr.y1 = (int)(hour_arm_length * (aux_hour == 12.0f || aux_hour == 24.0f ? 1.0f : -1.0f));

                        //we  draw the second arm vertically
                        _iocs_txyline(&ylineptr);

                    //if the hour is in an horizontal position...
                    } else if (aux_hour == 9.0f || aux_hour == 21.0f || aux_hour == 3.0f || aux_hour == 15.0f){
                         xlineptr.x1 = (int)(hour_arm_length * (aux_hour == 9.0f || aux_hour == 21.0f ? -1.0f : 1.0f));

                         //we  draw the second arm horizontally
                        _iocs_txxline(&xlineptr);
                    //if in another position...
                    } else {
                        calculate_arms(
                           aux_hour * 5.0f,
                           hour_arm_length,
                           &tlptr.x1,
                           &tlptr.y1
                        );
                        //we  draw the second arm
                        _iocs_txline(&tlptr);
                    }

                    //minute
                    //if the minute is in a vertical position...
                    if(minute == 0 || minute == 30){
                        ylineptr.y1 = (int)second_arm_length * (minute == 30 ? 1 : -1);

                        //we  draw the minute arm vertically
                        _iocs_txyline(&ylineptr);
                    //if the minute is in an horizontal position...
                    } else if(minute == 15 || minute == 45) {
                        xlineptr.x1 = (int)second_arm_length * (minute == 45 ? -1 : 1);

                        //we  draw the second arm horizontally
                        _iocs_txxline(&xlineptr);
                    //if in another position...
                    } else {
                        calculate_arms(minute, minute_arm_length, &tlptr.x1, &tlptr.y1);

                        //we  draw the second arm
                        _iocs_txline(&tlptr);
                    }

                    //second
                    //if the second is in a vertical position...
                    if(second == 0 || second == 30){
                        ylineptr.y1 = (int)second_arm_length * (second == 30 ? 1 : -1);

                        //we  draw the second arm vertically
                        _iocs_txyline(&ylineptr);
                    //if the second is in a vertical position...
                    } else if(second == 15 || second == 45) {
                        xlineptr.x1 = (int)second_arm_length * (second == 45 ? -1 : 1);

                        //we  draw the second arm horizontally
                        _iocs_txxline(&xlineptr);
                    //if in another position...
                    } else {

                        calculate_arms(second, second_arm_length, &tlptr.x1, &tlptr.y1);

                        //we  draw the second arm
                        _iocs_txline(&tlptr);
                    }
                }
            }
        }

        mutex = 1; //we release the mutex
    }
}

void clear_clock()
{
    struct _txfillptr txfillptr = {
        /* unsigned short vram_page */  TEXT_PLANE, //0, 1, 2, 4. Plane 2 is fixed. It won't move up with the scroll
        /* short x */                   centerX - 50,
        /* short y */                   centerY - 50,
        /* short x1 */                  100,
        /* short y1 */                  100,
        /* unsigned short fill_patn */  0x0000
    };

    _iocs_txfill(&txfillptr);
}


#include <math.h>

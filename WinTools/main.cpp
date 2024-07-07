#include <iostream>
#include <fstream>

#include "stdafx.h"

int main(int argc, char *argv[])
{
    uint8_t option = atoi(argv[1]);

    option = 20;

    switch(option){
    case 3:
        {
            //we load a 24 bit image
            Bmp bmp = Bmp::fromBmpFile("../assets/images/24BitsLandScape.bmp");

            //we save it as an image
            bmp.saveImage("../LdImgDmp16/default/16BitLandSc.pic");

            //we save load a 4 but image
            Bmp bmp2 = Bmp::fromBmpFile("../assets/sonic/4BitsSonic.bmp");

            //we turn it into 16 bits
            bmp2.convertTo16Bits();

            //we save it as an image
            bmp2.saveImage("../LdImgDmp16/default/16BSonic.pic");

        }
        break;

    case 4:
        {
            //we open a 24 bit image
            Bmp bmp = Bmp::fromBmpFile("../assets/images/24BitsLandScape.bmp");

            //we save it as an image (16 bit conversion is implicit)
            bmp.saveImage("../LoadImg16b/default/16BitLandSc.pic");

            //we open a 4 bit image
            Bmp bmp2 = Bmp::fromBmpFile("../assets/sonic/4BitsSonic.bmp");

            //we turn it into 16 bit
            bmp2.convertTo16Bits();

            //we save it as a 16 bit image
            bmp2.saveImage("../LoadImg16b/default/16BSonic.pic");

        }
        break;

    case 5:
        {
            Bmp bmp1 = Bmp::fromBmpFile("../assets/images/8BitsColors.bmp");

            bmp1.saveDump("../LdImgDmp8/default/8BitsColors.dmp");

            bmp1.savePalette("../LdImgDmp8/default/8BitsColors.pal");

        }
        break;

    case 6:
        {
            //we load an 8 bit image
            Bmp bmp1 = Bmp::fromBmpFile("../assets/images/8BitsColors.bmp");

            //we save it as an image
            bmp1.saveImage("../LoadImg8b/default/8BitsColors.pic");

            //we save the palete
            bmp1.savePalette("../LoadImg8b/default/8BitsColors.pal");

        }
        break;
    case 7:
        {
            //we load an image
            Bmp bmp2 = Bmp::fromBmpFile("../assets/images/4BitsLandScape.bmp");

            //we save it as a dump
            bmp2.saveDump("../LdImgDmp4/default/4BitLandSc.dmp");

            //we save the palete
            bmp2.savePalette("../LdImgDmp4/default/4BitLandSc.pal");
        }
        break;
    case 8:
        {
            /*
            Bmp bmp2 = Bmp::fromBmpFile("../assets/images/4BitsLandScape.bmp");

            bmp2.saveImage("../LoadImg4b/default/4BitLandSc.pic");

            bmp2.savePalette("../LoadImg4b/default/4BitLandSc.pal");
            */


            Bmp bmp2 = Bmp::fromBmpFile("../assets/images/Antonio4b.bmp");

            bmp2.saveImage("../LoadImg4b/default/Antonio4b.pic");

            bmp2.savePalette("../LoadImg4b/default/Antonio4b.pal");

            //32 x 48 sonic
            /*
            Bmp bmp3 = Bmp::fromBmpFile("../assets/images/4BitsSonic.bmp");

            bmp3.saveImage("../LoadImg4b/default/4BSonic.pic");

            bmp3.savePalette("../LoadImg4b/default/4BSonic.pal");
            */
        }
        break;
    case 9:
        {
            //PCGs::reformatPCG("sonic1.bmp", "sonic1.pcg");
            //Bmp bmp = Bmp::fromBmpFile("../assets/sonic/sonic1.bmp");

            //Palettes::savePalette(bmp, "../assets/sonicsonic.pal");

            //Image::saveAs4BitsImage(bmp, "../LoadImage/default/sonic1.pic");

            puts("Done");
        }
        break;

    case 10:
        {
            //PCGs::reformatPCG("sonic1.bmp", "sonic1.pcg");
            //Bmp bmp = Bmp::fromBmpFile("../assets/tiles2x2.bmp");
            //Bmp bmp = Bmp::fromBmpFile("../assets/tiles8x8.bmp");

            Bmp bmp = Bmp::fromBmpFile("../assets/sonic/4BitsSonic.bmp");


            // bmp.savePalette("../Sonic/default/tiles.pal");
            bmp.savePalette("../Sonic/default/sonic.pal");

            bmp.saveImage("../Sonic/default/tiles.pic");

            //bmp.saveTiles("../Sonic/default/tiles.til");

            bmp.saveSprites("../Sonic/default/tiles.til");

            puts("Done");
        }
        break;
    case 11: //sonic animation
        {
            Bmp bmp = Bmp::fromBmpFile("../assets/sonic/s_still.bmp");
            bmp.savePalette("../Sonic/default/sonic/sonic.pal");
            bmp.saveSprites("../Sonic/default/sonic/s_still.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/s_look_u.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_look_u.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/s_look_d.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_look_d.spt");


            bmp = Bmp::fromBmpFile("../assets/sonic/s_wait_0.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_wait_0.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/s_wait_1.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_wait_1.spt");


            bmp = Bmp::fromBmpFile("../assets/sonic/s_walk_0.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_walk_0.spt");

            bmp = Bmp::fromBmpFile("../assets/s_walk_1.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_walk_1.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/s_walk_2.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_walk_2.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/s_walk_3.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_walk_3.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/s_walk_4.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_walk_4.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/s_walk_5.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_walk_5.spt");


            bmp = Bmp::fromBmpFile("../assets/sonic/s_run_0.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_run_0.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/s_run_1.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_run_1.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/s_run_2.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_run_2.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/s_run_3.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_run_3.spt");


            bmp = Bmp::fromBmpFile("../assets/sonic/s_stop_0.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_stop_0.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/s_stop_1.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_stop_1.spt");


            bmp = Bmp::fromBmpFile("../assets/sonic/s_spin_0.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_spin_0.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/s_spin_1.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_spin_1.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/s_spin_2.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_spin_2.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/s_spin_3.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_spin_3.spt");

            puts("Done");
        }
        break;
    case 12: //tails aaimation
        {
            Bmp bmp = Bmp::fromBmpFile("../assets/sonic/t_still_0.bmp");
            bmp.savePalette("../Sonic/default/tails/tails.pal");
            bmp.saveSprites("../Sonic/default/tails/t_stil_0.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_still_1.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_stil_1.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_still_2.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_stil_2.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_still_3.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_stil_3.spt");



            bmp = Bmp::fromBmpFile("../assets/sonic/t_swing_0.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_swing_0.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_swing_1.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_swing_1.spt");



            bmp = Bmp::fromBmpFile("../assets/sonic/t_walk_0.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_walk_0.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_walk_1.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_walk_1.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_walk_2.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_walk_2.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_walk_3.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_walk_3.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_walk_4.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_walk_4.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_walk_5.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_walk_5.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_walk_6.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_walk_6.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_walk_7.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_walk_7.spt");



            bmp = Bmp::fromBmpFile("../assets/sonic/t_spin_d_0.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_spin_d_0.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_spin_d_1.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_spin_d_1.spt");



            bmp = Bmp::fromBmpFile("../assets/sonic/t_run_0.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_run_0.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_run_1.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_run_1.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_run_2.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_run_2.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_run_3.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_run_3.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_run_4.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_run_4.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_run_5.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_run_5.spt");



            bmp = Bmp::fromBmpFile("../assets/sonic/t_spin_0.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_spin_0.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_spin_1.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_spin_1.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_spin_2.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_spin_2.spt");



            bmp = Bmp::fromBmpFile("../assets/sonic/t_jump_0.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_jump_0.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_jump_1.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_jump_1.spt");



            bmp = Bmp::fromBmpFile("../assets/sonic/t_fly_0.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_fly_0.spt");

            bmp = Bmp::fromBmpFile("../assets/sonic/t_fly_1.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_fly_1.spt");


            puts("Done");
        }
        break;
    case 13:    //tetris
        {
            Bmp bmp = Bmp::fromBmpFile("../assets/tetris.bmp");

            bmp.savePalette("../Tetris68/default/tetris.pal");

            bmp.saveTiles("../Tetris68/default/tetris.til");
        }
        break;
    case 14:
        {

            Bmp bmp = Bmp::fromBmpFile("../assets/images/8BitsLandScape.bmp");

            bmp.savePalette("../RasterIm/default/landscap.pal");

            bmp.saveDump("../RasterIm/default/landscap.dmp");
            //bmp.saveImage("../RasterIm/default/landscap.pic");

            Bmp bmp2 = Bmp::fromBmpFile("../assets/images/8BitsColors.bmp");

            bmp2.savePalette("../RasterIm/default/colours.pal");

            bmp2.saveDump("../RasterIm/default/colours.dmp");
            //bmp2.saveImage("../RasterIm/default/colours.pic");

        }
        break;
    case 15:
        {
            Bmp bmp = Bmp::fromBmpFile("../assets/player2.bmp");
            bmp.savePalette("../RodLand/default/player1.pal");
            bmp.saveSprites("../RodLand/default/player1.spt");

        }
        break;
    case 17:
        {
            //we load an image
            Bmp bmp = Bmp::fromBmpFile("a../ssets/mariokart/4bitCircuit.bmp");

            //we save it as a image
            bmp.saveImage("../MarioKart/MarioKart_mariko/circuit.pic");

            //we save the palete
            bmp.savePalette("../MarioKart/MarioKart_mariko/circuit.pal");
        }
        break;
    case 18:
        {
            //we load an image
            Bmp bmp = Bmp::fromBmpFile("a../ssets/mariokart/circuit.bmp");

            //we save it as a image
            bmp.saveImage("../MarioKart/MarioKart_mariko/circuit.pic");

            //we save the palete
            bmp.savePalette("../MarioKart/MarioKart_mariko/circuit.pal");

            bmp.saveTiles("../MarioKart/MarioKart_mariko/circuit.til");
        }
        break;

     case 19:
        {
            //we load an image
            Bmp bmp = Bmp::fromBmpFile("../assets/mariokart/4bitCircuit.bmp");

            //we save the palete
            bmp.savePalette("../MarioKart2/MarioKart2_mariko/circuit.pal");

            //we save it as a image
            bmp.saveMultiPlanes("../MarioKart2/MarioKart2_mariko/circuit.pla");
        }
        break;

    case 20:
        {
            //we load an image
            Bmp bmp = Bmp::fromBmpFile("../assets/images/8BitsLandScapeAndColors.bmp");

            //we save the palete
            bmp.savePalette("../RasterIm/default/8BLndScpCols.pal");

            //we save it as a image
            bmp.saveMultiPlanes("../RasterIm/default/8BLndScpCols.pla");
        }
        break;
        //

    default:
        std::cout << "Nothing to do" << std::endl;
        break;
    }

    std::cout << "Done" << std::endl;
    return 0;
}

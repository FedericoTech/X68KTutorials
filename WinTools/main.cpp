#include <iostream>
#include <fstream>

#include "stdafx.h"


using namespace std;

int main(int argc, char *argv[])
{
    uint8_t option = atoi(argv[1]);

    option = 15;

    switch(option){
    case 0: //reformat pal
        {
            Palettes::reformatPalette("assets/palette.pal", "assets/palette_new.pal");
        }
        break;
    case 1:
        {
            PCGs::hardcodedPCG("assets/ship.pcg");
        }
        break;
    case 2:
        {
            Grafitti::doGrafitti("assets/R.bmp", "assets/grafitti.bmp", "assets/R.pic");
        }
        break;
    case 3:
        {

            Bmp bmp = Bmp::fromBmpFile("assets/24BitsLandScape.bmp");

            bmp.saveImage("../LdImgDmp16/default/16BitLandSc.pic");

            Bmp bmp2 = Bmp::fromBmpFile("assets/4BitsSonic.bmp");

            bmp2.convertTo16Bits();

            bmp2.saveImage("../LdImgDmp16/default/16BSonic.pic");

        }
        break;

    case 4:
        {

            Bmp bmp = Bmp::fromBmpFile("assets/24BitsLandScape.bmp");

            bmp.saveImage("../LoadImg16b/default/16BitLandSc.pic");

            Bmp bmp2 = Bmp::fromBmpFile("assets/4BitsSonic.bmp");

            bmp2.convertTo16Bits();

            bmp2.saveImage("../LoadImg16b/default/16BSonic.pic");

        }
        break;

    case 5:
        {
            Bmp bmp1 = Bmp::fromBmpFile("assets/8BitsColors.bmp");

            bmp1.saveDump("../LdImgDmp8/default/8BitsColors.dmp");

            bmp1.savePalette("../LdImgDmp8/default/8BitsColors.pal");

        }
        break;

    case 6:
        {
            Bmp bmp1 = Bmp::fromBmpFile("assets/8BitsColors.bmp");

            bmp1.saveImage("../LoadImg8b/default/8BitsColors.pic");

            bmp1.savePalette("../LoadImg8b/default/8BitsColors.pal");

        }
        break;
    case 7:
        {
            Bmp bmp2 = Bmp::fromBmpFile("assets/4BitsLandScape.bmp");

            bmp2.saveDump("../LdImgDmp4/default/4BitLandSc.dmp");

            bmp2.savePalette("../LdImgDmp4/default/4BitLandSc.pal");
        }
        break;
    case 8:
        {
            /*
            Bmp bmp2 = Bmp::fromBmpFile("assets/4BitsLandScape.bmp");

            bmp2.saveImage("../LoadImg4b/default/4BitLandSc.pic");

            bmp2.savePalette("../LoadImg4b/default/4BitLandSc.pal");
            */

            Bmp bmp2 = Bmp::fromBmpFile("assets/Antonio4b.bmp");

            bmp2.saveImage("../LoadImg4b/default/Antonio4b.pic");

            bmp2.savePalette("../LoadImg4b/default/Antonio4b.pal");

            //32 x 48 sonic
            /*
            Bmp bmp3 = Bmp::fromBmpFile("assets/4BitsSonic.bmp");

            bmp3.saveImage("../LoadImg4b/default/4BSonic.pic");

            bmp3.savePalette("../LoadImg4b/default/4BSonic.pal");
            */
        }
        break;
    case 9:
        {
            //PCGs::reformatPCG("sonic1.bmp", "sonic1.pcg");
            Bmp bmp = Bmp::fromBmpFile("assets/sonic1.bmp");

            Palettes::savePalette(bmp, "assets/sonic.pal");

            Image::saveAs4BitsImage(bmp, "../LoadImage/default/sonic1.pic");

            puts("Done");
        }
        break;

    case 10:
        {
            //PCGs::reformatPCG("sonic1.bmp", "sonic1.pcg");
            //Bmp bmp = Bmp::fromBmpFile("assets/tiles2x2.bmp");
            //Bmp bmp = Bmp::fromBmpFile("assets/tiles8x8.bmp");

            Bmp bmp = Bmp::fromBmpFile("assets/4BitsSonic.bmp");


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
            Bmp bmp = Bmp::fromBmpFile("assets/s_still.bmp");
            bmp.savePalette("../Sonic/default/sonic/sonic.pal");
            bmp.saveSprites("../Sonic/default/sonic/s_still.spt");

            bmp = Bmp::fromBmpFile("assets/s_look_u.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_look_u.spt");

            bmp = Bmp::fromBmpFile("assets/s_look_d.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_look_d.spt");


            bmp = Bmp::fromBmpFile("assets/s_wait_0.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_wait_0.spt");

            bmp = Bmp::fromBmpFile("assets/s_wait_1.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_wait_1.spt");


            bmp = Bmp::fromBmpFile("assets/s_walk_0.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_walk_0.spt");

            bmp = Bmp::fromBmpFile("assets/s_walk_1.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_walk_1.spt");

            bmp = Bmp::fromBmpFile("assets/s_walk_2.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_walk_2.spt");

            bmp = Bmp::fromBmpFile("assets/s_walk_3.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_walk_3.spt");

            bmp = Bmp::fromBmpFile("assets/s_walk_4.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_walk_4.spt");

            bmp = Bmp::fromBmpFile("assets/s_walk_5.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_walk_5.spt");


            bmp = Bmp::fromBmpFile("assets/s_run_0.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_run_0.spt");

            bmp = Bmp::fromBmpFile("assets/s_run_1.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_run_1.spt");

            bmp = Bmp::fromBmpFile("assets/s_run_2.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_run_2.spt");

            bmp = Bmp::fromBmpFile("assets/s_run_3.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_run_3.spt");


            bmp = Bmp::fromBmpFile("assets/s_stop_0.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_stop_0.spt");

            bmp = Bmp::fromBmpFile("assets/s_stop_1.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_stop_1.spt");


            bmp = Bmp::fromBmpFile("assets/s_spin_0.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_spin_0.spt");

            bmp = Bmp::fromBmpFile("assets/s_spin_1.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_spin_1.spt");

            bmp = Bmp::fromBmpFile("assets/s_spin_2.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_spin_2.spt");

            bmp = Bmp::fromBmpFile("assets/s_spin_3.bmp");
            bmp.saveSprites("../Sonic/default/sonic/s_spin_3.spt");

            puts("Done");
        }
        break;
    case 12: //tails aaimation
        {
            Bmp bmp = Bmp::fromBmpFile("assets/t_still_0.bmp");
            bmp.savePalette("../Sonic/default/tails/tails.pal");
            bmp.saveSprites("../Sonic/default/tails/t_stil_0.spt");

            bmp = Bmp::fromBmpFile("assets/t_still_1.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_stil_1.spt");

            bmp = Bmp::fromBmpFile("assets/t_still_2.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_stil_2.spt");

            bmp = Bmp::fromBmpFile("assets/t_still_3.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_stil_3.spt");



            bmp = Bmp::fromBmpFile("assets/t_swing_0.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_swing_0.spt");

            bmp = Bmp::fromBmpFile("assets/t_swing_1.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_swing_1.spt");



            bmp = Bmp::fromBmpFile("assets/t_walk_0.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_walk_0.spt");

            bmp = Bmp::fromBmpFile("assets/t_walk_1.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_walk_1.spt");

            bmp = Bmp::fromBmpFile("assets/t_walk_2.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_walk_2.spt");

            bmp = Bmp::fromBmpFile("assets/t_walk_3.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_walk_3.spt");

            bmp = Bmp::fromBmpFile("assets/t_walk_4.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_walk_4.spt");

            bmp = Bmp::fromBmpFile("assets/t_walk_5.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_walk_5.spt");

            bmp = Bmp::fromBmpFile("assets/t_walk_6.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_walk_6.spt");

            bmp = Bmp::fromBmpFile("assets/t_walk_7.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_walk_7.spt");



            bmp = Bmp::fromBmpFile("assets/t_spin_d_0.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_spin_d_0.spt");

            bmp = Bmp::fromBmpFile("assets/t_spin_d_1.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_spin_d_1.spt");



            bmp = Bmp::fromBmpFile("assets/t_run_0.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_run_0.spt");

            bmp = Bmp::fromBmpFile("assets/t_run_1.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_run_1.spt");

            bmp = Bmp::fromBmpFile("assets/t_run_2.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_run_2.spt");

            bmp = Bmp::fromBmpFile("assets/t_run_3.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_run_3.spt");

            bmp = Bmp::fromBmpFile("assets/t_run_4.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_run_4.spt");

            bmp = Bmp::fromBmpFile("assets/t_run_5.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_run_5.spt");



            bmp = Bmp::fromBmpFile("assets/t_spin_0.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_spin_0.spt");

            bmp = Bmp::fromBmpFile("assets/t_spin_1.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_spin_1.spt");

            bmp = Bmp::fromBmpFile("assets/t_spin_2.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_spin_2.spt");



            bmp = Bmp::fromBmpFile("assets/t_jump_0.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_jump_0.spt");

            bmp = Bmp::fromBmpFile("assets/t_jump_1.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_jump_1.spt");



            bmp = Bmp::fromBmpFile("assets/t_fly_0.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_fly_0.spt");

            bmp = Bmp::fromBmpFile("assets/t_fly_1.bmp");
            bmp.saveSprites("../Sonic/default/tails/t_fly_1.spt");


            puts("Done");
        }
        break;
    case 13:    //tetris
        {
            Bmp bmp = Bmp::fromBmpFile("assets/tetris.bmp");

            bmp.savePalette("../Tetris68/default/tetris.pal");

            bmp.saveTiles("../Tetris68/default/tetris.til");
        }
        break;
    case 14:
        {

            Bmp bmp = Bmp::fromBmpFile("assets/8BitsLandScape.bmp");

            bmp.savePalette("../RasterIm/default/landscap.pal");

            bmp.saveDump("../RasterIm/default/landscap.dmp");
            //bmp.saveImage("../RasterIm/default/landscap.pic");

            Bmp bmp2 = Bmp::fromBmpFile("assets/8BitsColors.bmp");

            bmp2.savePalette("../RasterIm/default/colours.pal");

            bmp2.saveDump("../RasterIm/default/colours.dmp");
            //bmp2.saveImage("../RasterIm/default/colours.pic");

        }
        break;
    case 15:
        {
            Bmp bmp = Bmp::fromBmpFile("assets/player2.bmp");
            bmp.savePalette("../RodLand/default/player1.pal");
            bmp.saveSprites("../RodLand/default/player1.spt");

        }
        break;
        /*
    case 6:
        ifstream imageFileIn("R.bmp", ios::binary);


        break;
        */
    default:
        cout << "Nothing to do" << endl;
        break;
    }

    cout << "Done" << endl;
    return 0;
}

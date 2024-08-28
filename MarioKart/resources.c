#include "stdafx.h"

#include "resources.h"


#define VERTICAL_BLANKING_DETECTION 0x80000000     //0b10000000000000000000000000000000
#define VERTICAL_BLANKING_NO_DETECT 0

#define SET_VBD_V(vbd, v) (vbd | v)

#define SP_DEFCG_8X8_TILE 0
#define SP_DEFCG_16X16_TILE 1

#define CIRCUIT_PALETTE "tiled.pal"
#define SPRITE_PALETTE "mk_sprite.pal"

#define CIRCUIT_TILESET "tiled.ts"

#define CIRCUIT_TILEMAP "tiled.tm"

static int16_t file_handler;
static union FileConf fconf;

const uint16_t* Res_getTilemap()
{
    return res_tilemap;
}

static inline int Res_loadCircuitPalette();

static inline int Res_loadSpritePalette();

static inline int Res_loadCircuitTileset();

static inline int Res_loadSpriteTileset();

static inline int Res_loadCircuitTilemap();

int Res_loadResources()
{
    int status;

    PRINT_FUNCTION();

    fconf.config = 0; //reset the whole union
    fconf.flags.access_dictionary = ACCESS_NORMAL;
    fconf.flags.sharing = SHARING_COMPATIBILITY_MODE;
    fconf.flags.mode = MODE_R;

    //if error loading circuit palette...
    if(status = Res_loadCircuitPalette()){
        return status;
    }

    //if error loading sprite palette...
    if(status = Res_loadSpritePalette()){
        return status;
    }

    //if error loading sprite palette...
    if(status = Res_loadCircuitTileset()){
        return status;
    }

    //if error loading sprite palette...
    if(status = Res_loadCircuitTilemap()){
        return status;
    }

    return 0;
}

int Res_unloadResources()
{
    int cont;

    //we clear the tiles
    for(cont = 0; cont < 255; cont++){
        _iocs_sp_cgclr(cont);
    }

    _dos_mfree(res_tilemap);

    return 0;
}

static inline int Res_loadCircuitPalette()
{
    int status = 0;

    // buffer for 3 palettes
    uint16_t s_colours[3][16] = {0};

    //we open the palette file
    int file_handler = _dos_open(
        CIRCUIT_PALETTE,
        fconf.config
    );

     PRINT_FUNCTION();

    //if any error...
    if(file_handler < 0){
        _dos_c_print("Can't open the file\r\n");
        return file_handler;
    }

    //we read the whole palette file
    if((status = _dos_read(file_handler, (char*)&s_colours, sizeof s_colours)) < 0){
        return status;
    }

    {
        uint8_t colour_in_palette, palette_index;
        for(palette_index = 0; palette_index < 3; palette_index++){
            for(colour_in_palette = 0; colour_in_palette < 16; colour_in_palette++){
                 status = _iocs_spalet( //returns 32 bit integer
                    SET_VBD_V(
                        VERTICAL_BLANKING_DETECTION,
                        colour_in_palette   //0 - 15 if higher it only takes from 0-15
                    ),
                    palette_index + 1,  //the first palette for tiles is 1. 0 is the palette for text
                    s_colours[palette_index][colour_in_palette]
                );

                //if any issue...
                if(status < 0){
                    _dos_c_print("The graphic screen is not initialized \r\n");
                    return status;
                }
            }
        }
    }

    //now we close the file
    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file\r\n");
        return status;
    }

    return 0;
}

static inline int Res_loadSpritePalette()
{
    int status = 0;
    // buffer for 3 palettes
    uint16_t s_colours[16] = {0};

    //we open the palette file
    int file_handler = _dos_open(
        SPRITE_PALETTE,
        fconf.config
    );

    //if any error...
    if(file_handler < 0){
        _dos_c_print("Can't open the file\r\n");
        return file_handler;
    }

    //we read the whole palette file
    if((status = _dos_read(file_handler, (char*)&s_colours, sizeof s_colours)) < 0){
        return status;
    }

    {
        uint8_t colour_in_palette;
        for(colour_in_palette = 0; colour_in_palette < 16; colour_in_palette++){
             status = _iocs_spalet( //returns 32 bit integer
                SET_VBD_V(
                    VERTICAL_BLANKING_DETECTION,
                    colour_in_palette   //0 - 15 if higher it only takes from 0-15
                ),
                4,  //next after the circuit palette
                s_colours[colour_in_palette]
            );

            //if any issue...
            if(status < 0){
                _dos_c_print("The graphic screen is not initialized \r\n");
                return status;
            }
        }
    }

    //now we close the file
    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file\r\n");
        return status;
    }

    return 0;
}

static inline int Res_loadCircuitTileset()
{
    int status;
    uint8_t tile[32];
    uint8_t cont;

    //we open the palette file
    int file_handler = _dos_open(
        CIRCUIT_TILESET,
        fconf.config
    );

    //if any error...
    if(file_handler < 0){
        _dos_c_print("Can't open the file\r\n");
        return file_handler;
    }

    //we go through the 84 tiles of the tileset
    for(cont = 1; cont < 72 /*84 */; cont++){
        //we read current tile
        if((status = _dos_read(file_handler, (char*) tile, sizeof tile)) < 0){
            return status;
        }

        //we load the tile as an 8 x 8 tile in "cont" position
        status = _iocs_sp_defcg(
            cont,               // position in 8 x 8 tiles
            SP_DEFCG_8X8_TILE,  // 8 x 8 tile = 1
            tile                // pointer to the data
        );

        if(status < 0){
            return status;
        }
    }

    //now we close the file
    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file\r\n");
        return status;
    }

    return 0;
}

static inline int Res_loadSpriteTileset()
{
    int status;
    uint8_t tile[128];
    uint8_t cont;

    //we open the palette file
    int file_handler = _dos_open(
        "mk_sprite.ts",
        fconf.config
    );

    //if any error...
    if(file_handler < 0){
        _dos_c_print("Can't open the file\r\n");
        return file_handler;
    }

    //we go through the 84 tiles of the tileset
    for(cont = 18 /*21*/; cont < 139; cont++){
        //we read current tile
        if((status = _dos_read(file_handler, (char*) tile, sizeof tile)) < 0){
            return status;
        }

        //we load the tile as an 8 x 8 tile in "cont" position
        status = _iocs_sp_defcg(
            cont,               // position in 8 x 8 tiles
            SP_DEFCG_16X16_TILE,  // 8 x 8 tile = 1
            tile                // pointer to the data
        );

        if(status < 0){
            return status;
        }
    }

    //now we close the file
    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file\r\n");
        return status;
    }

    return 0;
}

static inline int Res_loadCircuitTilemap()
{
    int status;

    //we open the palette file
    int file_handler = _dos_open(
        CIRCUIT_TILEMAP,
        fconf.config
    );

    //if any error...
    if(file_handler < 0){
        _dos_c_print("Can't open the file\r\n");
        return file_handler;
    }

    //we make room for a 128x128-tile tilemap
    res_tilemap = (uint16_t *) _dos_malloc(128 * 128 * sizeof(uint16_t));

    status = ((int)res_tilemap >> 24) & 0xFF;

    //if not enough space...
    if(status == 0x81){
        //we retrieve how much space is left
        printf("only this %d bytes are available\r\n", (uint16_t)res_tilemap & 0x00FFFFFF); //we retrieve the available amount
        return status;
    //if no room whatsoever
    } else if(status == 0x82) {
        printf("not size at all can be allocated. [%d]\r\n", (uint16_t)res_tilemap & 0x0000000F); //the last nibble is undefined
       return status;
    } else {
        #ifdef DEBUG
            printf("block successfully allocated [%x]\r\n", status);
        #endif
    }

    //we load the entire tilemap
    if((status = _dos_read(file_handler, (char*) res_tilemap, 128 * 128 * sizeof(uint16_t))) < 0){
        return status;
    }

    status = _dos_close(file_handler);

    //now we close the file
    if(status < 0){
        //if any error...
        _dos_c_print("Can't close the file\r\n");
        return status;
    }

    return 0;
}

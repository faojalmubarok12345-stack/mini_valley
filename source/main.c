#include <nds.h>
#include <stdio.h>
#include "tileset.h"
#include "player.h"
#include "plants.h"

#define GRID_W 8
#define GRID_H 6
#define TILE_PX 16

// 0 = Rumput, 1 = Cangkul, 2 = Bibit, 3 = Siram, 4 = Panen
int kebun[GRID_H][GRID_W];
int px = 3, py = 2;
int alat = 0; // 0: Cangkul, 1: Bibit, 2: Siram, 3: Panen
int hari = 1;
int uang = 100;

const char* namaAlat[] = {"CANGKUL", "BIBIT (10G)", "SIRAM", "PANEN"};

static u16 ambilTileIndex16(int sheetWidthPx, int col16, int row16) {
    int tilesPerRow = sheetWidthPx / 8;
    if (tilesPerRow <= 0) tilesPerRow = 16;
    int baseTile = (row16 * 2) * tilesPerRow + (col16 * 2);
    return (u16)baseTile;
}

static void tulisTile16KeBg(u16* mapPtr, int mapWidthTiles, int mapX16, int mapY16,
                           int sheetWidthPx, int col16, int row16, int palBank) {
    u16 t0 = ambilTileIndex16(sheetWidthPx, col16, row16);
    int tilesPerRow = sheetWidthPx / 8;
    if (tilesPerRow <= 0) tilesPerRow = 16;

    int x0 = mapX16 * 2;
    int y0 = mapY16 * 2;

    u16 attr00 = (u16)((t0) | (palBank << 12));
    u16 attr10 = (u16)((t0 + 1) | (palBank << 12));
    u16 attr01 = (u16)((t0 + tilesPerRow) | (palBank << 12));
    u16 attr11 = (u16)((t0 + tilesPerRow + 1) | (palBank << 12));

    mapPtr[y0 * mapWidthTiles + x0] = attr
        

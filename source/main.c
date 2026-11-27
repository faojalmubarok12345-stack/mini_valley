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

const char* namaAlat[] = {"CANGKUL", "BIBIT 10G", "SIRAM", "PANEN"};

static void tulisTile16KeBg(u16* mapPtr, int mapWidthTiles, int mapX16, int mapY16,
                           int tilesPerRow8, int col16, int row16, int palBank) {
    int baseTile = (row16 * 2) * tilesPerRow8 + (col16 * 2);
    int x0 = mapX16 * 2;
    int y0 = mapY16 * 2;

    mapPtr[y0 * mapWidthTiles + x0]             = (u16)((baseTile)                  | (palBank << 12));
    mapPtr[y0 * mapWidthTiles + x0 + 1]         = (u16)((baseTile + 1)              | (palBank << 12));
    mapPtr[(y0 + 1) * mapWidthTiles + x0]     = (u16)((baseTile + tilesPerRow8)     | (palBank << 12));
    mapPtr[(y0 + 1) * mapWidthTiles + x0 + 1] = (u16)((baseTile + tilesPerRow8 + 1) | (palBank << 12));
}

void tidurGantiHari(void) {
    hari++;
    for (int y = 0; y < GRID_H; y++) {
        for (int x = 0; x < GRID_W; x++) {
            if (kebun[y][x] == 3) {
                kebun[y][x] = 4;
            }
        }
    }
}

void gambarKebunGrafis(u16* bgMap, u16* cropMap) {
    for (int i = 0; i < 32 * 24; i++) {
        bgMap[i] = 0;
        cropMap[i] = 0;
    }

    int offsetX = 4;
    int offsetY = 3;

    // Hitung jumlah tile 8x8 per baris berdasarkan panjang data tilesetTiles (4bpp = 32 bytes/tile)
    int totalTiles8 = tilesetTilesLen / 32;
    int tilesPerRow8 = 16; // default 128px / 8px
    if (totalTiles8 >= 256) tilesPerRow8 = 16;
    if (totalTiles8 >= 512) tilesPerRow8 = 16;

    for (int y = 0; y < GRID_H; y++) {
        for (int x = 0; x < GRID_W; x++) {
            int gx = offsetX + x;
            int gy = offsetY + y;

            int tCol = 0, tRow = 0;
            if (kebun[y][x] >= 1) {
                tCol = 1;
                tRow = 0;
            }
            if (kebun[y][x] == 3) {
                tCol = 2;
                tRow = 0;
            }

            tulisTile16KeBg(bgMap, 32, gx, gy, tilesPerRow8, tCol, tRow, 0);

            if (kebun[y][x] >= 2) {
                int pCol = 0, pRow = 0;
                if (kebun[y][x] == 3) pCol = 1;
                if (kebun[y][x] == 4) pCol = 2;

                tulisTile16KeBg(cropMap, 32, gx, gy, 16, pCol, pRow, 1);
            }
        }
    }
}

void gambarLayarBawah(PrintConsole* bottom) {
    consoleSelect(bottom);
    consoleClear();
    printf("=== MINI VALLEY DS ===\n");
    printf("Hari:%d | Uang:%dG\n", hari, uang);
    printf("----------------------\n");
    printf("Alat : [%s]\n\n", namaAlat[alat]);
    printf("D-Pad: Jalan\n");
    printf("A    : Aksi Kebun\n");
    printf("L/R  : Ganti Alat\n\n");
    printf("======================\n");
    printf("[TAP LAYAR = TIDUR]\n");
    printf("======================\n");
}

int main(void) {
    PrintConsole bottomScreen;

    videoSetMode(MODE_0_2D | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankB(VRAM_B_MAIN_SPRITE);
    vramSetBankC(VRAM_C_SUB_BG);

    int bgKebun = bgInit(2, BgType_Text4bpp, BgSize_T_256x256, 28, 0);
    int bgTanaman = bgInit(3, BgType_Text4bpp, BgSize_T_256x256, 29, 2);
    bgSetPriority(bgKebun, 2);
    bgSetPriority(bgTanaman, 1);

    dmaCopy(tilesetTiles, bgGetGfxPtr(bgKebun), tilesetTilesLen);
    dmaCopy(tilesetPal, BG_PALETTE, tilesetPalLen);

    dmaCopy(plantsTiles, bgGetGfxPtr(bgTanaman), plantsTilesLen);
    dmaCopy(plantsPal, &BG_PALETTE[16], plantsPalLen);

    oamInit(&oamMain, SpriteMapping_1D_32, false);
    u16* sprGfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_16Color);
    dmaCopy(playerTiles, sprGfx, 16 * 16 / 2);
    dmaCopy(playerPal, SPRITE_PALETTE, playerPalLen);

    consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    u16* kebunMap = bgGetMapPtr(bgKebun);
    u16* tanamMap = bgGetMapPtr(bgTanaman);

    bool update = true;

    while (1) {
        swiWaitForVBlank();
        scanKeys();
        uint32_t keys = keysDown();

        if ((keys & KEY_UP) && py > 0) { py--; update = true; }
        if ((keys & KEY_DOWN) && py < GRID_H - 1) { py++; update = true; }
        if ((keys & KEY_LEFT) && px > 0) { px--; update = true; }
        if ((keys & KEY_RIGHT) && px < GRID_W - 1) { px++; update = true; }

        if (keys & KEY_R) { alat = (alat + 1) % 4; update = true; }
        if (keys & KEY_L) { alat = (alat + 3) % 4; update = true; }

        if (keys & KEY_A) {
            int tile = kebun[py][px];
            if (alat == 0 && tile == 0) kebun[py][px] = 1;
            else if (alat == 1 && tile == 1 && uang >= 10) {
                uang -= 10;
                kebun[py][px] = 2;
            }
            else if (alat == 2 && tile == 2) kebun[py][px] = 3;
            else if (alat == 3 && tile == 4) {
                kebun[py][px] = 1;
                uang += 35;
            }
            update = true;
        }

        if (keys & KEY_TOUCH) {
            tidurGantiHari();
            update = true;
        }

        if (update) {
            gambarKebunGrafis(kebunMap, tanamMap);
            gambarLayarBawah(&bottomScreen);

            int screenX = (4 + px) * TILE_PX;
            int screenY = (3 + py) * TILE_PX;

            oamSet(&oamMain,
                   0,
                   screenX, screenY,
                   0,
                   0,
                   SpriteSize_16x16,
                   SpriteColorFormat_16Color,
                   sprGfx,
                   -1,
                   false,
                   false,
                   false,
                   false,
                   false);

            oamUpdate(&oamMain);
            update = false;
        }
    }

    return 0;
}

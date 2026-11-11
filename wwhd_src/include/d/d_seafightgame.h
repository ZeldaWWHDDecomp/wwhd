/** Sea-fight minigame. GameCube layout retained in HD. */
#pragma once
#include "d/d_save_init.h"

struct dSeaFightGame_pos_c { be<u8> x, y; };
struct dSeaFightGame_ship_data {
    /* 0x0 */ be<u8> m_pos[4][2];
    /* 0x8 */ be<u8> mLength;  /* was field_0x8: put_ship 025BB198 stores the ship size (2-4); daMgBoard_c's MgBoardShipHD view (d_a_mgameboard.cpp) picks the ship model by it (mLength there) (tww the same) */
    /* 0x9 */ be<u8> mLife;  /* was field_0x9: put_ship 025BB198 sets the ship size; attack 025BB470 subtracts 1 per hit and counts the ship as sunk (mAliveShipNum - 1, mDeadShipNum + 1) at 0 (tww the same) */
    /* 0xA */ be<u8> field_0xa;
    /* 0xB */ be<u8> mX;  /* was field_0xb: put_ship 025BB198: the ship's first cell x; the board draw (d_a_mgameboard.cpp MgBoardShipHD::mX) places the ship model there (tww the same) */
    /* 0xC */ be<u8> mY;  /* was field_0xc: put_ship 025BB198: the ship's first cell y; the board draw (MgBoardShipHD::mY) places the ship model there (tww the same) */
    /* 0xD */ be<u8> mLengthX;  /* was field_0xd: put_ship 025BB198: the ship size when it lies along x (direction 1), else 0 (tww the same) */
    /* 0xE */ be<u8> mLengthY;  /* was field_0xe: put_ship 025BB198: the ship size when it lies along y (direction 0), else 0; the board draw (MgBoardShipHD::mVertical) turns the model by -0x8000 while it is non-zero, else 0x4000 (tww the same) */

    // HD: Full-width inputs preserve the PPC address wrap even outside the four normal positions.
    dSeaFightGame_pos_c* position(u32 index) {
        return gabi::at<dSeaFightGame_pos_c>(gabi::ea(m_pos) + index * 2);
    }
};
enum dSeaFightGame_cell_e { SEA_EMPTY = 0, SEA_MISS = 1, SEA_HIT = 3, SEA_OCCUPIED_LIMIT = 100, SEA_SHIP_BASE = 102 };
enum { MAX_SHIP = 4 };
struct dSeaFightGame_info_c {
    // HD: Retain the verified full-register arguments and s32 check result.
    s32 checkPutShip(s32 i_x, s32 i_y, s32 i_shipSize, s32 i_direction);
    s32 put_ship(u32 i_shipNo, u32 i_shipSize);
    s32 init(s32 i_bulletNum, u32 i_scenario);
    s32 attack(u32 i_x, u32 i_y);
    /* 0x00 */ be<u8> mGrid[8][8];
    /* 0x40 */ dSeaFightGame_ship_data mShips[4];
    /* 0x7C */ be<u8> mAliveShipNum;
    /* 0x7D */ be<u8> mBulletNum;
    /* 0x7E */ be<u8> mScore;
    u8 padding7F;
    /* 0x80 */ be<s32> mDeadShipNum;
    /* 0x84 */ be<u32> field_0x84[31];

    be<u8>& gridCell(u32 offset) {
        return *gabi::at<be<u8>>(gabi::ea(mGrid) + offset);
    }
    dSeaFightGame_ship_data* ship(u32 index) {
        return gabi::at<dSeaFightGame_ship_data>(gabi::ea(mShips) + index * sizeof(dSeaFightGame_ship_data));
    }
};
WWHD_SIZE(dSeaFightGame_pos_c, 2);
WWHD_OFFSET(dSeaFightGame_ship_data, mLife, 9);
WWHD_SIZE(dSeaFightGame_ship_data, 15);
WWHD_OFFSET(dSeaFightGame_info_c, mShips, 0x40);
WWHD_OFFSET(dSeaFightGame_info_c, mBulletNum, 0x7D);
WWHD_OFFSET(dSeaFightGame_info_c, mDeadShipNum, 0x80);
WWHD_SIZE(dSeaFightGame_info_c, 0x100);

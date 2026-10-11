/** d_seafightgame: HD compiler statics. */
#include "d/d_seafightgame.h"

static dHeaderStatics_c l_seaHeaderStatics;
static dHeaderStaticRecords_c l_seaRecords;
static f32 l_seaInitialFirst;
static f32 l_seaInitialSecond;
void sea_registerStatic(void*);
void sea_initFlag(void*);
void sea_initByte(void*);


static f32 l_seaGridScale;
static f32 l_seaDirectionScale;

/* 025BB358 */
s32 dSeaFightGame_info_c::init(s32 i_bulletNum, u32 i_scenario) {
    for (u32 x = 0; x < 8; x++) {
        for (u32 y = 0; y < 8; y++) mGrid[x][y] = SEA_EMPTY;
    }
    mBulletNum = i_bulletNum;
    mScore = 0;
    switch (i_scenario) {
    case 1:
        mAliveShipNum = 1;
        put_ship(0, 2);
        break;
    case 2:
        mAliveShipNum = 2;
        put_ship(0, 2);
        put_ship(1, 3);
        break;
    case 3:
        mAliveShipNum = 3;
        put_ship(0, 2);
        put_ship(1, 3);
        put_ship(2, 4);
        break;
    }
    mDeadShipNum = 0;
    return TRUE;
}

/* 025BB198 */
s32 dSeaFightGame_info_c::put_ship(u32 i_shipNo, u32 i_shipSize) {
    f32 gridScale = l_seaGridScale, directionScale = l_seaDirectionScale;
    u32 x, y, direction;
    do {
        s16 dir = (s16)gabi::ftoi(cM_rndF(directionScale));
        direction = (u8)(dir % 2);
        x = (u8)gabi::ftoi(cM_rndF(gridScale));
        y = (u8)gabi::ftoi(cM_rndF(gridScale));
    } while (!checkPutShip(x, y, (s32)i_shipSize, direction));
    auto* shipData = ship(i_shipNo);
    if (direction == 0) {
        for (u32 i = 0; i < i_shipSize && (s32)i_shipSize > 0; i++) {
            shipData->position(i)->x = x;
            shipData->position(i)->y = y + i;
            gridCell(x * 8 + y + i) = i_shipNo + SEA_SHIP_BASE;
        }
        // HD: The direction-size byte is stored before the origin coordinates.
        shipData->field_0xd = 0;
        shipData->field_0xb = x;
        shipData->field_0xc = y;
        shipData->field_0xe = i_shipSize;
    } else {
        for (u32 i = 0; i < i_shipSize && (s32)i_shipSize > 0; i++) {
            shipData->position(i)->x = x + i;
            shipData->position(i)->y = y;
            gridCell((x + i) * 8 + y) = i_shipNo + SEA_SHIP_BASE;
        }
        shipData->field_0xd = i_shipSize;
        shipData->field_0xb = x;
        shipData->field_0xc = y;
        shipData->field_0xe = 0;
    }
    shipData->field_0x8 = i_shipSize;
    shipData->field_0x9 = i_shipSize;
    return TRUE;
}

/* 025BB0E0 */
s32 dSeaFightGame_info_c::checkPutShip(s32 i_x, s32 i_y, s32 i_shipSize, s32 i_direction) {
    if (i_shipSize <= 0) return TRUE;
    u32 cell = (u32)i_x * 8 + (u32)i_y;
    if (i_direction == 0) {
        if (i_x > 7) return FALSE;
        for (s32 i = 0; i < i_shipSize; i++) {
            if (i_y > 7 || gridCell(cell) > SEA_OCCUPIED_LIMIT) return FALSE;
            cell++;
            i_y = (s32)((u32)i_y + 1);
        }
    } else {
        if (i_y > 7) return FALSE;
        for (s32 i = 0; i < i_shipSize; i++) {
            if (i_x > 7 || gridCell(cell) > SEA_OCCUPIED_LIMIT) return FALSE;
            cell += 8;
            i_x = (s32)((u32)i_x + 1);
        }
    }
    return TRUE;
}

/* 025BB470 */
s32 dSeaFightGame_info_c::attack(u32 i_x, u32 i_y) {
    u32 cell = i_x * 8 + i_y;
    u32 value = gridCell(cell);
    s32 rt = -1;
    if (value == SEA_EMPTY) {
        gridCell(cell) = SEA_MISS;
    } else if (value > SEA_OCCUPIED_LIMIT) {
        u32 ship_no = value - SEA_SHIP_BASE;
        // HD: An invalid occupied cell asserts and returns without consuming a bullet.
        if (ship_no >= MAX_SHIP) {
            JUTAssertion::showAssert("d_seafightgame.cpp", 0xAF, "ship_no >= 0 && ship_no < MAX_SHIP");
            return -1;
        }
        rt = ship_no;
        auto* shipData = ship(ship_no);
        u8 remaining = shipData->field_0x9 - 1;
        shipData->field_0x9 = remaining;
        if (remaining == 0) {
            u8 alive = mAliveShipNum;
            u32 dead = mDeadShipNum;
            mAliveShipNum = alive - 1;
            mDeadShipNum = (s32)(dead + 1);
        }
        gridCell(cell) = SEA_HIT;
    } else {
        return -2;
    }
    u8 bullets = mBulletNum, score = mScore;
    mBulletNum = bullets - 1;
    mScore = score + 1;
    return rt;
}

// HD: Compiler-generated initialization and destructor registration order.
/* 025BB568 */
void d_seafightgame_staticInit() {
    l_seaHeaderStatics.mZeroWords[2] = 0;
    l_seaHeaderStatics.mZeroWords[0] = 0;
    l_seaHeaderStatics.mZeroWords[3] = 0;
    l_seaHeaderStatics.mZeroWords[1] = 0;
    sea_registerStatic(&l_seaRecords.mFirst);
    f32 first = l_seaInitialFirst, second = l_seaInitialSecond;
    l_seaHeaderStatics.mFloatPair[0] = first;
    l_seaHeaderStatics.mFloatPair[1] = second;
    sea_initFlag(&l_seaHeaderStatics.mFlag);
    sea_registerStatic(&l_seaRecords.mSecond);
    sea_initByte(&l_seaHeaderStatics.mByte);
    sea_registerStatic(&l_seaRecords.mThird);
}

/* WWHD glue
ext 028F026C void sea_registerStatic(void*)
ext 028ED6F8 void sea_initFlag(void*)
ext 028EAB2C void sea_initByte(void*)
str 10054570
str 10054584
data l_seaGridScale 10054568
data l_seaDirectionScale 1005456C
data l_seaHeaderStatics 1047C964
data l_seaRecords 101EAE80
data l_seaInitialFirst 100545B4
data l_seaInitialSecond 100545B8
*/

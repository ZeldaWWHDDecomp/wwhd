/** HD rumble-pattern playback for Wii remotes and the GamePad.
 * GameCube-style source verified as m_Re_controller_pad; no GameCube pattern-player equivalent. */
#include "m_Re/m_Re_controller_pad.h"

static mReCPd_patternPlayer_c sPatternPlayers[4] = {};
static mReCPd_controllerGlobal_c sController = {};

enum { PAD_COUNT = 4 };

/* 025F29F0 */
void mReCPd_playerStart(mReCPd_patternPlayer_c* player, mReCPd_pattern_c* resource, u32 loop) {
    u16 length = resource->mLength;
    player->mLoop = u8(loop);
    player->mPosition = 0;
    player->mpPattern = resource->mBits;
    player->mLength = length;
}

/* 025F2A10 */
void mReCPd_playerStop(mReCPd_patternPlayer_c* player) {
    if (!player->mpPattern) return;
    u16 length = player->mLength;
    player->mLoop = 0;
    player->mPosition = length;
}

/* 025F2A30 */
void mReCPd_startPattern(u32 padNo, mReCPd_pattern_c* resource, u32 loop) {
    if (padNo >= PAD_COUNT) mReCPd_assert("m_Re_controller_pad.cpp", 0x34, "0 <= i_n && i_n < 4");
    mReCPd_playerStart(&sPatternPlayers[padNo], resource, loop);
}

/* 025F2AAC */
void mReCPd_stopPattern(u32 padNo) {
    if (padNo >= PAD_COUNT) mReCPd_assert("m_Re_controller_pad.cpp", 0x47, "0 <= i_n && i_n < 4");
    mReCPd_playerStop(&sPatternPlayers[padNo]);
}

/* 025F2B08 */
void mReCPd_update() {
    mReCPd_patternPlayer_c* player = sPatternPlayers;
    if (!sController.mpState->mGamePadRumble) {
        for (u32 i = 0; i < PAD_COUNT; ++i, ++player) {
            be<u8>* pattern = player->mpPattern;
            // HD: an idle player ends the entire update, including subsequent pads.
            if (!pattern) return;
            u32 position = player->mPosition;
            if (position >= player->mLength) {
                if (!player->mLoop) {
                    player->mpPattern = nullptr;
                    WPADControlMotor(i, 0);
                    continue;
                }
                pattern = player->mpPattern;
                position = 0;
                player->mPosition = 0;
            }
            u32 bit = pattern[s32(position) >> 3] & u32(0x80 >> (position & 7));
            // HD: active patterns drive remote 0; finished patterns stop their own remote.
            WPADControlMotor(0, bit != 0);
            player->mPosition = u16(player->mPosition + 1);
        }
        return;
    }
    be<u8>* pattern = player->mpPattern;
    if (!pattern) return;
    u32 length = player->mLength;
    if (player->mPosition >= length) {
        if (!player->mLoop) {
            player->mpPattern = nullptr;
            VPADStopMotor(0);
            return;
        }
        player->mPosition = 0;
        VPADControlMotor(0, pattern, length & 0xFF);
    } else if (!player->mPosition) {
        VPADControlMotor(0, pattern, length & 0xFF);
    }
    player->mPosition = u16(player->mPosition + 1);
}

/* 025F2C88 */
void mReCPd_stopAll() {
    for (u32 i = 0; i < PAD_COUNT; ++i) WPADControlMotor(i, 0);
    VPADStopMotor(0);
}

/* ---------------- HD: compiler-generated companion translation units ---------------- */

/* 025F2CDC */
void __sinit_m_Re_controller_pad_cpp() {
    sinit_header_statics_rodata(0x1048D55C, 0x101F4978, 0x1048D568, 0x1005914C);
}

/* 025F2D70 */
void mReCPd_emptyCallback1() {}

/* 025F2D74 */
void mReCPd_emptyCallback2() {}

/* 025F2D78 */
void __sinit_controller_pad_companion1_cpp() {
    sinit_header_statics_rodata(0x1048D578, 0x101F49CC, 0x1048D584, 0x10080144);
}

/* 025F2E0C */
void __sinit_controller_pad_companion2_cpp() {
    sinit_header_statics_rodata(0x1048D594, 0x101F49F0, 0x1048D5A0, 0x100C4594);
}

/* WWHD glue
data sPatternPlayers 101F499C nocheck
data sController 101F5088 nocheck
str 100590EC "m_Re_controller_pad.cpp" in 025F2A30
str 10059104 "0 <= i_n && i_n < 4" in 025F2A30
str 10059118 "m_Re_controller_pad.cpp" in 025F2AAC
str 10059130 "0 <= i_n && i_n < 4" in 025F2AAC
ext C0008398 void WPADControlMotor(u32 pad, u32 on)
ext C0007720 void VPADStopMotor(u32 pad)
ext C0007510 void VPADControlMotor(u32 pad, be<u8>* pattern, u32 length)
ext 0273AA24 void mReCPd_assert(const char* file, u32 line, const char* expression)
*/

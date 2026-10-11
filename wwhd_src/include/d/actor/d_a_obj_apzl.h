#pragma once
#include "bindings.h"

#include "gc_api/tu.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/f_op/f_op_msg.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/SSystem/c_math.h"
#include "d/d_save.h"
#include "d/d_vibration.h"
struct ApzlLinkage { /* 0x00 */
    be<u32> value;
};
WWHD_SIZE(ApzlLinkage, 4);
struct ApzlDestructorRecord { /* 0x00 */
    u8 data[12];
};
WWHD_SIZE(ApzlDestructorRecord, 12);
struct ApzlWordPosition {
    /* 0x00 */ be<u32> x;
    /* 0x04 */ be<u32> y;
    /* 0x08 */ be<u32> z;
};
WWHD_SIZE(ApzlWordPosition, 12);
struct ApzlSTControlVtable {
    /* 0x00 */ u8 _00[12];
    /* 0x0C */ gfn mDestructor;
};
WWHD_OFFSET(ApzlSTControlVtable, mDestructor, 12);
struct ApzlSTControl {
    /* 0x00 */ u8 _00[0x24];
    /* 0x24 */ gptr<ApzlSTControlVtable> __vtbl;
    void destroy(s32 flags) { gabi::call_ptr<void>(__vtbl->mDestructor.get(), this, flags); }
};
WWHD_OFFSET(ApzlSTControl, __vtbl, 0x24);
struct ApzlPad {
    /* 0x000 */ u8 _000[0x18];
    /* 0x018 */ be<u32> mButtonTrigger;
    /* 0x01C */ u8 _01C[4];
    /* 0x020 */ be<u32> mButtonRepeat;
    /* 0x024 */ u8 _024[0x100];
    /* 0x124 */ be<u32> mDirectionRestrict;
};
WWHD_OFFSET(ApzlPad, mButtonTrigger, 0x18);
WWHD_OFFSET(ApzlPad, mButtonRepeat, 0x20);
WWHD_OFFSET(ApzlPad, mDirectionRestrict, 0x124);
struct ApzlModelData {
    /* 0x00 */ u8 _00[0x38];
    /* 0x38 */ gptr<void> mpTexPatternAnm;
};
WWHD_OFFSET(ApzlModelData, mpTexPatternAnm, 0x38);
struct ApzlPlay {
    /* 0x0000 */ u8 _0000[0x52B8];
    /* 0x52B8 */ be<u16> mEventFlag;
    /* 0x52BA */ u8 _52BA[10];
    /* 0x52C4 */ dEvent_manager_c mEventManager;
    /* 0x52C4 + sizeof(manager) */ u8 _afterManager[0x599C - 0x52C4 - sizeof(dEvent_manager_c)];
    /* 0x599C */ dVibration_c mVibration;
    /* 0x599C + sizeof(vibration) */ u8 _afterVibration[0x5BB9 - 0x599C - sizeof(dVibration_c)];
    /* 0x5BB9 */ be<u8> mAStatusForce;
    /* 0x5BBA */ be<u8> mDoStatusForce;
};
WWHD_OFFSET(ApzlPlay, mEventManager, 0x52C4);
WWHD_OFFSET(ApzlPlay, mVibration, 0x599C);
WWHD_OFFSET(ApzlPlay, mAStatusForce, 0x5BB9);
using ApzlStringPointer = gptr<char>;
using ApzlMessagePointer = gptr<msg_class>;
using ApzlResourcePointer = gptr<void>;
using ApzlPadPointer = gptr<ApzlPad>;
enum ApzlHDConstants {
    APZL_ACTOR_VTABLE = 0x10024B14,
    APZL_BTP_CONSTRUCTOR = 0x025E7820,
    APZL_FIND_ITEM_BY_ID = 0x025E1234,
    APZL_MSG_DISPLAYED = 14,
    APZL_MSG_DESTROYED = 19,
    APZL_PAD_DIRECTION_MASK = 0xF00000,
    APZL_PAD_LEFT = 0x440000,
    APZL_PAD_RIGHT = 0x880000,
    APZL_PAD_UP = 0x110000,
    APZL_PAD_DOWN = 0x220000,
    JA_SE_TALK_CURSOR = 0x83D,
    JA_SE_15PUZZLE_START = 0x8F2,
    JA_SE_15PUZZLE_EXIT = 0x8F3,
    JA_SE_15PUZZLE_COMPLETE = 0x8F4,
};

enum dRes_INDEX_APZL {
    /* BDL  */
    dRes_INDEX_APZL_BDL_APZLP_e = 0x6,
    dRes_INDEX_APZL_BDL_APZLY_e = 0x7,
    /* BDLM */
    dRes_INDEX_APZL_BDL_VBSRP_e = 0xA,
    /* BMT  */
    dRes_INDEX_APZL_BMT_APZLP_I00_e = 0xD,
    dRes_INDEX_APZL_BMT_APZLP_I01_e = 0xE,
    dRes_INDEX_APZL_BMT_APZLP_I02_e = 0xF,
    dRes_INDEX_APZL_BMT_APZLP_I03_e = 0x10,
    dRes_INDEX_APZL_BMT_APZLP_I04_e = 0x11,
    dRes_INDEX_APZL_BMT_APZLP_I05_e = 0x12,
    dRes_INDEX_APZL_BMT_APZLP_I06_e = 0x13,
    dRes_INDEX_APZL_BMT_APZLP_I07_e = 0x14,
    dRes_INDEX_APZL_BMT_APZLP_I08_e = 0x15,
    dRes_INDEX_APZL_BMT_APZLP_I09_e = 0x16,
    dRes_INDEX_APZL_BMT_APZLP_I10_e = 0x17,
    dRes_INDEX_APZL_BMT_APZLP_I11_e = 0x18,
    dRes_INDEX_APZL_BMT_APZLP_I12_e = 0x19,
    dRes_INDEX_APZL_BMT_APZLP_I13_e = 0x1A,
    dRes_INDEX_APZL_BMT_APZLP_I14_e = 0x1B,
    dRes_INDEX_APZL_BMT_APZLP_I15_e = 0x1C,
    dRes_INDEX_APZL_BMT_APZLP_J00_e = 0x1D,
    dRes_INDEX_APZL_BMT_APZLP_J01_e = 0x1E,
    dRes_INDEX_APZL_BMT_APZLP_J02_e = 0x1F,
    dRes_INDEX_APZL_BMT_APZLP_J03_e = 0x20,
    dRes_INDEX_APZL_BMT_APZLP_J04_e = 0x21,
    dRes_INDEX_APZL_BMT_APZLP_J05_e = 0x22,
    dRes_INDEX_APZL_BMT_APZLP_J06_e = 0x23,
    dRes_INDEX_APZL_BMT_APZLP_J07_e = 0x24,
    dRes_INDEX_APZL_BMT_APZLP_J08_e = 0x25,
    dRes_INDEX_APZL_BMT_APZLP_J09_e = 0x26,
    dRes_INDEX_APZL_BMT_APZLP_J10_e = 0x27,
    dRes_INDEX_APZL_BMT_APZLP_J11_e = 0x28,
    dRes_INDEX_APZL_BMT_APZLP_J12_e = 0x29,
    dRes_INDEX_APZL_BMT_APZLP_J13_e = 0x2A,
    dRes_INDEX_APZL_BMT_APZLP_J14_e = 0x2B,
    dRes_INDEX_APZL_BMT_APZLP_J15_e = 0x2C,
    dRes_INDEX_APZL_BMT_APZLP_K00_e = 0x2D,
    dRes_INDEX_APZL_BMT_APZLP_K01_e = 0x2E,
    dRes_INDEX_APZL_BMT_APZLP_K02_e = 0x2F,
    dRes_INDEX_APZL_BMT_APZLP_K03_e = 0x30,
    dRes_INDEX_APZL_BMT_APZLP_K04_e = 0x31,
    dRes_INDEX_APZL_BMT_APZLP_K05_e = 0x32,
    dRes_INDEX_APZL_BMT_APZLP_K06_e = 0x33,
    dRes_INDEX_APZL_BMT_APZLP_K07_e = 0x34,
    dRes_INDEX_APZL_BMT_APZLP_K08_e = 0x35,
    dRes_INDEX_APZL_BMT_APZLP_K09_e = 0x36,
    dRes_INDEX_APZL_BMT_APZLP_K10_e = 0x37,
    dRes_INDEX_APZL_BMT_APZLP_K11_e = 0x38,
    dRes_INDEX_APZL_BMT_APZLP_K12_e = 0x39,
    dRes_INDEX_APZL_BMT_APZLP_K13_e = 0x3A,
    dRes_INDEX_APZL_BMT_APZLP_K14_e = 0x3B,
    dRes_INDEX_APZL_BMT_APZLP_K15_e = 0x3C,
    dRes_INDEX_APZL_BMT_APZLP_L00_e = 0x3D,
    dRes_INDEX_APZL_BMT_APZLP_L01_e = 0x3E,
    dRes_INDEX_APZL_BMT_APZLP_L02_e = 0x3F,
    dRes_INDEX_APZL_BMT_APZLP_L03_e = 0x40,
    dRes_INDEX_APZL_BMT_APZLP_L04_e = 0x41,
    dRes_INDEX_APZL_BMT_APZLP_L05_e = 0x42,
    dRes_INDEX_APZL_BMT_APZLP_L06_e = 0x43,
    dRes_INDEX_APZL_BMT_APZLP_L07_e = 0x44,
    dRes_INDEX_APZL_BMT_APZLP_L08_e = 0x45,
    dRes_INDEX_APZL_BMT_APZLP_L09_e = 0x46,
    dRes_INDEX_APZL_BMT_APZLP_L10_e = 0x47,
    dRes_INDEX_APZL_BMT_APZLP_L11_e = 0x48,
    dRes_INDEX_APZL_BMT_APZLP_L12_e = 0x49,
    dRes_INDEX_APZL_BMT_APZLP_L13_e = 0x4A,
    dRes_INDEX_APZL_BMT_APZLP_L14_e = 0x4B,
    dRes_INDEX_APZL_BMT_APZLP_L15_e = 0x4C,
    dRes_INDEX_APZL_BMT_APZLP_M00_e = 0x4D,
    dRes_INDEX_APZL_BMT_APZLP_M01_e = 0x4E,
    dRes_INDEX_APZL_BMT_APZLP_M02_e = 0x4F,
    dRes_INDEX_APZL_BMT_APZLP_M03_e = 0x50,
    dRes_INDEX_APZL_BMT_APZLP_M04_e = 0x51,
    dRes_INDEX_APZL_BMT_APZLP_M05_e = 0x52,
    dRes_INDEX_APZL_BMT_APZLP_M06_e = 0x53,
    dRes_INDEX_APZL_BMT_APZLP_M07_e = 0x54,
    dRes_INDEX_APZL_BMT_APZLP_M08_e = 0x55,
    dRes_INDEX_APZL_BMT_APZLP_M09_e = 0x56,
    dRes_INDEX_APZL_BMT_APZLP_M10_e = 0x57,
    dRes_INDEX_APZL_BMT_APZLP_M11_e = 0x58,
    dRes_INDEX_APZL_BMT_APZLP_M12_e = 0x59,
    dRes_INDEX_APZL_BMT_APZLP_M13_e = 0x5A,
    dRes_INDEX_APZL_BMT_APZLP_M14_e = 0x5B,
    dRes_INDEX_APZL_BMT_APZLP_M15_e = 0x5C,
    dRes_INDEX_APZL_BMT_APZLP_N00_e = 0x5D,
    dRes_INDEX_APZL_BMT_APZLP_N01_e = 0x5E,
    dRes_INDEX_APZL_BMT_APZLP_N02_e = 0x5F,
    dRes_INDEX_APZL_BMT_APZLP_N03_e = 0x60,
    dRes_INDEX_APZL_BMT_APZLP_N04_e = 0x61,
    dRes_INDEX_APZL_BMT_APZLP_N05_e = 0x62,
    dRes_INDEX_APZL_BMT_APZLP_N06_e = 0x63,
    dRes_INDEX_APZL_BMT_APZLP_N07_e = 0x64,
    dRes_INDEX_APZL_BMT_APZLP_N08_e = 0x65,
    dRes_INDEX_APZL_BMT_APZLP_N09_e = 0x66,
    dRes_INDEX_APZL_BMT_APZLP_N10_e = 0x67,
    dRes_INDEX_APZL_BMT_APZLP_N11_e = 0x68,
    dRes_INDEX_APZL_BMT_APZLP_N12_e = 0x69,
    dRes_INDEX_APZL_BMT_APZLP_N13_e = 0x6A,
    dRes_INDEX_APZL_BMT_APZLP_N14_e = 0x6B,
    dRes_INDEX_APZL_BMT_APZLP_N15_e = 0x6C,
    dRes_INDEX_APZL_BMT_APZLP_O00_e = 0x6D,
    dRes_INDEX_APZL_BMT_APZLP_O01_e = 0x6E,
    dRes_INDEX_APZL_BMT_APZLP_O02_e = 0x6F,
    dRes_INDEX_APZL_BMT_APZLP_O03_e = 0x70,
    dRes_INDEX_APZL_BMT_APZLP_O04_e = 0x71,
    dRes_INDEX_APZL_BMT_APZLP_O05_e = 0x72,
    dRes_INDEX_APZL_BMT_APZLP_O06_e = 0x73,
    dRes_INDEX_APZL_BMT_APZLP_O07_e = 0x74,
    dRes_INDEX_APZL_BMT_APZLP_O08_e = 0x75,
    dRes_INDEX_APZL_BMT_APZLP_O09_e = 0x76,
    dRes_INDEX_APZL_BMT_APZLP_O10_e = 0x77,
    dRes_INDEX_APZL_BMT_APZLP_O11_e = 0x78,
    dRes_INDEX_APZL_BMT_APZLP_O12_e = 0x79,
    dRes_INDEX_APZL_BMT_APZLP_O13_e = 0x7A,
    dRes_INDEX_APZL_BMT_APZLP_O14_e = 0x7B,
    dRes_INDEX_APZL_BMT_APZLP_O15_e = 0x7C,
    dRes_INDEX_APZL_BMT_APZLP_P00_e = 0x7D,
    dRes_INDEX_APZL_BMT_APZLP_P01_e = 0x7E,
    dRes_INDEX_APZL_BMT_APZLP_P02_e = 0x7F,
    dRes_INDEX_APZL_BMT_APZLP_P03_e = 0x80,
    dRes_INDEX_APZL_BMT_APZLP_P04_e = 0x81,
    dRes_INDEX_APZL_BMT_APZLP_P05_e = 0x82,
    dRes_INDEX_APZL_BMT_APZLP_P06_e = 0x83,
    dRes_INDEX_APZL_BMT_APZLP_P07_e = 0x84,
    dRes_INDEX_APZL_BMT_APZLP_P08_e = 0x85,
    dRes_INDEX_APZL_BMT_APZLP_P09_e = 0x86,
    dRes_INDEX_APZL_BMT_APZLP_P10_e = 0x87,
    dRes_INDEX_APZL_BMT_APZLP_P11_e = 0x88,
    dRes_INDEX_APZL_BMT_APZLP_P12_e = 0x89,
    dRes_INDEX_APZL_BMT_APZLP_P13_e = 0x8A,
    dRes_INDEX_APZL_BMT_APZLP_P14_e = 0x8B,
    dRes_INDEX_APZL_BMT_APZLP_P15_e = 0x8C,
    dRes_INDEX_APZL_BMT_APZLP_Q00_e = 0x8D,
    dRes_INDEX_APZL_BMT_APZLP_Q01_e = 0x8E,
    dRes_INDEX_APZL_BMT_APZLP_Q02_e = 0x8F,
    dRes_INDEX_APZL_BMT_APZLP_Q03_e = 0x90,
    dRes_INDEX_APZL_BMT_APZLP_Q04_e = 0x91,
    dRes_INDEX_APZL_BMT_APZLP_Q05_e = 0x92,
    dRes_INDEX_APZL_BMT_APZLP_Q06_e = 0x93,
    dRes_INDEX_APZL_BMT_APZLP_Q07_e = 0x94,
    dRes_INDEX_APZL_BMT_APZLP_Q08_e = 0x95,
    dRes_INDEX_APZL_BMT_APZLP_Q09_e = 0x96,
    dRes_INDEX_APZL_BMT_APZLP_Q10_e = 0x97,
    dRes_INDEX_APZL_BMT_APZLP_Q11_e = 0x98,
    dRes_INDEX_APZL_BMT_APZLP_Q12_e = 0x99,
    dRes_INDEX_APZL_BMT_APZLP_Q13_e = 0x9A,
    dRes_INDEX_APZL_BMT_APZLP_Q14_e = 0x9B,
    dRes_INDEX_APZL_BMT_APZLP_Q15_e = 0x9C,
    dRes_INDEX_APZL_BMT_APZLP_R00_e = 0x9D,
    dRes_INDEX_APZL_BMT_APZLP_R01_e = 0x9E,
    dRes_INDEX_APZL_BMT_APZLP_R02_e = 0x9F,
    dRes_INDEX_APZL_BMT_APZLP_R03_e = 0xA0,
    dRes_INDEX_APZL_BMT_APZLP_R04_e = 0xA1,
    dRes_INDEX_APZL_BMT_APZLP_R05_e = 0xA2,
    dRes_INDEX_APZL_BMT_APZLP_R06_e = 0xA3,
    dRes_INDEX_APZL_BMT_APZLP_R07_e = 0xA4,
    dRes_INDEX_APZL_BMT_APZLP_R08_e = 0xA5,
    dRes_INDEX_APZL_BMT_APZLP_R09_e = 0xA6,
    dRes_INDEX_APZL_BMT_APZLP_R10_e = 0xA7,
    dRes_INDEX_APZL_BMT_APZLP_R11_e = 0xA8,
    dRes_INDEX_APZL_BMT_APZLP_R12_e = 0xA9,
    dRes_INDEX_APZL_BMT_APZLP_R13_e = 0xAA,
    dRes_INDEX_APZL_BMT_APZLP_R14_e = 0xAB,
    dRes_INDEX_APZL_BMT_APZLP_R15_e = 0xAC,
    dRes_INDEX_APZL_BMT_APZLP_S00_e = 0xAD,
    dRes_INDEX_APZL_BMT_APZLP_S01_e = 0xAE,
    dRes_INDEX_APZL_BMT_APZLP_S02_e = 0xAF,
    dRes_INDEX_APZL_BMT_APZLP_S03_e = 0xB0,
    dRes_INDEX_APZL_BMT_APZLP_S04_e = 0xB1,
    dRes_INDEX_APZL_BMT_APZLP_S05_e = 0xB2,
    dRes_INDEX_APZL_BMT_APZLP_S06_e = 0xB3,
    dRes_INDEX_APZL_BMT_APZLP_S07_e = 0xB4,
    dRes_INDEX_APZL_BMT_APZLP_S08_e = 0xB5,
    dRes_INDEX_APZL_BMT_APZLP_S09_e = 0xB6,
    dRes_INDEX_APZL_BMT_APZLP_S10_e = 0xB7,
    dRes_INDEX_APZL_BMT_APZLP_S11_e = 0xB8,
    dRes_INDEX_APZL_BMT_APZLP_S12_e = 0xB9,
    dRes_INDEX_APZL_BMT_APZLP_S13_e = 0xBA,
    dRes_INDEX_APZL_BMT_APZLP_S14_e = 0xBB,
    dRes_INDEX_APZL_BMT_APZLP_S15_e = 0xBC,
    dRes_INDEX_APZL_BMT_APZLP_T00_e = 0xBD,
    dRes_INDEX_APZL_BMT_APZLP_T01_e = 0xBE,
    dRes_INDEX_APZL_BMT_APZLP_T02_e = 0xBF,
    dRes_INDEX_APZL_BMT_APZLP_T03_e = 0xC0,
    dRes_INDEX_APZL_BMT_APZLP_T04_e = 0xC1,
    dRes_INDEX_APZL_BMT_APZLP_T05_e = 0xC2,
    dRes_INDEX_APZL_BMT_APZLP_T06_e = 0xC3,
    dRes_INDEX_APZL_BMT_APZLP_T07_e = 0xC4,
    dRes_INDEX_APZL_BMT_APZLP_T08_e = 0xC5,
    dRes_INDEX_APZL_BMT_APZLP_T09_e = 0xC6,
    dRes_INDEX_APZL_BMT_APZLP_T10_e = 0xC7,
    dRes_INDEX_APZL_BMT_APZLP_T11_e = 0xC8,
    dRes_INDEX_APZL_BMT_APZLP_T12_e = 0xC9,
    dRes_INDEX_APZL_BMT_APZLP_T13_e = 0xCA,
    dRes_INDEX_APZL_BMT_APZLP_T14_e = 0xCB,
    dRes_INDEX_APZL_BMT_APZLP_T15_e = 0xCC,
    dRes_INDEX_APZL_BMT_APZLP_U00_e = 0xCD,
    dRes_INDEX_APZL_BMT_APZLP_U01_e = 0xCE,
    dRes_INDEX_APZL_BMT_APZLP_U02_e = 0xCF,
    dRes_INDEX_APZL_BMT_APZLP_U03_e = 0xD0,
    dRes_INDEX_APZL_BMT_APZLP_U04_e = 0xD1,
    dRes_INDEX_APZL_BMT_APZLP_U05_e = 0xD2,
    dRes_INDEX_APZL_BMT_APZLP_U06_e = 0xD3,
    dRes_INDEX_APZL_BMT_APZLP_U07_e = 0xD4,
    dRes_INDEX_APZL_BMT_APZLP_U08_e = 0xD5,
    dRes_INDEX_APZL_BMT_APZLP_U09_e = 0xD6,
    dRes_INDEX_APZL_BMT_APZLP_U10_e = 0xD7,
    dRes_INDEX_APZL_BMT_APZLP_U11_e = 0xD8,
    dRes_INDEX_APZL_BMT_APZLP_U12_e = 0xD9,
    dRes_INDEX_APZL_BMT_APZLP_U13_e = 0xDA,
    dRes_INDEX_APZL_BMT_APZLP_U14_e = 0xDB,
    dRes_INDEX_APZL_BMT_APZLP_U15_e = 0xDC,
    dRes_INDEX_APZL_BMT_APZLP_V00_e = 0xDD,
    dRes_INDEX_APZL_BMT_APZLP_V01_e = 0xDE,
    dRes_INDEX_APZL_BMT_APZLP_V02_e = 0xDF,
    dRes_INDEX_APZL_BMT_APZLP_V03_e = 0xE0,
    dRes_INDEX_APZL_BMT_APZLP_V04_e = 0xE1,
    dRes_INDEX_APZL_BMT_APZLP_V05_e = 0xE2,
    dRes_INDEX_APZL_BMT_APZLP_V06_e = 0xE3,
    dRes_INDEX_APZL_BMT_APZLP_V07_e = 0xE4,
    dRes_INDEX_APZL_BMT_APZLP_V08_e = 0xE5,
    dRes_INDEX_APZL_BMT_APZLP_V09_e = 0xE6,
    dRes_INDEX_APZL_BMT_APZLP_V10_e = 0xE7,
    dRes_INDEX_APZL_BMT_APZLP_V11_e = 0xE8,
    dRes_INDEX_APZL_BMT_APZLP_V12_e = 0xE9,
    dRes_INDEX_APZL_BMT_APZLP_V13_e = 0xEA,
    dRes_INDEX_APZL_BMT_APZLP_V14_e = 0xEB,
    dRes_INDEX_APZL_BMT_APZLP_V15_e = 0xEC,
    dRes_INDEX_APZL_BMT_APZLP_W00_e = 0xED,
    dRes_INDEX_APZL_BMT_APZLP_W01_e = 0xEE,
    dRes_INDEX_APZL_BMT_APZLP_W02_e = 0xEF,
    dRes_INDEX_APZL_BMT_APZLP_W03_e = 0xF0,
    dRes_INDEX_APZL_BMT_APZLP_W04_e = 0xF1,
    dRes_INDEX_APZL_BMT_APZLP_W05_e = 0xF2,
    dRes_INDEX_APZL_BMT_APZLP_W06_e = 0xF3,
    dRes_INDEX_APZL_BMT_APZLP_W07_e = 0xF4,
    dRes_INDEX_APZL_BMT_APZLP_W08_e = 0xF5,
    dRes_INDEX_APZL_BMT_APZLP_W09_e = 0xF6,
    dRes_INDEX_APZL_BMT_APZLP_W10_e = 0xF7,
    dRes_INDEX_APZL_BMT_APZLP_W11_e = 0xF8,
    dRes_INDEX_APZL_BMT_APZLP_W12_e = 0xF9,
    dRes_INDEX_APZL_BMT_APZLP_W13_e = 0xFA,
    dRes_INDEX_APZL_BMT_APZLP_W14_e = 0xFB,
    dRes_INDEX_APZL_BMT_APZLP_W15_e = 0xFC,
    dRes_INDEX_APZL_BMT_APZLP_X00_e = 0xFD,
    dRes_INDEX_APZL_BMT_APZLP_X01_e = 0xFE,
    dRes_INDEX_APZL_BMT_APZLP_X02_e = 0xFF,
    dRes_INDEX_APZL_BMT_APZLP_X03_e = 0x100,
    dRes_INDEX_APZL_BMT_APZLP_X04_e = 0x101,
    dRes_INDEX_APZL_BMT_APZLP_X05_e = 0x102,
    dRes_INDEX_APZL_BMT_APZLP_X06_e = 0x103,
    dRes_INDEX_APZL_BMT_APZLP_X07_e = 0x104,
    dRes_INDEX_APZL_BMT_APZLP_X08_e = 0x105,
    dRes_INDEX_APZL_BMT_APZLP_X09_e = 0x106,
    dRes_INDEX_APZL_BMT_APZLP_X10_e = 0x107,
    dRes_INDEX_APZL_BMT_APZLP_X11_e = 0x108,
    dRes_INDEX_APZL_BMT_APZLP_X12_e = 0x109,
    dRes_INDEX_APZL_BMT_APZLP_X13_e = 0x10A,
    dRes_INDEX_APZL_BMT_APZLP_X14_e = 0x10B,
    dRes_INDEX_APZL_BMT_APZLP_X15_e = 0x10C,
    /* BTP  */
    dRes_INDEX_APZL_BTP_VBSRP_e = 0x10F,
};

/** HD: enlarged actor base and BTP animation objects. The GC current-message pointer
 * becomes mMessageStarted; the message manager is an HD singleton. */
struct daObjApzl_c : fopAc_ac_c {
    enum EventIdx { EVENT_TALK, EVENT_GAME, EVENT_RUPEE };
    enum States { STATE_IDLE, STATE_TALK, STATE_START, STATE_PLAYING, STATE_END, STATE_RUPEE };
    enum Rupee {
        ITEM_GREEN_RUPEE = 1,
        ITEM_BLUE_RUPEE,
        ITEM_YELLOW_RUPEE,
        ITEM_RED_RUPEE,
        ITEM_PURPLE_RUPEE
    };
    enum Direction { DIRECTION_UP, DIRECTION_LEFT, DIRECTION_DOWN, DIRECTION_RIGHT };
    enum Action {
        ACTION_WAIT,
        ACTION_TALK,
        ACTION_GAME,
        ACTION_GETITEM,
        ACTION_STOP,
        ACTION_SOUND,
        ACTION_PUSH_A,
        ACTION_EVENT_END
    };
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpPieceModel[16];
    /* 0x3F4 */ gptr<J3DModel> mpArrowModel[4];
    /* 0x404 */ gptr<J3DModel> mpScoreboardModel[16];
    /* 0x444 */ mDoExt_btpAnm mScoreboardBtpAnm[16];
    /* 0xB84 */ be<u8> mPuzzleNo;
    /* 0xB85 */ u8 _B85[3];
    /* 0xB88 */ gptr<ApzlSTControl> stick;
    /* 0xB8C */ be<u8> mType;
    /* 0xB8D */ be<u8> mBlankIdx;
    /* 0xB8E */ be<u8> mSwappedPieceIdx;
    /* 0xB8F */ be<u8> mMoveTimer;
    /* 0xB90 */ be<u8> mMoveDirection;
    /* 0xB91 */ be<u8> mShowBlankPiece;
    /* 0xB92 */ be<u8> mPiecePos[16];
    /* 0xBA2 */ be<u8> mState;
    /* 0xBA3 */ be<u8> mDrawArrow[4];
    /* 0xBA7 */ u8 _BA7;
    /* 0xBA8 */ be<s16> mEventIdx[3];
    /* 0xBAE */ be<s8> mActIdx;
    /* 0xBAF */ be<u8> mQuitGame;
    /* 0xBB0 */ be<u8> mGameStarted;
    /* 0xBB1 */ be<u8> mGameCleared;
    /* 0xBB2 */ be<u8> mGaveReward;
    /* 0xBB3 */ be<u8> mShownRewardMessage;
    /* 0xBB4 */ be<u8> mPlayedStartSound;
    /* 0xBB5 */ u8 _BB5[3];
    /* 0xBB8 */ be<u32> mRupeeIds[30];
    /* 0xC30 */ be<s32> mGivenRupeeCount;
    /* 0xC34 */ be<u32> mMsgNo;
    /* 0xC38 */ be<u32> mCurrMsgId;
    /* 0xC3C */ be<u8> mMessageStarted;
    /* 0xC3D */ u8 _C3D[3];
    /* 0xC40 */ be<s32> mRewardTimer;
    BOOL CreateHeap();
    void randamize_piece();
    BOOL check_clear();
    void set_mtx();
    void CreateInit();
    s32 _create();
    BOOL _delete();
    BOOL _draw();
    BOOL _execute();
    u32 getMsg();
    u16 next_msgStatus(u32 *);
    u16 talk(s32);
    void privateCut();
    void save_piece();
    BOOL move_piece();
    void check_arrow_draw();
    void swap_piece(u8, u8);
    u8 search_piece(u8);
    u8 getblank() { return mPiecePos[(u8)mBlankIdx]; }
};
WWHD_OFFSET(daObjApzl_c, mPhs, 0x3AC);
WWHD_OFFSET(daObjApzl_c, mpPieceModel, 0x3B4);
WWHD_OFFSET(daObjApzl_c, mpArrowModel, 0x3F4);
WWHD_OFFSET(daObjApzl_c, mpScoreboardModel, 0x404);
WWHD_OFFSET(daObjApzl_c, mScoreboardBtpAnm, 0x444);
WWHD_OFFSET(daObjApzl_c, mPuzzleNo, 0xB84);
WWHD_OFFSET(daObjApzl_c, stick, 0xB88);
WWHD_OFFSET(daObjApzl_c, mPiecePos, 0xB92);
WWHD_OFFSET(daObjApzl_c, mState, 0xBA2);
WWHD_OFFSET(daObjApzl_c, mEventIdx, 0xBA8);
WWHD_OFFSET(daObjApzl_c, mRupeeIds, 0xBB8);
WWHD_OFFSET(daObjApzl_c, mMessageStarted, 0xC3C);
WWHD_SIZE(daObjApzl_c, 0xC44);

// HD: private bindings retain full return registers and the original outgoing arguments.
void apzlConstructArray(void *, u32, u32, u32);
s32 apzlEntrySolidHeap(fopAc_ac_c *, u32, u32);
void apzlActorDestructor(fopAc_ac_c *, s32);
u32 apzlMessageSet(msg_class *, u32, cXyz *);
u32 apzlMessageStatus(msg_class *);
void apzlMessageSetStatus(msg_class *, u32);
void *apzlGetResource(void *, SafeString *, s32);
ApzlSTControl *apzlSTControlCreate(ApzlSTControl *, s32, s32, s32, s32, f32, f32, s32, s32);
void apzlSTControlCheckTrigger(ApzlSTControl *);
s32 apzlGetEventIdx(dEvent_manager_c *, const char *, u32);
s32 apzlEventEndCheck(dEvent_manager_c *, s16);
void apzlStartSound(u32);
void apzlOrderChangeEvent(fopAc_ac_c *, s16, u8, u16);
void apzlStartShock(dVibration_c *, s32, s32, cXyz *);
s32 apzlCheckBTrigger(s32);
s32 apzlCheckATrigger(s32);
s32 apzlGetMyStaffId(dEvent_manager_c *, const char *, fopAc_ac_c *, s32);
s32 apzlGetMyActIdx(dEvent_manager_c *, s32, const ApzlStringPointer *, s32, s32, s32);
void apzlCutEnd(dEvent_manager_c *, s32);
s32 apzlGetIsAdvance(dEvent_manager_c *, s32);
fopAc_ac_c *apzlFindItem(u32, void *);
fopAc_ac_c *apzlFastCreateItem(cXyz *, u32, s8, csXyz *, u32, s32, s32, f32, f32, f32);
void apzlSetItemTimer(fopAc_ac_c *, s32);
s32 apzlSoundIsPlaying(u32);
void apzlRegisterGlobal(ApzlDestructorRecord *);
void apzlLimitInit(be<u8> *);
void apzlAngleInit(be<u8> *);

WWHD_SIZE(ApzlSTControlVtable, 0x10);
WWHD_SIZE(ApzlSTControl, 0x28);
WWHD_SIZE(ApzlPad, 0x128);
WWHD_SIZE(ApzlModelData, 0x3C);
WWHD_SIZE(ApzlPlay, 0x5BBB);

/** Game application framework. */
#include "f_ap/f_ap_game.h"

static fapGm_HIO_c g_HIO;
static u8 developmentMode;
static const GameApplicationConstants l_constants;
static GameApplicationHeaderStatics l_headerStatics;
enum { gc_tuHIO_VT = 0x10056E04 };

/* 025D4384 */
static fapGm_HIO_c* fapGm_HIO_c__ct(fapGm_HIO_c* i_this) {
    // HD: preserve nullable allocation and the compiler's packed initialization order.
    if (!i_this) {
        i_this = static_cast<fapGm_HIO_c*>(operator_new(sizeof(fapGm_HIO_c)));
        if (!i_this) return nullptr;
    }
    i_this->mLetterBottomShadowColor = 0xFFFFFFFF;
    i_this->mColor = 0xFFFFFFFF;
    i_this->mLetterTopColor = 0xFFFFFFFF;
    i_this->__vtbl = gc_tuHIO_VT;
    i_this->mLetterBottomColor = 0xFFFFFFFF;
    i_this->mUsingHostIO = 1;
    i_this->mLetterTopShadowColor = 0xFFFFFFFF;
    bool dev = developmentMode != 0;
    i_this->mDisplayMeter = dev ? 1 : 0;
    i_this->field_0x10 = 0;
    i_this->field_0x30 = 0;
    i_this->field_0x09 = 1;
    i_this->field_0x14 = 10;
    i_this->field_0x15 = 12;
    i_this->field_0x11 = 0;
    i_this->mDisplayFlag = dev ? 1 : 0;
    i_this->field_0x1b = 1;
    i_this->field_0x18 = 8;
    i_this->mAspectRatio = f32(l_constants.aspectRatio);
    i_this->field_0x12 = 8;
    i_this->mEfbHeight = 480;
    i_this->field_0x17 = 8;
    i_this->field_0x1c = 0;
    i_this->field_0x13 = 8;
    i_this->field_0x16 = 10;
    i_this->field_0x0a = 1;
    i_this->field_0x1a = 0;
    i_this->field_0x19 = -1;
    i_this->mFbWidth = 640;
    i_this->mColor = 0xFFFFFFFF;
    i_this->mTriggerThreshLo = f32(l_constants.triggerThreshLo);
    i_this->mTriggerThreshHi = f32(l_constants.triggerThreshHi);
    i_this->mLetterTopColor = 0xFF9600FF;
    i_this->mLetterBottomColor = 0xFF7800FF;
    i_this->mLetterTopShadowColor = 0x000000FF;
    i_this->mLetterBottomShadowColor = 0x000000FF;
    i_this->mLetterPositionY = 10;
    i_this->mLetterFontSize = 27;
    i_this->mLineSpacing = 0;
    i_this->mBackgroundAlpha = 130;
    i_this->mLetterSpacing = 0;
    i_this->mLetterPositionX = 0;
    return i_this;
}

/* 025D42C4 */
void fapGm_After() {
    fopScnM_Management();
    fopOvlpM_Management();
    fopCamM_Management();
}

/* 025D42EC */
void fapGm_Execute() {
    fpcM_Management(0, fapGm_After);
    cCt_Counter(0);
}

/* 025D4320 */
void fapGm_Create() {
    fpcM_Init();
    fopScnM_Init();
    fopOvlpM_Init();
    fopCamM_Init();
    fopDwTg_CreateQueue();
    fopScnM_CreateReq(fpcNm_LOGO_SCENE_e, fpcNm_INVALID_e, 0, 0);
    g_HIO.mNo = mDoHIO_createChild("ゲームシステム", &g_HIO);
}

/* 025D4564 */
static void __sinit_f_ap_game_cpp() {
    // HD: common-header statics and constructor registrations are retained in order.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F3038);
    l_headerStatics.negativePi = f32(l_constants.negativePi);
    l_headerStatics.positivePi = f32(l_constants.positivePi);
    GameApplicationFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F3044);
    GameApplicationSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F3050);
    fapGm_HIO_c__ct(&g_HIO);
}

/* WWHD glue (tools/verify/autoglue.py):
data g_HIO 104873CC
data developmentMode 101F48C0
data l_constants 10056E24
data l_headerStatics 104873B0
str 10056E14 "\u30b2\u30fc\u30e0\u30b7\u30b9\u30c6\u30e0" renamed
ext 025DC98C void fopScnM_Management()
ext 025DBEEC void fopOvlpM_Management()
ext 025DA750 void fopCamM_Management()
ext 025DF948 void fpcM_Management(u32, fapGm_AfterCallback)
ext 0200E6EC void cCt_Counter(u32)
ext 025DFA7C void fpcM_Init()
ext 025DCA60 void fopScnM_Init()
ext 025DC04C void fopOvlpM_Init()
ext 025DA754 void fopCamM_Init()
ext 025DA8AC void fopDwTg_CreateQueue()
ext 025DC91C void fopScnM_CreateReq(u32, u32, u32, u32)
ext 028ED6F8 void GameApplicationFirstHeaderObject_ct(u8*)
ext 028EAB2C void GameApplicationSecondHeaderObject_ct(u8*)
*/

/* Environmental effects and menu sky lighting, verified against WWHD. */
#include "d/d_kyeff.h"
static f32 l_eyeX = 9377.0;
static f32 l_zero = 0.0;
static f32 l_eyeZ = 7644.0;
static f32 l_centerX = 4300.0;
static f32 l_centerY = 4200.0;
static f32 l_centerZ = 1000.0;
static f32 l_windPower = 0.699999988079071;
// HD: advances twice as fast as the GameCube menu clock.
static f32 l_timeStep = 0.05999999865889549;
static f32 l_unsignedLimit = 2147483648.0;
static f32 l_dayDegrees = 360.0;
static f32 l_hourDegrees = 15.0;
static f32 l_microseconds = 1000000.0;
static f32 l_one = 1.0;
static f32 l_menuTime = 150.0;

/* inlined into dKyeff_execute and dKyeff_Create */
static bool stageEquals(const char* text) {
    gabi::Local<kyStageString> left, right;
    left->mStringTop = gabi::guest_addr(text);
    left->mVtable = 0x100504D0;
    right->mStringTop = gabi::ea(dKy_getGameInfo()->mStartStageName);
    right->mVtable = 0x100504D0;
    gabi::call_ptr<void>(gabi::at<gfn>(left->mVtable)[5].get(), left.get());
    gabi::call_ptr<void>(gabi::at<gfn>(left->mVtable)[5].get(), left.get());
    u32 a = left->mStringTop;
    gabi::call_ptr<void>(gabi::at<gfn>(right->mVtable)[5].get(), right.get());
    u32 b = right->mStringTop;
    if (a == b)
        return true;
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 x = gabi::at<be<u8>>(a)[i], y = gabi::at<be<u8>>(b)[i];
        if (x != y)
            return false;
        if (!x)
            return true;
    }
    return false;
}
/* 02585AC0 */
u32 dKyeff_Draw() {
    dKyw_wether_draw();
    return 1;
}
/* 02585AE4 */
s16 s16_data_ratio_set(s32 a, s32 b, f32 blend) {
    s32 delta = (s32)((u32)b - (u32)a);
    s16 step = (s16)gabi::ftoi(gabi::fmuls_ppc((f32)delta, blend));
    return (s16)((u32)a + (s32)step);
}
/* 02585B34 */
f32 f32_data_ratio_set(f32 a, f32 b, f32 blend) {
    return gabi::fmadds(gabi::fsubs_ppc(b, a), blend, a);
}
// HD: the verified entry ignores this; keep its zero-argument guest ABI.
/* 02585B40 */
u32 dKyeff_execute() {
    if (!stageEquals("Name")) {
        dKyw_wether_move();
        dKyw_wether_move_draw();
        return 1;
    }
    kyCameraView* camera = dKy_getGameInfo()->mCamera;
    camera->mLookat[0] = l_eyeX;
    camera->mLookat[1] = l_zero;
    camera->mLookat[2] = l_eyeZ;
    camera->mLookat[3] = l_centerX;
    camera->mLookat[4] = l_centerY;
    camera->mLookat[5] = l_centerZ;
    f32 zero = l_zero, one = l_one;
    dKy_getEnvironment()->mWindPower = l_windPower;
    f32 time = dKy_getEnvironment()->mCurTime;
    kyEnvironmentView* env = dKy_getEnvironment();
    env->mCurTime = gabi::fadds_ppc(env->mCurTime, l_timeStep);
    f32 now = dKy_getEnvironment()->mCurTime, threshold = l_unsignedLimit;
    u32 integer = now < threshold ? (u32)gabi::ftoi(now)
                                  : (u32)gabi::ftoi(gabi::fsubs_ppc(now, threshold)) + 0x80000000u;
    if (!((f32)integer < l_dayDegrees))
        dKy_getEnvironment()->mCurTime = zero;
    mDoAud_setHour(gabi::ftoi((f32)((f64)time / l_hourDegrees)));
    s32 scaled = gabi::ftoi(gabi::fmuls_ppc(time, l_microseconds));
    s32 remainder = scaled % 15000000;
    mDoAud_setMinute(gabi::ftoi((f32)((f64)(f32)remainder / l_microseconds)));
    env = dKy_getEnvironment();
    env->mpSchejule = dKyd_schejule_menu_getp();
    u32 a = 0, b = 0;
    f32 blend = zero;
    bool found = false;
    for (u32 i = 0; i < 11; ++i) {
        f32 start = dKy_getEnvironment()->mpSchejule.get()[i].mTimeEnd;
        if (time < start)
            continue;
        f32 end = dKy_getEnvironment()->mpSchejule.get()[i].mTimeBegin;
        if (time > end)
            continue;
        a = dKy_getEnvironment()->mpSchejule.get()[i].mPalIdx0;
        b = dKy_getEnvironment()->mpSchejule.get()[i].mPalIdx1;
        end = dKy_getEnvironment()->mpSchejule.get()[i].mTimeBegin;
        start = dKy_getEnvironment()->mpSchejule.get()[i].mTimeEnd;
        f32 width = gabi::fsubs_ppc(end, start);
        blend = one;
        if (width != zero) {
            blend = gabi::fsubs_ppc(one, (f32)((f64)gabi::fsubs_ppc(end, time) / width));
            if (!(blend < one))
                blend = one;
        }
        found = true;
        break;
    }
    if (!found)
        kyAssertion("d_kyeff.cpp", 0xF1, "0");
    env = dKy_getEnvironment();
    env->mpVrboxInfo = dKyd_dmvrbox_menu_getp();
    kyVrboxView* palA = dKy_getEnvironment()->mpVrboxInfo.get() + a;
    kyVrboxView* palB = dKy_getEnvironment()->mpVrboxInfo.get() + b;
    // HD: colors are byte components; reload the environment before every component.
    for (u32 i = 0; i < 3; ++i) {
        kyEnvironmentView* out = dKy_getEnvironment();
        s32 x = palA->mSkyColor[i], y = palB->mSkyColor[i];
        out->mVrSkyColor[i] = s16_data_ratio_set(x, y, blend);
    }
    dKy_getEnvironment()->mVrSkyColor[3] = 255;
    for (u32 i = 0; i < 3; ++i) {
        kyEnvironmentView* out = dKy_getEnvironment();
        s32 x = palA->mUsoUmiColor[i], y = palB->mUsoUmiColor[i];
        out->mVrUsoUmiColor[i] = s16_data_ratio_set(x, y, blend);
    }
    env = dKy_getEnvironment();
    s16 alphaA = (s16)((s16)gabi::ftoi(palA->mUsoUmiAlpha) * 255);
    s16 alphaB = (s16)((s16)gabi::ftoi(palB->mUsoUmiAlpha) * 255);
    env->mVrUsoUmiColor[3] = s16_data_ratio_set((s32)alphaA, (s32)alphaB, blend);
    for (u32 i = 0; i < 7; ++i) {
        kyEnvironmentView* out = dKy_getEnvironment();
        s32 x = palA->mKumoColors[i], y = palB->mKumoColors[i];
        out->mVrkumoColors[i] = s16_data_ratio_set(x, y, blend);
    }
    for (u32 i = 0; i < 3; ++i) {
        kyEnvironmentView* out = dKy_getEnvironment();
        s32 x = palA->mKasumiMaeColor[i], y = palB->mKasumiMaeColor[i];
        out->mVrKasumiMaeOpaqueColor[i] = s16_data_ratio_set(x, y, blend);
    }
    dKy_getEnvironment()->mVrKasumiMaeOpaqueColor[3] = 255;
    // HD: four additional palette parameters; their individual names are not established.
    const u32 paletteIndices[] = {2, 0, 1, 3};
    const u32 environmentIndices[] = {0, 2, 3, 4};
    for (u32 i = 0; i < 4; ++i) {
        env = dKy_getEnvironment();
        f32 x = palA->mHdPaletteParams[paletteIndices[i]],
            y = palB->mHdPaletteParams[paletteIndices[i]];
        env->mHdPaletteParams[environmentIndices[i]] = f32_data_ratio_set(x, y, blend);
    }
    for (u32 i = 0; i < 3; ++i) {
        kyEnvironmentView* out = dKy_getEnvironment();
        s32 x = palA->mKasumiMaeColor[i], y = palB->mKasumiMaeColor[i];
        out->mVrKasumiMaeColor[i] = s16_data_ratio_set(x, y, blend);
    }
    dKyw_wether_move_draw();
    return 1;
}
/* 02586254 */
u32 dKyeff_Execute() {
    return dKyeff_execute();
}
/* 02586258 */
u32 dKyeff_IsDelete() {
    return 1;
}
/* 02586260 */
u32 dKyeff_Delete(dKyeff_c* self) {
    if (self)
        fopKy_destructor(self, 0);
    dKyw_wether_delete();
    return 1;
}
/* 02586294 */
u32 dKyeff_Create(dKyeff_c* self) {
    if (self) {
        fopKy_constructor(self);
        self->base.__vtbl = 0x100504E8;
    }
    dKyw_wether_init_hd();
    if (stageEquals("Name")) {
        dKy_getEnvironment()->mWindVec[0] = l_one;
        dKy_getEnvironment()->mWindVec[1] = l_zero;
        dKy_getEnvironment()->mWindVec[2] = l_zero;
        dKy_getEnvironment()->mWindPower = l_windPower;
        dKy_getEnvironment()->mCurTime = l_menuTime;
        return cPhs_COMPLEATE_e;
    }
    if (!dKy_checkEventNightStop())
        return cPhs_COMPLEATE_e;
    kyStageView* stage = &dKy_getGameInfo()->mStage;
    kyStageInfo* info = gabi::call_ptr<kyStageInfo*>(stage->mVtable.get()[0x15C / 4].get(), stage);
    u32 type = (info->mStageType >> 16) & 7;
    if (type == kyeff::dStageType_OUTDOORS_e || type == kyeff::dStageType_SEA_e) {
        dKyw_rain_set(250);
        dKy_getEnvironment()->mThunderMode = 1;
        return cPhs_COMPLEATE_e;
    }
    if (type != kyeff::dStageType_MISC_e)
        return cPhs_COMPLEATE_e;
    const char* names[] = {"Ocrogh",  "Omori",   "Orichh", "Atorizk", "LinkRM",
                           "Ojhous2", "Onobuta", "Omasao", "Obombh",  "Opub"};
    for (const char* name : names)
        if (stageEquals(name)) {
            dKyw_rain_set(250);
            dKy_getEnvironment()->mThunderMode = 10;
            break;
        }
    return cPhs_COMPLEATE_e;
}
/* 02586B14 */
static void __sinit_d_kyeff_cpp() {
    sinit_header_statics_rodata(0x104775D8, 0x101E9AE4, 0x104775E4, 0x100505A8);
}

// HD: compiler-generated SafeString deleting destructor.
/* 02586BA8 */
static void SafeString_deletingDtor(kyStageString* p, u32 flags) {
    if (p == 0)
        return;
    if (flags & 1)
        operator_delete(p);
}

/* 02586BBC */
static void SafeString_assureTerminationImpl(kyStageString* p) {}
/* WWHD glue
data l_eyeX 10050504
data l_zero 10050508
data l_eyeZ 1005050C
data l_centerX 10050510
data l_centerY 10050514
data l_centerZ 10050518
data l_windPower 1005051C
data l_timeStep 10050520
data l_unsignedLimit 10050524
data l_dayDegrees 10050530
data l_hourDegrees 10050534
data l_microseconds 10050538
data l_one 1005053C
data l_menuTime 10050548
ext 02555D0C kyEnvironmentView* dKy_getEnvironment()
ext 025200D4 kyGameInfoView* dKy_getGameInfo()
ext 0257CE00 void dKyw_wether_draw()
ext 0257836C void dKyw_wether_move()
ext 0257ACAC void dKyw_wether_move_draw()
ext 025E1F80 void mDoAud_setHour(s32 hour)
ext 025E1F90 void mDoAud_setMinute(s32 minute)
ext 02563A1C dKyd_Schedule* dKyd_schejule_menu_getp()
ext 025639F8 kyVrboxView* dKyd_dmvrbox_menu_getp()
ext 0273AA24 void kyAssertion(const char* file, u32 line, const char* condition)
ext 025DD630 void fopKy_destructor(dKyeff_c* process, s32 flags)
ext 025DD5F0 void fopKy_constructor(dKyeff_c* process)
ext 0257802C void dKyw_wether_delete()
ext 02577CB0 void dKyw_wether_init_hd()
ext 02556BC0 u32 dKy_checkEventNightStop()
ext 0257E7C0 void dKyw_rain_set(u32 count)
str 10050540 "Name" in 02585B40
str 1005056C "Name" in 02586294
str 10050574 "Ocrogh"
str 10050564 "Omori"
str 1005057C "Orichh"
str 1005054C "Atorizk"
str 10050584 "LinkRM"
str 10050554 "Ojhous2"
str 1005055C "Onobuta"
str 1005058C "Omasao"
str 10050594 "Obombh"
str 1005059C "Opub"
str 100504F8 "d_kyeff.cpp"
str 100504CC "0"
*/

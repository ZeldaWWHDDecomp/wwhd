// HUD trailing range: GameCube names and pane fields with verified HD behavior.
#include "d/d_meter_02.h"
using namespace meter_detail;
using namespace gabi;
using GuestWord = be<u32>;
using GuestFloat = be<f32>;
using GuestHalf = be<s16>;
using GuestByte = be<u8>;
struct ThreeHalves {
    GuestHalf values[3];
};
struct EightBytes {
    GuestByte values[8];
};
struct ThreeBytes {
    GuestByte values[3];
};
static GuestWord l_saveRoot;
static GuestByte dMeter_collectMode;
static GuestByte dMeter_auctionFlag;
static GuestByte dMenu_menuFlag;
static GuestByte dMenu_menuButton;
static GuestByte dMenu_pause;
static GuestByte dMeter_itemMode;
static GuestByte rupy_soundSetFlag;
static GuestByte rupy_soundOnFlag;
static GuestHalf l_xyFadeCount;
static GuestHalf l_xyFadeMode;
static GuestHalf l_rFadeCount;
static GuestHalf l_rFadeMode;
static GuestHalf l_rPaneCount;
static GuestByte l_buttonAlphaState;
static GuestByte l_metronomeVisible;
static GuestByte l_magicPulse;
static GuestByte l_bossVisible;
static GuestByte l_zakoVisible;
static GuestByte l_itemButtonMode;
static GuestFloat l_bossHealth;
static GuestFloat l_zakoHealth;
static GuestWord l_arrowCapacityInitialized;
static GuestWord l_bombCapacityInitialized;
static GuestWord l_metronomeInitialized;
static GuestWord l_metronomeStateInitialized;
static GuestByte l_arrowCapacity;
static GuestByte l_bombCapacity;
static GuestByte l_metronomeFlag;
static GuestByte dMeter_windStatus;
static GuestByte l_cb1FlightActive;
static GuestHalf l_cb1FlightTime;
static GuestByte l_mdFlightActive;
static GuestHalf l_mdFlightTime;
static const GuestFloat l_one;
static const GuestFloat l_zero;
static const GuestFloat l_magicLengthScale;
static const GuestFloat l_half;
static const GuestFloat l_magicGaugeScale;
static const GuestFloat l_magicGaugeDivisor;
static const GuestFloat l_screenHalfHeight;
static const GuestFloat l_screenHalfWidth;
static const GuestFloat l_bowAlphaMin;
static const GuestFloat l_bowPulseDepth;
static ThreeHalves l_xyPaneCount;
static EightBytes dMenu_baitNum;
static const ThreeBytes l_itemShowMask;
static cXyz l_zakoPosition;
static dMeter_HIO_c g_meterHIO;
static GuestWord l_safeStringVtable;
// HD: named guest callback binding until autoglue supports external callback registration.
static GuestByte l_actorNameJudge;
static GuestByte l_bossStage0;        // Xboss0
static GuestByte l_bossStage1;        // Xboss1
static GuestByte l_bossStage2;        // Xboss2
static GuestByte l_bossStage3;        // Xboss3
static GuestByte l_magicStage;        // Ojhous
static GuestByte l_magicCounterStage; // Ojhous
static GuestByte l_assertFile;        // d_meter.cpp
static GuestByte l_stageAssertion;    // stage_info != NULL
static u32 meterStage() {
    auto data = &meterGame(dComIfGp_ea())->play.mStageData;
    auto stage = reinterpret_cast<StageData *>(data);
    return stage->mVtable->getStag(stage);
}
// HD: SafeString expands to three virtual cleanup calls before the bounded byte comparison.
static bool meterSceneEquals(u32 name) {
    gabi::Local<ScenePair> pair;
    pair->left.text = name;
    pair->left.vtable = gabi::ea(&l_safeStringVtable);
    u32 state = dComIfGp_ea();
    pair->right.text = gabi::ea(&meterGame(state)->play.mCurStage.mName);
    pair->right.vtable = gabi::ea(&l_safeStringVtable);
    auto left = &pair->left;
    auto right = &pair->right;
    auto leftVtable = gabi::at<SafeStringVtable>((u32)left->vtable);
    leftVtable->cleanup(left);
    leftVtable = gabi::at<SafeStringVtable>((u32)left->vtable);
    leftVtable->cleanup(left);
    u32 old = left->text;
    auto rightVtable = gabi::at<SafeStringVtable>((u32)right->vtable);
    rightVtable->cleanup(right);
    if (old == (u32)right->text)
        return true;
    auto a = gabi::at<const be<u8>>((u32)left->text);
    auto b = gabi::at<const be<u8>>((u32)right->text);
    for (u32 i = 0; i < 0x40001; i++) {
        u8 x = a[i], y = b[i];
        if (x != y)
            return false;
        if (!x)
            return true;
    }
    return false;
}

/* 0259C0EC */
void dMeter_magicAlpha(sub_meter_class *meter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 flags = (u32)meter->mStatusFlags;
    bool hide = true;
    bool input = false;
    if (!(flags & (dMtrStts_UNK4000_e)) && (flags & (dMtrStts_UNK40_e))) {
        u32 state = dComIfGp_ea();
        if ((u8)meterGame(state)->play.mEvtCtrl.mMode) {
            state = dComIfGp_ea();
            input = ((u16)meterGame(state)->play.mEvtCtrl.mHindFlag & 0x200) != 0;
        }
    }
    if (!(flags & (dMtrStts_UNK4000_e)) && !input && (u8)dMeter_collectMode != 4) {
        flags = (u32)meter->mStatusFlags;
        if (!(flags & (dMtrStts_UNK20_e | dMtrStts_UNK80_e | dMtrStts_UNK20000_e |
                       dMtrStts_UNK40000_e | dMtrStts_UNK80000_e | dMtrStts_UNK100000_e |
                       dMtrStts_UNK200000_e | dMtrStts_HD1000000_e)) &&
            !(u8)dMeter_auctionFlag &&
            !(flags & (dMtrStts_UNK400000_e | dMtrStts_UNK800000_e | dMtrStts_HD2000000_e))) {
            hide = meterSceneEquals(gabi::ea(&l_magicCounterStage)) &&
                   (u8)meterGame(dComIfGp_ea())->play.mMiniGameType == 6;
        }
    }
    if (hide || ((u32)meter->mStatusFlags & (dMtrStts_UNK8_e | dMtrStts_UNK10_e)))
        dMeter_hideCounter(6);
    else
        dMeter_showCounter(6);
}
/* 0259C2BC */
void dMeter_gaugeAlpha(sub_meter_class *meter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    f32 hearts = (f32)meter->field_0x2fbc, magic = (f32)meter->field_0x2fc0;
    for (u32 i = 0; i < 20; i++) {
        fopMsgM_setNowAlpha(&meter->mHeart[i], hearts);
        fopMsgM_setNowAlpha(&meter->mHeartShadow[i], hearts);
    }
    meter->field_0x0f00.mNowAlpha =
        (u8)((u8)ftoi(fmuls_ppc((f32)(u8)meter->field_0x0f00.mNowAlpha, hearts)));
    for (u32 i = 0; i < 8; i++)
        fopMsgM_setNowAlpha(&meter->field_0x0f38[i], magic);
    for (auto pane : {&meter->field_0x10f8, &meter->field_0x11a0, &meter->field_0x11d8})
        fopMsgM_setNowAlpha(pane, magic);
}
// HD: publish metronome HUD visibility separately from the GC menu path.
/* 0259C3CC */
void dMeter_metronomeVisibility(sub_meter_class *meter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 visible = 0;
    u32 state = dComIfGp_ea();
    if ((u8)meterGame(state)->play.mMetronome &&
        !((u32)meter->mStatusFlags &
          (dMtrStts_UNK8_e | dMtrStts_UNK10_e | dMtrStts_UNK20_e | dMtrStts_UNK800000_e))) {
        state = dComIfGp_ea();
        if ((u16)meterGame(state)->play.mItemNowLife && !meterIsMagicBlocked())
            visible = 1;
    }
    l_metronomeVisible = (u8)(visible);
}
/* 0259CCA8 */
void dMeter_compassMove(sub_meter_class *meter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 flags = (u32)meter->mStatusFlags;
    bool reset = (flags & (dMtrStts_UNK4000_e)) != 0;
    if (!reset) {
        if (flags & (dMtrStts_UNK40_e)) {
            u32 state = dComIfGp_ea();
            if ((u8)meterGame(state)->play.mEvtCtrl.mMode) {
                state = dComIfGp_ea();
                reset = ((u16)meterGame(state)->play.mEvtCtrl.mHindFlag & 0x100) != 0;
            }
            flags = (u32)meter->mStatusFlags;
        }
        reset = reset ||
                (flags & (dMtrStts_UNK200000_e | dMtrStts_UNK400000_e | dMtrStts_HD2000000_e)) ||
                (flags & (dMtrStts_UNK8_e | dMtrStts_UNK10_e | dMtrStts_UNK20_e | dMtrStts_UNK80_e |
                          dMtrStts_UNK100_e | dMtrStts_UNK20000_e | dMtrStts_UNK100000_e |
                          dMtrStts_UNK800000_e));
        if (!reset) {
            u32 state = dComIfGp_ea();
            reset = (u8)meterGame(state)->play.mMiniGameType == 1;
            if (!reset) {
                state = dComIfGp_ea();
                reset = (u8)meterGame(state)->play.mMiniGameType == 8;
            }
        }
    }
    if (reset) {
        dMeter_compassValueInit(meter);
        return;
    }
    if (((u32)meter->mStatusFlags & (dMtrStts_UNK400_e)) &&
        ((u8)meterStageView(meterStage())->mDisplayFlags >> 1) != 5)
        dMeter_showCounter(4);
    else
        dMeter_hideCounter(4);
}
/* 0259CDC4 */
void dMeter_metronomeMove(sub_meter_class *meter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!(u32)l_metronomeInitialized) {
        l_metronomeInitialized = (u32)(1);
        l_metronomeFlag = (u8)(meterGetMetronome());
    }
    if (!(u32)l_metronomeStateInitialized)
        l_metronomeStateInitialized = (u32)(1);
    u32 state = dComIfGp_ea();
    bool shown = ((u32)meterGame(state)->play.mPlayerStatus[0][0] & 0x1000) != 0;
    if (shown) {
        const u32 mask =
            (dMtrStts_UNK40_e | dMtrStts_UNK80_e | dMtrStts_UNK4000_e | dMtrStts_UNK200000_e) |
            (dMtrStts_UNK20_e | dMtrStts_UNK100_e | dMtrStts_UNK800000_e) |
            (dMtrStts_UNK8_e | dMtrStts_UNK10_e | dMtrStts_UNK20000_e | dMtrStts_UNK100000_e);
        shown = !((u32)meter->mStatusFlags & mask);
        if (shown)
            shown = (u8)meterGame(dComIfGp_ea())->play.mMiniGameType != 8;
    }
    if (shown)
        dMeter_showCounter(5);
    else
        dMeter_hideCounter(5);
}
/* 0259CEA0 */
void dMeter_windMove(sub_meter_class *meter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 state = dComIfGp_ea();
    if ((u8)meterScene(state)->mWindBlocked)
        return;
    state = dComIfGp_ea();
    meterUpdateEvent(gabi::ea(&meterGame(state)->play.mEvtCtrl));
    state = dComIfGp_ea();
    u32 mode = (u8)meterScene(state)->mOperateWind;
    bool photo = mode == 2;
    if (!photo) {
        state = dComIfGp_ea();
        photo = (u8)meterScene(state)->mOperateWind == 3;
    }
    if (photo) {
        meter->mWindItem = (u16)(89);
        dMeter_windStatus = (u8)(24);
    }
    u32 index = (u16)meter->mWindItem;
    if ((index == 99 || index == dItemNo_FOREST_WATER_e || index == 98) &&
        (u32)meter->mWindID == 0xFFFFFFFF)
        meter->mWindItem = (u16)(0);
}

/* 0259C454 */
void dMeter_rupyMove(sub_meter_class *meter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 state = (u32)l_saveRoot;
    if ((u32)(s32)(s16)meter->field_0x19f0[1].mUserArea !=
        (u8)meterSave(state)->mSave.mSavedata.mPlayer.mPlayerStatusA.mWalletSize) {
        dMeter_walletChange(meter);
        meter->field_0x19f0[1].mUserArea =
            (u16)((u8)meterSave((u32)l_saveRoot)
                      ->mSave.mSavedata.mPlayer.mPlayerStatusA.mWalletSize);
    }
    u32 game = dComIfGp_ea();
    u32 pending;
    if ((u32)meterGame(game)->play.mItemRupeeCount) {
        game = dComIfGp_ea();
        state = (u32)l_saveRoot;
        u32 delta = (u32)meterGame(game)->play.mItemRupeeCount,
            mode = (u8)meterSave(state)->mSave.mSavedata.mPlayer.mPlayerStatusA.mWalletSize;
        s32 total =
            (s32)((u32)(u16)meterSave(state)->mSave.mSavedata.mPlayer.mPlayerStatusA.mRupee +
                  delta);
        // HD: the first wallet holds 500 rupees (GameCube: 200).
        u32 cap = mode == 0 ? 500 : (mode == 1 ? 1000 : 5000), current = (u16)meter->mRupyCount;
        u32 value = total > (s32)cap ? cap : (total < 0 ? 0 : (u32)total);
        meter->mAdjustRupy = (u32)(value - current);
        meterSave((u32)l_saveRoot)->mSave.mSavedata.mPlayer.mPlayerStatusA.mRupee = (u16)(value);
        meterGame(dComIfGp_ea())->play.mItemRupeeCount = (u32)(0);
        pending = (u32)meter->mAdjustRupy;
        // HD: the sound threshold tests the unsigned adjustment, including negative values.
        if (pending >= 5)
            rupy_soundOnFlag = (u8)(1);
    } else
        pending = (u32)meter->mAdjustRupy;
    if (pending) {
        bool increase = (s32)pending > 0;
        u32 step = meterRupyStep(increase ? pending : 0u - pending);
        u32 current = (u16)meter->mRupyCount;
        pending = (u32)meter->mAdjustRupy;
        current = increase ? current + step : current - step;
        pending = increase ? pending - step : pending + step;
        meter->mRupyCount = (u16)(current);
        meter->mAdjustRupy = (u32)(pending);
        if ((u8)rupy_soundOnFlag) {
            if (!pending) {
                mDoAud_seStart(increase ? 0x83Au : 0x83Cu);
                rupy_soundOnFlag = (u8)(0);
                rupy_soundSetFlag = (u8)(0);
            } else if (!(u8)rupy_soundSetFlag) {
                mDoAud_seStart(increase ? 0x839u : 0x83Bu);
                rupy_soundSetFlag = (u8)(1);
            } else
                rupy_soundSetFlag = (u8)(0);
        }
        current = (u16)meter->mRupyCount;
        meterGame(dComIfGp_ea())->play.mItemNowRupee = (u16)(current);
    }
    u32 flags = (u32)meter->mStatusFlags;
    bool hide = (flags & (dMtrStts_UNK4000_e)) != 0;
    if (!hide) {
        if (flags & (dMtrStts_UNK40_e)) {
            game = dComIfGp_ea();
            if ((u8)meterGame(game)->play.mEvtCtrl.mMode) {
                game = dComIfGp_ea();
                hide = ((u16)meterGame(game)->play.mEvtCtrl.mHindFlag & 0x80) != 0;
            }
            flags = (u32)meter->mStatusFlags;
        }
        hide =
            hide || (flags & (dMtrStts_UNK8_e | dMtrStts_UNK10_e | dMtrStts_UNK20_e |
                              dMtrStts_UNK80_e | dMtrStts_UNK20000_e | dMtrStts_UNK100000_e |
                              dMtrStts_UNK200000_e | dMtrStts_UNK400000_e | dMtrStts_UNK800000_e));
        if (!hide) {
            game = dComIfGp_ea();
            hide = (u8)meterGame(game)->play.mMiniGameType == 1;
            if (!hide) {
                game = dComIfGp_ea();
                hide = (u8)meterGame(game)->play.mMiniGameType == 6;
            }
        }
    }
    if (hide)
        dMeter_hideCounter(2);
    else
        dMeter_showCounter(2);
    f32 alpha =
        (hide
             ? dMeter_alphaClose(&meter->field_0x2a20.mUserArea, &meter->field_0x19f0[0].mUserArea)
             : dMeter_alphaOpen(&meter->field_0x2a20.mUserArea, &meter->field_0x19f0[0].mUserArea));
    u32 emitter = (u32)meter->mpRupyParticle;
    if (emitter) {
        u32 bits = (u32)meterParticle(emitter)->mFlags;
        meterParticle(emitter)->mFlags = (u32)(hide ? bits | 4u : bits & ~4u);
    }
    for (u32 i = 0; i < 4; i++) {
        fopMsgM_setNowAlpha(&meter->field_0x19f0[i], alpha);
        fopMsgM_setNowAlpha(&meter->field_0x1b40[i], alpha);
    }
    fopMsgM_setNowAlpha(&meter->field_0x2a20, alpha);
    fopMsgM_setNowAlpha(&meter->field_0x2a58, alpha);
    meter->mRupyAlpha = (f32)(alpha);
}

/* 0259C7FC */
void dMeter_keyMove(sub_meter_class *meter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((u8)meterStageView(meterStage())->mDisplayFlags & 1) {
        u32 game = dComIfGp_ea();
        if ((s16)meterGame(game)->play.mItemKeyNumCount) {
            game = dComIfGp_ea();
            u32 state = (u32)l_saveRoot;
            s32 total = (s16)((u8)meterSave(state)->mSave.mMemory.mMembit.mKeyNum +
                              (s16)meterGame(game)->play.mItemKeyNumCount);
            u32 current = (u8)meter->field_0x301d;
            total = total > 99 ? 99 : (total < 0 ? 0 : total);
            meter->field_0x300e = (u16)((u32)total - current);
            meterSave((u32)l_saveRoot)->mSave.mMemory.mMembit.mKeyNum = (u8)(total);
            meterGame(dComIfGp_ea())->play.mItemKeyNumCount = (u16)(0);
        }
        s32 pending = (s16)meter->field_0x300e;
        if (pending > 0) {
            meter->field_0x300e = (u16)(pending - 1);
            meter->field_0x301d = (u8)((u8)meter->field_0x301d + 1);
        } else if (pending < 0) {
            meter->field_0x301d = (u8)((u8)meter->field_0x301d - 1);
            meter->field_0x300e = (u16)(pending + 1);
        }
    }
    Local<Thresholds> thresholds;
    const s16 values[] = {30, 40, 45, 50, 58};
    for (u32 i = 0; i < 5; i++)
        thresholds->values[i] = values[i];
    u32 flags = (u32)meter->mStatusFlags;
    bool hide = (flags & (dMtrStts_UNK4000_e)) != 0;
    if (!hide) {
        if (flags & (dMtrStts_UNK40_e)) {
            u32 game = dComIfGp_ea();
            if ((u8)meterGame(game)->play.mEvtCtrl.mMode) {
                game = dComIfGp_ea();
                hide = ((u16)meterGame(game)->play.mEvtCtrl.mHindFlag & 0x40) != 0;
            }
            flags = (u32)meter->mStatusFlags;
        }
        hide = hide ||
               (flags & (dMtrStts_UNK8_e | dMtrStts_UNK10_e | dMtrStts_UNK20_e | dMtrStts_UNK80_e |
                         dMtrStts_UNK400_e | dMtrStts_UNK20000_e | dMtrStts_UNK100000_e |
                         dMtrStts_UNK200000_e | dMtrStts_UNK400000_e | dMtrStts_UNK800000_e));
        if (!hide)
            hide = !((u8)meterStageView(meterStage())->mDisplayFlags & 1);
    }
    if (hide)
        dMeter_hideCounter(3);
    else
        dMeter_showCounter(3);
    f32 alpha =
        (hide
             ? dMeter_alphaClose(&meter->field_0x1c90.mUserArea, &meter->field_0x1ad0[0].mUserArea)
             : dMeter_alphaOpen(&meter->field_0x1c90.mUserArea, &meter->field_0x1ad0[0].mUserArea));
    for (u32 i = 0; i < 2; i++) {
        fopMsgM_setNowAlpha(&meter->field_0x1ad0[i], alpha);
        fopMsgM_setNowAlpha(&meter->field_0x1c20[i], alpha);
    }
    fopMsgM_setNowAlpha(&meter->field_0x1c90, alpha);
    if ((s16)meter->field_0x19b8.mUserArea == 1) {
        f32 rnd = cM_rndFX((f32)(s16)g_meterHIO.field_0x9a);
        s16 threshold = (s16)ftoi(fadds_ppc((f32)(s16)g_meterHIO.field_0x90, rnd));
        meter->field_0x2fda[0] = (u16)(threshold);
        for (u32 i = 0; i < 3; i++) {
            rnd = cM_rndFX((f32)(s16)g_meterHIO.field_0x92[i]);
            s32 base = threshold + (s16)g_meterHIO.field_0x88[i];
            threshold = (s16)ftoi(fadds_ppc((f32)base, rnd));
            meter->field_0x2fda[1 + i] = (u16)(threshold);
        }
        f32 base = (f32)(threshold + (s16)g_meterHIO.field_0x8e);
        rnd = cM_rndFX((f32)(s16)g_meterHIO.field_0x98);
        meter->field_0x2fda[4] = (u16)((s16)ftoi(fadds_ppc(base, rnd)));
    }
    dMeter_keyLight(&meter->field_0x1980, thresholds.get(), alpha);
    dMeter_keyLight(&meter->field_0x19b8, &meter->field_0x2fda[0], alpha);
    meter->mKeyAlpha = (f32)(alpha);
}

/* 0259A5B0 */
void dMeter_rAlpha(sub_meter_class *meter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 game = dComIfGp_ea();
    if ((u8)meterGame(game)->play.mRStatusForce) {
        u32 mode = (u8)meterGame(dComIfGp_ea())->play.mRStatusForce;
        meterGame(dComIfGp_ea())->play.mRStatus = (u8)(mode);
    }
    if ((s16)meter->field_0x1cc8.mUserArea) {
        game = dComIfGp_ea();
        if ((u8)meterGame(game)->play.mRStatus) {
            game = dComIfGp_ea();
            if ((u8)meterGame(game)->play.mRStatus != 7)
                dComIfGp_ea();
            dComIfGp_ea();
        }
        u32 mode = (u8)meterGame(dComIfGp_ea())->play.mRStatus;
        meter->field_0x1cc8.mUserArea = (u16)(0);
        meter->field_0x26d8.mUserArea = (u16)(1);
        meter->field_0x301c = (u8)(mode);
    }
    game = dComIfGp_ea();
    if ((u8)meter->field_0x301c != (u8)meterGame(game)->play.mRStatus)
        meter->field_0x1cc8.mUserArea = (u16)(1);
    Local<be<u8>> state;
    *state.get() = (u8)l_buttonAlphaState;
    u32 flags = (u32)meter->mStatusFlags, mode = 0;
    bool hide = (flags & (dMtrStts_UNK4000_e)) != 0;
    if (!hide) {
        if (flags & (dMtrStts_UNK40_e)) {
            game = dComIfGp_ea();
            if ((u8)meterGame(game)->play.mEvtCtrl.mMode) {
                game = dComIfGp_ea();
                hide = ((u16)meterGame(game)->play.mEvtCtrl.mHindFlag & 8) != 0;
            }
            flags = (u32)meter->mStatusFlags;
        }
        hide = hide || (flags & (dMtrStts_UNK100_e));
        if (!hide) {
            game = dComIfGp_ea();
            hide = (u8)meterGame(game)->play.mRStatus == 62;
        }
    }
    if (!hide) {
        flags = (u32)meter->mStatusFlags;
        if (flags & (dMtrStts_UNK20000_e)) {
            game = dComIfGp_ea();
            hide = (u8)meterScene(game)->mOperateWind == 2;
            mode = 2;
        } else if (flags & (dMtrStts_UNK40000_e))
            mode = 4;
        else {
            if ((u8)dMenu_menuFlag) {
                game = dComIfGp_ea();
                if ((u8)meterScene(game)->mOperateWind != 2) {
                    u32 m = (u8)dMenu_menuButton;
                    hide = m == 1 || ((u8)dMenu_pause == 1 && m == 0);
                }
                if (!hide && (u8)dMenu_menuFlag) {
                    u32 m = (u8)dMenu_menuButton;
                    hide = m == 2 || ((u8)dMenu_pause == 2 && m == 0);
                }
                flags = (u32)meter->mStatusFlags;
            }
            hide = hide || (flags & (dMtrStts_UNK800000_e));
            if (!hide && (flags & (dMtrStts_UNK20_e))) {
                hide = ((u16)meterStageView(meterStage())->mStageFlags & 3) == 1;
                flags = (u32)meter->mStatusFlags;
            }
            hide = hide ||
                   (flags & (dMtrStts_UNK80_e | dMtrStts_UNK80000_e | dMtrStts_UNK100000_e |
                             dMtrStts_UNK200000_e | dMtrStts_UNK400000_e | dMtrStts_HD1000000_e));
            if (!hide && (flags & (dMtrStts_UNK20_e))) {
                u32 value = (u8)l_itemButtonMode;
                if (value <= 3)
                    mode = 3;
                else if (value >= 7 && value <= 10)
                    mode = 5;
                else
                    hide = true;
            }
        }
    }
    if (hide)
        mode = 1;
    if ((s16)l_rFadeMode != (s32)mode) {
        dMeter_alphaFlagOff(&meter->field_0x3026, 8);
        if (mode == 1)
            dMeter_alphaFlagOff(state.get(), 8);
    }
    l_rFadeMode = (u16)(mode);
    if ((u8)meter->field_0x3026 != 15) {
        if (!dMeter_alphaFlagCheck(&meter->field_0x3026, 8)) {
            if (mode != 1)
                dMeter_alphaFlagOn(state.get(), 8);
            s32 count = (s16)l_rFadeCount;
            if (count >= 5) {
                l_rFadeCount = (u16)(5);
                dMeter_alphaFlagOn(&meter->field_0x3026, 8);
            } else
                l_rFadeCount = (u16)(count + 1);
        }
    } else {
        s32 count = (s16)l_rFadeCount;
        l_rFadeCount = (u16)(count > 0 ? count - 1 : 0);
    }
    if ((u8)*state.get() != (u8)l_buttonAlphaState)
        l_buttonAlphaState = (u8)((u8)*state.get());
    flags = (u32)meter->mStatusFlags;
    hide = (flags & (dMtrStts_UNK4000_e)) != 0;
    if (!hide) {
        if (flags & (dMtrStts_UNK40_e)) {
            game = dComIfGp_ea();
            if ((u8)meterGame(game)->play.mEvtCtrl.mMode) {
                game = dComIfGp_ea();
                hide = ((u16)meterGame(game)->play.mEvtCtrl.mHindFlag & 8) != 0;
            }
            flags = (u32)meter->mStatusFlags;
        }
        hide = hide ||
               (flags & (dMtrStts_UNK8_e | dMtrStts_UNK10_e | dMtrStts_UNK20_e | dMtrStts_UNK80_e |
                         dMtrStts_UNK100_e | dMtrStts_UNK200_e | dMtrStts_UNK40000_e |
                         dMtrStts_UNK80000_e | dMtrStts_UNK100000_e | dMtrStts_UNK200000_e |
                         dMtrStts_UNK400000_e | dMtrStts_UNK800000_e | dMtrStts_HD1000000_e));
    }
    s32 count = (s16)l_rPaneCount;
    if (hide) {
        l_rPaneCount = (u16)(count < 5 ? count + 1 : 5);
        meter->mButtonShowFlags = (u8)((u8)meter->mButtonShowFlags | 16);
        count = (s16)l_rPaneCount;
    } else if (flags & (dMtrStts_UNK2000_e | dMtrStts_UNK8000_e)) {
        count = count > 3 ? (s16)(count - 1) : (count < 3 ? (s16)(count + 1) : 3);
        l_rPaneCount = (u16)(count);
    } else {
        l_rPaneCount = (u16)(count < 5 ? count + 1 : 5);
        meter->field_0x3024 = (u8)((u8)meter->field_0x3024 | 16);
        meter->mButtonShowFlags = (u8)((u8)meter->mButtonShowFlags | 16);
        count = (s16)l_rPaneCount;
    }
    f32 alpha = fopMsgM_valueIncrease(5, count, 0);
    game = dComIfGp_ea();
    if (!(u8)meterGame(game)->play.mRStatus)
        fopMsgM_setNowAlphaZero(&meter->field_0x1cc8);
    else
        fopMsgM_setNowAlpha(&meter->field_0x1cc8, alpha);
    if (((u32)meter->mStatusFlags & (dMtrStts_UNK20_e)) &&
        !((u16)meterStageView(meterStage())->mStageFlags & 3))
        alpha = fsubs_ppc((f32)l_one, alpha);
    fopMsgM_setNowAlpha(&meter->field_0x26a0, alpha);
    fopMsgM_setNowAlpha(&meter->field_0x26d8, alpha);
    meterGame(dComIfGp_ea())->play.mRStatusForce = (u8)(0);
}
// HD: flight selection, gauge movement, pulse and alpha are combined in this entry.
/* 0259B698 */
void dMeter_magicGaugeMove(sub_meter_class *meter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((u32)meter->mStatusFlags & (dMtrStts_UNK40000_e)) {
        u32 player = gabi::ea(meterGame(dComIfGp_ea())->play.mpPlayerPtr[1].get());
        Local<be<u16>> id;
        id->set(fpcNm_NPC_CB1_e);
        if (player == meterActorJudge(gabi::ea(&l_actorNameJudge), id.a)) {
            u32 active = (u8)l_cb1FlightActive;
            u32 maximum = meterCb1MaxFlight();
            dMeter_flyGaugeMove(meter, active, maximum, (s16)l_cb1FlightTime);
        } else {
            player = gabi::ea(meterGame(dComIfGp_ea())->play.mpPlayerPtr[1].get());
            id->set(fpcNm_NPC_MD_e);
            if (player == meterActorJudge(gabi::ea(&l_actorNameJudge), id.a)) {
                u32 active = (u8)l_mdFlightActive;
                u32 maximum = meterMdMaxFlight();
                dMeter_flyGaugeMove(meter, active, maximum, (s16)l_mdFlightTime);
            } else
                dMeter_flyGaugeMove(meter, 0, 1, 1);
        }
        if ((s16)meter->field_0x0f38[2].mUserArea == 1)
            meter->field_0x0f38[2].mUserArea = (u16)(0);
        meter->field_0x0f38[5].mUserArea = (u16)(1);
        return;
    }
    f32 divisor = (f32)l_magicGaugeDivisor, length = (f32)l_one;
    if ((s16)meter->field_0x0f38[0].mUserArea == 1) {
        meter->field_0x0f38[0].mUserArea = (u16)(0);
        f32 zero = (f32)l_zero;
        dMeter_magicTransScale(meter, zero, zero, length);
        dMeter_magicInitTrans(meter);
        dMeter_magicChange(meter, ((f32)(u16)meter->field_0x3016 / divisor));
    }
    u32 game = dComIfGp_ea();
    if ((s16)meterGame(game)->play.mItemMaxMagicCount) {
        game = dComIfGp_ea();
        u32 save = (u32)l_saveRoot;
        s32 cap = (s16)((u8)meterSave(save)->mSave.mSavedata.mPlayer.mPlayerStatusA.mMaxMagic +
                        (s16)meterGame(game)->play.mItemMaxMagicCount);
        if (cap > 32)
            cap = 32;
        meterSave(save)->mSave.mSavedata.mPlayer.mPlayerStatusA.mMaxMagic = (u8)(cap);
        meterGame(dComIfGp_ea())->play.mItemMaxMagicCount = (u16)(0);
    }
    u32 save = (u32)l_saveRoot;
    u32 displayed = (u8)meter->field_0x3018,
        capacity = (u8)meterSave(save)->mSave.mSavedata.mPlayer.mPlayerStatusA.mMaxMagic;
    bool finalLength = false;
    if (displayed < capacity) {
        displayed = (u8)(displayed + 1);
        meter->field_0x3018 = (u8)(displayed);
        dMeter_magicLength(meter, fmuls_ppc((f32)displayed, (f32)l_magicLengthScale));
        save = (u32)l_saveRoot;
        capacity = (u8)meterSave(save)->mSave.mSavedata.mPlayer.mPlayerStatusA.mMaxMagic;
        finalLength = (u8)meter->field_0x3018 == capacity;
    } else if (displayed > capacity || (s16)meter->field_0x0f38[2].mUserArea == 0) {
        if (displayed > capacity) {
            displayed = (u8)(displayed - 1);
            meter->field_0x3018 = (u8)(displayed);
        }
        dMeter_magicLength(meter, fmuls_ppc((f32)displayed, (f32)l_magicLengthScale));
        save = (u32)l_saveRoot;
        capacity = (u8)meterSave(save)->mSave.mSavedata.mPlayer.mPlayerStatusA.mMaxMagic;
        finalLength = (u8)meter->field_0x3018 == capacity;
    }
    if (finalLength) {
        if (capacity <= 16)
            length = (f32)l_half;
        dMeter_magicLength(meter, length);
        meter->field_0x0f38[2].mUserArea = (u16)(1);
        save = (u32)l_saveRoot;
        capacity = (u8)meterSave(save)->mSave.mSavedata.mPlayer.mPlayerStatusA.mMaxMagic;
    }
    if (capacity) {
        game = dComIfGp_ea();
        if ((s16)meterGame(game)->play.mItemMagicCount) {
            game = dComIfGp_ea();
            save = (u32)l_saveRoot;
            s32 value = (s16)((u8)meterSave(save)->mSave.mSavedata.mPlayer.mPlayerStatusA.mMagic +
                              (s16)meterGame(game)->play.mItemMagicCount);
            if (value < 0)
                value = 0;
            meterSave(save)->mSave.mSavedata.mPlayer.mPlayerStatusA.mMagic = (u8)(value);
            meterGame(dComIfGp_ea())->play.mItemMagicCount = (u16)(0);
        }
        save = (u32)l_saveRoot;
        capacity = (u8)meterSave(save)->mSave.mSavedata.mPlayer.mPlayerStatusA.mMaxMagic;
    }
    if ((u8)meterSave(save)->mSave.mSavedata.mPlayer.mPlayerStatusA.mMagic > capacity)
        meterSave(save)->mSave.mSavedata.mPlayer.mPlayerStatusA.mMagic = (u8)(capacity);
    bool blocked = meterOverlapIsDoingReq() != 0;
    if (!blocked && ((u32)meter->mStatusFlags & (dMtrStts_UNK40_e))) {
        game = dComIfGp_ea();
        if ((u8)meterGame(game)->play.mEvtCtrl.mMode) {
            game = dComIfGp_ea();
            if ((u16)meterGame(game)->play.mEvtCtrl.mHindFlag & 0x20) {
                game = dComIfGp_ea();
                if (meterCheckStartDemo(gabi::ea(&meterGame(game)->play.mEvtManager))) {
                    game = dComIfGp_ea();
                    blocked = !((u32)meterGame(game)->play.mPlayerStatus[0][1] & 0x2000);
                }
            }
        }
    }
    if (blocked)
        meter->field_0x0f38[5].mUserArea = (u16)(1);
    else {
        save = (u32)l_saveRoot;
        f32 scaled = fmuls_ppc(
            fmuls_ppc((f32)(u8)meterSave(save)->mSave.mSavedata.mPlayer.mPlayerStatusA.mMagic,
                      (f32)meter->field_0x0f38[0].mSizeOrig.x),
            (f32)l_magicGaugeScale);
        u32 desired = (u16)ftoi(scaled), shown = (u16)meter->field_0x3016;
        if (desired != shown) {
            if (desired > shown) {
                shown = desired - shown < 625 ? desired : (u16)(shown + 625);
                meter->field_0x3016 = (u16)(shown);
                if (shown % 1250 == 0) {
                    mDoAud_seStartLevel(0x87D, 0, shown / 1250);
                    shown = (u16)meter->field_0x3016;
                }
            } else {
                if (shown - desired < 625) {
                    meter->field_0x3016 = (u16)(0);
                    mDoAud_seStart(0x87E);
                    shown = (u16)meter->field_0x3016;
                } else {
                    shown = (u16)(shown - 625);
                    meter->field_0x3016 = (u16)(shown);
                    if (shown % 1250 == 0) {
                        mDoAud_seStart(0x87E);
                        shown = (u16)meter->field_0x3016;
                    }
                }
                if (!shown) {
                    mDoAud_seStart(0x881);
                    shown = (u16)meter->field_0x3016;
                }
            }
            dMeter_magicChange(meter, ((f32)shown / divisor));
            meter->field_0x0f38[5].mUserArea = (u16)(1);
        } else if ((s16)meter->field_0x0f38[5].mUserArea) {
            dMeter_magicChange(meter, ((f32)shown / divisor));
            meter->field_0x0f38[5].mUserArea = (u16)(0);
        }
    }
    bool pulse = false;
    if (!((u32)meter->mStatusFlags &
          (dMtrStts_UNK8_e | dMtrStts_UNK10_e | dMtrStts_UNK100_e | dMtrStts_UNK800000_e))) {
        game = dComIfGp_ea();
        pulse = ((u8)meterScene(game)->mMagicPulse & 1) != 0;
        if (!pulse) {
            game = dComIfGp_ea();
            pulse = meterPlayerMagicUse(gabi::ea(meterGame(game)->play.mpPlayerPtr[0].get())) != 0;
        }
        if (!pulse) {
            game = dComIfGp_ea();
            pulse = ((u32)meterGame(game)->play.mPlayerStatus[0][1] & 0x20) != 0;
        }
        if (!pulse) {
            game = dComIfGp_ea();
            if (((u32)meterGame(game)->play.mPlayerStatus[0][0] & 0x1000) && meterGetMetronome()) {
                game = dComIfGp_ea();
                u32 actor = gabi::ea(meterGame(game)->play.mPlayerInfo[0].mpPlayer.get());
                pulse = gabi::at<PlayerVtable>((u32)meterActor(actor)->__vtbl)
                            ->getTactMusic(meterActor(actor)) != -1;
            }
        }
    }
    if (pulse) {
        if (!(s16)meter->field_0x0f38[4].mUserArea)
            meter->field_0x0f38[4].mUserArea = (u16)(1);
        s32 timer = (s16)meter->field_0x0f38[3].mUserArea;
        s32 value;
        if (timer >= 29) {
            meter->field_0x0f38[3].mUserArea = (u16)(0);
            value = 0;
        } else {
            timer = (s16)(timer + 1);
            meter->field_0x0f38[3].mUserArea = (u16)(timer);
            value = timer < 15 ? timer : 30 - timer;
        }
        fopMsgM_valueIncrease(15, value, 0);
        game = dComIfGp_ea();
        meterScene(game)->mMagicPulse = (u8)((u8)meterScene(game)->mMagicPulse & 0xFE);
        l_magicPulse = (u8)(1);
    } else {
        if ((s16)meter->field_0x0f38[4].mUserArea == 1)
            meter->field_0x0f38[4].mUserArea = (u16)(0);
        l_magicPulse = (u8)(0);
    }
    save = (u32)l_saveRoot;
    bool hide = (u8)meterSave(save)->mSave.mSavedata.mPlayer.mPlayerStatusA.mMaxMagic == 0;
    u32 flags = (u32)meter->mStatusFlags;
    if (!hide)
        hide = (flags & (dMtrStts_UNK4000_e)) != 0;
    if (!hide && (flags & (dMtrStts_UNK40_e))) {
        game = dComIfGp_ea();
        if ((u8)meterGame(game)->play.mEvtCtrl.mMode) {
            game = dComIfGp_ea();
            if ((u16)meterGame(game)->play.mEvtCtrl.mHindFlag & 0x20) {
                game = dComIfGp_ea();
                hide = !((u32)meterGame(game)->play.mPlayerStatus[0][1] & 0x2000);
            }
        }
    }
    if (!hide)
        hide = (u8)dMeter_collectMode == 4;
    if (!hide) {
        flags = (u32)meter->mStatusFlags;
        hide = (flags &
                (dMtrStts_UNK20_e | dMtrStts_UNK80_e | dMtrStts_UNK20000_e | dMtrStts_UNK80000_e |
                 dMtrStts_UNK100000_e | dMtrStts_UNK200000_e | dMtrStts_HD1000000_e)) ||
               (u8)dMeter_auctionFlag || (flags & (dMtrStts_UNK400000_e | dMtrStts_UNK800000_e));
    }
    if (!hide)
        hide = meterSceneEquals(gabi::ea(&l_magicStage)) &&
               (u8)meterGame(dComIfGp_ea())->play.mMiniGameType == 6;
    s32 timer;
    if (hide) {
        dMeter_hideCounter(1);
        timer = (s16)meter->field_0x10f8.mUserArea;
        timer = timer > 0 ? (s16)(timer - 1) : 0;
    } else if ((u32)meter->mStatusFlags & (dMtrStts_UNK8_e | dMtrStts_UNK10_e)) {
        dMeter_hideCounter(1);
        timer = (s16)meter->field_0x10f8.mUserArea;
        timer = timer > 3 ? (s16)(timer - 1) : (timer < 3 ? (s16)(timer + 1) : 3);
    } else {
        dMeter_showCounter(1);
        timer = (s16)meter->field_0x10f8.mUserArea;
        timer = timer < 5 ? (s16)(timer + 1) : 5;
    }
    meter->field_0x10f8.mUserArea = (u16)(timer);
    f32 alpha = fopMsgM_valueIncrease(5, timer, 0);
    meter->field_0x2fc0 = (f32)(alpha);
    if ((s16)meter->field_0x11d8.mUserArea)
        meter->field_0x11d8.mUserArea = (u16)(0);
}
/* 0259ACD4 */
void dMeter_enemyMove(sub_meter_class *meter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 attention = gabi::ea(&meterGame(dComIfGp_ea())->play.mAttention), stage = meterStage();
    if (!stage)
        meterAssertion(gabi::ea(&l_assertFile), 0x1D6F, gabi::ea(&l_stageAssertion));
    f32 zero = (f32)l_zero, one = (f32)l_one;
    u32 event = (u8)dMenu_menuFlag;
    bool track = false;
    if (attention) {
        if (!event) {
            bool eligible = !(u8)meterGame(dComIfGp_ea())->play.mEvtCtrl.mMode;
            if (eligible)
                eligible = meterLockonTarget(attention, 0) != 0;
            if (eligible) {
                u32 a = meterLockonTarget(gabi::ea(&meterGame(dComIfGp_ea())->play.mAttention), 0);
                eligible = (f32)(s8)meterActor(a)->max_health > zero;
            }
            if (eligible)
                eligible =
                    meterIsCollect(
                        gabi::ea(&meterSave((u32)l_saveRoot)->mSave.mSavedata.mPlayer.mCollect), 4,
                        1) != 0;
            if (eligible)
                eligible = meterLockonTruth(attention) != 0;
            if (eligible)
                eligible = (u8)meterActor(meterLockonTarget(attention, 0))->group == fopAc_ENEMY_e;
            for (s16 excluded : {fpcNm_BTD_e, fpcNm_BMD_e, fpcNm_BDK_e, fpcNm_BPW_e, fpcNm_BGN_e,
                                 fpcNm_BGN2_e, fpcNm_BGN3_e, fpcNm_GND_e}) {
                if (!eligible)
                    break;
                u32 actor = meterLockonTarget(attention, 0);
                if (actor && (s16)meterActor(actor)->mProcName == excluded)
                    eligible = false;
            }
            if (eligible) {
                u32 actor = meterLockonTarget(attention, 0);
                if (actor && (s16)meterActor(actor)->mProcName == fpcNm_BST_e) {
                    actor = meterLockonTarget(attention, 0);
                    if (!((u32)meterActor(actor)->mParameters & 15))
                        eligible = false;
                }
            }
            if (eligible) {
                u32 actor = meterLockonTarget(attention, 0);
                if (actor && (s16)meterActor(actor)->mProcName == fpcNm_BWD_e)
                    eligible = false;
            }
            if (eligible) {
                u32 actor =
                    meterLockonTarget(gabi::ea(&meterGame(dComIfGp_ea())->play.mAttention), 0);
                Local<cXyz> position;
                meterCopyPosition(gabi::ea(&meterActor(actor)->attention_info.position),
                                  position.a);
                f32 ratio =
                    (f32)(s8)meterActor(actor)->health / (f32)(s8)meterActor(actor)->max_health;
                if (ratio < zero)
                    ratio = zero;
                else if (ratio > one)
                    ratio = one;
                l_zakoPosition.x = (f32)(position->x.get());
                l_zakoPosition.y = (f32)(position->y.get());
                l_zakoPosition.z = (f32)(position->z.get());
                event = (u8)dMenu_menuFlag;
                l_zakoVisible = (u8)(1);
                l_zakoHealth = (f32)(ratio);
            } else {
                event = (u8)dMenu_menuFlag;
                l_zakoVisible = (u8)(0);
            }
        } else
            l_zakoVisible = (u8)(0);
    }
    if (!event &&
        meterIsCollect(gabi::ea(&meterSave((u32)l_saveRoot)->mSave.mSavedata.mPlayer.mCollect), 4,
                       1)) {
        if (!(u8)meterGame(dComIfGp_ea())->play.mEvtCtrl.mMode &&
            !((u32)meter->mStatusFlags & (dMtrStts_UNK80_e | dMtrStts_UNK20000_e))) {
            bool environment = (((u32)meterStageView(stage)->mTypeFlags >> 16) & 7) == 3;
            if (!environment) {
                Local<be<u16>> name;
                name->set(fpcNm_BDK_e);
                environment = meterActorJudge(gabi::ea(&l_actorNameJudge), name.a) != 0;
            }
            if (environment) {
                if (meterIsStageBoss(gabi::ea(&meterSave((u32)l_saveRoot)->mSave.mMemory.mMembit),
                                     5) &&
                    !meterIsStageBoss(gabi::ea(&meterSave((u32)l_saveRoot)->mSave.mMemory.mMembit),
                                      3))
                    track = true;
                else {
                    for (u32 name : {gabi::ea(&l_bossStage0), gabi::ea(&l_bossStage1),
                                     gabi::ea(&l_bossStage2), gabi::ea(&l_bossStage3)})
                        if (meterSceneEquals(name)) {
                            track = true;
                            break;
                        }
                }
            }
        }
    }
    if (!track) {
        dMeter_bossEnemyHide(meter);
        return;
    }
    u32 actor = 0;
    for (u16 kind :
         {fpcNm_BTD_e, fpcNm_BMD_e, fpcNm_BDK_e, fpcNm_BST_e, fpcNm_BWD_e, fpcNm_BPW_e}) {
        Local<be<u16>> first;
        first->set(kind);
        actor = meterActorJudge(gabi::ea(&l_actorNameJudge), first.a);
        if (actor) {
            Local<be<u16>> second;
            second->set(kind);
            actor = meterActorJudge(gabi::ea(&l_actorNameJudge), second.a);
            break;
        }
    }
    if (!actor) {
        dMeter_bossEnemyHide(meter);
        return;
    }
    s32 maximum = (s8)meterActor(actor)->max_health;
    f32 ratio;
    if (maximum) {
        ratio = (f32)(s8)meterActor(actor)->health / (f32)maximum;
        if (ratio < zero)
            ratio = zero;
        else if (ratio > one)
            ratio = one;
    } else
        ratio = (f32)g_meterHIO.field_0xc;
    l_bossHealth = (f32)(ratio);
    l_bossVisible = (u8)(1);
}
static void meterMaskBoth(sub_meter_class *meter, u8 mask) {
    u8 a = (u8)meter->field_0x3024, b = (u8)meter->mButtonShowFlags;
    meter->field_0x3024 = (u8)(a | mask);
    meter->mButtonShowFlags = (u8)(b | mask);
}
// HD: XY item change, quantity, alpha, bow pulse and forest-water particles share one entry.
/* 02599584 */
void dMeter_xyMove(sub_meter_class *meter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 game = dComIfGp_ea();
    if ((u8)meterScene(game)->mArrowModeForce != 77) {
        u32 previous = (u8)meterScene(dComIfGp_ea())->mArrowModeForce;
        meterScene(dComIfGp_ea())->mArrowMode = (u8)(previous);
    }
    bool unchanged = true, particles = false;
    for (u32 i = 0; i < 3; i++) {
        game = dComIfGp_ea();
        if ((u8)meter->field_0x3021[i] != (u8)meterGame(game)->play.mSelectItem[i]) {
            dMeter_xyItemChange(meter, i);
            unchanged = false;
            game = dComIfGp_ea();
            bool special = (u8)meterGame(game)->play.mSelectItem[i] == dItemNo_MAGIC_ARROW_e;
            if (!special) {
                game = dComIfGp_ea();
                special = (u8)meterGame(game)->play.mSelectItem[i] == dItemNo_LIGHT_ARROW_e;
            }
            if (!special)
                meter->field_0x2320[i].mUserArea = (u16)(0);
            meter->field_0x3021[i] = (u8)((u8)meterGame(dComIfGp_ea())->play.mSelectItem[i]);
        }
    }
    if (unchanged) {
        bool update = false;
        u32 save;
        if (!(u32)l_arrowCapacityInitialized) {
            save = (u32)l_saveRoot;
            l_arrowCapacityInitialized = (u32)(1);
            l_arrowCapacity =
                (u8)((u8)meterSave(save)->mSave.mSavedata.mPlayer.mItemMax.mItemMax2.mArrowNum);
        }
        if (!(u32)l_bombCapacityInitialized) {
            save = (u32)l_saveRoot;
            l_bombCapacityInitialized = (u32)(1);
            l_bombCapacity =
                (u8)((u8)meterSave(save)->mSave.mSavedata.mPlayer.mItemMax.mItemMax2.mBombNum);
        }
        game = dComIfGp_ea();
        if ((s16)meterGame(game)->play.mItemPictureNumCount) {
            u32 block = meterGetPictureRecord(meterPictureRecord((u32)l_saveRoot));
            s32 value = (u8)meterGallery(block)->mPictureNum;
            value = (s16)(value + (s16)meterGame(dComIfGp_ea())->play.mItemPictureNumCount);
            meterGame(dComIfGp_ea())->play.mItemPictureNumCount = (u16)(0);
            save = (u32)l_saveRoot;
            if (value < 0)
                value = 0;
            if (value > 12)
                value = 12;
            block = meterGetPictureRecord(meterPictureRecord(save));
            meterSetPictureNum(block, (u8)value);
            update = true;
        }
        bool obtained = meterCheckGetItem(dItemNo_BOW_e) != 0;
        if (!obtained)
            obtained = meterCheckGetItem(dItemNo_MAGIC_ARROW_e) != 0;
        if (!obtained)
            obtained = meterCheckGetItem(dItemNo_LIGHT_ARROW_e) != 0;
        bool change = false;
        if (obtained)
            change = (s16)meterGame(dComIfGp_ea())->play.mItemArrowNumCount != 0;
        if (!change) {
            save = (u32)l_saveRoot;
            change = (u8)l_arrowCapacity !=
                     (u8)meterSave(save)->mSave.mSavedata.mPlayer.mItemMax.mItemMax2.mArrowNum;
        }
        if (change) {
            if ((u8)g_meterHIO.field_0x62)
                meterGame(dComIfGp_ea())->play.mItemArrowNumCount = (u16)(0);
            game = dComIfGp_ea();
            save = (u32)l_saveRoot;
            s32 value = (s16)((u8)meterSave(save)
                                  ->mSave.mSavedata.mPlayer.mItemRecord.mItemRecord2.mArrowNum +
                              (s16)meterGame(game)->play.mItemArrowNumCount);
            meterGame(dComIfGp_ea())->play.mItemArrowNumCount = (u16)(0);
            save = (u32)l_saveRoot;
            u32 cap = (u8)meterSave(save)->mSave.mSavedata.mPlayer.mItemMax.mItemMax2.mArrowNum;
            if (value < 0)
                value = 0;
            if (value > (s32)cap)
                value = cap;
            meterSave(save)->mSave.mSavedata.mPlayer.mItemRecord.mItemRecord2.mArrowNum =
                (u8)(value);
            l_arrowCapacity = (u8)((u8)meterSave((u32)l_saveRoot)
                                       ->mSave.mSavedata.mPlayer.mItemMax.mItemMax2.mArrowNum);
            update = true;
        }
        change = false;
        if (meterCheckGetItem(dItemNo_BOMB_BAG_e))
            change = (s16)meterGame(dComIfGp_ea())->play.mItemBombNumCount != 0;
        if (!change) {
            save = (u32)l_saveRoot;
            change = (u8)l_bombCapacity !=
                     (u8)meterSave(save)->mSave.mSavedata.mPlayer.mItemMax.mItemMax2.mBombNum;
        }
        if (change) {
            if ((u8)g_meterHIO.field_0x63)
                meterGame(dComIfGp_ea())->play.mItemBombNumCount = (u16)(0);
            game = dComIfGp_ea();
            save = (u32)l_saveRoot;
            s32 value = (s16)((u8)meterSave(save)
                                  ->mSave.mSavedata.mPlayer.mItemRecord.mItemRecord2.mBombNum +
                              (s16)meterGame(game)->play.mItemBombNumCount);
            meterGame(dComIfGp_ea())->play.mItemBombNumCount = (u16)(0);
            save = (u32)l_saveRoot;
            u32 cap = (u8)meterSave(save)->mSave.mSavedata.mPlayer.mItemMax.mItemMax2.mBombNum;
            if (value < 0)
                value = 0;
            if (value > (s32)cap)
                value = cap;
            meterSave(save)->mSave.mSavedata.mPlayer.mItemRecord.mItemRecord2.mBombNum =
                (u8)(value);
            l_bombCapacity = (u8)((u8)meterSave((u32)l_saveRoot)
                                      ->mSave.mSavedata.mPlayer.mItemMax.mItemMax2.mBombNum);
            update = true;
        }
        for (u32 i = 0; i < 8; i++) {
            game = dComIfGp_ea();
            if ((s16)meterGame(game)->play.mItemBeastNumCounts[i]) {
                game = dComIfGp_ea();
                save = (u32)l_saveRoot;
                s32 value =
                    (s16)((u8)meterSave(save)->mSave.mSavedata.mPlayer.mBagItemRecord.mBeastNum[i] +
                          (s16)meterGame(game)->play.mItemBeastNumCounts[i]);
                meterGame(dComIfGp_ea())->play.mItemBeastNumCounts[i] = (u16)(0);
                if (value <= 0) {
                    meterSave((u32)l_saveRoot)
                        ->mSave.mSavedata.mPlayer.mBagItemRecord.mBeastNum[i] = (u8)(0);
                    for (u32 j = 0; j < 3; j++) {
                        dComIfGp_ea();
                        meterItemTable(24 + i);
                    }
                    u32 item = meterItemTable(24 + i);
                    meterRemoveBagItem(
                        gabi::ea(&meterSave((u32)l_saveRoot)->mSave.mSavedata.mPlayer.mBagItem),
                        item);
                } else {
                    save = (u32)l_saveRoot;
                    if (value > 99)
                        value = 99;
                    meterSave(save)->mSave.mSavedata.mPlayer.mBagItemRecord.mBeastNum[i] =
                        (u8)(value);
                }
                update = true;
            }
            save = (u32)l_saveRoot;
            u8 cap = (u8)meterSave(save)->mSave.mSavedata.mPlayer.mBagItemRecord.mBaitNum[i];
            if ((u8)dMenu_baitNum.values[i] != cap) {
                dMenu_baitNum.values[i] = (u8)(cap);
                update = true;
            }
        }
        for (u32 i = 0; i < 3; i++)
            if ((u8)meterGame(dComIfGp_ea())->play.mSelectItem[i] == dItemNo_BIRD_BAIT_5_e)
                update = true;
        if (update)
            for (u32 i = 0; i < 3; i++)
                dMeter_xyItemCountUp(meter, i);
    }
    Local<be<u8>> state;
    state->set((u8)l_buttonAlphaState);
    u32 flags = (u32)meter->mStatusFlags, mode = 0;
    bool hide = (flags & (dMtrStts_UNK4000_e)) != 0;
    if (!hide && (flags & (dMtrStts_UNK40_e))) {
        game = dComIfGp_ea();
        if ((u8)meterGame(game)->play.mEvtCtrl.mMode) {
            game = dComIfGp_ea();
            hide = ((u16)meterGame(game)->play.mEvtCtrl.mHindFlag & 1) != 0;
        }
        flags = (u32)meter->mStatusFlags;
    }
    if (!hide)
        hide = (flags & (dMtrStts_UNK100_e)) != 0;
    if (!hide && (u8)dMenu_menuFlag) {
        u32 action = (u8)dMenu_menuButton;
        hide = action == 2 || ((u8)dMenu_pause == 2 && action == 0);
    }
    if (!hide)
        hide = (flags & (dMtrStts_UNK800000_e)) != 0;
    if (!hide && (flags & (dMtrStts_UNK20_e))) {
        hide = ((u16)meterStageView(meterStage())->mStageFlags & 3) == 1;
        flags = (u32)meter->mStatusFlags;
    }
    if (!hide)
        hide = (flags & (dMtrStts_UNK80_e | dMtrStts_UNK100000_e | dMtrStts_UNK200000_e)) != 0;
    if (!hide && (flags & (dMtrStts_UNK20000_e))) {
        hide = (u8)meterScene(dComIfGp_ea())->mOperateWind == 2;
        flags = (u32)meter->mStatusFlags;
    }
    if (!hide)
        hide = (flags & (dMtrStts_UNK40000_e | dMtrStts_UNK80000_e | dMtrStts_UNK400000_e)) != 0;
    if (!hide) {
        bool eventMode = false;
        if ((u8)dMenu_menuFlag) {
            u32 action = (u8)dMenu_menuButton;
            eventMode = action == 1 || ((u8)dMenu_pause == 1 && action == 0);
        }
        if (eventMode) {
            if ((u8)dMeter_itemMode)
                hide = true;
            else
                mode = 2;
        } else if (flags & (dMtrStts_UNK20_e)) {
            u32 item = (u8)l_itemButtonMode;
            if (item <= 3)
                mode = 3;
            else if (item >= 7 && item <= 10)
                mode = 4;
            else
                hide = true;
        }
    }
    if (hide)
        mode = 1;
    if ((s16)l_xyFadeMode != (s32)mode) {
        dMeter_alphaFlagOff(&meter->field_0x3026, 4);
        if (mode == 1)
            dMeter_alphaFlagOff(state.get(), 4);
    }
    l_xyFadeMode = (u16)(mode);
    bool paneDone = (u8)meter->field_0x3026 == 15;
    if (!paneDone && !dMeter_alphaFlagCheck(&meter->field_0x3026, 4)) {
        if (mode != 1)
            dMeter_alphaFlagOn(state.get(), 4);
        s32 count = (s16)l_xyFadeCount;
        if (count >= 5) {
            l_xyFadeCount = (u16)(5);
            dMeter_alphaFlagOn(&meter->field_0x3026, 4);
        } else
            l_xyFadeCount = (u16)(count + 1);
    } else if (paneDone) {
        s32 count = (s16)l_xyFadeCount;
        l_xyFadeCount = (u16)(count > 0 ? count - 1 : 0);
    }
    if (state->get() != (u8)l_buttonAlphaState)
        l_buttonAlphaState = (u8)(state->get());
    f32 zero = (f32)l_zero, height = (f32)l_screenHalfHeight, width = (f32)l_screenHalfWidth,
        one = (f32)l_one, minimum = (f32)l_bowAlphaMin, pulseDepth = (f32)l_bowPulseDepth;
    for (u32 i = 0; i < 3; i++) {
        game = dComIfGp_ea();
        bool bow = (u8)meterGame(game)->play.mSelectItem[i] == dItemNo_MAGIC_ARROW_e;
        if (!bow)
            bow = (u8)meterGame(dComIfGp_ea())->play.mSelectItem[i] == dItemNo_LIGHT_ARROW_e;
        if (bow) {
            meter->field_0x2320[0].mUserArea = (u16)((s16)meter->field_0x2320[0].mUserArea + 1);
            auto pane = &meter->field_0x2320[0];
            auto pulsePane = &meter->field_0x2320[i];
            s32 timer = (s16)pulsePane->mUserArea, phase;
            if (timer >= 120) {
                pulsePane->mUserArea = (u16)(0);
                phase = 0;
            } else
                phase = timer < 60 ? timer : 120 - timer;
            f32 weight = fopMsgM_valueIncrease(60, phase, 0), base = (f32)(u8)pane->mInitAlpha;
            u8 alpha = (u8)ftoi(fnmsubs(fsubs_ppc(base, minimum), weight, base));
            f32 scale = fnmsubs(pulseDepth, weight, one);
            for (u32 j = 0; j < 3; j++) {
                auto panePart = j == 0
                                    ? &meter->field_0x2320[i]
                                    : (j == 1 ? &meter->field_0x23c8[i] : &meter->field_0x2470[i]);
                fopMsgM_paneScaleOne(panePart, scale);
                panePart->mNowAlpha = (u8)(alpha);
            }
        } else if ((u8)meterGame(dComIfGp_ea())->play.mSelectItem[i] == dItemNo_FOREST_WATER_e) {
            Local<cXyz> position;
            position->x = fsubs_ppc((f32)meter->field_0x2080[i].mPosCenter.x, width);
            position->y = fsubs_ppc((f32)meter->field_0x2080[i].mPosCenter.y, height);
            position->z = zero;
            particles = true;
            for (u32 j = 0; j < 2; j++) {
                auto emitterSlot = &meter->field_0x2f2c[j];
                u32 emitter = (u32)*emitterSlot;
                if (emitter) {
                    f32 y = position->y.get();
                    if ((u8)meterParticle(emitter)->mDrawMode >= 7)
                        y = -y;
                    meterParticle(emitter)->mGlobalTranslation.x = (f32)(position->x.get());
                    meterParticle(emitter)->mGlobalTranslation.y = (f32)(y);
                    meterParticle(emitter)->mGlobalTranslation.z = (f32)(position->z.get());
                    emitter = (u32)*emitterSlot;
                    meterParticle(emitter)->mFlags =
                        (u32)((u32)meterParticle(emitter)->mFlags & ~4u);
                    emitter = (u32)*emitterSlot;
                    meterParticle(emitter)->mGlobalAlpha =
                        (u8)((u8)meter->field_0x2080[i].mNowAlpha);
                } else {
                    game = dComIfGp_ea();
                    u32 p = meterParticleSet(gabi::ea(meterGame(game)->play.mParticle.get()), 9,
                                             0x2D, position.a, 0, 0, 255, 0, -1, 0, 0, 0);
                    *emitterSlot = (u32)(p);
                }
            }
        }
    }
    if (!particles)
        for (u32 j = 0; j < 2; j++) {
            auto emitterSlot = &meter->field_0x2f2c[j];
            u32 emitter = (u32)*emitterSlot;
            if (emitter) {
                meterParticle(emitter)->mMaxFrame = (u32)(0xFFFFFFFF);
                meterParticle(emitter)->mFlags = (u32)((u32)meterParticle(emitter)->mFlags | 1);
                emitter = (u32)*emitterSlot;
                meterParticle(emitter)->mFlags =
                    (u32)((u32)meterParticle(emitter)->mFlags & ~0x40u);
                *emitterSlot = (u32)(0);
            }
        }
    flags = (u32)meter->mStatusFlags;
    hide = (flags & (dMtrStts_UNK4000_e)) != 0;
    if (!hide && (flags & (dMtrStts_UNK40_e))) {
        game = dComIfGp_ea();
        if ((u8)meterGame(game)->play.mEvtCtrl.mMode) {
            game = dComIfGp_ea();
            hide = ((u16)meterGame(game)->play.mEvtCtrl.mHindFlag & 1) != 0;
        }
        flags = (u32)meter->mStatusFlags;
    }
    if (!hide)
        hide = (flags & (dMtrStts_UNK10_e | dMtrStts_UNK100_e | dMtrStts_UNK800000_e)) != 0;
    if (!hide && (flags & (dMtrStts_UNK20_e))) {
        hide = ((u16)meterStageView(meterStage())->mStageFlags & 3) == 1;
        flags = (u32)meter->mStatusFlags;
    }
    if (!hide)
        hide = (flags & (dMtrStts_UNK80_e | dMtrStts_UNK40000_e | dMtrStts_UNK80000_e |
                         dMtrStts_UNK100000_e | dMtrStts_UNK200000_e | dMtrStts_UNK400000_e)) != 0;
    if (!hide) {
        if (flags & (dMtrStts_UNK8_e)) {
            for (u32 i = 0; i < 3; i++) {
                auto paneCount = &l_xyPaneCount.values[i];
                s32 count = (s16)*paneCount;
                *paneCount = (u16)(count < 5 ? count + 1 : 5);
                meter->mButtonShowFlags =
                    (u8)((u8)meter->mButtonShowFlags | (u8)l_itemShowMask.values[i]);
            }
            meter->mButtonShowFlags = (u8)((u8)meter->mButtonShowFlags | 0x40);
        } else if (flags & (dMtrStts_UNK200_e | dMtrStts_UNK800_e | dMtrStts_UNK2000_e |
                            dMtrStts_UNK8000_e | dMtrStts_UNK10000_e)) {
            for (u32 i = 0; i < 3; i++) {
                auto paneCount = &l_xyPaneCount.values[i];
                s32 count = (s16)*paneCount;
                *paneCount = (u16)(count > 3 ? count - 1 : (count < 3 ? count + 1 : 3));
            }
        } else {
            bool stageZero = false;
            if (flags & (dMtrStts_UNK20_e)) {
                stageZero = !((u16)meterStageView(meterStage())->mStageFlags & 3);
                if (!stageZero)
                    flags = (u32)meter->mStatusFlags;
            }
            if (stageZero) {
                s32 count = (s16)l_xyPaneCount.values[0];
                l_xyPaneCount.values[0] = (u16)(count > 0 ? count - 1 : 0);
                count = (s16)l_xyPaneCount.values[1];
                l_xyPaneCount.values[1] = (u16)(count < 5 ? count + 1 : 5);
                meterMaskBoth(meter, 4);
                count = (s16)l_xyPaneCount.values[2];
                l_xyPaneCount.values[2] = (u16)(count > 0 ? count - 1 : 0);
            } else if (flags & (dMtrStts_HD1000000_e))
                meterMaskBoth(meter, 0x20);
            else if (flags & (dMtrStts_UNK20000_e))
                meterMaskBoth(meter, 4);
            else {
                for (u32 i = 0; i < 3; i++) {
                    u32 item = (u8)meterGame(dComIfGp_ea())->play.mSelectItem[i];
                    bool dim = meterItemDim(meter, item) != 0;
                    auto paneCount = &l_xyPaneCount.values[i];
                    s32 count = (s16)*paneCount;
                    if (dim)
                        *paneCount = (u16)(count > 3 ? count - 1 : (count < 3 ? count + 1 : 3));
                    else {
                        *paneCount = (u16)(count < 5 ? count + 1 : 5);
                        meterMaskBoth(meter, i == 0 ? 4 : (i == 1 ? 8 : 0x20));
                    }
                }
                if (!meterItemDim(meter, dItemNo_WIND_WAKER_e))
                    meterMaskBoth(meter, 0x40);
            }
        }
    }
    meterScene(dComIfGp_ea())->mArrowModeForce = (u8)(77);
}

// clang-format off
/* WWHD glue:
ext 02591AB4 void dMeter_hideCounter(u32)
ext 02591B00 void dMeter_showCounter(u32)
ext 025DB690 void fopMsgM_setNowAlpha(fopMsgM_pane_class*,f32)
ext 025DB654 void fopMsgM_paneScaleOne(fopMsgM_pane_class*,f32)
ext 025DB6E0 void fopMsgM_setNowAlphaZero(fopMsgM_pane_class*)
ext 025DB6F4 f32 fopMsgM_valueIncrease(s32,s32,u32)
ext 027161BC u32 meterIsMagicBlocked()
ext 02593434 void dMeter_compassValueInit(meter_detail::sub_meter_class*)
ext 02526C9C u32 meterGetMetronome()
ext 02540024 void meterUpdateEvent(u32)
ext 025931CC void dMeter_walletChange(meter_detail::sub_meter_class*)
ext 025931A8 u32 meterRupyStep(u32)
ext 025E1988 void mDoAud_seStart(u32)
ext 02593284 void dMeter_keyLight(fopMsgM_pane_class*,void*,f32)
ext 0259173C void dMeter_alphaFlagOff(void*,u32)
ext 02591718 u32 dMeter_alphaFlagCheck(void*,u32)
ext 0259172C void dMeter_alphaFlagOn(void*,u32)
ext 025D5218 u32 meterActorJudge(u32,u32)
ext 022271B8 u32 meterCb1MaxFlight()
ext 02526C94 u32 meterMdMaxFlight()
ext 02592EE8 void dMeter_flyGaugeMove(meter_detail::sub_meter_class*,u32,u32,s32)
ext 02592CD8 void dMeter_magicTransScale(meter_detail::sub_meter_class*,f32,f32,f32)
ext 02592BC4 void dMeter_magicInitTrans(meter_detail::sub_meter_class*)
ext 02592A88 void dMeter_magicChange(meter_detail::sub_meter_class*,f32)
ext 02592888 void dMeter_magicLength(meter_detail::sub_meter_class*,f32)
ext 025DBE38 u32 meterOverlapIsDoingReq()
ext 025449B0 u32 meterCheckStartDemo(u32)
ext 025E1A04 void mDoAud_seStartLevel(u32,u32,u32)
ext 0244363C u32 meterPlayerMagicUse(u32)
ext 0273AA24 void meterAssertion(u32,u32,u32)
ext 024EC8D0 u32 meterLockonTarget(u32,u32)
ext 025B7A2C u32 meterIsCollect(u32,u32,u32)
ext 024EDFCC u32 meterLockonTruth(u32)
ext 025F0EA4 void meterCopyPosition(u32,u32)
ext 025B9100 u32 meterIsStageBoss(u32,u32)
ext 02592878 void dMeter_bossEnemyHide(meter_detail::sub_meter_class*)
ext 02591F68 void dMeter_xyItemChange(meter_detail::sub_meter_class*,u32)
ext 02720144 u32 meterGetPictureRecord(u32)
ext 027262B0 void meterSetPictureNum(u32,u32)
ext 02520C0C u32 meterCheckGetItem(u32)
ext 025DB5AC u32 meterItemTable(u32)
ext 025B6308 void meterRemoveBagItem(u32,u32)
ext 02591E60 void dMeter_xyItemCountUp(meter_detail::sub_meter_class*,u32)
ext 02592144 u32 meterItemDim(meter_detail::sub_meter_class*,u32)
ext 025A847C u32 meterParticleSet(u32,u32,u32,u32,u32,u32,u32,u32,s32,u32,u32,u32)
ext 0259174C f32 dMeter_alphaClose(be<s16>*,be<s16>*)
ext 025917FC f32 dMeter_alphaOpen(be<s16>*,be<s16>*)
data l_saveRoot 101F84DC
data dMeter_collectMode 101EA073
data dMeter_auctionFlag 101EA06C
data dMenu_menuFlag 101EA069
data dMenu_menuButton 101EA06A
data dMenu_pause 101EA067
data dMeter_itemMode 101EA072
data rupy_soundSetFlag 101EA075
data rupy_soundOnFlag 101EA076
data l_xyFadeCount 101EA052
data l_xyFadeMode 101EA054
data l_rFadeCount 101EA060
data l_rFadeMode 101EA062
data l_rPaneCount 101EA064
data l_buttonAlphaState 1047B07A
data l_metronomeVisible 1047B097
data l_magicPulse 1047B098
data l_bossVisible 1047B09B
data l_zakoVisible 1047B09C
data l_itemButtonMode 1047B09F
data l_bossHealth 1047B064
data l_zakoHealth 1047B068
data l_arrowCapacityInitialized 1047A9B4
data l_bombCapacityInitialized 1047A9B8
data l_metronomeInitialized 1047A9BC
data l_metronomeStateInitialized 1047A9C0
data l_arrowCapacity 1047A9C6
data l_bombCapacity 1047A9C7
data l_metronomeFlag 1047A9C8
data dMeter_windStatus 101EACB7
data l_cb1FlightActive 101BDC2D
data l_cb1FlightTime 101BDC28
data l_mdFlightActive 101D5F3E
data l_mdFlightTime 101D5F38
data l_one 100512B4
data l_zero 100512B8
data l_magicLengthScale 100512C0
data l_half 100512C8
data l_magicGaugeScale 10051310
data l_magicGaugeDivisor 10051348
data l_screenHalfHeight 10051500
data l_screenHalfWidth 10051504
data l_bowAlphaMin 10051508
data l_bowPulseDepth 1005150C
data l_xyPaneCount 101EA058
data dMenu_baitNum 1047A98C
data l_itemShowMask 10051510
data l_zakoPosition 1047B03C
data g_meterHIO 1047AD14
data l_safeStringVtable 1005107C
data l_actorNameJudge 025E121C
data l_bossStage0 1005104C
data l_bossStage1 10051054
data l_bossStage2 1005105C
data l_bossStage3 10051064
data l_magicStage 1005106C
data l_magicCounterStage 10051074
data l_assertFile 10051130
data l_stageAssertion 10051140
*/

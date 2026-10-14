/* HD GameOverChar; private game extension using the canonical window framework. */
#pragma once
#include "d/hd_parts.h"
#include "d/particle/d_particle_local.h"
#include "d/hd_parts_window.h"
struct hdGameOverMessageVtable_l { u8 mPrefix[0x14]; gfn mDelete; };
WWHD_OFFSET(hdGameOverMessageVtable_l, mDelete, 0x14);
WWHD_OFFSET(hdWindowPart_c, mpTextPane, 0xC);
WWHD_OFFSET(hdWindowPart_c, mScheduling, 0xD10);
WWHD_OFFSET(hdWindowPane_l, mCharacterCount, 0xCE);
struct hdGameOverCharLoadLocals {
    hdUiSafeString mActiveName, mInName, mOutName;
    hdWindowConfig_l mConfig;
};
WWHD_OFFSET(hdGameOverCharLoadLocals, mInName, 8);
WWHD_OFFSET(hdGameOverCharLoadLocals, mOutName, 0x10);
WWHD_OFFSET(hdGameOverCharLoadLocals, mConfig, 0x18);
struct hdGameOverCharRuntime { be<u32> mWords[4]; };
struct hdGameOverCharRegistration { be<u32> mWords[3]; };
struct hdGameOverCharContext { be<u8> mByte; };
struct hdGameOverCharCallbackState_l {
    be<u32> mNumber;
    gptr<const char> mName;
    gptr<hdPartsStateIdVtbl_c> mVtable;
    hdPartsPtmf_c mEnter, mMove, mLeave, mDone;
    hdPartsStateId_c* stateId() { return reinterpret_cast<hdPartsStateId_c*>(this); }
};
WWHD_SIZE(hdGameOverCharRuntime, 0x10);
WWHD_SIZE(hdGameOverCharRegistration, 0xC);
WWHD_SIZE(hdGameOverCharContext, 1);
WWHD_SIZE(hdGameOverCharCallbackState_l, 0x2C);
WWHD_OFFSET(hdGameOverCharCallbackState_l, mNumber, 0);
WWHD_OFFSET(hdGameOverCharCallbackState_l, mName, 4);
WWHD_OFFSET(hdGameOverCharCallbackState_l, mVtable, 8);
WWHD_OFFSET(hdGameOverCharCallbackState_l, mEnter, 0xC);
WWHD_OFFSET(hdGameOverCharCallbackState_l, mMove, 0x14);
WWHD_OFFSET(hdGameOverCharCallbackState_l, mLeave, 0x1C);
WWHD_OFFSET(hdGameOverCharCallbackState_l, mDone, 0x24);
struct hdGameOverCharEffectLocals {
    hdUiSafeString mGlyphName;
    pa::EmitterVector mOrigin, mSampled;
    hdUiSafeString mMarkerName;
};
WWHD_SIZE(hdGameOverCharEffectLocals, 0x28);
WWHD_OFFSET(hdGameOverCharEffectLocals, mOrigin, 8);
WWHD_OFFSET(hdGameOverCharEffectLocals, mSampled, 0x14);
WWHD_OFFSET(hdGameOverCharEffectLocals, mMarkerName, 0x20);
struct hdGameOverChar_c : hdParts_c {
    gptr<hdPartsLayout_c> mpCharacterLayout;
    gptr<hdWindowPart_c> mpMessage;
    gptr<pa::JPABaseEmitter> mpFirstEmitter, mpSecondEmitter;
    be<u32> mDelay;
    be<u8> mVisible, mRequested, mFinished;
    u8 mPadding67;
    static hdGameOverChar_c* construct(hdGameOverChar_c*);
    static void destroy(hdGameOverChar_c*, u32);
    void inEnter();
    void inMove();
    void delayEnter();
    void delayMove();
    void outEnter();
    void outMove();
    void completeEnter();
    void waitEnter();
    void waitMove();
    void releaseResources();
    u32 initializeCharacter(hdPartsResAccessor_c* accessor, u32 resource, u32 option, void* context);
    void executeCharacter();
    void spawnCharacterEffect(pa::EmitterVector* position);
    void spawnFirstEffect(pa::EmitterVector* position);
    void spawnSecondEffect(pa::EmitterVector* position);
    void stopFirstEffect();
    void stopSecondEffect();
    void finishFirstEffect();
    void finishSecondEffect();
};
WWHD_SIZE(hdGameOverChar_c, 0x68);
WWHD_OFFSET(hdGameOverChar_c, mpCharacterLayout, 0x50);
WWHD_OFFSET(hdGameOverChar_c, mpMessage, 0x54);
WWHD_OFFSET(hdGameOverChar_c, mpFirstEmitter, 0x58);
WWHD_OFFSET(hdGameOverChar_c, mpSecondEmitter, 0x5C);
WWHD_OFFSET(hdGameOverChar_c, mDelay, 0x60);
WWHD_OFFSET(hdGameOverChar_c, mVisible, 0x64);
WWHD_OFFSET(hdGameOverChar_c, mRequested, 0x65);
WWHD_OFFSET(hdGameOverChar_c, mFinished, 0x66);
u32 hdGameOverCharStateKey(hdPartsStateId_c*);

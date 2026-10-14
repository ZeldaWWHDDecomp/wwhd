/* HD: the bubbles of the staff roll ("StaffRollBubble_00", "StaffRollBubbleS_00"): small layouts that
 * rise from below the screen, sway sideways and respawn (hd_screen_staff_roll_bubble_base.cpp, the
 * shared base and its motion; hd_screen_staff_roll_bubble.cpp, the bubble with a character inside;
 * hd_screen_staff_roll_bubble_s.cpp, the small bubble). No GameCube counterpart. Layout, pane, class and
 * state names are HD strings; class, member and method names are ours ("probably" marks a role read
 * from its use only). */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_screen_staff_roll.h"

/* the bubble layout's root pane (nw::lyt::Pane fields used here) */
struct hdStaffRollBubblePane_c {
    /* 0x00 */ u8 _00[0x1C];
    /* 0x1C */ cXyz mTranslate;
    /* 0x28 */ u8 _28[0xC];
    /* 0x34 */ be<f32> mScaleX;
    /* 0x38 */ be<f32> mScaleY;
    /* 0x3C */ u8 _3C[8];
    /* 0x44 */ be<u8> mFlags;
    /* 0x45 */ be<u8> mAlpha;
};
WWHD_OFFSET(hdStaffRollBubblePane_c, mTranslate, 0x1C);
WWHD_OFFSET(hdStaffRollBubblePane_c, mScaleX, 0x34);
WWHD_OFFSET(hdStaffRollBubblePane_c, mScaleY, 0x38);
WWHD_OFFSET(hdStaffRollBubblePane_c, mFlags, 0x44);
WWHD_OFFSET(hdStaffRollBubblePane_c, mAlpha, 0x45);
enum hdStaffRollBubblePaneFlag_e {
    STAFF_ROLL_BUBBLE_PANE_INFLUENCED_ALPHA = 0x02, /* probably: children take the pane's alpha */
    STAFF_ROLL_BUBBLE_PANE_DIRTY = 0x10,            /* probably: recompute the matrix */
};

/* one kind of bubble motion: start velocity, upward acceleration, sway amplitude, sway angle step and
 * start angle (degrees) */
struct hdStaffRollBubbleKind_c {
    /* 0x00 */ cXyz mVelocity;
    /* 0x0C */ be<f32> mAccel;
    /* 0x10 */ be<f32> mSwayAmplitude;
    /* 0x14 */ be<s32> mSwayStep;
    /* 0x18 */ be<s32> mSwayAngle;
};
WWHD_SIZE(hdStaffRollBubbleKind_c, 0x1C);

struct hdStaffRollBubbleXy_c {
    /* 0x00 */ be<f32> x;
    /* 0x04 */ be<f32> y;
};
WWHD_SIZE(hdStaffRollBubbleXy_c, 8);

enum { STAFF_ROLL_BUBBLE_KIND_COUNT = 13 };

/* a bubble's motion (0x50, at hdStaffRollBubbleBase_c+0x50): rises with its velocity, the velocity
 * grows by the acceleration, and the position sways sideways along a sine */
struct hdStaffRollBubbleMotion_c {
    /* 0x00 */ gptr<void> mVtable;
    /* 0x04 */ be<u32> mType;           /* probably: the character frame shown in the bubble */
    /* 0x08 */ cXyz mPos;
    /* 0x14 */ cXyz mVelocity;
    /* 0x20 */ cXyz mStartVelocity;
    /* 0x2C */ be<f32> mAccel;
    /* 0x30 */ cXyz mSway;               /* the sideways offset added to mPos (only x moves) */
    /* 0x3C */ be<f32> mSwayAmplitude;
    /* 0x40 */ be<s32> mSwayStep;
    /* 0x44 */ be<s32> mSwayAngle;
    /* 0x48 */ be<f32> mScale;
    /* 0x4C */ be<u32> mFlags;           /* probably */

    void reset();
    void update();
    u32 isOut();
    void addScaleRise();
    void set(u32 type, cXyz* pos, cXyz* velocity, s32 swayStep, u32 flags, f32 accel, f32 swayAmplitude, f32 scale);
    void copy(hdStaffRollBubbleMotion_c* other);
    void setKind(u32 type, u32 kind, u32 flags, f32 x, f32 scale);
    void getPos(cXyz* out);
    f32 getScale();
    void respawn();
};
WWHD_OFFSET(hdStaffRollBubbleMotion_c, mType, 4);
WWHD_OFFSET(hdStaffRollBubbleMotion_c, mPos, 8);
WWHD_OFFSET(hdStaffRollBubbleMotion_c, mVelocity, 0x14);
WWHD_OFFSET(hdStaffRollBubbleMotion_c, mStartVelocity, 0x20);
WWHD_OFFSET(hdStaffRollBubbleMotion_c, mAccel, 0x2C);
WWHD_OFFSET(hdStaffRollBubbleMotion_c, mSway, 0x30);
WWHD_OFFSET(hdStaffRollBubbleMotion_c, mSwayAmplitude, 0x3C);
WWHD_OFFSET(hdStaffRollBubbleMotion_c, mSwayStep, 0x40);
WWHD_OFFSET(hdStaffRollBubbleMotion_c, mSwayAngle, 0x44);
WWHD_OFFSET(hdStaffRollBubbleMotion_c, mScale, 0x48);
WWHD_OFFSET(hdStaffRollBubbleMotion_c, mFlags, 0x4C);
WWHD_SIZE(hdStaffRollBubbleMotion_c, 0x50);

/* a state of the bubbles (sead-style, 0x2C): number, name and four non-virtual delegates */
struct hdStaffRollBubbleStateId_c {
    /* 0x00 */ be<s32> mNumber;
    /* 0x04 */ gptr<const char> mName;
    /* 0x08 */ gptr<void> mVtable;
    /* 0x0C */ hdPartsPtmf_c mEnter;
    /* 0x14 */ hdPartsPtmf_c mExecute;
    /* 0x1C */ hdPartsPtmf_c mExit;
    /* 0x24 */ hdPartsPtmf_c mFinal;   /* returns 0; role unknown */

    static void setFunction(hdPartsPtmf_c* ptmf, u32 function) {
        ptmf->mThisDelta = 0;
        ptmf->mVirtualIndex = -1;
        ptmf->mFunction = function;
    }
    void setup(s32 number, const char* name, void* vtable, u32 enter, u32 execute, u32 exit, u32 final) {
        mNumber = number;
        mName.v = gabi::guest_addr(name);
        mVtable = vtable;
        setFunction(&mEnter, enter);
        setFunction(&mExecute, execute);
        setFunction(&mExit, exit);
        setFunction(&mFinal, final);
    }
};
WWHD_OFFSET(hdStaffRollBubbleStateId_c, mEnter, 0xC);
WWHD_OFFSET(hdStaffRollBubbleStateId_c, mFinal, 0x24);
WWHD_SIZE(hdStaffRollBubbleStateId_c, 0x2C);

/* 0275CEE4: the next value of a random number generator */
u32 hdStaffRollBubble_random(void* random);
/* 028F43F8: sin (radians, double precision) */
f64 hdStaffRollBubble_sin(f64 radians);
/* 0275CE48: restart a random number generator from `seed` */
void hdStaffRollBubble_seedRandom(void* random, u32 seed);

struct hdStaffRollBubbleBase_c : hdParts_c {
    /* 0x50 */ hdStaffRollBubbleMotion_c mMotion;
    /* 0xA0 */ be<s32> mDelay;          /* frames to wait before moving */

    static void destroy(hdStaffRollBubbleBase_c* i_this, s32 flags);
    void moveEnter();
    void moveExecute();
    static hdStaffRollBubbleBase_c* construct(hdStaffRollBubbleBase_c* i_this);
    void finalize();
    void executeState();
    f32 getAlpha();
    static f32 getRandomSwayX();
    static f32 getRandomRise();
    static f32 getRandomAccel();
    static f32 getRandomScale();
    void waitEnter();
    void waitExecute();
    void waitExit();
    u32 waitFinal();
    void moveExit();
    u32 moveFinal();
    static f32 getStartY();

    hdStaffRollBubblePane_c* rootPane() { return (hdStaffRollBubblePane_c*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane; }
};
WWHD_OFFSET(hdStaffRollBubbleBase_c, mMotion, 0x50);
WWHD_OFFSET(hdStaffRollBubbleBase_c, mDelay, 0xA0);
WWHD_SIZE(hdStaffRollBubbleBase_c, 0xA4);

/* the descriptor of a child part as this layout's createChild hook reads it (our view): the part's class
 * name and the name of the parts pane it fills */
struct hdStaffRollBubbleDesc_l {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ hdUiSafeString mClassName;
    /* 0x10 */ u8 _10[4];
    /* 0x14 */ hdUiSafeString mPaneName;
};
WWHD_OFFSET(hdStaffRollBubbleDesc_l, mClassName, 8);
WWHD_OFFSET(hdStaffRollBubbleDesc_l, mPaneName, 0x14);

/* inlined into createChild: sead SafeString equality (both strings terminated first) */
inline bool hdStaffRollBubble_isSameName(hdUiSafeString* name, hdUiSafeString* other) {
    name->assureTermination();
    u32 text = gabi::ea(name->mText.get());
    other->assureTermination();
    if (text == gabi::ea(other->mText.get())) {
        return true;
    }
    u32 left = gabi::ea(name->mText.get());
    u32 right = gabi::ea(other->mText.get());
    for (u32 n = 0; n < 0x40001; n++) {
        u8 c = *hdPartsView<be<u8>>(left + n);
        if (c != *hdPartsView<be<u8>>(right + n)) {
            return false;
        }
        if (c == 0) {
            return true;
        }
    }
    return false;
}

/* the character parts (hd_screen_character.cpp, layout "Bubble_00", 0x50; MINI-auction+battleship+3more):
 * callees by address until that unit merges (then hdScreenCharacter_c) */
void* hdStaffRollBubble_constructCharacter(void* parts);   /* 026458A0 */
u32 hdStaffRollBubble_initCharacter(void* parts, u32 arg1, u32 arg2, void* desc);   /* 0264590C */
/* 02645AB8: show character `frame` (the frame of its animation) */
void hdStaffRollBubble_setCharacterFrame(void* parts, u32 frame);
enum { STAFF_ROLL_BUBBLE_CHARACTER_SIZE = 0x50 };

/* the big bubble "StaffRollBubble_00" with a character (the character parts, layout "Bubble_00", in pane "L_Bubble_00") */
struct hdStaffRollBubble_c : hdStaffRollBubbleBase_c {
    /* 0xA4 */ gptr<hdStaffRollPane_c> mpCharacterPane; /* "L_Bubble_00" */
    /* 0xA8 */ cXyz mStartPos;
    /* 0xB4 */ u8 _B4[0xC];

    static void destroy(hdStaffRollBubble_c* i_this, s32 flags);
    static hdStaffRollBubble_c* construct(hdStaffRollBubble_c* i_this);
    u32 init(u32 arg1, u32 arg2, void* desc);
    void finalize();
    u32 createChild(hdPartsDesc_c* desc);
    void showCharacter(u32 frame);
    void start(hdStaffRollBubbleMotion_c* motion);

    u32 vCreateChildren() { return gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this); }
};
WWHD_OFFSET(hdStaffRollBubble_c, mpCharacterPane, 0xA4);
WWHD_OFFSET(hdStaffRollBubble_c, mStartPos, 0xA8);
WWHD_SIZE(hdStaffRollBubble_c, 0xC0);

/* the small bubble "StaffRollBubbleS_00" (also with the character parts, in pane "L_BubbleS_00"): a random
 * motion from its own seed */
struct hdStaffRollBubbleS_c : hdStaffRollBubbleBase_c {
    /* 0xA4 */ gptr<hdStaffRollPane_c> mpCharacterPane; /* "L_BubbleS_00" */
    /* 0xA8 */ be<u32> mSeed;

    static void destroy(hdStaffRollBubbleS_c* i_this, s32 flags);
    static hdStaffRollBubbleS_c* construct(hdStaffRollBubbleS_c* i_this);
    u32 init(u32 seed, u32 arg1, u32 arg2, void* desc);
    void finalize();
    u32 createChild(hdPartsDesc_c* desc);
    void restart();

    u32 vCreateChildren() { return gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this); }
};
WWHD_OFFSET(hdStaffRollBubbleS_c, mpCharacterPane, 0xA4);
WWHD_OFFSET(hdStaffRollBubbleS_c, mSeed, 0xA8);
WWHD_SIZE(hdStaffRollBubbleS_c, 0xAC);

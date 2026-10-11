/* HD-only auction digit roll screen, animation binding and state initialization. */
#include "d/hd_screen_auction_number.h"

static HdAuctionHeapPointer l_heap;
static u32 l_screenVtable;
static u32 l_memberVtable;
static u32 l_secondaryVtable;
static u32 l_stringVtable;
static u32 l_stateVtable;
static u32 l_stateParent;
static HdAuctionState l_idle;
static HdAuctionState l_roll;
static HdAuctionNames l_animationNames;
static HdAuctionSafeString l_layoutName;
static u32 l_animationNameIndex;
static u32 l_stateGuard;
static u32 l_lastStateID;
static f32 l_three;
static HdAuctionGlobals l_globals;
static HdAuctionRange l_range;
static f32 l_lower;
static f32 l_upper;
static HdAuctionRegistration l_registration;
static HdAuctionRegistration l_signedRegistration;
static HdAuctionRegistration l_unsignedRegistration;

enum HdAuctionSlots : u32 { HdAuction_GetState = 5, HdAuction_EnterState = 7, HdAuction_LoadLayout = 5 };
enum HdAuctionConstants : u32 { HdAuction_Delete = 1, HdAuction_DigitCount = 10, HdAuction_RollFrames = 3 };

static HdAuctionNumber* HdAuctionNumber__ct(HdAuctionNumber*);

/* 0261D088 */
static HdAuctionNumber* hdAuction_createDigit(u32 a, u32 b, u32 c) {
    HdAuctionNumber* screen = static_cast<HdAuctionNumber*>(hdAuctionAllocateOnHeap(sizeof(HdAuctionNumber), l_heap.mpOwner->mpHeap, 4));
    if (screen == NULL) return NULL;
    screen = HdAuctionNumber__ct(screen);
    if (screen == NULL) return NULL;
    screen->setup(a, b, c);
    return screen;
}

/* 0261D138 */
static void HdAuctionNumber__dt(HdAuctionNumber* self, u32 flags) {
    if (self == NULL) return;
    self->mMemberVtable = &l_memberVtable;
    self->mSecondaryVtable = &l_secondaryVtable;
    hdAuctionBaseDestroy(self, 0);
    if (flags & HdAuction_Delete) hdAuctionDelete(self);
}

/* 0261D1A4 */
void HdAuctionNumber::rollInit() { mRollFrame = 0; }

/* 0261D1B0 */
void HdAuctionNumber::rollMove() {
    u32 frame = mRollFrame + 1;
    s32 digit = mShownDigit;
    f32 three = l_three;
    mRollFrame = frame;
    if (mRollingUp) {
        f32 progress = f32(f64(frame)) / three;
        hdAuctionSetFrame(mpLayout->mpAnimation, 0, 0, gabi::fadds_ppc(f32(f64(digit)), progress));
    } else {
        if (digit == 0) digit = HdAuction_DigitCount;
        f32 progress = f32(f64(frame)) / three;
        hdAuctionSetFrame(mpLayout->mpAnimation, 0, 0, gabi::fsubs_ppc(f32(f64(digit)), progress));
    }
    if (mRollFrame < HdAuction_RollFrames) return;
    u32 up = mRollingUp;
    s32 target = mTargetDigit;
    if (up) {
        digit = s16(digit + 1);
        if (digit >= (s32)HdAuction_DigitCount) {
            digit = 0;
            mShownDigit = 0;
            mRollFrame = 0;
            if (u32(digit) != u32(target)) return;
            hdAuctionChangeState(&mpStateMachine, &l_idle);
            return;
        }
    } else digit = s16(digit - 1);
    mShownDigit = (s16)digit;
    mRollFrame = 0;
    if (u32(digit) != u32(target)) return;
    hdAuctionChangeState(&mpStateMachine, &l_idle);
}

/* 0261D340 */
static HdAuctionNumber* HdAuctionNumber__ct(HdAuctionNumber* self) {
    if (self == NULL) {
        self = static_cast<HdAuctionNumber*>(hdAuctionAllocate(sizeof(HdAuctionNumber)));
        if (self == NULL) return NULL;
    }
    hdAuctionBaseConstruct(self);
    self->mMemberVtable = &l_memberVtable;
    self->mVtable = &l_screenVtable;
    self->mSecondaryVtable = &l_secondaryVtable;
    self->mTargetDigit = 0;
    self->mRollingUp = 0;
    self->mRollFrame = 0;
    self->mShownDigit = 0;
    return self;
}

/* 0261D3C0 */
u32 HdAuctionNumber::setup(u32 a, u32 b, u32 c) {
    if (!hdAuctionBaseSetup(this, &l_layoutName, a, b, c)) return 0;
    HdAuctionStateMachine* state = mpStateMachine;
    HdAuctionAction* action = gabi::call_ptr<HdAuctionAction*>(state->mVtable[HdAuction_GetState].get(), state, &l_idle);
    mpCurrentAction = action;
    gabi::call_ptr(action->mVtable[HdAuction_EnterState].get(), action);
    return 1;
}

/* 0261D454 */
void HdAuctionNumber::setDigit(u32 digit) {
    hdAuctionSetFrame(mpLayout->mpAnimation, 0, 0, f32(f64(s32(digit))));
}

/* 0261D4A8 */
void HdAuctionNumber::roll(u32 from, u32 to, u32 up) {
    if (from == to) return;
    mShownDigit = (s16)from;
    mTargetDigit = (s16)to;
    mRollingUp = (u8)up;
    hdAuctionChangeState(&mpStateMachine, &l_roll);
}

/* 0261D4D0 */
u32 HdAuctionNumber::loadScreen(u32 a, u32 b, u32 c, u32 d, u32 e) {
    HdAuctionLayout* screen = static_cast<HdAuctionLayout*>(hdAuctionAllocateOnHeap(sizeof(HdAuctionLayout), l_heap.mpOwner->mpHeap, 4));
    if (screen != NULL) screen = hdAuctionLayoutConstruct(screen);
    mpLayout = screen;
    if (screen == NULL) return 0;
    if (!gabi::call_ptr<u32>(screen->mVtable[HdAuction_LoadLayout].get(), screen, a, b, c, d, 1u, 1u, e)) return 0;
    hdAuctionBindAnimation(mpLayout->mpAnimation, 0, &l_animationNames.mNames[1], &l_animationNames.mNames[l_animationNameIndex]);
    return 1;
}

/* 0261D5BC */
static void __sinit_hd_screen_auction_number_cpp() {
    l_globals.mThird = 0;
    l_globals.mFirst = 0;
    l_globals.mFourth = 0;
    l_globals.mSecond = 0;
    hdAuctionRegister(&l_registration);
    f32 upper = l_upper;
    f32 lower = l_lower;
    l_range.mUpper = upper;
    l_range.mLower = lower;
    hdAuctionInitSigned(&l_range.mSignedInitialized);
    hdAuctionRegister(&l_signedRegistration);
    hdAuctionInitUnsigned(&l_range.mUnsignedInitialized);
    hdAuctionRegister(&l_unsignedRegistration);
    u32 guard = l_stateGuard;
    l_animationNames.mNames[1].mVtable = &l_stringVtable;
    l_animationNames.mNames[0].mVtable = &l_stringVtable;
    l_animationNames.mNames[0].mpString = gabi::at<void>(gabi::guest_addr("digit animation"));
    l_animationNames.mNames[1].mpString = gabi::at<void>(gabi::guest_addr("digit layout part"));
    l_layoutName.mVtable = &l_stringVtable;
    l_layoutName.mpString = gabi::at<void>(gabi::guest_addr("AuctionNumber_00"));
    u32 id;
    if (guard) id = l_lastStateID + 1;
    else { id = 1; l_stateGuard = 1; }
    l_roll.mInit.mAdjust = 0;
    l_roll.mInit.mSlot = 0x19;
    l_roll.mMove.mAdjust = 0;
    l_roll.mMove.mSlot = 0x1A;
    l_roll.mExit.mAdjust = 0;
    l_roll.mID = id;
    l_roll.mName = gabi::at<void>(gabi::guest_addr("Roll"));
    l_roll.mInit.mKind = 4;
    l_roll.mMove.mKind = 4;
    l_roll.mExit.mKind = 4;
    l_lastStateID = id;
    l_roll.mFlags = 0;
    l_roll.mVtable = &l_stateVtable;
    l_roll.mExit.mSlot = 0x1B;
    l_roll.mEmptyAdjust = 0;
    l_roll.mEmptySlot = 0;
    // HD: the last empty delegate's halfwords are stored separately before the parent link.
    l_roll.mpParent = &l_stateParent;
}

/* 0261D7CC */
static void hdAuction_emptyCompanion() {}
/* 0261D7D8 */
static void hdAuction_emptyRollInit() {}
/* 0261D7DC */
static void hdAuction_emptyRollMove() {}
/* 0261D7E0 */
static void hdAuction_emptyRollExit() {}

/* WWHD glue (tools/verify/autoglue.py): HD screen, states and animation names.
data l_heap 1018C404
data l_screenVtable 100E3270
data l_memberVtable 100E3350
data l_secondaryVtable 100E3360
data l_stringVtable 100E3170
data l_stateVtable 100E31D0
data l_stateParent 101FF32C
data l_idle 1049DD60
data l_roll 1048E140
data l_animationNames 1048E114
data l_layoutName 1048E170
data l_animationNameIndex 100E316C
data l_stateGuard 101FD8E4
data l_lastStateID 101FDD50
data l_three 100E321C
data l_globals 1048E130
data l_range 1048E124
data l_lower 100E3230
data l_upper 100E3234
data l_registration 101F5170
data l_signedRegistration 101F517C
data l_unsignedRegistration 101F5188
str 100E3250 "digit animation" renamed
str 100E3238 "digit layout part" renamed
str 100E325C "AuctionNumber_00" renamed
str 100E3240 "Roll" renamed
ext 0273AD10 void* hdAuctionAllocate(u32)
ext 0273B050 void* hdAuctionAllocateOnHeap(u32, void*, u32)
ext 0273AF40 void hdAuctionDelete(void*)
ext 02700ADC void hdAuctionBaseConstruct(HdAuctionNumber*)
ext 02700A70 void hdAuctionBaseDestroy(HdAuctionNumber*, u32)
ext 02700B48 u32 hdAuctionBaseSetup(HdAuctionNumber*, HdAuctionSafeString*, u32, u32, u32)
ext 0200552C void hdAuctionSetFrame(void*, u32, u32, f32)
ext 020063C0 void hdAuctionChangeState(void*, HdAuctionState*)
ext 026FADB0 HdAuctionLayout* hdAuctionLayoutConstruct(HdAuctionLayout*)
ext 02004E04 void hdAuctionBindAnimation(void*, u32, HdAuctionSafeString*, HdAuctionSafeString*)
ext 028F026C void hdAuctionRegister(void*)
ext 028ED6F8 void hdAuctionInitSigned(be<u8>*)
ext 028EAB2C void hdAuctionInitUnsigned(be<u8>*)
*/

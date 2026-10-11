/* Battery and paired-picture draw lists, GameCube names and HD layout.
 * Verified against WWHD. */
#include "d/d_2dnumber.h"

static const f32 l_shadowOffset = 4.0f;
static const f32 l_zero = 0.0f;
static const f32 l_numX = 551.0f;
static const f32 l_half = 0.5f;
static const f32 l_numY = 399.0f;
static const f32 l_initialRotation = 20.0f;
static const f32 l_ruleX = 519.0f;
static const f32 l_ruleY = 367.0f;
static const f32 l_batteryX = 498.0f;
static const f32 l_batteryY = 397.0f;
static const f32 l_degreeShadowX = 14.0f;
static const f32 l_onesShadowX = 9.0f;
static const f32 l_degreeX = 10.0f;
static const f32 l_onesX = 5.0f;
static const f32 l_rotationMin = 22.5f;
static const f32 l_rotationMax = 45.0f;
static const f32 l_textAngleBase = 15.0f;
static const f32 l_textAngleScale = 35.0f;
static const f32 l_belowRotation = 345.0f;
static const f32 l_aboveRotation = 310.0f;
static const f32 l_fullRotation = 360.0f;
static const f32 l_visualAngleScale = 1.399999976158142f;
static const f32 l_unitScale = 1.0f;

static void drawPicture(d2d_picture *picture, f32 x, f32 y, f32 width, f32 height) {
    J2DPicture_draw(picture, 0, 0, 0, x, y, width, height);
}
static void setup2D() {
    d2d_graf *graf = ((d2d_gameInfo *)dComIfGp_get())->mpCurrentGrafPort;
    gabi::call_ptr<void>(graf->__vtbl->setPort.get(), graf);
}

// HD: GHS initializes the draw-list base and each storage block out of line.
/* 0204698C */
dDlst_2DBattery_c::dDlst_2DBattery_c() {
    dDlst_base_construct(this);
    mRule = 0;
    mBatteryBase = 0;
    mBattery = 0;
    __vtbl = 0x10006C80u;
    for (u32 i = 0; i < 3; ++i)
        initializeStorage(mNum[i], 8);
    initializeStorage(&mPicturePos, 8);
    initializeStorage(&mNumPos, 8);
    initializeStorage(&mRuleSize, 8);
    initializeStorage(&mBatterySize, 8);
    initializeStorage(&mBatteryBaseSize, 8);
    initializeStorage(&mNumSize, 8);
    mRotation = l_zero;
}

/* 02046A68 */
u32 dDlst_2DBattery_c::init(ResTIMG *rule, ResTIMG *battery, ResTIMG *batteryBase,
                            ResTIMG *degree) {
    d2d_picture *q = J2DPicture_createImage(NULL, rule);
    mRule = q;
    if (!q)
        return 0;
    mRuleSize.x = (f32)rule->width;
    mRuleSize.y = (f32)rule->height;
    q = J2DPicture_createImage(NULL, battery);
    mBattery = q;
    if (!q)
        return 0;
    mBatterySize.x = (f32)battery->width;
    mBatterySize.y = (f32)battery->height;
    q = J2DPicture_createImage(NULL, batteryBase);
    mBatteryBase = q;
    if (!q)
        return 0;
    mBatteryBaseSize.x = (f32)batteryBase->width;
    mBatteryBaseSize.y = (f32)batteryBase->height;
    for (u32 n = 0; n < 8; n += 4) {
        q = J2DPicture_createImage(NULL, degree);
        mNum[0][n / 4] = q;
        if (!q)
            return 0;
        q = J2DPicture_createName(NULL, "rupy_num_00.bti");
        mNum[1][n / 4] = q;
        if (!q)
            return 0;
        q = J2DPicture_createName(NULL, "rupy_num_00.bti");
        mNum[2][n / 4] = q;
        if (!q)
            return 0;
    }
    mBattery->mAlpha = 200;
    f32 w = (f32)degree->width;
    d2d_picture *base = mRule;
    mNumSize.x = w;
    f32 h = (f32)degree->height, ch = mBatteryBaseSize.y, zero = l_zero, x = l_numX, y = l_numY,
        half = l_half;
    mNumSize.y = h;
    mPicturePos.y = zero;
    mPicturePos.x = zero;
    mNumPos.x = x;
    mNumPos.y = gabi::fmadds(ch, half, y);
    base->mAlpha = 128;
    mRule->mColorWhite = 0xFFFFFFFFu;
    mRule->mColorBlack = 0xFFFFFF00u;
    mNum[0][0]->mColorWhite = 0xFFC800FFu;
    d2d_picture *back = mNum[0][1];
    for (u32 n = 0; n < 4; ++n)
        back->mCornerColor[n] = 0xFF;
    mNum[0][1]->mAlpha = 80;
    mNum[1][0]->mColorWhite = 0xFFC800FFu;
    back = mNum[1][1];
    for (u32 n = 0; n < 4; ++n)
        back->mCornerColor[n] = 0xFF;
    mNum[1][1]->mAlpha = 80;
    mNum[2][0]->mColorWhite = 0xFFC800FFu;
    back = mNum[2][1];
    for (u32 n = 0; n < 4; ++n)
        back->mCornerColor[n] = 0xFF;
    mNum[2][1]->mAlpha = 80;
    mRotation = l_initialRotation;
    return 1;
}

/* 02046E70 */
void dDlst_2DBattery_c::draw() {
    setup2D();
    d2d_picture *q = mBattery;
    d2d_picture::VTable *vt = q->__vtbl;
    f32 h = mBatterySize.y, half = l_half, w = mBatterySize.x, angle = mRotation;
    q->mBasePosition.y = gabi::fmuls_ppc(h, half);
    q->mBasePosition.x = w;
    q->mRotation = angle;
    q->mRotationAxis = 0x7A;
    gabi::call_ptr<void>(vt->makeMatrix.get(), q);
    drawPicture(mRule, gabi::fadds_ppc(mPicturePos.x, l_ruleX),
                gabi::fadds_ppc(mPicturePos.y, l_ruleY), mRuleSize.x, mRuleSize.y);
    drawPicture(mBattery, gabi::fadds_ppc(mPicturePos.x, l_batteryX),
                gabi::fadds_ppc(mPicturePos.y, l_batteryY), mBatterySize.x, mBatterySize.y);
    drawPicture(mBatteryBase, gabi::fadds_ppc(mPicturePos.x, l_numX),
                gabi::fadds_ppc(mPicturePos.y, l_numY), mBatteryBaseSize.x, mBatteryBaseSize.y);
    f32 four = l_shadowOffset;
    drawPicture(mNum[2][1], gabi::fadds_ppc(mNumPos.x, l_degreeShadowX),
                gabi::fadds_ppc(mNumPos.y, four), mNumSize.x, mNumSize.y);
    w = mNumSize.x;
    drawPicture(mNum[1][1], gabi::fadds_ppc(gabi::fadds_ppc(mNumPos.x, w), l_onesShadowX),
                gabi::fadds_ppc(mNumPos.y, four), w, mNumSize.y);
    w = mNumSize.x;
    drawPicture(mNum[0][1],
                gabi::fadds_ppc(gabi::fadds_ppc(mNumPos.x, gabi::fadds_ppc(w, w)), four),
                gabi::fadds_ppc(mNumPos.y, four), w, mNumSize.y);
    drawPicture(mNum[2][0], gabi::fadds_ppc(mNumPos.x, l_degreeX), mNumPos.y, mNumSize.x,
                mNumSize.y);
    w = mNumSize.x;
    drawPicture(mNum[1][0], gabi::fadds_ppc(gabi::fadds_ppc(mNumPos.x, w), l_onesX), mNumPos.y, w,
                mNumSize.y);
    w = mNumSize.x;
    drawPicture(mNum[0][0], gabi::fadds_ppc(mNumPos.x, gabi::fadds_ppc(w, w)), mNumPos.y, w,
                mNumSize.y);
}

// HD: bounded formatting, assertion and NaN branch behavior differ from GC.
/* 020470F4 */
void dDlst_2DBattery_c::setRotate(f32 rot) {
    f32 low = l_rotationMin, high = l_rotationMax;
    if (low > rot || !(rot <= high))
        assertRotation("d_2dnumber.cpp", 0x123u, "deg_min_in <= rot && rot <= deg_max_in");
    f32 fifteen = l_textAngleBase, ratio = gabi::fsubs_ppc(rot, low) / low;
    s32 value = gabi::ftoi(gabi::fadds_ppc(gabi::fmadds(l_textAngleScale, ratio, fifteen), l_half));
    s32 tens = value / 10;
    gabi::Local<HudName16> name;
    char *buf = (char *)name.get();
    formatTextureName(buf, 16u, "rupy_num_%02d.bti", tens);
    J2DPicture_changeTexture(mNum[2][0], buf, 0u);
    J2DPicture_changeTexture(mNum[2][1], buf, 0u);
    formatTextureName(buf, 16u, "rupy_num_%02d.bti", (u32)value - (u32)tens * 10u);
    J2DPicture_changeTexture(mNum[1][0], buf, 0u);
    J2DPicture_changeTexture(mNum[1][1], buf, 0u);
    f32 twenty = l_initialRotation, result;
    if (!(rot >= twenty))
        result = l_belowRotation;
    else if (!(rot <= high))
        result = l_aboveRotation;
    else
        result =
            gabi::fsubs_ppc(l_fullRotation, gabi::fmadds(l_visualAngleScale,
                                                         gabi::fsubs_ppc(rot, twenty), fifteen));
    mRotation = result;
}

/* 020472CC */
dDlst_2DObject_c::dDlst_2DObject_c() {
    dDlst_base_construct(this);
    __vtbl = 0x10006C98u;
    initializeStorage(mPicture, 8);
    initializeStorage(&mPos, 8);
    initializeStorage(mSize, 16);
    mCurrentNo = 0;
    mScale = l_zero;
}

/* 02047358 */
u32 dDlst_2DObject_c::init(ResTIMG *img1, ResTIMG *img2) {
    d2d_picture *q = J2DPicture_createImage(NULL, img1);
    mPicture[0] = q;
    if (!q)
        return 0;
    mSize[0].x = (f32)img1->width;
    mSize[0].y = (f32)img1->height;
    q = J2DPicture_createImage(NULL, img2);
    mPicture[1] = q;
    if (!q)
        return 0;
    mSize[1].x = (f32)img2->width;
    f32 h = (f32)img2->height;
    mCurrentNo = 0;
    mSize[1].y = h;
    mScale = l_unitScale;
    return 1;
}

/* 0204747C */
void dDlst_2DObject_c::draw() {
    setup2D();
    u8 flag = (u8)mCurrentNo;
    f32 scale = mScale, y = mPos.y, x = mPos.x;
    u32 n = flag ? 1 : 0;
    f32 w = mSize[n].x, h = mSize[n].y;
    drawPicture(mPicture[flag ? 1 : 0], x, y, gabi::fmuls_ppc(w, scale), gabi::fmuls_ppc(h, scale));
}

/* ---- HD: compiler-generated header statics ---- */
/* 02047520 */
static void __sinit_d_2dnumber_cpp() {
    sinit_header_statics_rodata(0x10461344, 0x1018F68C, 0x10461350, 0x10006C78);
}
/* WWHD glue
data l_shadowOffset 10006BBC
data l_zero 10006BC0
data l_numX 10006BC4
data l_half 10006BC8
data l_numY 10006BCC
data l_initialRotation 10006BD0
data l_ruleX 10006BE4
data l_ruleY 10006BE8
data l_batteryX 10006BEC
data l_batteryY 10006BF0
data l_degreeShadowX 10006BF4
data l_onesShadowX 10006BF8
data l_degreeX 10006C00
data l_onesX 10006C04
data l_rotationMin 10006C08
data l_rotationMax 10006C0C
data l_textAngleBase 10006C10
data l_textAngleScale 10006C14
data l_belowRotation 10006C18
data l_aboveRotation 10006C1C
data l_fullRotation 10006C20
data l_visualAngleScale 10006C24
data l_unitScale 10006C74
str 10006BD4 "rupy_num_00.bti"
str 10006C28 "rupy_num_%02d.bti"
str 10006C3C "d_2dnumber.cpp"
str 10006C4C "deg_min_in <= rot && rot <= deg_max_in"
ext 0252CCBC void dDlst_base_construct(void* object)
ext 028F521C void initializeStorage(void* storage, u32 size)
ext 027EF308 d2d_picture* J2DPicture_createImage(d2d_picture* object, ResTIMG* image)
ext 027EF484 d2d_picture* J2DPicture_createName(d2d_picture* object, const char* name)
ext 027EFA58 void J2DPicture_draw(d2d_picture* picture, u32 mirrorX, u32 mirrorY, u32 transpose, f32 x, f32 y, f32 width, f32 height)
ext 027EFB68 void J2DPicture_changeTexture(d2d_picture* picture, const char* name, u32 index)
ext 028F17A8 void formatTextureName(void* buffer, u32 size, const char* format, u32 digit)
ext 0273AA24 void assertRotation(const char* file, u32 line, const char* condition)
*/

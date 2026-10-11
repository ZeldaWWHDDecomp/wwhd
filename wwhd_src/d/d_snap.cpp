/** Picto Box photo registration, geometry and HD texture capture.
 * Verified against the original WWHD unit. */
#include "d/d_snap.h"

static u32 l_snapAddress;
static u32 l_snapInitialized;
static f32 l_pixelScale;
static f32 l_captureWidth;
static f32 l_captureHeight;
static f32 l_largeCaptureWidth;
static f32 l_largeCaptureHeight;
static f32 l_shutterWidth;
static f32 l_shutterHeight;
static f32 l_shutterCenterX;
static f32 l_shutterCenterY;
static f32 l_minAngle;
static f32 l_maxAngle;
static u32 l_stateWord0;
static u32 l_stateWord1;
static u32 l_stateWord2;
static u32 l_stateWord3;
static u32 l_opaDrawList;
static u32 l_xluDrawList;
static u32 l_textureManager;
static u32 l_debugDrawer;
static u32 l_postProcessService;
static u32 l_graphicsResource;
static u32 l_textureNameInitialized;
static u32 l_textureNameVtable;
static u32 l_textureRenderer;
static u32 l_renderContext;
static u32 l_saveData;
static f32 l_viewportOriginX;
static f32 l_viewportOriginY;
static u32 l_initialScaleX;
static u32 l_initialScaleY;
static const dSnap_CharaData l_CharaData[] = {
    {0, 10, 0, 25, 80, 500, 8192, 5000, 3, 2},      {0, 20, 20, 40, 90, 500, 8192, 5000, 3, 2},
    {0, 10, 0, 35, 130, 500, 8192, 5000, 3, 2},     {0, 10, 10, 50, 130, 300, 32767, 3000, 3, 2},
    {0, 20, 0, 45, 160, 500, 10000, 2000, 3, 2},    {0, -30, 0, 30, 60, 500, 16383, 2000, 3, 2},
    {0, 20, 20, 55, 140, 500, 10000, 5000, 3, 2},   {0, 20, 20, 35, 150, 500, 10000, 5000, 3, 2},
    {0, 10, 0, 25, 70, 500, 8192, 5000, 3, 2},      {0, 10, 0, 25, 70, 500, 8192, 5000, 3, 2},
    {0, 0, 0, 50, 150, 550, 8192, 5000, 3, 12},     {0, 0, 0, 50, 60, 400, 16000, 4000, 3, 0},
    {0, -40, 0, 60, 80, 300, 32767, 3000, 3, 0},    {0, 0, 0, 20, 20, 400, 32000, 3000, 3, 0},
    {0, 30, 0, 50, 120, 200, 18000, 3000, 2, 2},    {0, 10, 0, 35, 120, 500, 8192, 5000, 2, 2},
    {0, 10, 20, 60, 160, 500, 10000, 5000, 2, 2},   {0, 10, 0, 50, 140, 500, 22000, 5000, 2, 2},
    {0, 10, 20, 60, 200, 500, 10000, 5000, 2, 2},   {0, 90, 30, 60, 120, 550, 8192, 5000, 2, 2},
    {0, 75, 30, 40, 80, 550, 10000, 5000, 2, 2},    {0, 20, 0, 80, 180, 550, 19000, 5000, 2, 2},
    {0, 15, 0, 60, 135, 550, 13000, 5000, 2, 2},    {0, 15, 0, 50, 155, 550, 10000, 5000, 2, 2},
    {0, 10, 0, 30, 80, 550, 13000, 5000, 2, 2},     {0, 5, 0, 30, 80, 550, 10000, 5000, 2, 2},
    {0, 5, 10, 45, 180, 550, 10000, 5000, 2, 2},    {0, 20, 0, 45, 160, 550, 21000, 5000, 2, 2},
    {0, 10, 10, 45, 130, 550, 11000, 5000, 2, 2},   {0, 80, 0, 35, 80, 500, 18000, 5000, 2, 2},
    {0, 80, 0, 40, 85, 550, 10000, 5000, 2, 2},     {0, 30, 10, 45, 115, 550, 22000, 5000, 2, 2},
    {0, 10, 0, 45, 150, 550, 17000, 5000, 2, 2},    {0, 10, 10, 45, 150, 550, 10000, 5000, 2, 2},
    {0, 10, 10, 45, 145, 550, 15000, 5000, 2, 2},   {0, 20, 10, 45, 145, 550, 16000, 5000, 2, 2},
    {0, 20, 10, 45, 145, 550, 16000, 5000, 2, 2},   {0, 30, 35, 80, 160, 550, 15000, 5000, 2, 2},
    {0, 30, 30, 75, 170, 550, 9000, 5000, 2, 2},    {0, 30, 30, 75, 170, 550, 9000, 5000, 2, 2},
    {0, 30, 30, 75, 170, 550, 9000, 5000, 2, 2},    {0, 20, 0, 30, 100, 500, 10000, 4000, 1, 2},
    {0, 30, 0, 50, 160, 500, 12000, 4500, 1, 2},    {0, 30, 0, 50, 160, 500, 12000, 4500, 1, 2},
    {0, 30, 0, 50, 160, 500, 10000, 4500, 1, 2},    {0, 20, 10, 50, 80, 500, 12000, 4500, 1, 2},
    {0, 10, 0, 50, 90, 500, 13000, 5000, 1, 2},     {0, 20, 10, 50, 80, 500, 12000, 4500, 1, 2},
    {0, 10, 0, 55, 120, 550, 22000, 5000, 1, 2},    {0, 10, 0, 55, 120, 550, 22000, 5000, 1, 2},
    {0, 10, 0, 55, 120, 550, 22000, 5000, 1, 2},    {0, 10, 0, 55, 120, 550, 22000, 5000, 1, 2},
    {0, 0, 0, 80, 120, 200, 32767, 2000, 1, 0},     {0, 30, 10, 80, 150, 500, 10000, 3000, 1, 2},
    {0, 20, 20, 40, 120, 500, 8192, 5000, 1, 2},    {0, 20, 30, 40, 90, 500, 8192, 5000, 1, 2},
    {0, 100, 40, 50, 60, 500, 17000, 5000, 1, 2},   {0, 10, 0, 65, 200, 550, 11000, 5000, 1, 2},
    {0, 50, 0, 300, 400, 100, 32767, 4000, 1, 0},   {0, 0, 0, 25, 25, 550, 11000, 5000, 1, 0},
    {0, 0, 0, 50, 150, 550, 8192, 5000, 1, 12},     {0, 0, 0, 50, 150, 550, 8192, 5000, 1, 12},
    {0, 10, 0, 35, 120, 550, 11000, 5000, 1, 2},    {0, 0, 0, 50, 150, 550, 8192, 5000, 1, 14},
    {0, 0, 0, 50, 150, 550, 8192, 5000, 1, 12},     {0, 20, 0, 30, 100, 500, 10000, 4000, 6, 2},
    {0, 10, 0, 30, 60, 500, 8192, 2000, 6, 2},      {0, 20, 0, 60, 200, 500, 8192, 5000, 6, 2},
    {0, 20, 0, 50, 150, 500, 12000, 5000, 6, 2},    {0, 20, 25, 70, 170, 500, 8192, 5000, 6, 2},
    {0, 20, 25, 70, 170, 500, 8192, 5000, 6, 2},    {0, 20, 25, 70, 170, 500, 8192, 5000, 6, 2},
    {0, 20, 25, 70, 170, 500, 8192, 5000, 6, 2},    {0, 20, 25, 70, 170, 500, 8192, 5000, 6, 2},
    {0, 20, 25, 70, 170, 500, 8192, 5000, 6, 2},    {0, 20, 10, 50, 170, 500, 8192, 5000, 6, 2},
    {0, 20, 10, 50, 170, 500, 8192, 5000, 6, 2},    {0, 90, 30, 60, 90, 500, 24576, 5000, 6, 2},
    {0, 20, 25, 70, 170, 500, 8192, 5000, 6, 2},    {0, 30, 0, 50, 150, 500, 8192, 5000, 6, 2},
    {0, -300, 0, 870, 700, 100, 24576, 3000, 6, 0}, {0, 0, 0, 1000, 800, 100, 18000, 1000, 6, 0},
    {0, 0, 0, 50, 150, 550, 8192, 5000, 6, 14},     {0, 0, 0, 40, 55, 500, 11000, 4000, 7, 0},
    {0, 15, 0, 40, 100, 500, 11000, 4000, 7, 0},    {0, 15, 0, 40, 100, 500, 11000, 4000, 7, 0},
    {0, 15, 0, 40, 100, 500, 11000, 4000, 7, 0},    {0, 15, 0, 40, 100, 500, 11000, 4000, 7, 0},
    {0, 15, 0, 40, 100, 500, 11000, 4000, 7, 0},    {0, 15, 0, 40, 100, 500, 11000, 4000, 7, 0},
    {0, 15, 0, 40, 100, 500, 11000, 4000, 7, 0},    {0, 15, 0, 40, 100, 500, 11000, 4000, 7, 0},
    {0, 15, 0, 40, 100, 500, 11000, 4000, 7, 0},    {0, 700, 800, 200, 200, 300, 32767, 3000, 7, 0},
    {0, 85, 0, 70, 80, 550, 8192, 5000, 7, 2},      {0, 0, 0, 50, 150, 550, 8192, 5000, 7, 6},
    {0, 0, 0, 50, 150, 550, 8192, 5000, 7, 14},     {0, 20, 80, 60, 100, 500, 8192, 5000, 5, 0},
    {0, 10, 20, 60, 100, 400, 16000, 4000, 5, 0},   {0, 20, 20, 40, 50, 500, 8192, 3000, 5, 0},
    {0, 0, 0, 60, 60, 200, 30000, 3000, 5, 0},      {0, -25, 30, 40, 60, 300, 20000, 3000, 5, 0},
    {0, -40, 50, 50, 80, 300, 12000, 3000, 5, 0},   {0, -50, 0, 50, 100, 200, 16384, 3000, 5, 0},
    {0, -80, 0, 80, 120, 300, 32767, 2000, 5, 0},   {0, 30, 0, 120, 50, 100, 11000, 2000, 5, 0},
    {0, -30, 0, 30, 60, 300, 32767, 3000, 5, 0},    {0, -20, 0, 60, 100, 400, 32767, 3000, 5, 0},
    {10, -50, 0, 60, 100, 500, 11000, 3000, 5, 0},  {0, 10, 10, 60, 130, 500, 11000, 4000, 5, 0},
    {0, 20, 20, 130, 300, 500, 12000, 5000, 5, 0},  {0, 20, 20, 80, 160, 400, 12000, 3000, 5, 0},
    {0, 20, 50, 70, 170, 300, 18000, 3000, 5, 0},   {0, 20, 50, 70, 170, 200, 18000, 2000, 5, 0},
    {0, 0, 0, 400, 400, 200, 30000, 3000, 5, 0},    {0, 0, 0, 200, 170, 300, 32700, 2000, 5, 0},
    {0, 20, 50, 100, 140, 300, 11000, 2000, 4, 0},  {0, -120, 0, 100, 100, 300, 11000, 2000, 4, 0},
    {0, 10, 10, 100, 200, 300, 11000, 2000, 4, 0},  {0, 10, 10, 100, 200, 300, 11000, 2000, 4, 0},
    {0, 10, 10, 100, 200, 300, 11000, 2000, 4, 0},  {0, 10, 10, 100, 250, 300, 11000, 2000, 4, 0},
    {0, 20, 10, 70, 210, 300, 11000, 3000, 4, 0},   {0, 20, 30, 70, 150, 300, 11000, 3000, 4, 0},
    {0, 20, 30, 70, 150, 300, 11000, 3000, 4, 0},   {0, 200, 0, 1000, 1200, 100, 28000, 1000, 4, 0},
    {0, -200, 0, 360, 450, 500, 20000, 500, 4, 0},  {0, 0, 0, 400, 380, 300, 32000, 2000, 4, 0},
    {0, 0, 50, 350, 400, 100, 18000, 1000, 4, 0},   {0, 200, 50, 300, 550, 200, 16000, 2000, 4, 0},
    {0, 200, 50, 350, 400, 200, 16000, 4000, 4, 0}, {0, 100, 30, 500, 300, 200, 16000, 2000, 4, 0},
    {0, 400, 0, 800, 800, 200, 18000, 1500, 4, 0},  {0, 10, 0, 130, 280, 200, 18000, 1200, 4, 10},
};
static const cXyz cylvtx[] = {
    {0.0f, 0.0f, 1.0f},  {0.707099974155426f, 0.0f, 0.707099974155426f},
    {1.0f, 0.0f, 0.0f},  {0.707099974155426f, 0.0f, -0.707099974155426f},
    {0.0f, 0.0f, -1.0f},
};
static const cXyz vdata[] = {
    {-0.525731086730957f, 0.0f, 0.8506507873535156f},
    {0.525731086730957f, 0.0f, 0.8506507873535156f},
    {-0.525731086730957f, 0.0f, -0.8506507873535156f},
    {0.525731086730957f, 0.0f, -0.8506507873535156f},
    {0.0f, 0.8506507873535156f, 0.525731086730957f},
    {0.0f, 0.8506507873535156f, -0.525731086730957f},
    {0.0f, -0.8506507873535156f, 0.525731086730957f},
    {0.0f, -0.8506507873535156f, -0.525731086730957f},
    {0.8506507873535156f, 0.525731086730957f, 0.0f},
    {-0.8506507873535156f, 0.525731086730957f, 0.0f},
    {0.8506507873535156f, -0.525731086730957f, 0.0f},
    {-0.8506507873535156f, -0.525731086730957f, 0.0f},
};
static const u32 tindices[][3] = {
    {0, 4, 1}, {0, 9, 4},  {9, 5, 4},  {4, 5, 8},  {4, 8, 1},  {8, 10, 1}, {8, 3, 10},
    {5, 3, 8}, {5, 2, 3},  {2, 7, 3},  {7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6},
    {0, 1, 6}, {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5},  {7, 2, 11},
};
static const dSnap_JudgeEntry m_judge_tbl[] = {
    {&dSnap_packet::JudgePost},  {&dSnap_packet::JudgeBikutsuki}, {&dSnap_packet::JudgeCoupleLook},
    {&dSnap_packet::JudgeGF},    {&dSnap_packet::JudgeGenzo},     {&dSnap_packet::JudgeObasan4},
    {&dSnap_packet::JudgeGene},  {&dSnap_packet::JudgeGene},      {&dSnap_packet::JudgeGene},
    {&dSnap_packet::JudgeTestM},
};

static dSnapHD_IdentityWords l_identityWords;
static dSnap_packet l_snap;

// GHS float copy pairs retain the exact word bits; arithmetic fields still use normal f32 loads.
static f32 dSnapHD_copyFloatBits(const be<f32> &value) {
    u32 bits = snapFieldBits<u32>(value);
    f32 result;
    memcpy(&result, &bits, 4);
    return result;
}

static void snap_texture_copy(u32 dst, u32 src) {
    dSnapHD_Texture *destination = gabi::at<dSnapHD_Texture>(dst);
    dSnapHD_Texture *source = gabi::at<dSnapHD_Texture>(src);

    for (u32 i = 0; i < 0x78; i += 4)
        destination->mWords[i / 4] = source->mWords[i / 4];

    for (u32 i = 0x78; i < 0x7C; ++i)
        destination->mBytes[i] = source->mBytes[i];

    for (u32 i = 0x7C; i <= 0x88; i += 4)
        destination->mWords[i / 4] = source->mWords[i / 4];

    destination->mBytes[0x90] = source->mBytes[0x90];
    destination->mBytes[0x91] = source->mBytes[0x91];

    for (u32 i = 0x94; i <= 0x148; i += 4)
        destination->mWords[i / 4] = source->mWords[i / 4];

    destination->mBytes[0x14C] = source->mBytes[0x14C];

    for (u32 i = 0x150; i <= 0x18C; i += 4)
        destination->mWords[i / 4] = source->mWords[i / 4];
}

/* 025BD600 */
s32 dSnap_PhotoIndex2TableIndex(s32 index) {

    u32 n = u32(index) - DSNAP_TYPE_NPC_LS1;
    if (n >= CharaDataTblSize)
        JUTAssertion::showAssert("d_snap.cpp", 0x1A7, "index >= 0 && index < CharaDataTblSize");
    return s32(n);
}

/* 025BD64C */
u32 dSnap_GetFigRoomId(s32 index) {

    if (index >= CharaDataTblSize)
        return 255;
    return l_CharaData[index].figRoom;
}

/* 025BD71C */
u32 dSnap_Obj_ctor(u32 s) {

    if (!s)
        s = snap_api::alloc(0x34);
    if (s) {
        dSnap_RegistObjElm *object = gabi::at<dSnap_RegistObjElm>(s);
        object->mMaxY = 0;
        object->mMaxX = 0;
        object->mActorPID = 0xFFFFFFFF;
        object->__vtbl = 0x100546AC;
        object->mCullAngle = 32767;
        object->mMinY = 0;
        object->mRadius = 0;
        object->field_0x1b = 0;
        object->mCapturedRatio = 0;
        object->mPhotoNo = 0;
        object->mHeight = 0;
        object->mCapturedPixels = 0;
        object->field_0x19 = 4;
        object->mAngleY = 0;
        object->mMinX = 0;
        object->field_0x1a = 255;
    }
    return s;
}

/* 025BDC78 */
f32 dSnap_Obj::CalcArea2D() {

    f32 total = 0;

    f32 x = (f32)mCenter.x, y = (f32)mCenter.y, z = (f32)mCenter.z;

    bool sphere = ((u8)field_0x1b & 1) != 0;

    f32 radius = (f32)mRadius, height = (f32)mHeight;

    snap_api::MTXTrans(gabi::ea(mDoMtx_stack_c::get()), x, y, z);

    gabi::Local<cXyz> input, world, p0, p1, p2, p3, base, top, c0, c1, c2, c3;

    auto vset = [&](u32 a, f32 vx, f32 vy, f32 vz) {
        cXyz *v = gabi::at<cXyz>(a);
        v->x = vx;
        v->y = vy;
        v->z = vz;
    };

    auto project = [&](u32 dst) {
        snap_api::MTXMultVec(gabi::ea(mDoMtx_stack_c::get()), input.a, world.a);
        dSnap_project(world.a, dst);
    };

    auto tri = [&](u32 a, u32 b, u32 c) {
        f32 a0 = (f32)gabi::at<cXyz>(a)->x, a1 = (f32)gabi::at<cXyz>(a)->y,
            b0 = (f32)gabi::at<cXyz>(b)->x, b1 = (f32)gabi::at<cXyz>(b)->y,
            c0 = (f32)gabi::at<cXyz>(c)->x, c1 = (f32)gabi::at<cXyz>(c)->y;
        f32 product = snap_api::vectorProduct2d(a0, a1, b0, b1, c0, c1);
        f32 area = 0.5f * product;
        if (area > 0)
            total += area;
    };

    if (!sphere) {

        snap_api::scaleM(radius, height, radius);

        snap_api::YrotM(gabi::ea(mDoMtx_stack_c::get()), (s16)mAngleY);

        vset(base.a, 0, 0, 0);
        snap_api::MTXMultVec(gabi::ea(mDoMtx_stack_c::get()), base.a, base.a);

        vset(top.a, 0, 1, 0);
        snap_api::MTXMultVec(gabi::ea(mDoMtx_stack_c::get()), top.a, top.a);

        for (u32 i = 0; i < (u8)field_0x19; ++i) {

            const cXyz *curr = &cylvtx[i];
            const cXyz *next = &cylvtx[i + 1];

            f32 cx = (f32)curr->x, cz = (f32)curr->z;

            vset(input.a, cx, 0, cz);
            project(p0.a);
            input->y = 1;
            project(p1.a);

            f32 nx = (f32)next->x, nz = (f32)next->z;

            input->x = nx;
            input->z = nz;
            project(p2.a);
            input->y = 0;
            project(p3.a);

            tri(p0.a, p1.a, p2.a);
            tri(p0.a, p2.a, p3.a);

            dSnap_project(top.a, c0.a);
            tri(c0.a, p2.a, p1.a);

            dSnap_project(base.a, c1.a);
            tri(c1.a, p0.a, p3.a);

            vset(input.a, -cx, 1, cz);
            project(p0.a);
            input->y = 0;
            project(p1.a);

            input->x = -nx;
            input->z = nz;
            project(p2.a);
            input->y = 1;
            project(p3.a);

            tri(p0.a, p1.a, p2.a);
            tri(p0.a, p2.a, p3.a);

            dSnap_project(top.a, c2.a);
            tri(c2.a, p0.a, p3.a);

            dSnap_project(base.a, c3.a);
            tri(c3.a, p2.a, p1.a);
        }

    } else {

        snap_api::scaleM(radius, radius, radius);

        for (u32 i = 0; i < 20; ++i) {

            u32 outputs[] = {p0.a, p1.a, p2.a};

            for (u32 j = 0; j < 3; ++j) {
                u32 index = tindices[i][j];
                const cXyz *vert = &vdata[index];
                vset(input.a, (f32)vert->x, (f32)vert->y, (f32)vert->z);
                project(outputs[j]);
            }

            tri(p0.a, p1.a, p2.a);
        }
    }

    return total;
}

/* 025BF01C */
void dSnap_Obj::draw(s32 alpha) {

    gabi::Local<dSnapHD_Color> color;
    color->r = 1;
    color->g = 0;
    color->b = 0;
    color->a = f32(alpha) / 255.0f;
    f32 diameter = (f32)mRadius * 2;
    bool sphere = ((u8)field_0x1b & 1) != 0;
    f32 x = (f32)mCenter.x, y = (f32)mCenter.y, z = (f32)mCenter.z;
    if (!sphere) {
        f32 h = (f32)mHeight;
        snap_api::MTXTrans(gabi::ea(mDoMtx_stack_c::get()), x, gabi::fmadds(h, 0.5f, y), z);
        snap_api::scaleM(diameter, h, diameter);
        snap_api::YrotM(gabi::ea(mDoMtx_stack_c::get()), (s16)mAngleY);
    } else {
        snap_api::MTXTrans(gabi::ea(mDoMtx_stack_c::get()), x, y, z);
        snap_api::scaleM(diameter, diameter, diameter);
    }
    u32 drawer = (u32)l_debugDrawer;
    gabi::Local<u8[48]> mtx;
    dSnap_MtxCopy(mtx.a, gabi::ea(mDoMtx_stack_c::get()));
    snap_api::setDebugModelMtx(drawer, mtx.a);
    if (sphere)
        snap_api::drawSphere(drawer, color.a, color.a);
    else
        snap_api::drawCylinder(drawer, color.a, color.a);
}

/* 025BEB5C */
void dSnap_Obj::SetGeo(u32 v, f32 radius, f32 height, s16 angle) {
    cXyz *center = gabi::at<cXyz>(v);

    field_0x1b = (u8)field_0x1b & 0xFE;
    mCenter.x = (f32)center->x;
    mCenter.y = (f32)center->y;
    f32 z = (f32)center->z;
    mRadius = radius;
    mCenter.z = z;
    mHeight = height;
    mAngleY = angle;
}

/* 025BEF3C */
void dSnap_Obj::SetGeoSph(u32 v, f32 radius) {
    cXyz *center = gabi::at<cXyz>(v);

    field_0x1b = (u8)field_0x1b | 1;
    mCenter.x = (f32)center->x;
    mCenter.y = (f32)center->y;
    f32 z = (f32)center->z;
    mHeight = 0;
    mCenter.z = z;
    mAngleY = 0;
    mRadius = radius;
}

/* 025BEB90 */
void dSnap_Obj::SetInf(u32 type, u32 actor, u32 room, u32 points, s16 angle) {

    mPhotoNo = u8(type);
    u32 id = actor ? (u32)gabi::at<fopAc_ac_c>(actor)->mBsPcId : 0xFFFFFFFF;
    field_0x19 = u8(points);
    field_0x1a = u8(room);
    mCullAngle = angle;
    mActorPID = id;
}

/* 025BE208 */
void dSnap_Obj::SetAreaClear() {

    mMinX = 2048;
    mMaxX = -2048;
    mMinY = 2048;
    mMaxY = -2048;
}

/* 025BE224 */
void dSnap_Obj::SetArea(s16 x, s16 y) {

    s16 minx = (s16)mMinX, miny = (s16)mMinY;
    if (minx > x)
        mMinX = x;
    s16 maxx = (s16)mMaxX;
    if (miny > y)
        mMinY = y;
    s16 maxy = (s16)mMaxY;
    if (maxx < x)
        mMaxX = x;
    if (maxy < y)
        mMaxY = y;
}

/* 025BF1C8 */
u32 dSnap_Obj::ChkPhoto(u32 type) { return u32((u8)mPhotoNo) == type; }

// HD: scale the pixel threshold by the capture resolution.
/* 025BF1DC */
u32 dSnap_Obj::ChkSuccess(s32 area, f32 ratio) {

    s32 threshold = gabi::ftoi(f32(area) * (f32)l_pixelScale);
    if ((s32)mCapturedPixels <= threshold)
        return 0;
    return (f32)mCapturedRatio > ratio;
}

// HD: retain the full callee register for the angle-distance comparison.
/* 025BE268 */
u32 dSnap_Obj::ChkCamCull() {

    if (!((u8)field_0x1b & 1) && (s16)mCullAngle != 32767) {
        u32 play = snap_api::getPlay();
        s32 index = (s8)gabi::at<dComIfG_play_c>(play + 0x12A0)->mPlayerInfo[0].mCameraID;
        play = snap_api::getPlay();
        u32 camera = (u32)gabi::at<dComIfG_play_c>(play + 0x12A0)->mCameraInfo[u32(index)].mpCamera;
        gabi::Local<cXyz> diff;
        snap_api::VECSubtract(camera + 0xDC, gabi::ea(this), diff.a);
        f32 z = (f32)diff->z, x = (f32)diff->x;
        s32 angle = snap_api::atan2s(x, z);
        s32 distance = snap_api::distanceAngleS(angle, (s16)mAngleY);
        if (distance > (s16)mCullAngle)
            return 1;
    }
    return 0;
}

/* 025BD7B4 */
u32 dSnap_RegistObjElm_ctor(u32 s) {

    if (!s)
        s = snap_api::alloc(0x3C);
    if (s) {
        dSnap_RegistObjElm *object = gabi::at<dSnap_RegistObjElm>(s);
        object->__elmVtable = 0x100546BC;
        dSnap_Obj_ctor(s);
        object->field_0x34 = 0;
    }
    return s;
}

/* 025BE908 */
u32 dSnap_RegistObjElm::Regist(u32 src) {
    const dSnap_Obj *source = gabi::at<dSnap_Obj>(src);

    // GHS copies the center as integer words, preserving signalling NaNs.
    mCenter = source->mCenter;
    mRadius = (f32)source->mRadius;
    mHeight = (f32)source->mHeight;
    mActorPID = (u32)source->mActorPID;
    mPhotoNo = (u8)source->mPhotoNo;
    field_0x19 = (u8)source->field_0x19;
    field_0x1a = (u8)source->field_0x1a;
    field_0x1b = (u8)source->field_0x1b;
    mCullAngle = (s16)source->mCullAngle;
    mAngleY = (s16)source->mAngleY;
    mCapturedPixels = (u32)source->mCapturedPixels;
    mCapturedRatio = (f32)source->mCapturedRatio;
    mMinX = (s16)source->mMinX;
    mMinY = (s16)source->mMinY;
    mMaxX = (s16)source->mMaxX;
    mMaxY = (s16)source->mMaxY;
    return gabi::ea(this);
}

// HD: allocate capture targets, camera, projection and post-process resources from the supplied heap.
/* 025BD8E4 */
void dSnap_packet::Create(u32 heap) {

    mpHeap = heap;
    mResult = 0;
    mResultDetail = 0;
    mFlag = 0;

    u32 p = snap_api::allocFromHeap(0x148, heap, 4);
    if (p)
        p = snap_api::colorTargetCtor(p);

    heap = (u32)mpHeap;
    mpColorTexture = p;

    p = snap_api::allocFromHeap(0x154, heap, 4);
    if (p)
        p = snap_api::depthTargetCtor(p);

    heap = (u32)mpHeap;
    mpDepthTexture = p;

    p = snap_api::allocFromHeap(0x40, heap, 4);
    if (p)
        p = snap_api::renderBufferCtor(p);

    heap = (u32)mpHeap;
    mpRenderBuffer = p;

    p = snap_api::allocFromHeap(0x58, heap, 4);

    if (p) {

        gabi::at<dSnapHD_Camera>(p)->mVtable = 0x101450D0;
        for (u32 i = 0; i < 12; ++i)
            gabi::at<dSnapHD_Camera>(p)->mMatrixWords[i] = l_identityWords.mWords[i];

        gabi::at<dSnapHD_Camera>(p)->mVtable = 0x101450F8;

        u32 v = p + 0x34;
        if (!v)
            v = snap_api::alloc(12);
        if (v) {
            gabi::at<Quaternion>(v)->y = 0;
            gabi::at<Quaternion>(v)->x = 0;
            gabi::at<Quaternion>(v)->z = 10;
        }

        v = p + 0x40;
        if (!v)
            v = snap_api::alloc(12);
        if (v) {
            gabi::at<Quaternion>(v)->y = 0;
            gabi::at<Quaternion>(v)->x = 0;
            gabi::at<Quaternion>(v)->z = 0;
        }

        v = p + 0x4C;
        if (!v)
            v = snap_api::alloc(12);
        if (v) {
            gabi::at<Quaternion>(v)->y = 1;
            gabi::at<Quaternion>(v)->x = 0;
            gabi::at<Quaternion>(v)->z = 0;
        }
    }

    heap = (u32)mpHeap;
    mpCamera = p;

    p = snap_api::allocFromHeap(0xD4, heap, 4);
    if (p)
        p = snap_api::projectionCtor(p);

    heap = (u32)mpHeap;
    mpProjection = p;

    p = snap_api::allocFromHeap(0x1C, heap, 4);

    if (p) {
        gabi::at<dSnapHD_Buffer>(p)->mVtable = 0x1005466C;
        gabi::at<dSnapHD_Buffer>(p)->mHeight = 1;
        gabi::at<dSnapHD_Buffer>(p)->mWidth = 1;
        u32 v = p + 8;
        if (!v)
            v = snap_api::alloc(16);
        if (v) {
            gabi::at<Quaternion>(v)->z = 1;
            gabi::at<Quaternion>(v)->y = 0;
            gabi::at<Quaternion>(v)->x = 0;
            gabi::at<Quaternion>(v)->w = 1;
        }
    }

    mpViewport = p;

    u32 service = (u32)l_postProcessService, graphics = (u32)l_graphicsResource;

    u32 first = service + 0x190;

    mpEffect = first;
    u32 vt = (u32)gabi::at<dSnapHD_Service>(service)->mVtable;
    dSnapHD_Vtable *vtView = gabi::at<dSnapHD_Vtable>(vt);

    u32 desc = (u32)gabi::at<dSnapHD_Graphics>(graphics)->mpTexture;
    u32 target = (u32)vtView->mSetTexture;

    gabi::Local<be<u32>> token, arg1, arg2;
    u32 ref = snap_api::getTextureResource(token.a, desc);
    be<u32> *refView = gabi::at<be<u32>>(ref);

    *arg1 = (u32)*refView;
    gabi::call_ptr<void>(target, first + 4, arg1.a);

    u32 second = service + 0xC00;
    mpAlternateEffect = second;
    desc = (u32)gabi::at<dSnapHD_Graphics>(graphics)->mpAlternateTexture;
    vt = (u32)gabi::at<dSnapHD_Service>(service)->mAlternateVtable;
    vtView = gabi::at<dSnapHD_Vtable>(vt);

    target = (u32)vtView->mSetTexture;

    ref = snap_api::getTextureResource(token.a, desc);

    refView = gabi::at<be<u32>>(ref);

    *arg2 = (u32)*refView;
    gabi::call_ptr<void>(target, second + 4, arg2.a);
}

/* 025BDC38 */
void dSnap_packet::ReleaseShutter() { mFlag = (u32)mFlag | SnapCapture; }

/* 025BDC54 */
u32 dSnap_packet::ChkCapture() { return (u32)mFlag & SnapCapture; }

/* 025BDC6C */
u32 dSnap_packet::ChkReleaseShutter() { return (u32)mFlag & SnapShutterReleased; }

/* 025BE870 */
void dSnap_packet::Execute() {

    if (ChkReleaseShutter()) {
        Judge();
        mFlag = (u32)mFlag & ~2u;
        for (u32 i = 0; i < SnapObjectCount; ++i)
            m_tbl[i].field_0x34 = 1000000000.0f;
        field_0x14 = 0;
    }
}

/* 025BE99C */
u32 dSnap_packet::Regist(u32 obj) {

    if (!ChkCapture() || ChkReleaseShutter())
        return 0;
    u32 play = snap_api::getPlay();
    s32 count = (s32)field_0x14;
    u32 actor = gabi::ea(gabi::at<dComIfG_play_c>(play + 0x12A0)->mPlayerInfo[0].mpPlayer.get()),
        base = gabi::ea(this) + 0xA0;
    if (count >= SnapObjectCount) {
        f32 far = -1000000000.0f;
        u32 best = 0;
        bool found = false;
        for (u32 i = 0; i < SnapObjectCount; ++i) {
            dSnap_RegistObjElm *elem = &m_tbl[i];
            f32 distance = (f32)elem->field_0x34;
            if (far < distance && (u8)elem->mPhotoNo >= 72) {
                far = distance;
                best = i;
                found = true;
            }
        }
        if (!found)
            return 0;
        f32 distance = snap_api::VECSquareDistance(actor + 0x314, obj);
        if ((u8)gabi::at<dSnap_Obj>(obj)->mPhotoNo >= 72 && distance > far)
            return 0;
        u32 registered = gabi::at<dSnap_RegistObjElm>(base + best * 60)->Regist(obj);
        dSnap_RegistObjElm *registeredView = gabi::at<dSnap_RegistObjElm>(registered);

        registeredView->field_0x34 = distance;
        return 0;
    }
    gabi::at<dSnap_RegistObjElm>(base + u32(count) * 60)->Regist(obj);
    count = (s32)field_0x14;
    f32 distance = snap_api::VECSquareDistance(actor + 0x314, base + u32(count) * 60);
    count = (s32)field_0x14;
    m_tbl[u32(count)].field_0x34 = distance;
    field_0x14 = (u32)field_0x14 + 1;
    return 1;
}

// HD: texture targets and post-processing replace the GC alpha-buffer display-list path.
/* 025BF250 */
void dSnap_packet::draw(u32 context) {

    u32 manager = (u32)l_textureManager;

    u32 large = snap_api::getTexture(manager, SnapLargeTexture),
        depth = snap_api::getTexture(manager, SnapDepthTexture),
        small = snap_api::getTexture(manager, SnapColorTexture);

    if (!large) {
        large = snap_api::createTexture(manager, SnapLargeTexture, gabi::ea(&l_largeCaptureWidth));

        depth = snap_api::createTexture(manager, SnapDepthTexture, gabi::ea(&l_captureWidth));
        small = snap_api::createTexture(manager, SnapColorTexture, gabi::ea(&l_captureWidth));
    }

    u32 frame = (u32)gabi::at<dSnapHD_Context>(context)->mpFrame;
    dSnapHD_Frame *frameView = gabi::at<dSnapHD_Frame>(frame);
    u32 source = frameView->mpBuffer;
    dSnapHD_Buffer *sourceView = gabi::at<dSnapHD_Buffer>(source);

    if (!(u32)l_textureNameInitialized) {
        l_textureNameInitialized = 1;
        l_textureNameVtable = 0x1005469C;
    }

    if (source) {
        u32 vt = (u32)sourceView->mVtable, target = (u32)gabi::at<dSnapHD_Vtable>(vt)->mIsTexture;

        if (!gabi::call_ptr<u32>(target, source, gabi::ea(&l_textureNameVtable))) {
            source = 0;
            sourceView = gabi::at<dSnapHD_Buffer>(source);
        }
    }

    snap_api::prepareColorTarget((u32)sourceView->mpColorTarget);
    snap_api::invalidateTexture((u32)sourceView->mpColorTarget);

    gabi::Local<dSnapHD_Texture> firstView, secondView, texture;

    gabi::Local<dSnapHD_RenderState> stateA, stateB;

    gabi::Local<u8[24]> viewport;

    gabi::Local<dSnapHD_Float2> scale, origin, extent, position;

    gabi::Local<u8[48]> matrix;

    gabi::Local<u8[172]> camera;

    snap_api::createTextureView(firstView.a, (u32)sourceView->mpColorTarget);

    snap_api::renderStateCtor(stateA.a);
    stateA->mCullEnabled = 0;
    stateA->mAlphaWriteEnabled = 0;
    stateA->mDepthWriteEnabled = 1;
    stateA->mAlphaTestEnabled = 0;
    stateA->mColorWriteEnabled = 1;
    stateA->mDepthTestEnabled = 1;
    stateA->mBlendEnabled = 0;
    snap_api::applyRenderState(stateA.a);

    auto dimensions = [&](u32 tex) {
        const dSnapHD_Texture *descriptor = gabi::at<dSnapHD_Texture>(tex);
        s32 w = (s32)descriptor->mWidth, wb = (u8)descriptor->mMinWidth,
            h = (s32)descriptor->mHeight, hb = (u8)descriptor->mMinHeight;
        return std::pair<f32, f32>(f32(u32(w > wb ? w : wb)), f32(u32(h > hb ? h : hb)));
    };

    auto sizeViewport = [&](f32 w, f32 h, f32 ox, f32 oy) {
        u32 p = (u32)mpRenderBuffer;
        dSnapHD_Buffer *renderBuffer = gabi::at<dSnapHD_Buffer>(p);

        renderBuffer->mWidth = w;
        renderBuffer->mHeight = h;
        p = (u32)mpRenderBuffer;
        renderBuffer = gabi::at<dSnapHD_Buffer>(p);

        renderBuffer->mOriginX = ox;
        renderBuffer->mOriginY = oy;
        renderBuffer->mViewportWidth = w;
        renderBuffer->mViewportHeight = h;
    };

    auto wh = dimensions(large);
    f32 w = wh.first, h = wh.second;

    dSnapHD_assignColorTexture((u32)mpColorTexture, large);

    u32 p = (u32)mpRenderBuffer;
    dSnapHD_Buffer *renderBuffer = gabi::at<dSnapHD_Buffer>(p);

    renderBuffer->mWidth = w;
    renderBuffer->mHeight = h;

    f32 ox = (f32)l_viewportOriginX, oy = (f32)l_viewportOriginY;

    p = (u32)mpRenderBuffer;

    renderBuffer = gabi::at<dSnapHD_Buffer>(p);

    renderBuffer->mOriginX = ox;
    renderBuffer->mOriginY = oy;
    renderBuffer->mViewportWidth = w;
    renderBuffer->mViewportHeight = h;

    p = (u32)mpRenderBuffer;

    renderBuffer = gabi::at<dSnapHD_Buffer>(p);

    snap_api::clearRenderTargets(p);
    renderBuffer->mpDepthTarget = 0;
    p = (u32)mpRenderBuffer;
    renderBuffer = gabi::at<dSnapHD_Buffer>(p);

    renderBuffer->mpColorTarget = (u32)mpColorTexture;
    snap_api::applyRenderBuffer((u32)mpRenderBuffer);

    snap_api::getColorViewport(viewport.a, (u32)mpRenderBuffer);
    snap_api::applyViewport(viewport.a, (u32)mpRenderBuffer);

    u32 initX = (u32)l_initialScaleX, initY = (u32)l_initialScaleY;
    snapFieldBits<u32>(scale->x) = initX;
    snapFieldBits<u32>(scale->y) = initY;

    u32 bitsX = (u32)snapFieldBits<u32>(sourceView->mViewportWidth);
    f32 viewportX = (f32)l_shutterWidth;
    f32 sourceX;
    memcpy(&sourceX, &bitsX, 4);
    u32 bitsY = (u32)snapFieldBits<u32>(sourceView->mViewportHeight);
    (void)bitsY;
    f32 fraction = sourceX / 1280.0f;
    f32 extentX = (viewportX + viewportX) * fraction, viewportY = (f32)l_shutterHeight;
    f32 extentY = (viewportY + viewportY) * fraction;

    scale->x = extentX / w;
    scale->y = extentY / h;
    extent->x = extentX;
    extent->y = extentY;

    u32 screen = snap_api::getScreenSize();

    dSnapHD_Float2 *screenView = gabi::at<dSnapHD_Float2>(screen);

    f32 screenX = (f32)screenView->x, screenY = (f32)screenView->y;

    f32 positionX = gabi::fnmsubs((f32)l_shutterWidth, 0.5f, (f32)l_shutterCenterX) / screenX;

    f32 positionY = gabi::fnmsubs((f32)l_shutterHeight, 0.5f, (f32)l_shutterCenterY) / screenY;

    position->x = positionX;
    position->y = positionY;

    u32 ref = snap_api::getDrawFlags(0x104B45C0);

    dSnapHD_DrawFlags *refView = gabi::at<dSnapHD_DrawFlags>(ref);

    u32 old = (u32)refView->mFlags;
    u32 renderer = (u32)l_textureRenderer;

    u32 handle =
        snap_api::drawColorTexture(renderer, firstView.a, viewport.a, scale.a, position.a, old);

    snap_api::prepareDepthTarget((u32)sourceView->mpDepthTarget);
    u32 tex = (u32)sourceView->mpDepthTarget;
    dSnapHD_Texture *depthTarget = gabi::at<dSnapHD_Texture>(tex);

    if ((u32)depthTarget->mDepthBuffer) {
        if ((u8)depthTarget->mDirty) {
            snap_api::updateDepthTarget(tex);
            depthTarget->mDirty = 0;
        }
        snap_api::invalidateDepthBuffer(tex + 0xA8);
        snap_api::flushRenderContext((u32)l_renderContext);
        tex = (u32)sourceView->mpDepthTarget;
        depthTarget = gabi::at<dSnapHD_Texture>(tex);
    }

    snap_api::invalidateTexture(tex);

    wh = dimensions(depth);
    f32 dw = wh.first, dh = wh.second;
    dSnapHD_assignDepthTexture((u32)mpDepthTexture, depth);

    p = (u32)mpRenderBuffer;

    renderBuffer = gabi::at<dSnapHD_Buffer>(p);

    snap_api::clearRenderTargets(p);
    renderBuffer->mpDepthTarget = 0;
    p = (u32)mpRenderBuffer;
    renderBuffer = gabi::at<dSnapHD_Buffer>(p);

    renderBuffer->mpDepthTarget = (u32)mpDepthTexture;
    sizeViewport(dw, dh, ox, oy);
    snap_api::applyRenderBuffer((u32)mpRenderBuffer);

    snap_api::createTextureView(secondView.a, (u32)sourceView->mpDepthTarget);
    secondView->mLayer = 0;
    secondView->mSlice = 0;
    secondView->mLevel = 0;
    secondView->mViewFlags = (u8)secondView->mViewFlags | 4;

    snap_api::getDepthViewport(viewport.a, (u32)mpRenderBuffer);
    snap_api::applyViewport(viewport.a, (u32)mpRenderBuffer);

    wh = dimensions(secondView.a);
    scale->x = (f32)extent->x / wh.first;
    scale->y = (f32)extent->y / wh.second;

    handle = snap_api::drawDepthTexture((u32)l_textureRenderer, secondView.a, viewport.a, scale.a,
                                        position.a, handle);

    p = (u32)mpRenderBuffer;

    renderBuffer = gabi::at<dSnapHD_Buffer>(p);

    snap_api::prepareDepthTarget((u32)renderBuffer->mpDepthTarget);
    p = (u32)mpRenderBuffer;
    renderBuffer = gabi::at<dSnapHD_Buffer>(p);

    snap_api::invalidateTexture((u32)renderBuffer->mpDepthTarget);
    snap_api::textureViewDtor(secondView.a, 2);

    snap_api::prepareColorTarget((u32)mpColorTexture);
    snap_api::invalidateTexture((u32)mpColorTexture);
    snap_api::textureViewCtor(texture.a);

    u32 save = (u32)l_saveData;

    dSnapHD_SaveView *saveView = gabi::at<dSnapHD_SaveView>(save);

    u32 effect = (u8)saveView->mStageType == 38 ? (u32)mpAlternateEffect : (u32)mpEffect;

    dSnapHD_PostProcess *effectView = gabi::at<dSnapHD_PostProcess>(effect);

    auto effectReady = [&]() {
        if (!effect || !(u8)effectView->mEnabled)
            return false;
        if ((s32)effectView->mEffectCount > 0 && (u32)effectView->mpEffect)
            return true;
        return (u8)effectView->mFallbackEnabled != 0;
    };

    u8 flags;

    if (effectReady()) {
        handle = snap_api::applyPostProcess(effect, (u32)mpColorTexture, handle);
        if ((u32)effectView->mpTexture && effect + 0x564) {
            u32 copy = effect + 0x564;
            dSnapHD_Texture *copyView = gabi::at<dSnapHD_Texture>(copy);

            snap_texture_copy(texture.a, copy);
            flags = (u8)copyView->mViewFlags;
        } else {
            bool same = texture->hasSameDescriptor(*gabi::at<dSnapHD_Texture>(large));
            if (!same)
                snap_api::assignTextureView(texture.a, large);
            else {
                u32 image = (u32)gabi::at<dSnapHD_Texture>(large)->mImageData,
                    mip = (u32)gabi::at<dSnapHD_Texture>(large)->mMipData;
                texture->mImageData = image;
                texture->mViewImageData = image;
                texture->mViewMipData = mip;
                texture->mMipData = mip;
            }
            flags = (u8)texture->mViewFlags;
        }
    }

    else {
        bool same = texture->hasSameDescriptor(*gabi::at<dSnapHD_Texture>(large));
        if (!same)
            snap_api::assignTextureView(texture.a, large);
        else {
            u32 mip = (u32)gabi::at<dSnapHD_Texture>(large)->mMipData,
                image = (u32)gabi::at<dSnapHD_Texture>(large)->mImageData;
            texture->mMipData = mip;
            texture->mViewMipData = mip;
            texture->mViewImageData = image;
            texture->mImageData = image;
        }
        flags = (u8)texture->mViewFlags;
    }

    texture->mLevel = 0;
    texture->mSlice = 0;
    texture->mLayer = 0;
    texture->mViewFlags = flags | 4;

    wh = dimensions(small);
    f32 sw = wh.first, sh = wh.second;
    dSnapHD_assignColorTexture((u32)mpColorTexture, small);
    sizeViewport(sw, sh, ox, oy);

    p = (u32)mpRenderBuffer;

    renderBuffer = gabi::at<dSnapHD_Buffer>(p);

    snap_api::clearRenderTargets(p);
    renderBuffer->mpDepthTarget = 0;
    p = (u32)mpRenderBuffer;
    renderBuffer = gabi::at<dSnapHD_Buffer>(p);

    renderBuffer->mpColorTarget = (u32)mpColorTexture;
    p = (u32)mpRenderBuffer;
    renderBuffer = gabi::at<dSnapHD_Buffer>(p);

    renderBuffer->mpDepthTarget = 0;
    snap_api::setRenderAlpha((u32)mpRenderBuffer, 255);

    snap_api::getColorViewport(secondView.a, (u32)mpRenderBuffer);
    snap_api::applyViewport(secondView.a, (u32)mpRenderBuffer);
    snap_api::clearRenderBuffer((u32)mpRenderBuffer, 0, 1, 0x104A01CC, 0, 1.0f);

    snap_api::renderStateCtor(stateB.a);
    stateB->mAlphaWriteEnabled = 0;
    stateB->mAlphaTestEnabled = 0;
    stateB->mColorWriteEnabled = 1;
    stateB->mDepthFunction = 7;
    stateB->mDepthTestEnabled = 1;
    stateB->mCullEnabled = 0;
    stateB->mDepthWriteEnabled = 1;
    stateB->mBlendEnabled = 0;
    snap_api::applyRenderState(stateB.a);

    dSnapHD_scaleTransMtx(matrix.a, 0x104A0958, 0x104A0928);
    snap_api::orthoCameraCtor(camera.a);
    u32 projection = snap_api::getProjectionMtx(camera.a);

    handle = snap_api::drawTextureQuad(texture.a, matrix.a, projection, 0x104A01EC, handle);

    p = (u32)mpRenderBuffer;

    renderBuffer = gabi::at<dSnapHD_Buffer>(p);

    snap_api::prepareColorTarget((u32)renderBuffer->mpColorTarget);
    p = (u32)mpRenderBuffer;
    renderBuffer = gabi::at<dSnapHD_Buffer>(p);

    snap_api::invalidateTexture((u32)renderBuffer->mpColorTarget);
    snap_api::applyRenderBuffer((u32)mpRenderBuffer);
    snap_api::orthoCameraDtor(camera.a, 2);

    if (effectReady())
        snap_api::resetPostProcess(effect);

    ref = snap_api::getDrawFlags(0x104B45C0);

    refView = gabi::at<dSnapHD_DrawFlags>(ref);

    refView->mFlags = handle;
    refView->mSecondaryFlags = 0;
    snap_api::textureViewDtor(texture.a, 2);
    snap_api::textureViewDtor(firstView.a, 2);
}

// HD: read the capture texture across its full dimensions, retaining bounds reloads after pixel calls.
/* 025BE664 */
void dSnap_packet::Judge() {

    mResult = 0;
    for (u32 i = 0; i < SnapObjectCount; ++i) {
        dSnap_RegistObjElm *elem = &m_tbl[i];
        elem->mCapturedPixels = 0;
        elem->SetAreaClear();
    }
    u32 tex = snap_api::getTexture((u32)l_textureManager, SnapColorTexture);

    gabi::Local<u8[0x158]> reader;
    snap_api::textureReaderCtor(reader.a);
    snap_api::allocateTextureReader(reader.a, tex, (u32)mpHeap);
    snap_api::setReadTexture(reader.a, tex);
    snap_api::beginTextureRead(reader.a);
    gabi::Local<dSnapHD_Pixel> sample;

    u32 y = 0;
    for (;;) {
        s32 height = (s32)gabi::at<dSnapHD_Texture>(tex)->mHeight;
        u32 small = (u8)gabi::at<dSnapHD_Texture>(tex)->mMinHeight;
        if (s32(small) > height)
            height = s32(small);
        if (y >= u32(height))
            break;
        u32 x = 0;
        for (;;) {
            s32 width = (s32)gabi::at<dSnapHD_Texture>(tex)->mWidth;
            small = (u8)gabi::at<dSnapHD_Texture>(tex)->mMinWidth;
            if (s32(small) > width)
                width = s32(small);
            if (x >= u32(width))
                break;
            snap_api::readPixel(reader.a, sample.a, x, y);
            u32 index = (u32)sample->a >> 2;
            if (index < SnapObjectCount) {
                dSnap_RegistObjElm *elem = &m_tbl[index];
                elem->mCapturedPixels = (u32)elem->mCapturedPixels + 1;
                elem->SetArea(s16(x), s16(y));
            }
            ++x;
        }
        ++y;
    }

    snap_api::endTextureRead(reader.a);
    for (s32 i = 0; i < (s32)field_0x14; ++i) {
        dSnap_RegistObjElm *elem = &m_tbl[u32(i)];
        if (!(u32)elem->mCapturedPixels)
            continue;
        f32 area = elem->CalcArea2D();
        if (!(std::fabs(area) < 3.814697265625e-06f))
            elem->mCapturedRatio = f32((s32)elem->mCapturedPixels) / area;
    }
    SetResult();
    snap_api::textureReaderDtor(reader.a, 2);
}

/* 025C019C */
s32 dSnap_packet::FindPhoto(s32 start, u32 type) {

    if (start < 0)
        return -1;
    s32 count = (s32)field_0x14;
    if (start >= count)
        return -1;
    dSnap_RegistObjElm *object = &m_tbl[u32(start)];
    for (s32 i = start; i < count; ++i, ++object)
        if (object->ChkPhoto(type))
            return i;
    return -1;
}

/* 025C020C */
u32 dSnap_packet::JudgePost() {

    s32 index = FindPhoto(0, DSNAP_TYPE_POST);
    if (index == -1)
        return 0;
    if (gabi::at<dSnap_Obj>(gabi::ea(this) + 0xA0 + u32(index) * 60)->ChkSuccess(2000, 0.55f)) {
        mResultDetail = 0;
        return 1;
    }
    return 0;
}

/* 025C0284 */
u32 dSnap_packet::JudgeBikutsuki() {

    s32 index = FindPhoto(0, DSNAP_TYPE_BIKUTSUKI);
    if (index == -1)
        return 0;
    if (gabi::at<dSnap_Obj>(gabi::ea(this) + 0xA0 + u32(index) * 60)->ChkSuccess(5000, 0.55f))
        return 2;
    mResultDetail = 0;
    return 0;
}

/* 025C030C */
u32 dSnap_packet::JudgeCoupleLook() {

    s32 a = FindPhoto(0, DSNAP_TYPE_COUPLE_LOOK);
    if (a == -1)
        return 0;
    s32 b = FindPhoto(u32(a) + 1, DSNAP_TYPE_COUPLE_LOOK);
    if (b == -1)
        return 0;
    u32 base = gabi::ea(this) + 0xA0;
    if (!gabi::at<dSnap_Obj>(base + u32(a) * 60)->ChkSuccess(2000, 0.55f))
        return 0;
    if (!gabi::at<dSnap_Obj>(base + u32(b) * 60)->ChkSuccess(2000, 0.55f))
        return 0;
    mResultDetail = 0;
    return 3;
}

/* 025C03D0 */
u32 dSnap_packet::JudgeGF() {

    s32 index = FindPhoto(0, DSNAP_TYPE_UNK04);
    if (index == -1)
        return 0;
    if (gabi::at<dSnap_Obj>(gabi::ea(this) + 0xA0 + u32(index) * 60)->ChkSuccess(5000, 0.55f)) {
        mResultDetail = 0;
        return 4;
    }
    return 0;
}

// HD: the second subject is NPC_UB4; keep its HD threshold and partner selection.
/* 025C0448 */
u32 dSnap_packet::JudgeGenzo() {

    s32 a = FindPhoto(0, DSNAP_TYPE_UNK05);
    if (a == -1)
        return 0;
    s32 b = FindPhoto(0, DSNAP_TYPE_NPC_UB4);
    if (b == -1)
        return 0;
    u32 base = gabi::ea(this) + 0xA0;
    if (!gabi::at<dSnap_Obj>(base + u32(a) * 60)->ChkSuccess(1500, 0.55f))
        return 0;
    if (!gabi::at<dSnap_Obj>(base + u32(b) * 60)->ChkSuccess(1500, 0.55f))
        return 0;
    mResultDetail = 0;
    return 5;
}

// HD: the minimum captured-pixel threshold is 3000 rather than GC 4000.
/* 025C0500 */
u32 dSnap_packet::JudgeObasan4() {

    s32 a = FindPhoto(0, DSNAP_TYPE_UNK06);
    if (a == -1)
        return 0;
    if (FindPhoto(0, DSNAP_TYPE_UNK05) != -1)
        return 0;
    if (!gabi::at<dSnap_Obj>(gabi::ea(this) + 0xA0 + u32(a) * 60)->ChkSuccess(3000, 0.55f))
        return 0;
    mResultDetail = 0;
    return 6;
}

/* 025C0788 */
u32 dSnap_packet::JudgeTestM() { return 0; }

// HD: the minimum captured-pixel threshold is 3000 rather than GC 4000.
/* 025C0594 */
u32 dSnap_packet::JudgeGene() {

    s32 a = FindPhoto(0, (u32)mPhotoNo);
    if (a == -1)
        return 0;
    if (!gabi::at<dSnap_Obj>(gabi::ea(this) + 0xA0 + u32(a) * 60)->ChkSuccess(3000, 0.55f))
        return 0;
    mResultDetail = 0;
    return (u32)mPhotoNo;
}

/* 025BE31C */
u32 dSnap_packet::JudgeFigure(u32 index) {

    s32 photo = (s32)mPhotoNo;
    if (photo >= 208 || u32(photo) - 73 >= 134)
        return 0;
    const dSnap_CharaData *table = &l_CharaData[dSnap_PhotoIndex2TableIndex(photo)];
    u32 pixels = (u16)table->minPixels;
    s32 threshold = gabi::ftoi(f32(pixels) * (f32)l_pixelScale);
    f32 ratio = f32((u16)table->minRatio) / 1000.0f;
    dSnap_RegistObjElm *elem = &m_tbl[index];
    if ((s32)elem->mCapturedPixels < threshold)
        return 0;
    f32 actual = (f32)elem->mCapturedRatio;
    u8 flags = (u8)mResultDetail;
    if (actual < ratio)
        flags |= 2;
    else
        flags &= 0xFD;
    mResultDetail = flags;
    u32 culled = elem->ChkCamCull();
    flags = (u8)mResultDetail;
    u32 result = (u32)mPhotoNo;
    if (culled)
        flags |= 1;
    else
        flags &= 0xFE;
    mResultDetail = flags;
    return result;
}

// HD: judge special subjects first, then figure subjects in registration order.
/* 025BE480 */
void dSnap_packet::SetResult() {

    mResultDetail = 0;
    mResult = 0;
    gabi::Local<be<u8>[SnapJudgeCount]> seen;
    for (u32 k = 0; k < SnapJudgeCount; ++k)
        (*seen)[k] = 0;

    for (s32 i = 0; i < (s32)field_0x14; ++i) {
        dSnap_RegistObjElm *elem = &m_tbl[u32(i)];
        if (!(u32)elem->mCapturedPixels)
            continue;
        u32 type = (u8)elem->mPhotoNo;
        if (!type)
            continue;
        if (type >= DSNAP_TYPE_LAST_INDEX)
            JUTAssertion::showAssert("d_snap.cpp", 0xBD4,
                                     "0 <= photo_no && photo_no < DSNAP_TYPE_LAST_INDEX");
        if (type >= SnapJudgeCount)
            continue;
        if ((u8)(*seen)[type])
            continue;
        const dSnap_JudgeEntry &judge = m_judge_tbl[type - 1];
        if (judge.fn.i) {
            mPhotoNo = type;
            u32 result = (this->*judge.fn)();
            mResult = result;
            if (result)
                return;
        }
        (*seen)[type] = 1;
    }

    for (s32 i = 0; i < (s32)field_0x14; ++i) {
        dSnap_RegistObjElm *elem = &m_tbl[u32(i)];
        s32 pixels = (s32)elem->mCapturedPixels;
        if (!pixels)
            continue;
        u32 type = (u8)elem->mPhotoNo;
        if (!type || type < SnapJudgeCount)
            continue;
        if (type >= DSNAP_TYPE_LAST_INDEX)
            JUTAssertion::showAssert("d_snap.cpp", 0xC03,
                                     "0 <= photo_no && photo_no < DSNAP_TYPE_LAST_INDEX");
        pixels = (s32)elem->mCapturedPixels;
        if (pixels < 0)
            continue;
        mPhotoNo = type;
        u32 result = JudgeFigure(i);
        mResult = result;
        if (result && !(u8)mResultDetail)
            return;
    }
}

/* 025BDBB0 */
void dSnap_Create(u32 heap) {

    u32 packet = (u32)l_snapAddress;

    dSnap_packet *packetView = gabi::at<dSnap_packet>(packet);

    if (!packet) {
        if (!(u32)l_snapInitialized) {
            l_snapInitialized = 1;
            dSnap_packet_ctor(gabi::ea(&l_snap));
            snap_api::registerGlobalObject(0x101EBA28);
        }
        packet = gabi::ea(&l_snap);
        packetView = gabi::at<dSnap_packet>(packet);

        l_snapAddress = packet;
    }
    packetView->Create(heap);
}

/* 025BDC48 */
void dSnap_ReleaseShutter() { gabi::at<dSnap_packet>((u32)l_snapAddress)->ReleaseShutter(); }

/* 025BDC60 */
u32 dSnap_ChkCapture() { return gabi::at<dSnap_packet>((u32)l_snapAddress)->ChkCapture(); }

/* 025BE8DC */
void dSnap_Execute() { gabi::at<dSnap_packet>((u32)l_snapAddress)->Execute(); }

/* 025BE8E8 */
u32 dSnap_GetResult() {

    u32 s = (u32)l_snapAddress;
    return (u32)gabi::at<dSnap_packet>(s)->mResult;
}

/* 025BE8F8 */
u32 dSnap_GetResultDetail() {

    u32 s = (u32)l_snapAddress;
    return (u8)gabi::at<dSnap_packet>(s)->mResultDetail;
}

/* 025BEB4C */
u32 dSnap_RegistSnapObj(u32 obj) { return gabi::at<dSnap_packet>((u32)l_snapAddress)->Regist(obj); }

/* 025BED80 */
void dSnap_RegistFigActor(u32 type, u32 actor, f32 x, f32 y, f32 z) {

    s16 angle = (s16)gabi::at<fopAc_ac_c>(actor)->shape_angle.y;
    dSnap_RegistFigGuest(type, actor, actor + 0x314, angle, x, y, z);
}

// HD: guest-address overload avoids the shared actor-pointer overload during harness registration.
/* 025BEBB8 */
void dSnap_RegistFigGuest(u32 type, u32 actor, u32 pos, s16 angle, f32 rScale, f32 hScale,
                          f32 pScale) {

    if (!gabi::at<dSnap_packet>((u32)l_snapAddress)->ChkCapture() || type >= DSNAP_TYPE_UNKCF)
        return;
    u32 index = dSnap_PhotoIndex2TableIndex(type);
    const dSnap_CharaData *table = &l_CharaData[index];
    snap_api::YrotS(gabi::ea(mDoMtx_stack_c::get()), angle);
    gabi::Local<cXyz> offset, world;
    f32 x = f32((s16)table->offsetX) * pScale, y = f32((s16)table->offsetY) * pScale,
        z = f32((s16)table->offsetZ) * pScale;
    offset->x = x;
    offset->y = y;
    offset->z = z;
    snap_api::MTXMultVec(gabi::ea(mDoMtx_stack_c::get()), offset.a, world.a);
    snap_api::VECAdd(world.a, pos, world.a);
    gabi::Local<dSnap_Obj> obj;
    dSnap_Obj_ctor(obj.a);
    f32 radius = f32((s16)table->radius) * hScale, height = f32((s16)table->height) * rScale;
    obj->SetGeo(world.a, radius, height, angle);
    obj->SetInf(type, actor, 0, 4, (s16)table->cullAngle);
    dSnap_RegistSnapObj(obj.a);
}

/* 025BED8C */
void dSnap_DebugDraw() {

    if (gabi::at<dSnap_packet>((u32)l_snapAddress)->ChkCapture()) {
        u32 p = snap_api::getPlay();
        dSnapHD_GameDrawLists *pView = gabi::at<dSnapHD_GameDrawLists>(p);

        u32 buffer = (u32)pView->mpSnapList;
        l_xluDrawList = buffer;
        snap_api::entryImm(buffer, (u32)l_snapAddress, 0);
        p = snap_api::getPlay();
        pView = gabi::at<dSnapHD_GameDrawLists>(p);

        l_opaDrawList = (u32)pView->mpOpaList;
        p = snap_api::getPlay();
        pView = gabi::at<dSnapHD_GameDrawLists>(p);

        l_xluDrawList = (u32)pView->mpXluList;
    }
}

/* 025BEF30 */
void dSnap_Delete() { gabi::at<dSnap_packet>((u32)l_snapAddress)->Remove(); }

// HD: project through the packet camera and viewport rather than the GC screen matrix.
/* 025BD670 */
void dSnap_project(u32 in, u32 dst) {
    cXyz *source = gabi::at<cXyz>(in);
    cXyz *destination = gabi::at<cXyz>(dst);

    u32 packet = (u32)l_snapAddress;

    dSnap_packet *packetView = gabi::at<dSnap_packet>(packet);

    u32 projection = (u32)packetView->mpProjection, viewport = (u32)packetView->mpViewport,
        camera = (u32)packetView->mpCamera;
    gabi::Local<u8[24]> params;
    snap_api::getColorViewport(params.a, viewport);
    gabi::Local<cXyz> vec;
    f32 z = (f32)source->z, x = (f32)source->x, y = (f32)source->y;
    vec->z = z;
    vec->y = y;
    vec->x = x;
    gabi::Local<dSnapHD_Float2> xy;
    snap_api::project(camera, xy.a, vec.a, projection, params.a);
    f32 xx = (f32)xy->x, yy = (f32)xy->y;
    destination->x = xx;
    destination->y = yy;
    destination->z = 0.0f;
}

/* 025BEF7C */
void dSnap_MtxCopy(u32 dst, u32 src) {
    Mtx34 *destination = gabi::at<Mtx34>(dst);
    Mtx34 *source = gabi::at<Mtx34>(src);

    f32 a[12];
    for (u32 i = 0; i < 12; ++i)
        a[i] = source->m[i / 4][i % 4];
    for (u32 i = 0; i < 12; ++i)
        destination->m[i / 4][i % 4] = a[i];
}

/* 025BEE04 */
void dSnap_packet::Remove() {

    if (mpColorTexture) {
        snap_api::free(mpColorTexture);
        mpColorTexture = 0;
    }
    if (mpDepthTexture) {
        snap_api::free(mpDepthTexture);
        mpDepthTexture = 0;
    }
    if (mpRenderBuffer) {
        dSnapHD_destroy(gabi::at<dSnapHD_Buffer>(mpRenderBuffer));
        mpRenderBuffer = 0;
    }
    if (mpCamera) {
        dSnapHD_destroy(gabi::at<dSnapHD_Camera>(mpCamera));
        mpCamera = 0;
    }
    if (mpProjection) {
        dSnapHD_destroy(gabi::at<dSnapHD_ProjectionView>(mpProjection));
        mpProjection = 0;
    }
    if (mpViewport) {
        dSnapHD_destroy(gabi::at<dSnapHD_Buffer>(mpViewport));
        mpViewport = 0;
    }
    mpEffect = 0;
    mpAlternateEffect = 0;
    snap_api::removeTexture((u32)l_textureManager, SnapColorTexture);
    snap_api::removeTexture((u32)l_textureManager, SnapDepthTexture);
    snap_api::removeTexture((u32)l_textureManager, SnapLargeTexture);
}

// HD: debug geometry uses the current render context camera and projection.
/* 025BFDC0 */
void dSnap_packet::DebugDraw(u32 context) {

    if (!ChkCapture())
        return;

    draw(context);

    u32 p = (u32)mpRenderBuffer;

    dSnapHD_Buffer *pView = gabi::at<dSnapHD_Buffer>(p);

    snap_api::clearRenderTargets(p);
    pView->mpDepthTarget = 0;

    p = (u32)mpRenderBuffer;

    pView = gabi::at<dSnapHD_Buffer>(p);

    pView->mpColorTarget = (u32)mpColorTexture;
    p = (u32)mpRenderBuffer;
    pView = gabi::at<dSnapHD_Buffer>(p);

    pView->mpDepthTarget = (u32)mpDepthTexture;

    snap_api::applyRenderBuffer((u32)mpRenderBuffer);

    gabi::Local<u8[24]> params;
    snap_api::getColorViewport(params.a, (u32)mpRenderBuffer);
    snap_api::applyViewport(params.a, (u32)mpRenderBuffer);

    gabi::Local<dSnapHD_RenderState> state;
    snap_api::renderStateCtor(state.a);
    state->mDepthTestEnabled = 0;
    state->mBlendMode = 0;
    state->mAlphaWriteEnabled = 1;
    state->mColorMask = 1;
    state->mDepthWriteEnabled = 0;
    state->mColorWriteEnabled = 0;
    snap_api::applyRenderState(state.a);

    u32 drawer = (u32)l_debugDrawer;

    if (drawer) {

        u32 frame = (u32)gabi::at<dSnapHD_Context>(context)->mpFrame;

        snap_api::setDebugCamera(drawer, (u32)gabi::at<dSnapHD_Frame>(frame)->mpCamera);
        frame = (u32)gabi::at<dSnapHD_Context>(context)->mpFrame;

        u32 camera = (u32)gabi::at<dSnapHD_Frame>(frame)->mpProjection,
            projection = snap_api::getProjectionMtx(camera);

        gabi::Local<dSnapHD_Mtx44> matrix;
        for (u32 i = 0; i < 64; i += 4)
            snapFieldBits<u32>(matrix->m[i / 16][(i % 16) / 4]) =
                snapFieldBits<u32>(gabi::at<dSnapHD_Mtx44>(projection)->m[i / 16][(i % 16) / 4]);

        frame = (u32)gabi::at<dSnapHD_Context>(context)->mpFrame;

        u32 buffer = (u32)gabi::at<dSnapHD_Frame>(frame)->mpBuffer;

        f32 screenX = (f32)l_shutterWidth, screenY = (f32)l_shutterHeight;

        f32 width = (f32)gabi::at<dSnapHD_Buffer>(buffer)->mViewportWidth,
            scaledX = screenX * (width / 1280.0f),
            height = (f32)gabi::at<dSnapHD_Buffer>(buffer)->mViewportHeight,
            scaledY = screenY * (height / 720.0f);

        f32 scaleX = ((f32)gabi::at<dSnapHD_Buffer>(buffer)->mViewportWidth / scaledX) * 0.5f;
        matrix->m[0][0] = (f32)matrix->m[0][0] * scaleX;

        f32 scaleY = ((f32)gabi::at<dSnapHD_Buffer>(buffer)->mViewportHeight / scaledY) * 0.5f;
        matrix->m[0][2] = 0.0f;
        matrix->m[1][1] = (f32)matrix->m[1][1] * scaleY;

        height = (f32)gabi::at<dSnapHD_Buffer>(buffer)->mViewportHeight;
        f32 ratio = height / 720.0f;
        screenY = (f32)l_shutterHeight;
        f32 center = gabi::fmadds(screenY, 0.5f, 140.0f) * ratio;
        f32 delta = gabi::fmsubs(height, 0.5f, center);
        matrix->m[1][2] = (delta + delta) / (screenY * ratio);

        gabi::Local<u8[212]> adjusted;
        snap_api::matrixProjectionCtor(adjusted.a, matrix.a, 0);
        snap_api::updateProjection(adjusted.a);
        snap_api::setDebugProjection(drawer, adjusted.a);
        snap_api::beginDebugDraw(drawer);

        projection = snap_api::getProjectionMtx(camera);

        snap_api::copyProjectionMtx(projection, matrix.a);
        matrix->m[0][0] = (f32)matrix->m[0][0] * 0.5f;
        matrix->m[1][1] = (f32)matrix->m[1][1] * 0.5f;

        snap_api::setProjectionMtx((u32)mpProjection, matrix.a, 0);
        snap_api::matrixProjectionDtor(adjusted.a, 2);
    }

    snap_api::setRenderMode(0);
    u32 flags = snap_api::getDrawFlags(0x104B45C0);
    dSnapHD_DrawFlags *flagsView = gabi::at<dSnapHD_DrawFlags>(flags);

    flagsView->mSecondaryFlags = 0;
    flagsView->mFlags = 3;

    for (s32 i = 0; i < (s32)field_0x14; ++i)
        m_tbl[u32(i)].draw(u32(i) * 4);

    if (drawer)
        snap_api::endDebugDraw(drawer);

    snap_api::removeTexture((u32)l_textureManager, SnapLargeTexture);
    snap_api::removeTexture((u32)l_textureManager, SnapDepthTexture);

    u32 frame = (u32)gabi::at<dSnapHD_Context>(context)->mpFrame,
        view = (u32)gabi::at<dSnapHD_Frame>(frame)->mpCamera, dest = (u32)mpCamera;

    gabi::at<dSnapHD_Camera>(dest)->mEyeX =
        dSnapHD_copyFloatBits(gabi::at<dSnapHD_Camera>(view)->mEyeX);
    gabi::at<dSnapHD_Camera>(dest)->mEyeY =
        dSnapHD_copyFloatBits(gabi::at<dSnapHD_Camera>(view)->mEyeY);
    gabi::at<dSnapHD_Camera>(dest)->mEyeZ =
        dSnapHD_copyFloatBits(gabi::at<dSnapHD_Camera>(view)->mEyeZ);
    f32 fourth = (f32)gabi::at<dSnapHD_Camera>(view)->mAtX;
    dest = (u32)mpCamera;
    gabi::at<dSnapHD_Camera>(dest)->mAtX = fourth;
    gabi::at<dSnapHD_Camera>(dest)->mAtY =
        dSnapHD_copyFloatBits(gabi::at<dSnapHD_Camera>(view)->mAtY);
    gabi::at<dSnapHD_Camera>(dest)->mAtZ =
        dSnapHD_copyFloatBits(gabi::at<dSnapHD_Camera>(view)->mAtZ);
    dest = (u32)mpCamera;
    gabi::at<dSnapHD_Camera>(dest)->mUpX =
        dSnapHD_copyFloatBits(gabi::at<dSnapHD_Camera>(view)->mUpX);
    gabi::at<dSnapHD_Camera>(dest)->mUpY =
        dSnapHD_copyFloatBits(gabi::at<dSnapHD_Camera>(view)->mUpY);
    gabi::at<dSnapHD_Camera>(dest)->mUpZ =
        dSnapHD_copyFloatBits(gabi::at<dSnapHD_Camera>(view)->mUpZ);
    snap_api::normalizeVec(dest + 0x4C);

    dest = (u32)mpCamera;
    u32 table = (u32)gabi::at<dSnapHD_Camera>(dest)->mVtable;
    dSnapHD_Vtable *tableView = gabi::at<dSnapHD_Vtable>(table);

    gabi::call_ptr<void>((u32)tableView->mUpdateCamera, dest, dest);

    frame = (u32)gabi::at<dSnapHD_Context>(context)->mpFrame;

    u32 buffer = (u32)gabi::at<dSnapHD_Frame>(frame)->mpBuffer;

    dest = (u32)mpViewport;
    gabi::at<dSnapHD_Buffer>(dest)->mWidth =
        dSnapHD_copyFloatBits(gabi::at<dSnapHD_Buffer>(buffer)->mWidth);
    gabi::at<dSnapHD_Buffer>(dest)->mHeight =
        dSnapHD_copyFloatBits(gabi::at<dSnapHD_Buffer>(buffer)->mHeight);

    frame = (u32)gabi::at<dSnapHD_Context>(context)->mpFrame;

    buffer = (u32)gabi::at<dSnapHD_Frame>(frame)->mpBuffer;

    dest = (u32)mpViewport;
    gabi::at<dSnapHD_Buffer>(dest)->mOriginX =
        dSnapHD_copyFloatBits(gabi::at<dSnapHD_Buffer>(buffer)->mOriginX);
    gabi::at<dSnapHD_Buffer>(dest)->mOriginY =
        dSnapHD_copyFloatBits(gabi::at<dSnapHD_Buffer>(buffer)->mOriginY);
    gabi::at<dSnapHD_Buffer>(dest)->mViewportWidth =
        dSnapHD_copyFloatBits(gabi::at<dSnapHD_Buffer>(buffer)->mViewportWidth);
    gabi::at<dSnapHD_Buffer>(dest)->mViewportHeight =
        dSnapHD_copyFloatBits(gabi::at<dSnapHD_Buffer>(buffer)->mViewportHeight);
    mFlag = ((u32)mFlag & ~1u) | 2u;
    frame = (u32)gabi::at<dSnapHD_Context>(context)->mpFrame;

    snap_api::applyRenderBuffer((u32)gabi::at<dSnapHD_Frame>(frame)->mpBuffer);
}

// HD: compiler-generated and template code of this translation unit.

/* 025C0734 */
void dSnap_packet_dt(u32 s, u32 flags) {

    if (s) {
        snap_api::packetDtor(s, 0);
        if (flags & 1)
            snap_api::free(s);
    }
}

/* 025C0790 */
void dSnap_Obj_dt(u32 s, u32 flags) {

    if (s && (flags & 1))
        snap_api::free(s);
}

/* 025C07A4 */
void dSnapHD_createColorTexture(u32 s, u32 src, u32 mode, u32 alloc) {
    dSnapHD_Texture *target = gabi::at<dSnapHD_Texture>(s);
    dSnapHD_Texture *source = gabi::at<dSnapHD_Texture>(src);

    for (u32 i = 0; i < 30; ++i)
        target->mWords[i] = source->mWords[i];
    for (u32 i = 0x78; i < 0x7C; ++i)
        target->mBytes[i] = source->mBytes[i];
    for (u32 i = 0x7C; i < 0x88; i += 4)
        target->mWords[i / 4] = source->mWords[i / 4];
    u32 word = (u32)source->mWords[0x88 / 4];
    target->mAllocator = alloc;
    target->mWords[0x88 / 4] = word;
    target->mMode = mode;
    snap_api::createColorTarget(s);
    target->mDirty = 1;
}

/* 025C0934 */
void dSnapHD_createDepthTexture(u32 s, u32 src, u32 mode, u32 alloc) {
    dSnapHD_Texture *target = gabi::at<dSnapHD_Texture>(s);
    dSnapHD_Texture *source = gabi::at<dSnapHD_Texture>(src);

    for (u32 i = 0; i < 30; ++i)
        target->mWords[i] = source->mWords[i];
    for (u32 i = 0x78; i < 0x7C; ++i)
        target->mBytes[i] = source->mBytes[i];
    for (u32 i = 0x7C; i < 0x88; i += 4)
        target->mWords[i / 4] = source->mWords[i / 4];
    u32 word = (u32)source->mWords[0x88 / 4];
    target->mAllocator = alloc;
    target->mWords[0x88 / 4] = word;
    target->mMode = mode;
    snap_api::createDepthTarget(s);
    target->mDirty = 1;
}

/* 025C0848 */
void dSnapHD_updateColorTexture(u32 s, u32 src, u32 mode, u32 alloc) {
    dSnapHD_Texture *target = gabi::at<dSnapHD_Texture>(s);
    dSnapHD_Texture *source = gabi::at<dSnapHD_Texture>(src);

    if (!target->hasSameDescriptor(*source)) {
        dSnapHD_createColorTexture(s, src, mode, alloc);
        return;
    }
    u32 image = (u32)source->mImageData, mip = (u32)source->mMipData;
    target->mColorImageData = image;
    target->mColorMipData = mip;
    u32 oldAlloc = (u32)target->mAllocator;
    target->mMipData = mip;
    u32 oldMode = (u32)target->mMode;
    target->mImageData = image;
    if (oldAlloc != alloc) {
        target->mAllocator = alloc;
        target->mDirty = 1;
    }
    if (oldMode != mode) {
        target->mMode = mode;
        target->mDirty = 1;
    }
}

/* 025C09D8 */
void dSnapHD_updateDepthTexture(u32 s, u32 src, u32 mode, u32 alloc) {
    dSnapHD_Texture *target = gabi::at<dSnapHD_Texture>(s);
    dSnapHD_Texture *source = gabi::at<dSnapHD_Texture>(src);

    if (!target->hasSameDescriptor(*source)) {
        dSnapHD_createDepthTexture(s, src, mode, alloc);
        return;
    }
    u32 image = (u32)source->mImageData, mip = (u32)source->mMipData;
    target->mViewMipData = image;
    target->mDepthMipData = mip;
    u32 oldAlloc = (u32)target->mAllocator;
    target->mMipData = mip;
    u32 oldMode = (u32)target->mMode;
    target->mImageData = image;
    if (oldAlloc != alloc) {
        target->mAllocator = alloc;
        target->mDirty = 1;
    }
    if (oldMode != mode) {
        target->mMode = mode;
        target->mDirty = 1;
    }
}

/* 025C0928 */
void dSnapHD_assignColorTexture(u32 s, u32 src) {
    dSnapHD_Texture *target = gabi::at<dSnapHD_Texture>(s);

    u32 alloc = (u32)target->mAllocator, mode = (u32)target->mMode;
    dSnapHD_updateColorTexture(s, src, mode, alloc);
}

/* 025C0AB8 */
void dSnapHD_assignDepthTexture(u32 s, u32 src) {
    dSnapHD_Texture *target = gabi::at<dSnapHD_Texture>(s);

    u32 alloc = (u32)target->mAllocator, mode = (u32)target->mMode;
    dSnapHD_updateDepthTexture(s, src, mode, alloc);
}

/* 025C0AC4 */
void dSnapHD_scaleTransMtx(u32 dst, u32 scale, u32 pos) {
    Mtx34 *matrix = gabi::at<Mtx34>(dst);
    cXyz *scaleVec = gabi::at<cXyz>(scale);
    cXyz *position = gabi::at<cXyz>(pos);

    f32 x = dSnapHD_copyFloatBits(scaleVec->x);
    matrix->m[0][0] = x;
    matrix->m[1][0] = 0;
    matrix->m[2][0] = 0;
    matrix->m[0][1] = 0;
    f32 y = dSnapHD_copyFloatBits(scaleVec->y);
    matrix->m[2][1] = 0;
    matrix->m[1][1] = y;
    matrix->m[0][2] = 0;
    matrix->m[1][2] = 0;
    matrix->m[2][2] = dSnapHD_copyFloatBits(scaleVec->z);
    matrix->m[0][3] = dSnapHD_copyFloatBits(position->x);
    matrix->m[1][3] = dSnapHD_copyFloatBits(position->y);
    matrix->m[2][3] = dSnapHD_copyFloatBits(position->z);
}

/* 025BD814 */
u32 dSnap_packet_ctor(u32 s) {

    if (!s)
        s = snap_api::alloc(0xF94);
    if (s) {
        dSnap_packet *object = gabi::at<dSnap_packet>(s);
        snap_api::packetCtor(s);
        object->mPhotoNo = 0;
        object->__vtbl = 0x100552B0;
        object->field_0x14 = 0;
        snap_api::constructArray(s + 0xA0, 63, 60, 0x025BD7B4);
        object->mpDepthTexture = 0;
        object->mpViewport = 0;
        object->mpProjection = 0;
        object->mpRenderBuffer = 0;
        object->mpCamera = 0;
        object->mResultDetail = 0;
        object->mResult = 0;
        object->mpHeap = 0;
        object->mFlag = 0;
        object->mpColorTexture = 0;
        if (s + 0xF8C == 0)
            snap_api::alloc(8);
        object->mpEffect = 0;
        object->mpAlternateEffect = 0;
    }
    return s;
}

/* 025C060C */
void __sinit_d_snap_cpp() {

    l_stateWord3 = 0;
    l_stateWord2 = 0;
    l_stateWord1 = 0;
    l_stateWord0 = 0;
    snap_api::registerGlobalObject(0x101EBA34);
    l_minAngle = -3.1415927410125732f;
    l_maxAngle = 3.1415927410125732f;
    snap_api::initializeAngleState(0x1047C9CC);
    snap_api::registerGlobalObject(0x101EBA40);
    snap_api::initializeByteState(0x1047C9CD);
    snap_api::registerGlobalObject(0x101EBA4C);
    l_largeCaptureWidth = 1600;
    l_shutterWidth = 560;
    l_shutterHeight = 315;
    l_captureWidth = 800;
    l_shutterCenterX = 360;
    l_shutterCenterY = 203;
    l_captureHeight = 450;
    l_largeCaptureHeight = 900;
    l_pixelScale = (800.0f / 294.0f) * (450.0f / 198.0f);
}

/* WWHD glue
str 10054618 in 025BD600
str 10054624 in 025BD600
str 10055250 in 025BE480
str 1005521C in 025BE480
data l_snapAddress 1047C9BC
data l_snapInitialized 1047D974
data l_pixelScale 1047C9C8
data l_captureWidth 1047C99C
data l_captureHeight 1047C9A0
data l_largeCaptureWidth 1047C9A4
data l_largeCaptureHeight 1047C9A8
data l_shutterWidth 1047C9AC
data l_shutterHeight 1047C9B0
data l_shutterCenterX 1047C9B4
data l_shutterCenterY 1047C9B8
data l_minAngle 1047C9C0
data l_maxAngle 1047C9C4
data l_stateWord0 1047C9D0
data l_stateWord1 1047C9D4
data l_stateWord2 1047C9D8
data l_stateWord3 1047C9DC
data l_opaDrawList 104B4634
data l_xluDrawList 104B4638
data l_textureManager 101F9968
data l_debugDrawer 101F8B28
data l_postProcessService 101F86E8
data l_graphicsResource 101F8664
data l_textureNameInitialized 101FD7BC
data l_textureNameVtable 101FDCF8
data l_textureRenderer 101F8710
data l_renderContext 101F8BD8
data l_saveData 101F84DC
data l_viewportOriginX 104A0908
data l_viewportOriginY 104A090C
data l_initialScaleX 104A0920
data l_initialScaleY 104A0924
data l_CharaData 10054708
data cylvtx 100546CC
data vdata 10055074
data tindices 10055104
data l_snap 1047C9E0
data l_identityWords 104A041C
data m_judge_tbl 101EBA60
*/

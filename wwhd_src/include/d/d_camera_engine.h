#pragma once
#include "d/d_camera.h"
#include "gc_api/d/d_com_inf_game.h"
namespace camera_engine {
using GuestWord = be<u32>;
using CanonPointer = gptr<fopAc_ac_c>;
struct VectorWords {
    GuestWord x, y, z;
};
using GuestFloat = be<f32>;
struct FixedFrame {
    /* 0x00 */ cXyz m378;      /* arrow position */
    /* 0x0C */ cSGlobe_l m384; /* direction to frame */
    /* 0x14 */ be<s32> m38C;   /* frames */
    /* 0x18 */ be<f32> m390;   /* remaining weight sum */
    /* 0x1C */ be<f32> m394;
    /* 0x20 */ be<f32> m398; /* fovy */
    /* 0x24 */ cXyz m39C;    /* centre */
    /* 0x30 */ cXyz m3A8;
};
struct FixedPos {
    /* 0x00 */ be<s32> m378; /* frames */
    /* 0x04 */ be<f32> m37C; /* remaining weight sum */
    /* 0x08 */ be<f32> m380;
    /* 0x0C */ cXyz m384;   /* smoothed target */
    /* 0x18 */ cXyz m390;   /* last arrow position */
    /* 0x24 */ be<u8> m39C; /* fixed frame count from the map */
};
struct Crawl {
    /* 0x00 */ be<u32> m378; /* 'CRWL' */
    /* 0x04 */ be<s32> m37C; /* frames */
    /* 0x08 */ be<f32> m380; /* remaining weight sum */
    /* 0x0C */ be<f32> m384;
    /* 0x10 */ be<s32> m388; /* look from behind */
    /* 0x14 */ cXyz m38C;    /* map arrow eye */
    /* 0x20 */ be<u8> m398;  /* low tunnel */
    /* 0x21 */ be<u8> m399;  /* arrow eye active */
    /* 0x22 */ u8 _22[2];
    /* 0x24 */ be<s32> m39C;
};

struct HookWork {
    GuestWord m378;
    u8 _04[4];
    cXyz m380, m38C;
    be<u8> m398;
};
WWHD_OFFSET(HookWork, m380, 8);
WWHD_OFFSET(HookWork, m38C, 0x14);
WWHD_OFFSET(HookWork, m398, 0x20);
struct ArrowEntry {
    cXyz position;
    csXyz angle;
    u8 _12[2];
};
struct CameraEntry {
    u8 _00[0x12];
    be<u8> field_0x12, field_0x13;
};
struct Style {
    u8 _00[0x80];
    be<u16> mFlags;
    u8 _82[2];
};
WWHD_SIZE(Style, 0x84);
struct LineFrame {
    cM3dGLin mLine;
    u8 _1C[4];
};
WWHD_SIZE(LineFrame, 0x20);
struct TipView {
    u8 _00[0xD510];
    csXyz angle;
};
struct PlayerVtable {
    u8 _00[0xAC];
    gfn getHookshotTipID;
};
inline ArrowEntry* arrow(dCamera_c* c) {
    return reinterpret_cast<ArrowEntry*>(c->mCurRoomArrowEntry);
}
inline CameraEntry* entry(dCamera_c* c) {
    return reinterpret_cast<CameraEntry*>(c->mCurRoomCamEntry);
}
inline Style* styleEntry(s32 index) {
    return gabi::at<Style>(CAM_STYLES + index * 0x84);
}
inline cSAngle_l& angle270() {
    return *gabi::at<cSAngle_l>(0x101FF35C);
}
inline cSAngle_l& angle90() {
    return *gabi::at<cSAngle_l>(0x101FF358);
}
inline cXyz& zeroVector() {
    return *gabi::at<cXyz>(0x101FFBA8);
}
inline u32 hookshotTipID(fopAc_ac_c* p) {
    return gabi::call_ptr<u32>(gabi::at<PlayerVtable>(p->__vtbl)->getHookshotTipID.get(), p);
}
enum CheckVtable : u32 {
    CAM_LINE = 0x1004A66C,
    CAM_POLY_INFO = 0x1004A67C,
    CAM_POLY_PASS = 0x1004A69C,
    CAM_GROUP_PASS = 0x1004A68C,
    BASE_POLY_PASS = 0x1004A61C,
    BASE_GROUP_PASS = 0x1004A4EC,
    BASE_POLY_INFO = 0x1004A4DC
};
enum CameraParamFlag { dCamPrmFlg_UNK040 = 0x40, dCamPrmFlg_UNK080 = 0x80 };
enum WorkTag : u32 { CRAWL_WORK = 0x4352574C, HOOKSHOT_WORK = 0x484F4F4B };
enum ActorJudge : u32 { JUDGE_BY_ID = 0x025E1234 };
enum PlayerStatus : u32 { daPyStts0_CRAWL_e = 0x08000000 };
enum AttentionStatus { dCamAttnStts_00000080_e = 0x80 };
} // namespace camera_engine
using namespace camera_engine;
void cameraEngineLineCtor(dBgS_LinChk*);
void cameraEngineLineDtor(dBgS_LinChk*, s32);
void cameraEngineGlobeValShort(cSGlobe_l*, f32, s16, s16);
void cameraEngineGlobeXyz(cSGlobe_l*, cXyz*);
void cameraEngineGlobeFromVector(cSGlobe_l*, cXyz*);
void cameraEngineAngleSubtractShort(cSAngle_l*, cSAngle_l*, s16);
cSAngle_l* cameraEngineAngleShortCtor(cSAngle_l*, s32);
void cameraEngineAngleAddShort(cSAngle_l*, cSAngle_l*, s16);
cSAngle_l* cameraEngineAngleFloatCtor(cSAngle_l*, f32);
void cameraEngineAngleScale(cSAngle_l*, cSAngle_l*, f32);
void cameraEngineAngleAdd(cSAngle_l*, cSAngle_l*, cSAngle_l*);
void cameraEngineAngleSubtract(cSAngle_l*, cSAngle_l*, cSAngle_l*);
void cameraEngineGlobeCtor(cSGlobe_l*, f32, s16, s16);
void cameraEngineLineSet(LineFrame*, cXyz*, cXyz*);
bool cameraEnginePointAndLine(LineFrame*, cXyz*, cXyz*, GuestFloat*);
bool cameraEngineLineCross(dCamera_c*, cXyz*, cXyz*, cXyz*, u32);
bool cameraEngineVectorNotEqual(cXyz*, cXyz*);
void cameraEngineVectorMultiply(cXyz*, cXyz*, cXyz*);
void cameraEngineShortSubtractAngle(cSAngle_l*, s16, cSAngle_l*);
void cameraEngineAngleVal(cSAngle_l*, cSAngle_l*);
void cameraEngineGlobeValAngles(cSGlobe_l*, f32, cSAngle_l*, cSAngle_l*);
bool cameraEngineLineBlocked(dCamera_c*, cXyz*, cXyz*, u32);
void cameraEngineMoveBgPosition(dBgS*, cBgS_PolyInfo*, s32, cXyz*, u32, u32);

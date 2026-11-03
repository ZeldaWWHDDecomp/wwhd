// Local WWHD render-math layouts, derived from loads and stores in the game.
#pragma once
#include "wwhd.h"
namespace sead {
struct RenderVector2 { be<f32> x, y; };
struct RenderVector3 { be<f32> x, y, z; };
struct RenderQuaternion { be<f32> x, y, z, w; };
struct RenderMatrix34 {
    be<f32> mRows[3][4];
    void setColumn(u32 column, const RenderVector3* vector);
    void setTranslation(const RenderVector3* vector);
    void setQuaternion(const RenderQuaternion* rotation);
};
struct RenderMatrix44 {
    be<f32> mRows[4][4];
    void invert(const RenderMatrix44* source);
};
}
WWHD_OFFSET(sead::RenderVector2,x,0);
WWHD_OFFSET(sead::RenderVector2,y,4);
WWHD_SIZE(sead::RenderVector2,8);
WWHD_OFFSET(sead::RenderVector3,z,8);
WWHD_SIZE(sead::RenderVector3,12);
WWHD_OFFSET(sead::RenderQuaternion,w,12);
WWHD_SIZE(sead::RenderQuaternion,16);
WWHD_OFFSET(sead::RenderMatrix34,mRows,0);
WWHD_SIZE(sead::RenderMatrix34,48);
WWHD_OFFSET(sead::RenderMatrix44,mRows,0);
WWHD_SIZE(sead::RenderMatrix44,64);

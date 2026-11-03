#pragma once
#include "wwhd.h"
#include "sead/RenderState.h"
namespace sead {
struct ProjectionState;
struct ViewportBounds;
union ViewScalar { be<f32> value; be<u32> bits; };
struct ViewVector { ViewScalar x, y, z; };
struct ViewTransform {
    ViewScalar mRows[3][4];
    void inverseTranslation(ViewVector*) const;
    void copyForward(ViewVector*) const;
    void copyRight(ViewVector*) const;
    void copyUp(ViewVector*) const;
    void transformPoint(ViewVector*, const ViewVector*) const;
    void projectToViewport(ViewVector*,const ViewVector*,ProjectionState*,const ViewportBounds*) const;
};
struct ViewCamera {
    ViewTransform mTransform;
    gptr<void> mVtable;
    ViewVector mEye,mTarget,mUp;
    void buildView(ViewTransform*) const;
};
WWHD_OFFSET(ViewCamera,mVtable,0x30);
WWHD_OFFSET(ViewCamera,mEye,0x34);
WWHD_OFFSET(ViewCamera,mTarget,0x40);
WWHD_OFFSET(ViewCamera,mUp,0x4C);
WWHD_SIZE(ViewCamera,0x58);
ViewCamera* createLookAtCamera(ViewCamera*,const ViewVector*,const ViewVector*,const ViewVector*);
ViewCamera* createProjectionCamera(ViewCamera*,const struct ProjectionState*);
void* allocateGraphicsFrame(u32,s32);
WWHD_SIZE(ViewScalar,4);
WWHD_SIZE(ViewVector,12);
WWHD_SIZE(ViewTransform,48);
WWHD_OFFSET(ViewTransform,mRows,0);
BlendState* createBlendChannels(BlendState*);
void destroyBlendChannels(BlendState*, u32);
void blendChannelsChanged();
void destroyViewTransform(ViewTransform*, u32);
void destroyLookAtTransform(ViewTransform*, u32);
void destroyOrthographicView(ViewTransform*, u32);
void copyStoredView(const ViewTransform*, ViewTransform*);
}
void* seadAllocateViewStorage(u32);
void seadReleaseViewStorage(void*);
void seadMultiplyViewPoint(const sead::ViewTransform*,const sead::ViewVector*,sead::ViewVector*);
void seadCopyViewMatrix(const sead::ViewTransform*,sead::ViewTransform*);
void seadNormalizeViewVector(sead::ViewVector*);
void* seadBaseFrameHeap(s32);
void* seadFrameHeapAllocate(void*,u32,s32);
void seadCrossViewVectors(const sead::ViewVector*,const sead::ViewVector*,sead::ViewVector*);

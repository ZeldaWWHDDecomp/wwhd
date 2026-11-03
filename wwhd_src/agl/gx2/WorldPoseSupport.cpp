/* Affine hierarchy update reconstructed from the assigned WWHD body. */
#include "agl/gx2/WorldPoseSupport.h"
using namespace world_pose_support;
namespace world_pose_support {
static void copyAffine(AffineMatrix_l* output,const AffineMatrix_l* input) {
    for (u32 i=0;i<12;++i) output->mValues[i].mBits=input->mValues[i].mBits;
}
static void concatenate(AffineMatrix_l* output,const AffineMatrix_l* parent,const FloatWord* local) {
    f32 right[12];
    for (u32 i=0;i<12;++i) right[i]=local[i].mValue;
    for (u32 row=0;row<3;++row) {
        f32 x=parent->mValues[row*4].mValue;
        f32 y=parent->mValues[row*4+1].mValue;
        f32 z=parent->mValues[row*4+2].mValue;
        f32 w=parent->mValues[row*4+3].mValue;
        f32 values[4];
        for (u32 col=0;col<4;++col) {
            f32 value=gabi::fmuls_ppc(right[col],x);
            value=gabi::fmadds(right[4+col],y,value);
            value=gabi::fmadds(right[8+col],z,value);
            if (col>=2) value=gabi::fmadds(col==2?0.0f:1.0f,w,value);
            values[col]=value;
        }
        for(u32 col=0;col<4;++col) output->mValues[row*4+col].mValue=values[col];
    }
}
static void scaleAffineValues(AffineMatrix_l* output,const AffineMatrix_l* input,const be<u32>* scale) {
    f32 z=*reinterpret_cast<const be<f32>*>(&scale[2]);
    for (u32 row=0;row<3;++row) {
        f32 x=*reinterpret_cast<const be<f32>*>(&scale[0]);
        f32 y=*reinterpret_cast<const be<f32>*>(&scale[1]);
        f32 a=input->mValues[row*4].mValue;
        f32 b=input->mValues[row*4+1].mValue;
        f32 c=input->mValues[row*4+2].mValue;
        f32 d=input->mValues[row*4+3].mValue;
        output->mValues[row*4].mValue=gabi::fmuls_ppc(a,x);
        output->mValues[row*4+1].mValue=gabi::fmuls_ppc(b,y);
        output->mValues[row*4+2].mValue=gabi::fmuls_ppc(c,z);
        output->mValues[row*4+3].mValue=gabi::fmuls_ppc(d,1.0f);
    }
}
static void scaleAffine(AffineMatrix_l* output,const AffineMatrix_l* input,const TransformBone_l* bone) {
    scaleAffineValues(output,input,bone->mScale);
}
static void notify(WorldCallback_l* receiver,WorldCursor_l* cursor,void* update) {
    be<u32>* table=receiver->mVtable;
    gabi::call_ptr<void>(table[5],receiver,cursor,update);
}
/* 027E4D20 */
void WorldSkeleton_l::updateWorld(const AffineMatrix_l* base) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mFlags=static_cast<u16>(mFlags)|4;
    TransformBone_l* local=mLocal;
    u32 flags=local->mFlags;
    s32 count=mBoneCount;
    AffineMatrix_l* world=mWorld;
    if ((flags&0x0C000000)==0x0C000000) copyAffine(world,base);
    else concatenate(world,base,local->mMatrix);
    flags=local->mFlags;
    local->mFlags=(flags&0x0FFFFFFF)|((flags<<4)&0x30000000);
    gabi::FrameLocal<be<u32>> index(8);
    gabi::FrameLocal<WorldCursor_l> cursor(0x18);
    cursor->mIndex=index.get(); cursor->mNext=mNextCallback;
    u32 current=0;
    if (cursor->mNext==0) {
        gabi::FrameLocal<WorldUpdate_l> update(0x20);
        update->mLocal=mLocal; update->mWorld=mWorld; update->mFlags=0;
        *index=0;
        notify(mCallback,cursor.get(),update.get());
        current=*index;
    }
    for (;;) {
        ++current; *index=current;
        if (static_cast<s32>(current)>=count) break;
        const BoneTopology_l* topology=mTopology;
        u32 parent=topology[current].mParent;
        u32 child=topology[current].mIndex;
        TransformBone_l* bone=local+child;
        u32 childFlags=bone->mFlags;
        u32 parentFlags=local[parent].mFlags;
        childFlags=(childFlags&0x0FFFFFFF)|((parentFlags&(childFlags<<4))&0xF0000000);
        bone->mFlags=childFlags;
        if ((childFlags&0x0C000000)==0x0C000000) copyAffine(world+child,world+parent);
        else concatenate(world+child,world+parent,bone->mMatrix);
        if (cursor->mNext==static_cast<u32>(*index)) {
            current=*index;
            gabi::FrameLocal<WorldUpdate_l> update(0xC);
            AffineMatrix_l* freshWorld=mWorld;
            WorldCallback_l* receiver=mCallback;
            TransformBone_l* freshLocal=mLocal;
            update->mWorld=freshWorld+current; update->mFlags=0; update->mLocal=freshLocal+current;
            notify(receiver,cursor.get(),update.get());
            current=*index;
        }
    }
}
/* 027E50A8 */
void WorldSkeleton_l::updateWorldScaled(const AffineMatrix_l* base) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mFlags=static_cast<u16>(mFlags)|4;
    TransformBone_l* local=mLocal;
    u32 flags=local->mFlags;
    s32 count=mBoneCount;
    AffineMatrix_l* world=mWorld;
    if ((flags&0x0C000000)==0x0C000000) copyAffine(world,base);
    else concatenate(world,base,local->mMatrix);
    flags=local->mFlags;
    local->mFlags=(flags&0x0FFFFFFF)|((flags<<4)&0x30000000);
    gabi::FrameLocal<be<u32>> index(8);
    gabi::FrameLocal<WorldCursor_l> cursor(0x18);
    cursor->mIndex=index.get(); cursor->mNext=mNextCallback;
    u32 current=0;
    if (cursor->mNext==0) {
        gabi::FrameLocal<ScaledWorldUpdate_l> update(0x20);
        update->mLocal=mLocal; update->mWorld=mWorld; update->mScale=local->mScale;
        *index=0;
        notify(mCallback,cursor.get(),update.get());
        current=*index;
    }
    for (;;) {
        ++current; *index=current;
        if (static_cast<s32>(current)>=count) break;
        const BoneTopology_l* topology=mTopology;
        u32 parent=topology[current].mParent;
        u32 child=topology[current].mIndex;
        TransformBone_l* bone=local+child;
        u32 childFlags=bone->mFlags;
        u32 parentFlags=local[parent].mFlags;
        childFlags=(childFlags&0x0FFFFFFF)|((parentFlags&(childFlags<<4))&0xF0000000);
        bone->mFlags=childFlags;
        if ((static_cast<u32>(local[parent].mFlags)&0x03000000)==0x03000000) {
            if ((childFlags&0x0C000000)==0x0C000000) copyAffine(world+child,world+parent);
            else concatenate(world+child,world+parent,bone->mMatrix);
        } else {
            scaleAffine(world+child,world+parent,local+parent);
            if ((static_cast<u32>(bone->mFlags)&0x0C000000)!=0x0C000000)
                concatenate(world+child,world+child,bone->mMatrix);
        }
        if (cursor->mNext==static_cast<u32>(*index)) {
            current=*index;
            gabi::FrameLocal<ScaledWorldUpdate_l> update(0xC);
            AffineMatrix_l* freshWorld=mWorld;
            WorldCallback_l* receiver=mCallback;
            TransformBone_l* freshLocal=mLocal;
            update->mWorld=freshWorld+current; update->mScale=(local+current)->mScale; update->mLocal=freshLocal+current;
            notify(receiver,cursor.get(),update.get());
            current=*index;
        }
    }
    for (s32 i=0;i<count;++i) {
        if ((static_cast<u32>(local[i].mFlags)&0x03000000)!=0x03000000)
            scaleAffine(world+i,world+i,local+i);
    }
}
/* 027E565C */
void WorldSkeleton_l::updateWorldCompensated(const AffineMatrix_l* base) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mFlags=static_cast<u16>(mFlags)|4;
    TransformBone_l* local=mLocal;
    u32 flags=local->mFlags;
    s32 count=mBoneCount;
    AffineMatrix_l* world=mWorld;
    if ((flags&0x0C000000)==0x0C000000) copyAffine(world,base);
    else concatenate(world,base,local->mMatrix);
    flags=local->mFlags;
    local->mFlags=(flags&0x0FFFFFFF)|((flags<<4)&0x30000000);
    gabi::FrameLocal<be<u32>> index(8);
    gabi::FrameLocal<WorldCursor_l> cursor(0x18);
    cursor->mIndex=index.get(); cursor->mNext=mNextCallback;
    u32 current=0;
    if (cursor->mNext==0) {
        gabi::FrameLocal<ScaledWorldUpdate_l> update(0x20);
        update->mLocal=mLocal; update->mWorld=mWorld; update->mScale=local->mScale;
        *index=0;
        notify(mCallback,cursor.get(),update.get());
        current=*index;
    }
    for (;;) {
        ++current; *index=current;
        if (static_cast<s32>(current)>=count) break;
        const BoneTopology_l* topology=mTopology;
        u32 parent=topology[current].mParent;
        u32 child=topology[current].mIndex;
        TransformBone_l* bone=local+child;
        u32 childFlags=bone->mFlags;
        u32 parentFlags=local[parent].mFlags;
        childFlags=(childFlags&0x0FFFFFFF)|((parentFlags&(childFlags<<4))&0xF0000000);
        bone->mFlags=childFlags;
        bool parentUnitScale=(static_cast<u32>(local[parent].mFlags)&0x03000000)==0x03000000;
        if ((childFlags&0x0C000000)==0x0C000000) {
            if ((childFlags&0x00800000)||parentUnitScale) copyAffine(world+child,world+parent);
            else scaleAffine(world+child,world+parent,local+parent);
        } else if (parentUnitScale) {
            concatenate(world+child,world+parent,bone->mMatrix);
        } else if (childFlags&0x00800000) {
            AffineMatrix_l* output=world+child;
            copyAffine(output,reinterpret_cast<const AffineMatrix_l*>(bone->mMatrix));
            for (u32 axis=0;axis<3;++axis) {
                f32 scale=*reinterpret_cast<const be<f32>*>(&local[parent].mScale[axis]);
                f32 translation=output->mValues[axis*4+3].mValue;
                output->mValues[axis*4+3].mValue=gabi::fmuls_ppc(translation,scale);
            }
            concatenate(output,world+parent,output->mValues);
        } else {
            scaleAffine(world+child,world+parent,local+parent);
            concatenate(world+child,world+child,bone->mMatrix);
        }
        if (cursor->mNext==static_cast<u32>(*index)) {
            current=*index;
            gabi::FrameLocal<ScaledWorldUpdate_l> update(0xC);
            AffineMatrix_l* freshWorld=mWorld;
            WorldCallback_l* receiver=mCallback;
            TransformBone_l* freshLocal=mLocal;
            update->mWorld=freshWorld+current; update->mScale=(local+current)->mScale; update->mLocal=freshLocal+current;
            notify(receiver,cursor.get(),update.get());
            current=*index;
        }
    }
    for (s32 i=0;i<count;++i) {
        if ((static_cast<u32>(local[i].mFlags)&0x03000000)!=0x03000000)
            scaleAffine(world+i,world+i,local+i);
    }
}
/* 027E5DA4 */
void WorldSkeleton_l::updateWorldCumulativeScale(const AffineMatrix_l* base) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    mFlags=static_cast<u16>(mFlags)|4;
    TransformBone_l* local=mLocal;
    u32 flags=local->mFlags;
    s32 count=mBoneCount;
    AffineMatrix_l* world=mWorld;
    be<u32>* cumulative=mCumulativeScale;
    if ((flags&0x0C000000)==0x0C000000) copyAffine(world,base);
    else concatenate(world,base,local->mMatrix);
    flags=local->mFlags;
    local->mFlags=(flags&0x0FFFFFFF)|((flags<<4)&0x30000000);
    if ((static_cast<u32>(local->mFlags)&0x03000000)==0x03000000) {
        cumulative[0]=0x3F800000; cumulative[2]=0x3F800000; cumulative[1]=0x3F800000;
    } else {
        for(u32 axis=0;axis<3;++axis) cumulative[axis]=local->mScale[axis];
    }
    gabi::FrameLocal<be<u32>> index(8);
    gabi::FrameLocal<WorldCursor_l> cursor(0x18);
    cursor->mIndex=index.get(); cursor->mNext=mNextCallback;
    u32 current=0;
    if (cursor->mNext==0) {
        gabi::FrameLocal<ScaledWorldUpdate_l> update(0x20);
        update->mLocal=mLocal; update->mWorld=mWorld; update->mScale=cumulative;
        *index=0;
        notify(mCallback,cursor.get(),update.get());
        current=*index;
    }
    for (;;) {
        ++current; *index=current;
        if (static_cast<s32>(current)>=count) break;
        const BoneTopology_l* topology=mTopology;
        u32 parent=topology[current].mParent;
        u32 child=topology[current].mIndex;
        TransformBone_l* bone=local+child;
        u32 childFlags=bone->mFlags;
        u32 parentFlags=local[parent].mFlags;
        childFlags=(childFlags&0x0FFFFFFF)|((parentFlags&(childFlags<<4))&0xF0000000);
        bone->mFlags=childFlags;
        if ((childFlags&0x0C000000)==0x0C000000) {
            copyAffine(world+child,world+parent);
        } else if ((static_cast<u32>(local[parent].mFlags)&0x30000000)==0x30000000) {
            concatenate(world+child,world+parent,bone->mMatrix);
        } else {
            AffineMatrix_l* output=world+child;
            copyAffine(output,reinterpret_cast<const AffineMatrix_l*>(bone->mMatrix));
            for (u32 axis=0;axis<3;++axis) {
                f32 scale=*reinterpret_cast<const be<f32>*>(&cumulative[parent*3+axis]);
                f32 translation=output->mValues[axis*4+3].mValue;
                output->mValues[axis*4+3].mValue=gabi::fmuls_ppc(translation,scale);
            }
            concatenate(output,world+parent,output->mValues);
        }
        if ((static_cast<u32>(bone->mFlags)&0x03000000)==0x03000000) {
            for(u32 axis=0;axis<3;++axis) cumulative[child*3+axis]=cumulative[parent*3+axis];
        } else {
            f32 x=*reinterpret_cast<const be<f32>*>(&cumulative[parent*3]);
            f32 y=*reinterpret_cast<const be<f32>*>(&cumulative[parent*3+1]);
            f32 sx=*reinterpret_cast<const be<f32>*>(&bone->mScale[0]);
            f32 sy=*reinterpret_cast<const be<f32>*>(&bone->mScale[1]);
            *reinterpret_cast<be<f32>*>(&cumulative[child*3])=gabi::fmuls_ppc(x,sx);
            *reinterpret_cast<be<f32>*>(&cumulative[child*3+1])=gabi::fmuls_ppc(y,sy);
            f32 z=*reinterpret_cast<const be<f32>*>(&cumulative[parent*3+2]);
            f32 sz=*reinterpret_cast<const be<f32>*>(&bone->mScale[2]);
            *reinterpret_cast<be<f32>*>(&cumulative[child*3+2])=gabi::fmuls_ppc(z,sz);
        }
        if (cursor->mNext==static_cast<u32>(*index)) {
            current=*index;
            gabi::FrameLocal<ScaledWorldUpdate_l> update(0xC);
            AffineMatrix_l* freshWorld=mWorld;
            WorldCallback_l* receiver=mCallback;
            TransformBone_l* freshLocal=mLocal;
            update->mWorld=freshWorld+current; update->mScale=cumulative+3*current; update->mLocal=freshLocal+current;
            notify(receiver,cursor.get(),update.get());
            current=*index;
        }
     }
    for(s32 i=0;i<count;++i) {
        if ((static_cast<u32>(local[i].mFlags)&0x30000000)!=0x30000000)
            scaleAffineValues(world+i,world+i,cumulative+3*i);
    }
}
}
/* WWHD glue
*/

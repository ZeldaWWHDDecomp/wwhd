// WWHD inline object-lifetime helpers, layouts inferred from local instructions.
#pragma once
#include "sead/Foundation.h"
namespace sead {
struct ObjectHeader {
    u8 mLinks[12];
    be<u32> mTable;
};
struct EmbeddedObject {
    ObjectHeader mHeader;
    Link mRegistration;
    be<u32> mRegistrationData;
    gptr<LinkList> mOwner;
    gptr<void> mCallback;
    u8 mBody[20];
    be<u8> mRegistered;
    u8 mAlignment[3];
};
struct ResourceOwner { gptr<be<u32>> mTable; };
struct TaskOwner { u8 mContext[12]; gptr<be<u32>> mTable; };
struct RenderObject {
    u8 mRenderBase[28];
    ObjectHeader mObjectBase;
    u8 mRenderData[36];
    be<u32> mRenderTable;
};
}
WWHD_OFFSET(sead::ObjectHeader,mTable,12);
WWHD_SIZE(sead::ObjectHeader,16);
WWHD_OFFSET(sead::EmbeddedObject,mRegistration,16);
WWHD_OFFSET(sead::EmbeddedObject,mOwner,28);
WWHD_OFFSET(sead::EmbeddedObject,mCallback,32);
WWHD_OFFSET(sead::EmbeddedObject,mRegistered,56);
WWHD_SIZE(sead::EmbeddedObject,60);
WWHD_OFFSET(sead::RenderObject,mObjectBase,28);
WWHD_OFFSET(sead::RenderObject,mRenderTable,80);
WWHD_SIZE(sead::RenderObject,84);

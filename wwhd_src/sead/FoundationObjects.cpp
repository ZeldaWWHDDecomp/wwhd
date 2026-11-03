// WWHD inline singleton and registration lifetime helpers; reconstructed from local instructions.
#include "sead/FoundationObjects.h"

static sead::ObjectHeader* sTaskSingleton;
static sead::TaskOwner* sTaskSingletonOwner;
static sead::ObjectHeader* sResourceSingleton;
static sead::ResourceOwner* sResourceSingletonOwner;
static void* sRegistrationSingleton;
static void* sLocalRegistrationSingleton;
static u32 sTaskObjectTable;
static u32 sResourceObjectTable;
static u32 sEmbeddedObjectTable;
static u32 sRenderObjectTable;
static u32 sRenderBaseTable;
void destroyInlineObject(void*, u32);
void deleteInlineObject(void*);
void finalizeRenderObject(void*);

/* 02720548 */
static void destroyTaskSingleton(sead::ObjectHeader* object, u32 flags) {
    if (object == NULL) return;
    object->mTable = gabi::ea(&sTaskObjectTable);
    if (object == sTaskSingleton) {
        sead::TaskOwner* owner = sTaskSingletonOwner;
        sTaskSingleton = NULL;
        gabi::call_ptr<void>(owner->mTable[3], owner, 2);
        sTaskSingletonOwner = NULL;
    }
    destroyInlineObject(object, 0);
    if (flags & 1) deleteInlineObject(object);
}

/* 027293E0 */
static void destroyResourceSingleton(sead::ObjectHeader* object, u32 flags) {
    if (object == NULL) return;
    object->mTable = gabi::ea(&sResourceObjectTable);
    if (object == sResourceSingleton) {
        sead::ResourceOwner* owner = sResourceSingletonOwner;
        sResourceSingleton = NULL;
        gabi::call_ptr<void>(owner->mTable[5], owner, 2);
        sResourceSingletonOwner = NULL;
    }
    destroyInlineObject(object, 0);
    if (flags & 1) deleteInlineObject(object);
}

/* 02729208 */
static void destroyRegistrationToken(be<u8>* token, u32 flags) {
    if (token == NULL) return;
    if (*token != 0) sRegistrationSingleton = NULL;
    if (flags & 1) deleteInlineObject(token);
}

/* 0273C6E0 */
static void destroyControllerToken(be<u8>* token, u32 flags) {
    if (token == NULL) return;
    if (*token != 0) sLocalRegistrationSingleton = NULL;
    if (flags & 1) deleteInlineObject(token);
}

/* 0272B268 */
static void destroyEmbeddedObject(sead::EmbeddedObject* object, u32 flags) {
    if (object == NULL) return;
    u8 registered = object->mRegistered;
    object->mHeader.mTable = gabi::ea(&sEmbeddedObjectTable);
    if (registered != 0) {
        sead::LinkList* owner = object->mOwner;
        if (owner != NULL) {
            object->mOwner = NULL;
            object->mRegistration.detach();
            owner->mCount = (u32)owner->mCount - 1;
        }
        object->mRegistered = 0;
    }
    destroyInlineObject(object, 0);
    if (flags & 1) deleteInlineObject(object);
}

/* 0272B060 */
static void destroyRenderObject(sead::RenderObject* object, u32 flags) {
    if (object == NULL) return;
    object->mRenderTable = gabi::ea(&sRenderObjectTable);
    object->mObjectBase.mTable = gabi::ea(&sRenderBaseTable);
    finalizeRenderObject(object);
    destroyInlineObject(&object->mObjectBase, 0);
    if (flags & 1) deleteInlineObject(object);
}

/* WWHD glue
 data sTaskSingleton 101F84E0
 data sTaskSingletonOwner 101F84DC
 data sResourceSingleton 101F86E4
 data sResourceSingletonOwner 101F86E0
 data sRegistrationSingleton 101F86E8
 data sLocalRegistrationSingleton 101F8AE8
 data sTaskObjectTable 10140770
 data sResourceObjectTable 10141398
 data sEmbeddedObjectTable 10141200
 data sRenderObjectTable 101411A0
 data sRenderBaseTable 101411C0
 ext 02752BEC void destroyInlineObject(void*, u32)
 ext 0273AF40 void deleteInlineObject(void*)
 ext 02747A84 void finalizeRenderObject(void*)
*/

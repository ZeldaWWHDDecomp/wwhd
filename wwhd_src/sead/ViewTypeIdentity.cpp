// Lazy view type identities reconstructed from WWHD pointer comparisons.
#include "sead/ViewTypeIdentity.h"
#include "sead/ViewTransform.h"
#include "sead/RuntimeType.h"
namespace sead {}
using namespace sead;
static u32 sViewGuard;
static RuntimeType sViewTag;
static u32 sLookAtGuard;
static RuntimeType sLookAtTag;
static u32 sDerivedLookAtGuard;
static RuntimeType sDerivedLookAtTag;
static u32 sOrthographicGuard;
static RuntimeType sOrthographicTag;
static u32 sStoredProjectionGuard;
static RuntimeType sStoredProjectionTag;
static u8 sViewDefinition;
static u8 sLookAtDefinition;
static u8 sDerivedLookAtDefinition;
/* 0274CF2C */
bool sead::isViewType(const void*,const RuntimeType* type) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!sViewGuard) {
        sViewGuard=1;
        sViewTag.mTable=&sViewDefinition;
    }
    return type==&sViewTag;
}
/* 0274CF68 */
bool sead::isLookAtViewType(const void*,const RuntimeType* type) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!sLookAtGuard) {
        sLookAtGuard=1;
        sLookAtTag.mTable=&sLookAtDefinition;
    }
    if(type==&sLookAtTag) return true;
    if(!sViewGuard) {
        sViewGuard=1;
        sViewTag.mTable=&sViewDefinition;
    }
    return type==&sViewTag;
}
/* 0274CFE0 */
bool sead::isDerivedLookAtViewType(const void*,const RuntimeType* type) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!sDerivedLookAtGuard) {
        sDerivedLookAtGuard=1;
        sDerivedLookAtTag.mTable=&sDerivedLookAtDefinition;
    }
    if(type==&sDerivedLookAtTag) return true;
    if(!sLookAtGuard) {
        sLookAtGuard=1;
        sLookAtTag.mTable=&sLookAtDefinition;
    }
    if(type==&sLookAtTag) return true;
    if(!sViewGuard) {
        sViewGuard=1;
        sViewTag.mTable=&sViewDefinition;
    }
    return type==&sViewTag;
}
/* 0274D09C */
bool sead::isOrthographicViewType(const void*,const RuntimeType* type) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!sOrthographicGuard) {
        sOrthographicGuard=1;
        sOrthographicTag.mTable=&sLookAtDefinition;
    }
    if(type==&sOrthographicTag) return true;
    if(!sViewGuard) {
        sViewGuard=1;
        sViewTag.mTable=&sViewDefinition;
    }
    return type==&sViewTag;
}
/* 0274EB98 */
bool sead::isProjectionType(const void*,const RuntimeType* type) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!sStreamBaseGuard) {
        sStreamBaseGuard=1;
        sStreamBaseType.mTable=&sStreamBaseTable;
    }
    return type==&sStreamBaseType;
}
/* 0274EBD4 */
bool sead::isFrustumProjectionType(const void*,const RuntimeType* type) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!sStreamGuard) {
        sStreamGuard=1;
        sStreamType.mTable=&sStreamTable;
    }
    if(type==&sStreamType) return true;
    if(!sStreamBaseGuard) {
        sStreamBaseGuard=1;
        sStreamBaseType.mTable=&sStreamBaseTable;
    }
    return type==&sStreamBaseType;
}
/* 0274EC54 */
bool sead::isStoredProjectionType(const void*,const RuntimeType* type) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!sStoredProjectionGuard) {
        sStoredProjectionGuard=1;
        sStoredProjectionTag.mTable=&sStreamTable;
    }
    if(type==&sStoredProjectionTag) return true;
    if(!sStreamBaseGuard) {
        sStreamBaseGuard=1;
        sStreamBaseType.mTable=&sStreamBaseTable;
    }
    return type==&sStreamBaseType;
}
/* 0274D67C */
void sead::destroyViewResource(void* object,u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(object && (flags&1)) seadReleaseViewStorage(object);
}
/* WWHD glue
data sViewGuard 101FD6B0
data sViewTag 101FDF6C
data sLookAtGuard 101FD6B4
data sLookAtTag 101FDF70
data sDerivedLookAtGuard 101FD6B8
data sDerivedLookAtTag 101FDF74
data sOrthographicGuard 101FD6BC
data sOrthographicTag 101FDF78
data sStoredProjectionGuard 101FD6A4
data sStoredProjectionTag 101FDF80
data sViewDefinition 1014509C
data sLookAtDefinition 101450AC
data sDerivedLookAtDefinition 101450BC
*/

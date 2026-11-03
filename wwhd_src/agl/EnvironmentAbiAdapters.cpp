// Adjusted interface entry points preserve their observed tail-call targets and guest stack depth.
#include "agl/EnvironmentAbiViews.h"
using namespace agl::env;
const sead::SafeText* forwardManagerObjectName(EnvironmentManager*,s32,u32);
u32 forwardManagerClassObjectCount(EnvironmentManager*,u32);
void forwardManagerUpdate(EnvironmentManager*);
void forwardManagerApplyResource(EnvironmentManager*,const EnvironmentArchiveReference*);
const sead::SafeText* forwardProjectorGroupText(Projector*);
const sead::SafeText* forwardProjectorNameData(Projector*);
s32 forwardProjectorTypeId(Projector*);
u32 forwardProjectorEnabled(Projector*);
const sead::SafeText* forwardFogGroupText(Fog*);
const sead::SafeText* forwardFogNameData(Fog*);
s32 forwardFogTypeId(Fog*);
u32 forwardFogEnabled(Fog*);
const sead::SafeText* forwardEnvironmentObjectGroupText(EnvironmentObject*);
const sead::SafeText* forwardEnvironmentObjectNameData(EnvironmentObject*);
s32 forwardEnvironmentObjectTypeId(EnvironmentObject*);
u32 forwardEnvironmentObjectEnabled(EnvironmentObject*);
void releaseEnvironmentIconIo(void*);

/* 0276B3D0 */
const sead::SafeText* EnvironmentManagerParameterView_l::objectName(s32 objectIndex,u32 typeId) const {
    EnvironmentManager* object = const_cast<EnvironmentManager*>(reinterpret_cast<const EnvironmentManager*>(reinterpret_cast<const u8*>(this) - offsetof(EnvironmentManager,mParameters)));
    return gabi::tail_ptr<const sead::SafeText*>(gabi::guest_fn((const void*)forwardManagerObjectName),object,objectIndex,typeId);
}
/* 0276B3D8 */
u32 EnvironmentManagerParameterView_l::classObjectCount(u32 typeId) const {
    EnvironmentManager* object = const_cast<EnvironmentManager*>(reinterpret_cast<const EnvironmentManager*>(reinterpret_cast<const u8*>(this) - offsetof(EnvironmentManager,mParameters)));
    return gabi::tail_ptr<u32>(gabi::guest_fn((const void*)forwardManagerClassObjectCount),object,typeId);
}
/* 0276B708 */
void EnvironmentManagerParameterView_l::update() {
    EnvironmentManager* object = const_cast<EnvironmentManager*>(reinterpret_cast<const EnvironmentManager*>(reinterpret_cast<const u8*>(this) - offsetof(EnvironmentManager,mParameters)));
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)forwardManagerUpdate),object);
}
/* 0276C1D4 */
void EnvironmentManagerIoApplyView_l::applyResource(const EnvironmentArchiveReference* resource) {
    EnvironmentManager* object = const_cast<EnvironmentManager*>(reinterpret_cast<const EnvironmentManager*>(reinterpret_cast<const u8*>(this) - offsetof(EnvironmentManager,mIo)));
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)forwardManagerApplyResource),object,resource);
}
/* 0276CDF0 */
const sead::SafeText* ProjectorIoView_l::groupText() const {
    Projector* object = const_cast<Projector*>(reinterpret_cast<const Projector*>(reinterpret_cast<const u8*>(this) - offsetof(EnvironmentObject,mParameterIoMethods)));
    return gabi::tail_ptr<const sead::SafeText*>(gabi::guest_fn((const void*)forwardProjectorGroupText),object);
}
/* 0276CDF8 */
const sead::SafeText* ProjectorIoView_l::nameData() const {
    Projector* object = const_cast<Projector*>(reinterpret_cast<const Projector*>(reinterpret_cast<const u8*>(this) - offsetof(EnvironmentObject,mParameterIoMethods)));
    return gabi::tail_ptr<const sead::SafeText*>(gabi::guest_fn((const void*)forwardProjectorNameData),object);
}
/* 0276CE00 */
s32 ProjectorIoView_l::typeId() const {
    Projector* object = const_cast<Projector*>(reinterpret_cast<const Projector*>(reinterpret_cast<const u8*>(this) - offsetof(EnvironmentObject,mParameterIoMethods)));
    return gabi::tail_ptr<s32>(gabi::guest_fn((const void*)forwardProjectorTypeId),object);
}
/* 0276CE08 */
u32 ProjectorIoView_l::enabled() const {
    Projector* object = const_cast<Projector*>(reinterpret_cast<const Projector*>(reinterpret_cast<const u8*>(this) - offsetof(EnvironmentObject,mParameterIoMethods)));
    return gabi::tail_ptr<u32>(gabi::guest_fn((const void*)forwardProjectorEnabled),object);
}
/* 0276EE50 */
const sead::SafeText* FogIoView_l::groupText() const {
    Fog* object = const_cast<Fog*>(reinterpret_cast<const Fog*>(reinterpret_cast<const u8*>(this) - offsetof(EnvironmentObject,mParameterIoMethods)));
    return gabi::tail_ptr<const sead::SafeText*>(gabi::guest_fn((const void*)forwardFogGroupText),object);
}
/* 0276EE58 */
const sead::SafeText* FogIoView_l::nameData() const {
    Fog* object = const_cast<Fog*>(reinterpret_cast<const Fog*>(reinterpret_cast<const u8*>(this) - offsetof(EnvironmentObject,mParameterIoMethods)));
    return gabi::tail_ptr<const sead::SafeText*>(gabi::guest_fn((const void*)forwardFogNameData),object);
}
/* 0276EE60 */
s32 FogIoView_l::typeId() const {
    Fog* object = const_cast<Fog*>(reinterpret_cast<const Fog*>(reinterpret_cast<const u8*>(this) - offsetof(EnvironmentObject,mParameterIoMethods)));
    return gabi::tail_ptr<s32>(gabi::guest_fn((const void*)forwardFogTypeId),object);
}
/* 0276EE68 */
u32 FogIoView_l::enabled() const {
    Fog* object = const_cast<Fog*>(reinterpret_cast<const Fog*>(reinterpret_cast<const u8*>(this) - offsetof(EnvironmentObject,mParameterIoMethods)));
    return gabi::tail_ptr<u32>(gabi::guest_fn((const void*)forwardFogEnabled),object);
}
/* 0276FC80 */
const sead::SafeText* EnvironmentObjectIoView_l::groupText() const {
    EnvironmentObject* object = const_cast<EnvironmentObject*>(reinterpret_cast<const EnvironmentObject*>(reinterpret_cast<const u8*>(this) - offsetof(EnvironmentObject,mParameterIoMethods)));
    return gabi::tail_ptr<const sead::SafeText*>(gabi::guest_fn((const void*)forwardEnvironmentObjectGroupText),object);
}
/* 0276FC88 */
const sead::SafeText* EnvironmentObjectIoView_l::nameData() const {
    EnvironmentObject* object = const_cast<EnvironmentObject*>(reinterpret_cast<const EnvironmentObject*>(reinterpret_cast<const u8*>(this) - offsetof(EnvironmentObject,mParameterIoMethods)));
    return gabi::tail_ptr<const sead::SafeText*>(gabi::guest_fn((const void*)forwardEnvironmentObjectNameData),object);
}
/* 0276FC90 */
s32 EnvironmentObjectIoView_l::typeId() const {
    EnvironmentObject* object = const_cast<EnvironmentObject*>(reinterpret_cast<const EnvironmentObject*>(reinterpret_cast<const u8*>(this) - offsetof(EnvironmentObject,mParameterIoMethods)));
    return gabi::tail_ptr<s32>(gabi::guest_fn((const void*)forwardEnvironmentObjectTypeId),object);
}
/* 0276FC98 */
u32 EnvironmentObjectIoView_l::enabled() const {
    EnvironmentObject* object = const_cast<EnvironmentObject*>(reinterpret_cast<const EnvironmentObject*>(reinterpret_cast<const u8*>(this) - offsetof(EnvironmentObject,mParameterIoMethods)));
    return gabi::tail_ptr<u32>(gabi::guest_fn((const void*)forwardEnvironmentObjectEnabled),object);
}
/* 0276CD88 */
void EnvironmentIconIoDeletionView_l::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEnvironmentIconIo),this);
}
/* WWHD glue
 ext 0276CB1C const sead::SafeText* forwardManagerObjectName(EnvironmentManager*,s32,u32) callback
 ext 0276CBB0 u32 forwardManagerClassObjectCount(EnvironmentManager*,u32) callback
 ext 0276B3E0 void forwardManagerUpdate(EnvironmentManager*) callback
 ext 0276CBD0 void forwardManagerApplyResource(EnvironmentManager*,const EnvironmentArchiveReference*) callback
 ext 0276D7F4 const sead::SafeText* forwardProjectorGroupText(Projector*) callback
 ext 0276D804 const sead::SafeText* forwardProjectorNameData(Projector*) callback
 ext 0276D80C s32 forwardProjectorTypeId(Projector*) callback
 ext 0276D81C u32 forwardProjectorEnabled(Projector*) callback
 ext 0276F1F4 const sead::SafeText* forwardFogGroupText(Fog*) callback
 ext 0276F204 const sead::SafeText* forwardFogNameData(Fog*) callback
 ext 0276F20C s32 forwardFogTypeId(Fog*) callback
 ext 0276F21C u32 forwardFogEnabled(Fog*) callback
 ext 027706DC const sead::SafeText* forwardEnvironmentObjectGroupText(EnvironmentObject*) callback
 ext 027706EC const sead::SafeText* forwardEnvironmentObjectNameData(EnvironmentObject*) callback
 ext 027706F4 s32 forwardEnvironmentObjectTypeId(EnvironmentObject*) callback
 ext 02770704 u32 forwardEnvironmentObjectEnabled(EnvironmentObject*) callback
 ext 0273AF40 void releaseEnvironmentIconIo(void*) callback
*/

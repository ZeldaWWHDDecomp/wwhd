// Resource acceptance and hash matching shared by the set's owner and parameter views.
#include "agl/EnvironmentSet.h"
#include "agl/EnvironmentBlendResource.h"
using namespace agl::env;

/* 0276EDE8 */
u32 EnvironmentSetOwner::supportsParameters() const { return 1; }
/* 0276EDF0 */
void EnvironmentSetOwner::beforeResourceRead() { }
/* 0276EDF4 */
u32 EnvironmentSetOwner::acceptsResourceLists() const { return 1; }
/* 0276EDFC */
void EnvironmentSetOwner::afterResourceRead() { }
/* 0276EE00 */
u32 EnvironmentSetOwner::matchesResource(const EnvironmentResourceReference* resource) const {
    return u32(mNameHash) == u32(resource->mObject->mNameHash);
}
/* 0276EE20 */
u32 EnvironmentSetParameterObject::supportsParameters() const { return 1; }
/* 0276EE28 */
void EnvironmentSetParameterObject::beforeResourceRead() { }
/* 0276EE2C */
u32 EnvironmentSetParameterObject::matchesClassResource(const EnvironmentBlendListReference* resource) const {
    return u32(mNameHash) == u32(resource->mList->mNameHash);
}
/* 0276EE48 */
void EnvironmentSetParameterObject::afterResourceRead() { }
/* 0276EE4C */
void EnvironmentSetParameterObject::onUpdateComplete() { }

/* WWHD glue
*/

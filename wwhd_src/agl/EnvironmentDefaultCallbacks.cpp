// Default callback bodies and constant acceptance queries from the local method tables.
#include "agl/EnvironmentManager.h"
using namespace agl::env;

/* 0276D890 */
u32 Projector::supportsParameters() const { return 1; }
/* 0276D898 */
u32 Projector::acceptsResourceLists() const { return 1; }
/* 0276D8A0 */
void Projector::beforeResourceRead() {  }
/* 0276D8C0 */
void Projector::onInitialize(void*,void*) {  }
/* 0276D8C4 */
void Projector::onMatricesUpdated(const sead::RenderMatrix34*,const sead::RenderMatrix44*,u32) {  }
/* 0276D8C8 */
void Projector::onRefresh() {  }
/* 0276F290 */
u32 Fog::supportsParameters() const { return 1; }
/* 0276F298 */
u32 Fog::acceptsResourceLists() const { return 1; }
/* 0276F2A0 */
void Fog::beforeResourceRead() {  }
/* 0276F2C0 */
void Fog::onInitialize(void*,void*) {  }
/* 0276F2C4 */
void Fog::onReset() {  }
/* 0276F2C8 */
void Fog::onMatricesUpdated(const sead::RenderMatrix34*,const sead::RenderMatrix44*,u32) {  }
/* 0276F2CC */
void Fog::onRefresh() {  }
/* 0276CD9C */
u32 EnvironmentClassIcon::supportsParameters() const { return 1; }
/* 0276CDA4 */
void EnvironmentClassIcon::onRefresh() {  }
/* 0276CDC4 */
void EnvironmentClassIcon::beforeResourceRead() {  }
/* 0276CDC8 */
void EnvironmentClassIcon::afterResourceRead() {  }
/* 0276CDCC */
void EnvironmentClassIcon::onUpdateComplete() {  }
/* 0276CDA8 */
u32 EnvironmentClassIcon::matchesClassResource(const EnvironmentBlendListReference* resource) const {
    return u32(mNameHash) == u32(resource->mList->mNameHash);
}

/* WWHD glue
*/

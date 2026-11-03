/* Glue-only source of the SSAO/shadow-map unit: the allocation used by the inlined parameter construction
 * (agl/ParameterConstruction.h) and the bindings of the merged agl/sead methods this unit calls (their definitions
 * live in other units' sources, which the unit's specs do not link). No functions here. */
#include "agl/ParameterConstruction.h"
#include "agl/SsaoTargetSet.h"
#include "agl/EffectShaderViews.h"
#include "agl/ShaderContext.h"
#include "agl/BitmapTransfer.h"
#include "sead/RenderState.h"
#include "sead/DisplayTargets.h"
#include "sead/GraphicsRuntime.h"
// d_snap-TU helpers that attach a cache texture (or one slice/mip of it) to a colour surface.
void attachColorTexture(agl::ColorSurface* color, const agl::TextureData* texture);
void attachColorSlice(agl::ColorSurface* color, const agl::TextureData* texture, u32 mip, u32 slice);
/* WWHD glue
ext 0273AD10 void* allocateEffectParameter(u32)
ext 02755FEC sead::RegistryHeap* findRegistryHeap(void*,void*)
ext 027BE6B8 agl::TextureData* agl::TextureData::create(agl::TextureData*) callback
ext 027BE930 void agl::TextureData::configure(u32,u32,u32,u32,u32,s32,u32,u32)
ext 027BE794 void agl::TextureData::invalidate() const
ext 027BDF7C agl::TextureSampler* agl::TextureSampler::create(agl::TextureSampler*) callback
ext 027BE2B0 void agl::TextureSampler::destroy(agl::TextureSampler*,u32) callback
ext 027BDEB4 void agl::TextureSampler::copyTexture(const agl::TextureData*)
ext 027B6CB8 agl::ColorSurface* agl::ColorSurface::construct()
ext 027B6D64 void agl::ColorSurface::invalidate()
ext 027B6E64 agl::DepthSurface* agl::DepthSurface::construct()
ext 027B6F18 void agl::DepthSurface::invalidate()
ext 027B6F90 void agl::DepthSurface::initializeRegisters()
ext 027B95B4 agl::RenderBuffer* agl::RenderBuffer::construct()
ext 027B954C void agl::RenderBuffer::clearColors()
ext 027B9694 void agl::RenderBuffer::destroy(u32)
ext 0279E674 agl::ParameterBase* agl::ParameterBase::create(agl::ParameterBase*)
ext 0279E618 void agl::ParameterBase::initialize(sead::SafeText*,sead::SafeText*,sead::SafeText*,agl::parameter_tree::Value*)
ext 027A84B8 agl::ParameterObject* agl::ParameterObject::create(agl::ParameterObject*)
ext 0279A07C agl::ParameterRoot* agl::ParameterRoot::create(agl::ParameterRoot*,sead::SafeText*,u32)
ext 0279CB9C void agl::parameter_tree::Node::appendValue(agl::parameter_tree::Value*,sead::SafeText*)
ext 0276B114 void agl::ShaderContext::applyContext()
ext 027BB7CC void agl::ShaderProgram::bindStagePrograms()
ext 027B9F68 void agl::setProgramShaderMode(u32)
ext 027A03A0 agl::ResourceRecord* agl::ResourceRecord::create(agl::ResourceRecord*)
ext 027A0998 void agl::ResourceRecord::detach()
ext 02752BEC void sead::RegisteredObject::destroy(u32)
ext 027BFE5C void agl::VertexInputSet::bind()
ext 027BB9E0 void refreshEffectProgram(agl::ShaderProgram*,u32)
ext 025C0928 void attachColorTexture(agl::ColorSurface*,const agl::TextureData*)
ext 025C0848 void attachColorSlice(agl::ColorSurface*,const agl::TextureData*,u32,u32)
ext 027B6030 void agl::replaceBitmapMip(agl::SurfaceData_l*,const void*,u32)
ext 0279FC6C agl::TextureCacheEntry* agl::DynamicTextureCache::acquire2D(sead::SafeText*,u32,u32,u32,u32,u32,u32,u8,u8)
ext 0279FD18 agl::TextureCacheEntry* agl::DynamicTextureCache::acquire2DArray(sead::SafeText*,u32,u32,u32,u32,u32,u32,u32,u8,u8)
ext 0279FDC8 agl::TextureCacheEntry* agl::DynamicTextureCache::acquireShared2D(sead::SafeText*,u32,u32,u32,u32,u32,u32,u8,u8)
ext 0279FE74 agl::TextureCacheEntry* agl::DynamicTextureCache::acquireShared2DArray(sead::SafeText*,u32,u32,u32,u32,u32,u32,u32,u8,u8)
ext 027A0040 void agl::DynamicTextureCache::release(agl::TextureCacheEntry*)
ext 027A0114 void agl::DynamicTextureCache::releaseShared(agl::TextureCacheEntry*)
ext 0274FDBC sead::CompactRenderState* sead::CompactRenderState::create(sead::CompactRenderState*)
ext 0274FEB0 void sead::CompactRenderState::apply()
ext 0274D690 void sead::activateFrameBuffer(sead::DisplayFrame*)
ext 0274F424 sead::Viewport* sead::Viewport::createRect(sead::Viewport*,f32,f32,f32,f32)
ext 0274F5F0 sead::Viewport* sead::Viewport::createSurface(sead::Viewport*,const sead::RenderVector2*)
ext 0274F964 void sead::Viewport::apply(const sead::RenderSurface*) const
ext 0274FA70 void sead::Viewport::applyScissor(const sead::RenderSurface*) const
*/

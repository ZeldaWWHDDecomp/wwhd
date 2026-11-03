// The merged agl/sead methods this unit calls, bound under their canonical names (method-form glue).
// Glue only: the bodies live in the owning units' sources.
#include "agl/ShadowPrePass.h"
#include "agl/ShadowShaderSetup.h"
#include "agl/ShadowControlInstances.h"
#include "agl/ProjectionShadow.h"
#include "agl/DynamicTextureCache.h"
#include "agl/RootNode.h"
#include "agl/VertexInput.h"
#include "sead/RenderState.h"
#include "sead/ViewTransform.h"
#include "sead/GraphicsRuntime.h"
#include "bindings.h"

/* WWHD glue
ext 0279FC6C agl::TextureCacheEntry* agl::DynamicTextureCache::acquire2D(sead::SafeText*,u32,u32,u32,u32,u32,u32,u8,u8)
ext 027A0040 void agl::DynamicTextureCache::release(agl::TextureCacheEntry*)
ext 027BE794 void agl::TextureData::invalidate() const
ext 027BDF7C agl::TextureSampler* agl::TextureSampler::create(agl::TextureSampler*) callback
ext 027BE2B0 void agl::TextureSampler::destroy(agl::TextureSampler*,u32) callback
ext 027BDEB4 void agl::TextureSampler::copyTexture(const agl::TextureData*)
ext 027B95B4 agl::RenderBuffer* agl::RenderBuffer::construct()
ext 027B9694 void agl::RenderBuffer::destroy(u32)
ext 027B6CB8 agl::ColorSurface* agl::ColorSurface::construct()
ext 027B6D64 void agl::ColorSurface::invalidate()
ext 027B6E64 agl::DepthSurface* agl::DepthSurface::construct()
ext 027BEB5C agl::UniformBuffer* agl::UniformBuffer::create(agl::UniformBuffer*)
ext 027BEC90 void agl::UniformBuffer::destroy(agl::UniformBuffer*,u32)
ext 027BECEC void agl::UniformBuffer::allocateFormat(u32,void*)
ext 027BED84 void agl::UniformBuffer::appendField(u32,u32)
ext 027BEEE4 void agl::UniformBuffer::allocateStorage(void*,u32)
ext 027BEE9C void agl::UniformBuffer::adoptFormat(const agl::UniformBuffer*)
ext 027BF048 void agl::UniformBuffer::writeWords(void*,u32,const be<u32>*,u32,s32) const
ext 027BEFE4 void agl::UniformBuffer::flushNoSync(void*,u32)
ext 027A03A0 agl::ResourceRecord* agl::ResourceRecord::create(agl::ResourceRecord*)
ext 027A0998 void agl::ResourceRecord::detach()
ext 02752BEC void sead::RegisteredObject::destroy(u32)
ext 0279A07C agl::ParameterRoot* agl::ParameterRoot::create(agl::ParameterRoot*,sead::SafeText*,u32)
ext 027A84B8 agl::ParameterObject* agl::ParameterObject::create(agl::ParameterObject*)
ext 0279E674 agl::ParameterBase* agl::ParameterBase::create(agl::ParameterBase*)
ext 0279E618 void agl::ParameterBase::initialize(sead::SafeText*,sead::SafeText*,sead::SafeText*,agl::parameter_tree::Value*)
ext 0279CB9C void agl::parameter_tree::Node::appendValue(agl::parameter_tree::Value*,sead::SafeText*)
ext 02785CB4 void agl::detail::RootNode::setNodeMeta(void*,const sead::SafeText*)
ext 027BB7CC void agl::ShaderProgram::bindStagePrograms()
ext 027B9F68 void agl::setProgramShaderMode(u32)
ext 027BFE5C void agl::VertexInputSet::bind()
ext 0274FDBC sead::CompactRenderState* sead::CompactRenderState::create(sead::CompactRenderState*)
ext 0274FEB0 void sead::CompactRenderState::apply()
ext 0274F5F0 sead::Viewport* sead::Viewport::createSurface(sead::Viewport*,const sead::RenderVector2*)
ext 0274F964 void sead::Viewport::apply(const sead::RenderSurface*) const
ext 0274D690 void sead::activateFrameBuffer(sead::DisplayFrame*)
ext 0274CCB0 void sead::destroyViewTransform(sead::ViewTransform*,u32)
*/

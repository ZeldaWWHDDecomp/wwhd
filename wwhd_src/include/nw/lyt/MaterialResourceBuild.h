/* Layout material-resource records and texture-provider view. */
#pragma once
#include "nw/lyt/MaterialTransformUpload.h"
#include "nw/lyt/MaterialGpuCopy.h"
namespace nw { namespace lyt {
struct MaterialResourceHeader { be<u8> name[28]; be<u32> colors[2],flags; };
struct MaterialTextureReference { be<u16> index; be<u8> horizontal,vertical; };
struct MaterialTextureNames { u8 prefix[0xC]; be<u32> offsets[1]; };
struct MaterialTextureProviderMethods { u8 prefix[0x2C]; be<u32> load; };
struct MaterialTextureProvider { gptr<MaterialTextureProviderMethods> methods; };
struct MaterialResourceSet {
    gptr<MaterialTextureNames> textures; gptr<void> fonts,materials;
    gptr<MaterialTextureProvider> provider;
};
WWHD_SIZE(MaterialResourceHeader,0x28); WWHD_SIZE(MaterialTextureReference,4);
} }
void materialResourceZero(void*,u32);
void materialResourceTextureRegisters(nw::lyt::TextureObject*);

// Data-only owner of the dynamic texture cache singleton and its registered disposer (declared in
// agl/DynamicTextureCache.h); AGL#7/#8/#9 and the merged capture/bootstrap units read these words.
#include "agl/DynamicTextureCache.h"
static agl::DynamicTextureCache* sDynamicTextureCache;
static agl::DynamicTextureCacheDisposer* sDynamicTextureCacheDisposer;
/* WWHD glue
data sDynamicTextureCache 101F8DD4
data sDynamicTextureCacheDisposer 101F8DD8
*/

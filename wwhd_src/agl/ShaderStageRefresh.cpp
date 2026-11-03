// Shader stage text refresh: when the stage has a variation layout and its program asks for it (027BB9E0 passes the
// program's flag bit 0), the stage text is generated into the library's shader text and written out through the
// capture file provider under the stage's temporary file name.
#include "agl/ShaderStageState.h"
#include "agl/RenderInputCollections.h"
#include "agl/PrivateResource.h"
#include "agl/CaptureFileOptions.h"
#include "agl/CaptureProvider.h"
using namespace agl::render;
typedef gptr<agl::detail::PrivateResource> PrivateResourcePointer;
typedef gptr<agl::CaptureProvider> CaptureProviderPointer;
static PrivateResourcePointer sPrivateResource_l;
static CaptureProviderPointer sCaptureProvider_l;
// per stage kind: the temporary file the stage text is written to
static const char* sStageTemporaryFiles_l[3] = {
    "%AGL_ROOT%/tools/temporary/temp_vs.sh",
    "%AGL_ROOT%/tools/temporary/temp_fs.sh",
    "%AGL_ROOT%/tools/temporary/temp_gs.sh",
};
static u32 stageKind(ShaderStageState* stage) {
    return gabi::call_ptr<u32>(stage->mMethods->mGetKind.mFunction,stage);
}
/* 027D4D30 */
u32 agl::render::ShaderStageState::refreshText(u32 enabled) {
    gabi::AutoFrame frame{gabi::current_function()};
    ShaderInputLayout* variations = mVariations;
    u32 result = 2;
    if (variations && enabled) {
        u32 getKind = mMethods->mGetKind.mFunction;
        sead::SafeTextBuffer* text = sPrivateResource_l->mShaderText;
        u32 kind = gabi::call_ptr<u32>(getKind,this);
        generateShaderStageText(variations,kind,text,1,1);
        agl::CaptureProvider* provider = sCaptureProvider_l;
        gabi::FrameLocal<agl::CaptureFileOptions> options(8);
        agl::constructCaptureFileOptions(options.get());
        options->mPath.mText.v = gabi::ea(sStageTemporaryFiles_l[stageKind(this)]);
        text->assureTermination();
        u32 terminate = text->mVtable->mAssureTermination.mFunction;
        be<u8>* data = text->mText;
        gabi::call_ptr<void>(terminate,text);
        const be<u8>* characters = text->mText;
        s32 length = 0;
        while (characters[length] != 0) {
            if (++length > sead::kMaxTextLength) { length = 0; break; }
        }
        provider->writeCaptureFile(data,length,options.get());
        result = 0;
    }
    if (mCode) result = 0;
    return result;
}
/* WWHD glue
data sPrivateResource_l 101F8C14
data sCaptureProvider_l 101F95F8
data sStageTemporaryFiles_l 101F96BC
ext 027C57E8 agl::CaptureFileOptions* agl::constructCaptureFileOptions(agl::CaptureFileOptions*)
ext 027C5474 u32 agl::CaptureProvider::writeCaptureFile(const void*,u32,agl::CaptureFileOptions*)
*/

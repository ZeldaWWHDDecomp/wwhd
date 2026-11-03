// Shader stage text: the stage source with the library's version line, the AGL target macros and the variation's
// #define values, written into the library's shader text (027D4D30) and copied to the layout's output text.
#include "agl/ShaderStageState.h"
#include "agl/RenderInputCollections.h"
#include "agl/RenderStorageCopies.h"
#include "agl/PrivateResource.h"
#include "agl/CaptureTextTools.h"
using namespace agl::render;
// The shader TU's plain sead text: vtable 1016B360, whose termination hook 027D4C9C does nothing.
typedef shader_input_text::Text StageText;
static sead::SafeTextVtable sStageTextMethods_l;
typedef gptr<agl::detail::PrivateResource> PrivateResourcePointer;
// per output mode (the only caller passes mode 1, GX2): the version line, the stage macros by stage kind and the target
// macros
static const char* sVersionLines_l[3] = {
    "#version 400\n#extension GL_ARB_texture_cube_map_array : enable\n#extension GL_ARB_shading_language_420pack : enable\n",
    "#version 330\n#extension GL_ARB_texture_cube_map_array : enable\n",
    "#version 100\n",
};
static const char* sStageMacros_l[9] = {
    "#define AGL_VERTEX_SHADER\n#define AGL_VARYING out\n",
    "#define AGL_FRAGMENT_SHADER\n#define AGL_VARYING in\n",
    "#define AGL_GEOMETRY_SHADER \n",
    "#define AGL_VERTEX_SHADER\n#define AGL_VARYING out\n",
    "#define AGL_FRAGMENT_SHADER\n#define AGL_VARYING in\n",
    "#define AGL_GEOMETRY_SHADER\n",
    "#define AGL_VERTEX_SHADER\n#define AGL_VARYING varying\nprecision highp float;\nprecision highp int;\nvec4 textureLod( sampler2D   sampler, vec2 coord, float lod ) { return texture2D( sampler, coord ); }\nvec4 textureLod( samplerCube sampler, vec3 coord, float lod ) { return textureCube( sampler, coord.xyz ); }\nvec4 textureLod( samplerCube sampler, vec4 coord, float lod ) { return textureCube( sampler, coord.xyz ); }\n",
    "#define AGL_FRAGMENT_SHADER\n#define AGL_VARYING varying\nprecision highp float;\nprecision highp int;\nvec4 textureLod( sampler2D   sampler, vec2 coord, float lod ) { return texture2D( sampler, coord, lod ); }\nvec4 textureLod( samplerCube sampler, vec3 coord, float lod ) { return textureCube( sampler, coord.xyz, lod ); }\nvec4 textureLod( samplerCube sampler, vec4 coord, float lod ) { return textureCube( sampler, coord.xyz, lod ); }\n#define AGL_GEOMETRY_SHADER\n",
    0,
};
static const char* sTargetMacros_l[3] = {
    "#define AGL_SHADER\n#define AGL_TARGET_GL\n#define AGL_ATTRIBUTE( loc )      layout( location = loc ) in\n#define AGL_UNIFORM_BLOCK( name ) layout( std140 ) uniform name\n",
    "#define AGL_SHADER\n#define AGL_TARGET_GX2\n#define AGL_ATTRIBUTE( loc )      layout( location = loc ) in\n#define AGL_UNIFORM_BLOCK( name ) layout( std140 ) uniform name\n",
    "#define AGL_SHADER\n#define AGL_TARGET_GL\n#define AGL_TARGET_GLES2\n#define AGL_UNIFORM_BLOCK( name ) \n#define AGL_ATTRIBUTE( loc ) attribute\n#define sampler2DArray   sampler2D\n#define sampler3D        sampler2D\n#define samplerCubeArray samplerCube\n#define sampler2DMS      sampler2D\n#define gl_VertexID      0\n#define gl_InstanceID    0\n#define gl_FragDepth     float temp_gl_frag_depth\nvec4 texture( sampler2D   sampler, vec2 coord )               { return texture2D( sampler, coord );    }\nvec4 texture( sampler2D   sampler, vec3 coord )               { return texture2D( sampler, coord.xy ); }\nvec4 texture( sampler2D   sampler, vec4 coord )               { return texture2D( sampler, coord.xy ); }\nvec4 texture( samplerCube sampler, vec3 coord )               { return textureCube( sampler, coord );  }\nvec4 texture( samplerCube sampler, vec4 coord )               { return textureCube( sampler, coord.xyz ); }\nvec2 textureSize( sampler3D sampler )                         { return vec2( 1.0 ); }\nvec4 textureGather( sampler2D sampler, vec2 coord )           { return texture2D( sampler, coord ); }\nvec4 textureGather( sampler2D sampler, vec3 coord )           { return texture2D( sampler, coord.xy ); }\nvec4 texelFetch( sampler3D sampler, ivec2 c, int i )          { return texture2D( sampler, vec2( c.x, c.y ) ); }\n",
};
static u8 sMissingLineText_l[1] = {0};    // the empty text used when the version line's end lies outside the text
static PrivateResourcePointer sPrivateResource_l;
using agl::capture_text::findLineEnding;
using agl::capture_text::replaceDefineValues;
void* copyStageText(void* destination,const void* source,u32 size,u32 flags);

/* the inline sead text length: 0 for an empty text or one longer than kMaxTextLength */
static s32 textLength(const be<u8>* text) {
    s32 length = 0;
    while (text[length] != 0) {
        if (++length > sead::kMaxTextLength) return 0;
    }
    return length;
}
static void setStageText(StageText* text,const be<u8>* characters) {
    text->mVtable = &sStageTextMethods_l;
    text->mText = const_cast<be<u8>*>(characters);
}
static void setStageText(StageText* text,const char* characters) {
    text->mVtable = &sStageTextMethods_l;
    text->mText.v = gabi::guest_addr(characters);
}
/* sead::BufferedSafeString::copy after the caller's first termination of the source: the source (clipped to the
   destination's capacity) replaces the text in `buffer` (the destination's text, read before that call) */
static void copyText(sead::SafeTextBuffer* destination,sead::SafeText* source,be<u8>* buffer) {
    s32 length = textLength(source->mText);
    s32 capacity = destination->mCapacity;
    u32 terminate = source->mVtable->mAssureTermination.mFunction;
    if (length >= capacity) length = capacity - 1;
    gabi::call_ptr<void>(terminate,source);
    copyStageText(buffer,source->mText,length,0);
    buffer[length] = 0;
}
/* sead::BufferedSafeString::append: the source, clipped to the room left, goes after the current text */
static void appendText(sead::SafeTextBuffer* destination,sead::SafeText* source) {
    be<u8>* buffer = destination->mText;
    destination->assureTermination();
    s32 oldLength = textLength(destination->mText);
    s32 offset = oldLength < 0 ? 0 : oldLength;
    source->assureTermination();
    s32 length = textLength(source->mText);
    s32 room = (s32)destination->mCapacity - offset;
    if (length >= room) length = room - 1;
    if (length > 0) {
        be<u8>* target = buffer + offset;
        source->assureTermination();
        copyStageText(target,source->mText,length,0);
        s32 end = offset + length;
        if (end > oldLength) buffer[end] = 0;
    }
}
/* compare of `count` leading characters (equal when both texts are the same object) */
static bool equalPrefix(sead::SafeText* candidate,sead::SafeText* marker,s32 count) {
    candidate->assureTermination();
    const be<u8>* first = candidate->mText;
    marker->assureTermination();
    const be<u8>* second = marker->mText;
    if (first == second || count <= 0) return true;
    for (s32 remaining = count; remaining; --remaining, ++first, ++second) {
        u8 a = *first;
        if (a == 0) return *second == 0;
        u8 b = *second;
        if (b == 0 || a != b) return false;
    }
    return true;
}
/* 027D4028 */
void generateShaderStageText(agl::render::ShaderInputLayout* layout,u32 kind,sead::SafeTextBuffer* destination,u32 mode,
                             u32 unused) {
    gabi::AutoFrame frame{gabi::current_function()};
    (void)unused;
    gabi::FrameLocal<StageText> scratch(8);
    setStageText(scratch.get(),"");
    be<u8>* buffer = destination->mText;
    scratch->assureTerminationImpl();
    copyText(destination,scratch.get(),buffer);
    sead::SafeText* source = layout->mCompiledSource;
    if (!source) return;
    if (mode == 3) mode = 1;
    // Find "#version" in the source and drop everything up to the end of its line.
    gabi::FrameLocal<StageText> remainder(0x18);
    setStageText(remainder.get(),source->mText);
    setStageText(scratch.get(),"#version");
    remainder->assureTerminationImpl();
    s32 sourceLength = textLength(remainder->mText);
    scratch->assureTermination();
    s32 markerLength = textLength(scratch->mText);
    s32 last = sourceLength - markerLength;
    s32 position = -1;
    if (last >= 0) {
        gabi::FrameLocal<StageText> candidate(0x10);
        for (s32 index = 0; index <= last; ++index) {
            setStageText(candidate.get(),remainder->mText.get() + index);
            candidate->assureTerminationImpl();
            if (equalPrefix(candidate.get(),scratch.get(),markerLength)) { position = index; break; }
        }
    }
    if (position != -1) {
        source->assureTermination();
        gabi::FrameLocal<be<u32>> endingWidth(0x20);
        s32 line = findLineEnding(source->mText.get() + position,endingWidth.get());
        if (line != -1) {
            s32 end = position + line + (s32)*endingWidth;
            source->assureTermination();
            be<u8>* text = source->mText;
            s32 length = textLength(text);
            if (end < 0 || end > length) remainder->mText = sMissingLineText_l;
            else remainder->mText = text + end;
        }
    }
    setStageText(scratch.get(),sVersionLines_l[mode]);
    appendText(destination,scratch.get());
    setStageText(scratch.get(),"// ----- These macros are auto defined by AGL.-----\n");
    appendText(destination,scratch.get());
    setStageText(scratch.get(),sStageMacros_l[mode * 3 + kind]);
    appendText(destination,scratch.get());
    setStageText(scratch.get(),sTargetMacros_l[mode]);
    appendText(destination,scratch.get());
    setStageText(scratch.get(),"// ------------------------------------------------\n");
    appendText(destination,scratch.get());
    appendText(destination,remainder.get());
    if ((s32)layout->mNames.mCount > 0) {
        agl::detail::PrivateResource* resource = sPrivateResource_l;
        replaceDefineValues(destination,static_cast<gptr<be<u8>>*>(layout->mNames.mStorage.get()),
                            static_cast<gptr<be<u8>>*>(layout->mValues.mStorage.get()),layout->mNames.mCount,
                            reinterpret_cast<be<u8>*>(resource->mShaderScratch.mBuffer.get()),resource->mShaderScratch.mSize);
    }
    if ((s32)layout->mSelectedNames.mCount > 0) {
        agl::detail::PrivateResource* resource = sPrivateResource_l;
        replaceDefineValues(destination,static_cast<gptr<be<u8>>*>(layout->mSelectedNames.mStorage.get()),
                            static_cast<gptr<be<u8>>*>(layout->mSelectedValues.mStorage.get()),
                            layout->mSelectedNames.mCount,
                            reinterpret_cast<be<u8>*>(resource->mShaderScratch.mBuffer.get()),
                            resource->mShaderScratch.mSize);
    }
    sead::SafeTextBuffer* output = layout->mOutputText;
    if (output) {
        be<u8>* outputBuffer = output->mText;
        destination->assureTermination();
        copyText(output,destination,outputBuffer);
    }
}
/* WWHD glue
data sStageTextMethods_l 1016B360
data sVersionLines_l 101F96B0
data sStageMacros_l 101F968C
data sTargetMacros_l 101F967C
data sMissingLineText_l 10000160
data sPrivateResource_l 101F8C14
str 1016BF6C
str 1016BF70
str 1016BF7C
str 1016BFB4
ext 027C5AAC s32 agl::capture_text::findLineEnding(const be<u8>*,be<u32>*)
ext 027C5B20 void agl::capture_text::replaceDefineValues(sead::SafeTextBuffer*,const gptr<be<u8>>*,const gptr<be<u8>>*,s32,be<u8>*,u32)
ext C0009988 void* copyStageText(void*,const void*,u32,u32)
*/

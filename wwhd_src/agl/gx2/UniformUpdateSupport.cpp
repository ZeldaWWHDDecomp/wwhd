/* Material uniform updates reconstructed from the assigned WWHD bodies. */
#include "agl/gx2/UniformUpdateSupport.h"
using namespace uniform_update_support;
using agl_gx2_support::UniformMember;
namespace uniform_update_support {
template<class T> static T* relativeField(const be<s32>* member) {
    u32 offset = *member;
    return offset ? gabi::at<T>(gabi::ea(member) + offset) : nullptr;
}
/* 027E64B4 */
void MaterialUniforms_l::writeSwapped(be<u8>* destination,be<u32>* dirty) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    UniformResource_l* initialResource = mResource;
    const be<u32>* defaults = relativeField<be<u32>>(&initialResource->mDefaultMaskRelative);
    u32 count = initialResource->mConversionCount;
    u32 wordCount = (count + 31) >> 5;
    gabi::FrameLocal<ConvertedUniform> converted(8);
    for (u32 word = 0; word < wordCount; ++word) {
        u32 requested = dirty[word];
        u32 persistent = defaults[word];
        u32 active = requested | persistent;
        while (active) {
            u32 bit = 31 - __builtin_clz(active);
            UniformResource_l* resource = mResource;
            UniformMember* members = relativeField<UniformMember>(&resource->mConversionsRelative);
            UniformMember* member = members + word * 32 + bit;
            const be<u8>* sourceBase = mSource;
            void* context = mConversionContext;
            u32 destinationOffset = member->mDestinationOffset;
            u32 converter = member->mConverter;
            u32 sourceOffset = member->mSourceOffset;
            be<u8>* target = destination + destinationOffset;
            const be<u8>* source = sourceBase + sourceOffset;
            if (converter) {
                u32 encoding = member->mEncoding;
                u32 bytes = gabi::call_ptr<u32>(converter,converted.get(),source,encoding,context);
                u32 rounded = (bytes + 3) & ~3u;
                for (u32 byte = 0; byte < rounded; byte += 4) {
                    u32 value = converted->mWords[byte >> 2];
                    reinterpret_cast<be<u32>*>(target)[byte >> 2] = __builtin_bswap32(value);
                }
            } else {
                member->copySwapped(reinterpret_cast<be<u32>*>(target),reinterpret_cast<const be<u32>*>(source));
            }
            active ^= 1u << bit;
        }
        dirty[word] = 0;
    }
}
/* 027E66B4 */
void MaterialUniforms_l::writeNative(be<u8>* destination,be<u32>* dirty) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    UniformResource_l* initialResource = mResource;
    const be<u32>* defaults = relativeField<be<u32>>(&initialResource->mDefaultMaskRelative);
    u32 count = initialResource->mConversionCount;
    u32 wordCount = (count + 31) >> 5;
    gabi::FrameLocal<ConvertedUniform> converted(8);
    for (u32 word = 0; word < wordCount; ++word) {
        u32 requested = dirty[word];
        u32 persistent = defaults[word];
        u32 active = requested | persistent;
        while (active) {
            u32 bit = 31 - __builtin_clz(active);
            UniformResource_l* resource = mResource;
            UniformMember* members = relativeField<UniformMember>(&resource->mConversionsRelative);
            UniformMember* member = members + word * 32 + bit;
            const be<u8>* sourceBase = mSource;
            void* context = mConversionContext;
            u32 destinationOffset = member->mDestinationOffset;
            u32 converter = member->mConverter;
            u32 sourceOffset = member->mSourceOffset;
            be<u8>* target = destination + destinationOffset;
            const be<u8>* source = sourceBase + sourceOffset;
            if (converter) {
                u32 encoding = member->mEncoding;
                u32 bytes = gabi::call_ptr<u32>(converter,converted.get(),source,encoding,context);
                u32 rounded = (bytes + 3) & ~3u;
                uniformOutputCopy(target,converted.get(),rounded);
            } else {
                member->copyNative(reinterpret_cast<be<u32>*>(target),reinterpret_cast<const be<u32>*>(source));
            }
            active ^= 1u << bit;
        }
        dirty[word] = 0;
    }
}
}
/* WWHD glue
ext C000A848 void uniformOutputCopy(void*,const void*,u32)
*/

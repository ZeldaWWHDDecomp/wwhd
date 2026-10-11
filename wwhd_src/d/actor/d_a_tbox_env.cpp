/** Treasure chest environment texture replacement (HD).
 * GameCube daTbox_c::envShapeSet names and flow, preserving the verified HD graphics path. */
#include "d/actor/d_a_tbox_env.h"

static TboxControlPtr resourceControl;
static TboxControlPtr graphics;

/* 024B4858 */
static s32 envShapeSet(daTbox_c* self) {
    // HD: partially initialized image bytes and relative offsets escape from this exact frame.
    gabi::Local<TboxTextureFrame> frame;
    TboxModelInfo* mdlInfo = Tbox_getModelInfo(self);
    s32 modelId = mdlInfo->modelId;
    frame->name.vtable = 0x1003FDC4;
    void* control = resourceControl;
    frame->name.text = gabi::guest_addr("Dalways");
    TboxModelData* modelData = Tbox_getRes(control, &frame->name, modelId);
    TboxTexture* mdlTex = modelData->mpTexture;
    if (mdlTex == nullptr)
        return cPhs_ERROR_e;
    JUTNameTab* texNameTable = modelData->mpTextureName;
    if (texNameTable == nullptr)
        return cPhs_ERROR_e;

    for (u16 i = 0; i < mdlTex->mNum; i++) {
        const char* texName = Tbox_getTextureName(texNameTable, i);
        if (texName == nullptr) {
            // HD: release code retains this assertion; HD text is "name != (0)".
            JUTAssertion::showAssert("d_a_tbox.cpp", 0x183, "texName != NULL");
        } else {
            // Compare guest bytes without a library call; preserve short-circuit read order.
            const char* expected = gabi::at<char>(gabi::guest_addr("__dummy"));
            u8 a, b;
            do {
                a = *reinterpret_cast<const be<u8>*>(texName++);
                b = *reinterpret_cast<const be<u8>*>(expected++);
            } while (a == b && a);
            if (a == b) {
                // HD: the framebuffer image is wrapped in a newly allocated graphics texture.
                void* image = Tbox_getGraphicsImage(graphics, 6);
                void* allocation = Tbox_allocateTexture(192);
                TboxGraphicsTexture* object = Tbox_constructTexture(allocation, image);
                TboxResTIMG& local = frame->image.image;
                local.mpTexture = object;
                local.width = object->width;
                local.height = object->height;
                local.indexTexture = 0;

                TboxImageWords* dest = mdlTex->mpRes.get() + i;
                // GHS copies three words per group; source and destination may alias.
                for (int j = 0; j < 3; j++) {
                    u32 x = frame->image.words[j * 3];
                    u32 y = frame->image.words[j * 3 + 1];
                    u32 z = frame->image.words[j * 3 + 2];
                    dest->words[j * 3] = x;
                    dest->words[j * 3 + 1] = y;
                    dest->words[j * 3 + 2] = z;
                }
                // Reload the destination each time: the preceding stores can alias its pointer.
                dest = mdlTex->mpRes.get() + i;
                dest->image.imageOffset =
                    dest->image.imageOffset + gabi::ea(&local) - gabi::ea(dest);
                dest = mdlTex->mpRes.get() + i;
                dest->image.paletteOffset =
                    dest->image.paletteOffset + gabi::ea(&local) - gabi::ea(dest);
                dest = mdlTex->mpRes.get() + i;
                dest->image.mpTexture = local.mpTexture;

                u32 hi = mdlTex->mUseMask[0], lo = mdlTex->mUseMask[1];
                u32 n = i < 64 ? i : i - 64;
                u32 half = i < 64 ? 0 : 1;
                u32 upperUsed = mdlTex->mUsed[half][0], lowerUsed = mdlTex->mUsed[half][1];
                u32 upper =
                    tboxShiftLeft(hi, n) | tboxShiftLeft(lo, n + 32) | tboxShiftRight(lo, 32 - n);
                u32 lower = tboxShiftLeft(lo, n);
                // HD: the upper range stores the low word first.
                if (i < 64) {
                    mdlTex->mUsed[half][0] = upperUsed | upper;
                    mdlTex->mUsed[half][1] = lowerUsed | lower;
                } else {
                    mdlTex->mUsed[half][1] = lowerUsed | lower;
                    mdlTex->mUsed[half][0] = upperUsed | upper;
                }
            }
        }
    }
    // HD: no final mDoExt_modelTexturePatch call.
    return cPhs_COMPLEATE_e;
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
str 1003FEC4
str 1003FECC
str 1003FED4 "texName != NULL" renamed
str 1003FEE0
 data resourceControl 101F4F28
 data graphics 101F9968
ext 024B432C TboxModelInfo* Tbox_getModelInfo(daTbox_c*)
ext 026066C4 TboxModelData* Tbox_getRes(void*, TboxName*, s32)
ext 027ED1F0 const char* Tbox_getTextureName(JUTNameTab*, u16)
ext 027F81A4 void* Tbox_getGraphicsImage(void*, s32)
ext 0273AD10 void* Tbox_allocateTexture(u32)
ext 02773680 TboxGraphicsTexture* Tbox_constructTexture(void*, void*)
*/
// clang-format on

// J3D material descriptions (the port's native renderer, gx2/native_j3d_desc.h; port switch WWHD_NATIVE_J3D_DESC).
// Game build with GABI_NATIVE_DRAW only (PORT_NATIVE_DRAW); every other build compiles none of this.
//
// One hook in j3d::ShapePacket::submitShape, right after its program choice:
//     if (j3ddesc::submit(this, input, frame, program, directProgram, binding, bindingIndex)) return;
// j3ddesc::submit (J3DNativeDesc.cpp) reads the same objects the site's bind calls read and hands the port:
//   - the description key (DescKey + its variable part, below): compared on the game thread with the one the
//     description was defined with; on a difference (or a new key) the description is defined again: the site's own
//     bind callees run into the port's capture (segments program, material, state; never emitted);
//   - the env key (EnvKey): once per game frame per (input, frame record, pass, view) and on every change, the
//     frame-texture binds (and, pass 0, the projection's render state) are captured the same way;
//   - the mesh (IndexedMesh::draw's arguments, computed as it computes them) and the draw's uniform blocks (view,
//     model matrices, shape, material, extra; stage, location, address, size as the bind calls compute them);
//   - then wwhd_j3ddesc_draw: OP_J3D_DESC_DRAW. Port mode 1 (D0): returns 0, the site makes all its calls and its
//     draw (the port verifies the description against them). Mode 2 (D1): returns 1, the site makes no GX2 call.
// Game-memory writes of the site that the description path keeps (D1; in D0 the site makes them, j3ddesc::submit makes
// only the idempotent ones itself: the shader refresh and the polygon-offset setters): sRenderContext
// mMode/mArgument (changeShapeShader), the refresh (flag bit 2), the polygon-offset setters of buffer type 1, the
// named frame's activation (before the hook: the site's own program choice).
#pragma once
#include "port_native_draw.h"
#if PORT_NATIVE_DRAW
#include "JSystem/J3DGraph/J3DShapeSubmission.h"
#include "JSystem/J3DGraph/J3DShapePacketResources.h"

namespace j3ddesc {

// DescKey: the fixed words (in this order), then the variable part:
//   stream count, per vertex stream {StridedBuffer EA, data, slice size, element stride};
//   sampler count, per active material sampler binding {key EA, sampler object EA (0: none)};
//   range count, per content range {address, bytes} (the sampler objects' GX2Texture words and the sampler keys'
//   GX2Sampler words: checked by the port once per game frame, not per draw)
enum DescKeyWord : unsigned {
    kKeyBinding = 0,     // PacketMaterialBinding_l EA
    kKeyPass,            // input pass (0, 1, 2)
    kKeyRecord,          // program record: ModelProgram_l EA (direct programs) or the frame record EA
    kKeyFrame,           // FrameRecord EA (FrameShader_l)
    kKeyProgram,         // ProgramInstance EA (material program)
    kKeyViewClass,       // 1: view 2 (passes 1/2 bind no draw state), 0: other views
    kKeyShader,          // ShaderProgram_l EA
    kKeyShaderFlags,     // its mFlags after the refresh
    kKeyRefreshed,       // 1: this draw refreshed the shader (forces a define)
    kKeyShaderState,     // ShaderState_l EA
    kKeyShaderMode,      // ShaderState_l::mProgram (the shader mode value changeShapeShader binds)
    kKeyList,            // the shader's display list EA
    kKeyListBytes,       // its size in bytes (0: bindStagePrograms instead)
    kKeyDrawState,       // 12 words: ModelMaterialDrawState_l::mState (agl render state words)
    kKeyDrawDirty = kKeyDrawState + 12,  // mDirty | buffer type << 8
    kKeyStatePart,       // 10 words: the binding's relative state, +0x00..+0x27 (pass-0 mode at +4, alpha +0xC, blend +0x18)
    kKeyProjectionFlags = kKeyStatePart + 10,  // pass 0: input projection mFlags[0..2] packed (0 otherwise)
    kKeyFetch,           // DisplayCommandState mList: fetch shader words +0x08 (size) and +0x0C (program)
    kKeyFetchProgram,
    kKeySamplerTable,    // PixelCommandState mActiveBindings: entries EA
    kKeySamplerCount,    //   and count
    kKeyFixedWords
};
static_assert(kKeyFixedWords==41,"the port's native_j3d_desc.h kKeyFixed");
// EnvKey: input EA, frame record EA, pass, view, projection EA (pass 0, else 0), the seven frame textures bindExtra
// binds (after its fallbacks: units 8, 10, 11, 9, 12, 13, 14), their sampler-slot words (7) and the textures' dirty
// bytes (7: a dirty sampler is updated by the bind, a game-memory write the env capture must make)
enum EnvKeyWord : unsigned {
    kEnvInput = 0, kEnvFrame, kEnvPass, kEnvView, kEnvProjection, kEnvTextures, kEnvSlots = kEnvTextures + 7,
    kEnvDirty = kEnvSlots + 7, kEnvKeyWords = kEnvDirty + 7
};
// the inventory reasons (native_j3d_desc.h Reason: the same values)
enum Reason : unsigned {
    kDescribed = 0, kPassMasked, kNoResource, kNamedFrame, kNoProgram, kListBinds, kForeign, kPortFeature,
    kTableFull, kPrepareOnly, kNotRecording
};

// the hook (submitShape after its program choice). Nonzero: the draw is made by the port (D1); the site returns.
unsigned submit(j3d::ShapePacket* packet, j3d::ShapeDrawInput_l* input, j3d::FrameRecord* frame, void* program,
                unsigned directProgram, j3d::PacketMaterialBinding_l* binding, unsigned bindingIndex);
}  // namespace j3ddesc
#endif

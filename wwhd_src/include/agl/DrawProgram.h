// AGL#12's draw helpers run agl::ShaderProgram records (ShaderProgram.h / ShaderProgramActivation.h) from the
// shader holder's program table, with the primitive-shape singletons' vertex inputs and index buffers.
#pragma once
#include "wwhd.h"
#include "sead/ProjectionState.h"
#include "sead/ColorChannels.h"
#include "agl/EffectSurfaceViews.h"
#include "agl/ShaderProgramActivation.h"
#include "agl/ShaderHolder.h"
#include "agl/PrimitiveShapes.h"
namespace agl {
struct DrawFloat4 { be<f32> mValues[4]; };
// The two draw shapes used here: the quad (vertex input 6 with indexed shape 0, at 101F8DDC+0x5C8 / 101F8DE4+0x180)
// and the full-screen shape (input 7 with indexed shape 1, +0x6BC / +0x31C; 027A9888).
enum : u32 { DrawQuadInput = 6, DrawFullscreenInput = 7, DrawQuadShape = 0, DrawFullscreenShape = 1 };
}

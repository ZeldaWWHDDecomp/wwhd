#pragma once
#include "f_op/f_op_actor.h"
// Engine-owned GX2 objects remain opaque; guest pointers and actor-owned arrays
// expose the actual HD layout without host pointers or compiler padding.
struct daMajuuFlagUniformBuffer_c {
  be<u32> data;
  u8 uniformObject[0x154];
  u8 attributeObject[0xF4];
  be<u32> elementCount, allocation;
};
struct daMajuuFlagUniformPairs_c {
  daMajuuFlagUniformBuffer_c buffers[4];
  be<u32> current, format, stride, reserved, flags;
  u8 ready, padding[3];
};
struct daMajuuFlagPacket_c {
  u8 packetBase[0x98];
  be<u32> programCount, materialCount, materials;
  daMajuuFlagUniformPairs_c uniforms;
  u8 indexBuffer[0x18];
  be<u32> transformCount, transforms, reservedA2C;
  u8 primaryShader[0x74];
  be<f32> primaryMatrix[12];
  u8 reservedAD4[4];
  u8 secondaryShader[0x74];
  u8 shaderUniforms[0x2F0];
  u8 textureFlag, reservedE3D[3];
  u8 primaryImage[0x90], secondaryImage[0x90];
  u8 primaryTexture[0x198], secondaryTexture[0x198];
  be<f32> drawMatrix[12];
  be<u32> lighting;
  u8 reserved12C4[0x80];
  be<f32> positions[2][21][3], normals[2][21][3], backNormals[2][21][3];
  be<f32> velocities[21][3];
  be<s16> normalRotationY, rotationX, rotationZ;
  u8 currentBuffer, flags;
};
static_assert(sizeof(daMajuuFlagUniformBuffer_c) == 0x254);
static_assert(sizeof(daMajuuFlagUniformPairs_c) == 0x968);
WWHD_OFFSET(daMajuuFlagPacket_c, programCount, 0x98);
WWHD_OFFSET(daMajuuFlagPacket_c, uniforms, 0xA4);
WWHD_OFFSET(daMajuuFlagPacket_c, transformCount, 0xA24);
WWHD_OFFSET(daMajuuFlagPacket_c, primaryMatrix, 0xAA4);
WWHD_OFFSET(daMajuuFlagPacket_c, secondaryShader, 0xAD8);
WWHD_OFFSET(daMajuuFlagPacket_c, primaryImage, 0xE40);
WWHD_OFFSET(daMajuuFlagPacket_c, primaryTexture, 0xF60);
WWHD_OFFSET(daMajuuFlagPacket_c, drawMatrix, 0x1290);
WWHD_OFFSET(daMajuuFlagPacket_c, positions, 0x1344);
WWHD_OFFSET(daMajuuFlagPacket_c, normals, 0x153C);
WWHD_OFFSET(daMajuuFlagPacket_c, velocities, 0x192C);
WWHD_OFFSET(daMajuuFlagPacket_c, currentBuffer, 0x1A2E);
struct daMajuuFlag_c : fopAc_ac_c {
  daMajuuFlagPacket_c packet;
  u8 clothPhase[8], flagPhase[8];
  u8 pad1dec[0x10];
  be<f32> flagScale;
  u8 flagType, textureType, usePlayerLighting, pad1e03;
  be<f32> matrix[12];
  be<u32> parentMatrix, parentPosition;
};
static_assert(sizeof(daMajuuFlagPacket_c) == 0x1A30);
WWHD_OFFSET(daMajuuFlag_c, packet, 0x3AC);
WWHD_OFFSET(daMajuuFlag_c, clothPhase, 0x1DDC);
WWHD_OFFSET(daMajuuFlag_c, flagPhase, 0x1DE4);
WWHD_OFFSET(daMajuuFlag_c, flagScale, 0x1DFC);
WWHD_OFFSET(daMajuuFlag_c, matrix, 0x1E04);
static_assert(sizeof(daMajuuFlag_c) == 0x1E3C);

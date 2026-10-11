// Follow-camera controls for verified HD callers.
#pragma once
#include "d/d_camera.h"
/* 02514F2C / 02514F38 / 02515280 [v d_a_bmd] */
inline void dCamera_c::Stop() { gabi::call(0x02514F2C, this); }
inline void dCamera_c::Start() { gabi::call(0x02514F38, this); }
inline bool dCamera_c::SetTrimSize(s32 size) { return gabi::call<bool>(0x02515280, this, size); }
/* HD adapters for the GC Reset/Set overloads absent from the shared class declaration. */
/* 0251510C [v d_a_bmd] */
inline void dCamera_Reset(dCamera_c* camera, cXyz* center, cXyz* eye) { gabi::call(0x0251510C, camera, center, eye); }
/* 02514FE8 [v d_a_bmd] */
inline void dCamera_Set(dCamera_c* camera, cXyz* center, cXyz* eye, s16 bank, f32 fovy) { gabi::call(0x02514FE8, camera, center, eye, bank, fovy); }
/* 02514EE4 [v d_a_bmd] */
inline void dCamera_SetTypeForce(dCamera_c* camera, const char* type, s32 flags) { gabi::call(0x02514EE4, camera, type, flags); }
/* 024F8044 [v d_a_bmd] */
inline dCamera_c* dCam_getBody() { return gabi::call<dCamera_c*>(0x024F8044); }

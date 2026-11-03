// JStudio matrix helpers; Cafe retains the GameCube degree-based interface.
#pragma once
#include "sdk/mtx/vec.h"
namespace JStudio::math {
void getRotation_xyz(Mtx34*, f32, f32, f32);
void rotate_xyz(Mtx34*, const Mtx34*, f32, f32, f32);
void rotate_y(Mtx34*, const Mtx34*, f32);
void getTransformation_SRxyzT(Mtx34*, const Vec&, const Vec&, const Vec&);
void getTransformation_RyT(Mtx34*, f32, const Vec&);
}

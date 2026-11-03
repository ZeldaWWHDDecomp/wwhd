// JPA matrix decomposition, using the unchanged GameCube matrix/vector layouts.
#pragma once
#include "SSystem/SComponent/c_xyz.h"
void JPASetSVecfromMtx(const Mtx34*, cXyz&);
void JPASetRMtxSTVecfromMtx(const Mtx34*, Mtx34*, cXyz&, cXyz&);
void JPASetRMtxTVecfromMtx(const Mtx34*, Mtx34*, cXyz&);
struct JPAMathMatrixWords { be<u32> m[3][4]; };
struct JPAMathVectorWords { be<u32> x, y, z; };
WWHD_SIZE(JPAMathMatrixWords, 0x30);
WWHD_SIZE(JPAMathVectorWords, 0xC);

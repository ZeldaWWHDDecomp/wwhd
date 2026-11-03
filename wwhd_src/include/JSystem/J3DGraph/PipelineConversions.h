#pragma once
#include "wwhd.h"
#include "sead/ColorChannels.h"
namespace j3d::pipeline {
typedef sead::ViewScalar Scalar;
// Scalar-layout matrix view; no camera or projection identity is implied.
typedef sead::ViewTransform MatrixRows;
struct ByteColor { be<u8> r,g,b,a; };
struct SignedColor { be<s16> r,g,b,a; };
typedef sead::ColorChannels FloatColor;
struct Conversions {
    static void copyMatrix(MatrixRows*,const MatrixRows*);
    static void expandByteColor(FloatColor*,const ByteColor*);
    static void expandSignedColor(FloatColor*,const SignedColor*);
};
}
WWHD_SIZE(j3d::pipeline::Scalar,4);
WWHD_SIZE(j3d::pipeline::MatrixRows,48);
WWHD_OFFSET(j3d::pipeline::MatrixRows,mRows,0);
WWHD_SIZE(j3d::pipeline::ByteColor,4);
WWHD_OFFSET(j3d::pipeline::ByteColor,a,3);
WWHD_SIZE(j3d::pipeline::SignedColor,8);
WWHD_OFFSET(j3d::pipeline::SignedColor,a,6);
WWHD_SIZE(j3d::pipeline::FloatColor,16);
WWHD_OFFSET(j3d::pipeline::FloatColor,g,4);
WWHD_OFFSET(j3d::pipeline::FloatColor,b,8);
WWHD_OFFSET(j3d::pipeline::FloatColor,a,12);

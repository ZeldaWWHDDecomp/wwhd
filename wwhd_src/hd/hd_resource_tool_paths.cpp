/* HD resource conversion tool paths retained in the render translation unit. Private decomp source. */
#include "hd/hd_resource_tool_paths.h"
static hdResourceToolPathsHeaderZero_c sResourceToolPathsHeaderZero;
static hdResourceToolPathsHeaderDescriptor_c sResourceToolPathsHeaderDescriptor;
static hdResourceToolPathsHeaderDescriptor_c sResourceToolPathsRotationDescriptor;
static hdResourceToolPathsHeaderDescriptor_c sResourceToolPathsPositiveRotationDescriptor;
static f32 sResourceToolPathsMinRotation;
static f32 sResourceToolPathsMaxRotation;
static u8 sResourceToolPathsRotationGuard;
static u8 sResourceToolPathsPositiveRotationGuard;
static void initializeRotationGuard(void* guard);
static void initializePositiveRotationGuard(void* guard);
static SafeString sResourceToolPathsPreBinaryConverter;
static SafeString sResourceToolPathsG3dConverter;
static u32 sResourceToolPathsSafeStringVtable;
/* 02738898 */
static void __sinit_hd_resource_tool_paths_cpp() {
    for (u32 i = 0; i < 4; ++i) sResourceToolPathsHeaderZero.mWords[i] = 0;
    __register_global_object(gabi::ea(&sResourceToolPathsHeaderDescriptor));
    sResourceToolPathsMinRotation = -3.1415927410125732f;
    sResourceToolPathsMaxRotation = 3.1415927410125732f;
    initializeRotationGuard(&sResourceToolPathsRotationGuard);
    __register_global_object(gabi::ea(&sResourceToolPathsRotationDescriptor));
    initializePositiveRotationGuard(&sResourceToolPathsPositiveRotationGuard);
    __register_global_object(gabi::ea(&sResourceToolPathsPositiveRotationDescriptor));
    sResourceToolPathsPreBinaryConverter.__vtbl = gabi::ea(&sResourceToolPathsSafeStringVtable);
    sResourceToolPathsPreBinaryConverter.mStringTop = gabi::guest_addr("%CKING_ROOT%/Tools/convert/pre_binary_convert.py");
    sResourceToolPathsG3dConverter.__vtbl = gabi::ea(&sResourceToolPathsSafeStringVtable);
    sResourceToolPathsG3dConverter.mStringTop = gabi::guest_addr("%NW4F_G3D_ROOT%/Tool/G3dTool/win64/NW4F_g3dfilter.exe");
}
/* WWHD glue
 data sResourceToolPathsSafeStringVtable 10142BB4 nocheck
 data sResourceToolPathsHeaderZero 1049FB08
 data sResourceToolPathsHeaderDescriptor 101F8A28
 data sResourceToolPathsRotationDescriptor 101F8A34
 data sResourceToolPathsPositiveRotationDescriptor 101F8A40
 data sResourceToolPathsMinRotation 1049FAFC
 data sResourceToolPathsMaxRotation 1049FB00
 data sResourceToolPathsRotationGuard 1049FB04
 data sResourceToolPathsPositiveRotationGuard 1049FB05
 ext 028ED6F8 void initializeRotationGuard(void* guard)
 ext 028EAB2C void initializePositiveRotationGuard(void* guard)
 data sResourceToolPathsPreBinaryConverter 1049FAF4
 data sResourceToolPathsG3dConverter 1049FAEC
 str 10142CB0
 str 10142C78
*/

/* Compiler-emitted geometry helpers and the adjacent HD callback adapter. */
#include "JSystem/J3DU/J3DUClipper_extra.h"
struct ClipperExtraStaticState { be<u32> words[4]; };
static ClipperExtraStaticState sStaticState;
static u8 sStaticRecord;
/* 02837F54 */
static void J3DU_invokeCallback(void*,J3DUCallbackArgument* argument) {
    J3DUCallback* callback=argument->mOwner->mCallback;
    if(callback) gabi::call_ptr<void>(callback->mVtable->invoke,callback);
}
/* 02837F74 */
static void __sinit_J3DUClipper_extra_cpp() {
    sStaticState.words[2]=0;sStaticState.words[0]=0;sStaticState.words[3]=0;sStaticState.words[1]=0;
    clipperExtraRegisterStatic(&sStaticRecord);
}
/* 02837F9C */
static Clipper::Vec* J3DUVec3__ct(Clipper::Vec* vector) {
    if(!vector) vector=static_cast<Clipper::Vec*>(clipperExtraAllocate(sizeof(Clipper::Vec)));
    return vector;
}
/* 02837FC8 */
static void J3DUVec3_destroy(Clipper::Vec*) {}
/* 02837FCC */
static void J3DUVec3_defaultInitialize(Clipper::Vec*) {}
/* 02837FD0 */
static J3DUVec2* J3DUVec2__ct(J3DUVec2* vector) {
    if(!vector) vector=static_cast<J3DUVec2*>(clipperExtraAllocate(sizeof(J3DUVec2)));
    return vector;
}
/* 02837FFC */
static void J3DUVec2_destroy(J3DUVec2*) {}
/* WWHD glue
data sStaticState 104B5FEC
data sStaticRecord 101FACFC
ext 0273AD10 void* clipperExtraAllocate(u32)
ext 028F026C void clipperExtraRegisterStatic(void*)
*/

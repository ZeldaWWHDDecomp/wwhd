// Draw-pass visitor interface with canonical emitter and particle state.
#pragma once
#include "JSystem/JParticle/JPADraw.h"
#include "JSystem/JParticle/JPAEmitter.h"
#include "JSystem/JParticle/JPA_particle.h"
namespace JPADrawExtension {
struct JPADrawExecVtable_l { u8 mPrefix[0x14]; be<u32> mExec; };
struct JPADrawExecVisitor_l {
    gptr<JPADrawExecVtable_l> __vtbl;
    void exec(JPAMathDraw::JPADrawContext* context,void* particle) { gabi::call_ptr<void>(__vtbl->mExec,this,context,particle); }
    void exec(JPAMathDraw::JPADrawContext* context) { gabi::call_ptr<void>(__vtbl->mExec,this,context); }
};
}

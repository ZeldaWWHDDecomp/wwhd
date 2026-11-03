/** JMath platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JMath/JMath.h"
static JMathStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 027ED484 */
static void __sinit_JMath_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JMathConstructRegister(&sStaticRegister);
}


/* 027ED3AC */
void JMAQuatLerp(Quaternion* a,Quaternion* b,f32 t,Quaternion* out) {
    f32 ax = a->x;
    f32 bx = b->x;
    f32 dot = gabi::fmadds(ax,bx,(f32)a->y * (f32)b->y);
    dot = gabi::fmadds((f32)a->z,(f32)b->z,dot);
    dot = gabi::fmadds((f32)a->w,(f32)b->w,dot);
    // HD: GHS fuses the interpolation and writes one component at a time.
    if (dot < 0.0) {
        out->x = gabi::fnmsubs(t,ax + bx,ax);
        f32 y = a->y; out->y = gabi::fnmsubs(t,y + (f32)b->y,y);
        f32 z = a->z; out->z = gabi::fnmsubs(t,z + (f32)b->z,z);
        f32 w = a->w; out->w = gabi::fnmsubs(t,w + (f32)b->w,w);
    } else {
        out->x = gabi::fnmsubs(t,ax - bx,ax);
        f32 y = a->y; out->y = gabi::fnmsubs(t,y - (f32)b->y,y);
        f32 z = a->z; out->z = gabi::fnmsubs(t,z - (f32)b->z,z);
        f32 w = a->w; out->w = gabi::fnmsubs(t,w - (f32)b->w,w);
    }
}

/* WWHD glue

 data sStaticBlock 104A24D0
 data sStaticRegister 101F97FC
 ext 028F026C void JMathConstructRegister(void*)
*/

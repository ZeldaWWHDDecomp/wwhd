/* Orthographic 2D graphics context, following tww J2DOrthoGraph. */
#include "JSystem/J2DGraph/J2DOrthoGraph.h"

static f32 l_zero = 0.0f;
static f32 l_half = 0.5f;
// HD: adjacent initializer attribution is inferred from TU placement.
static u32 l_staticFirstLow;
static u32 l_staticFirstHigh;
static u32 l_staticSecondLow;
static u32 l_staticSecondHigh;
struct OrthoStaticRecord {
    u8 bytes[1];
};
static OrthoStaticRecord l_staticRecord;

/* 027EDE30 */
void J2DOrthoGraph::setLookat() {
    // HD: matrix setup does not submit GX position state.
    orthoMatrixIdentity(&mPosMtx);
}

/* 027EDE38 */
J2DOrthoGraph* J2DOrthoGraph__ct(J2DOrthoGraph* i_this, f32 x, f32 y, f32 width, f32 height,
                                 f32 nearPlane, f32 farPlane) {
    if (!i_this)
        i_this = static_cast<J2DOrthoGraph*>(orthoAllocate(sizeof(J2DOrthoGraph)));
    if (!i_this)
        return nullptr;
    orthoBaseConstruct(i_this, x, y, width, height);
    i_this->__vtbl = 0x1016DEAC;
    // HD: retain the GHS embedded-box allocation check with guest-address wrapping.
    if (gabi::ea(&i_this->mOrtho) == 0)
        orthoAllocate(0x10);
    f32 zero = l_zero;
    i_this->mNear = nearPlane;
    i_this->mOrtho.f.x = width;
    i_this->mOrtho.f.y = height;
    i_this->mOrtho.i.y = zero;
    i_this->mFar = farPlane;
    i_this->mOrtho.i.x = zero;
    i_this->setLookat();
    return i_this;
}

/* 027EDF68 */
void J2DOrthoGraph::setPort() {
    orthoBaseSetPort(this);
    f32 bottom = gabi::fadds_ppc(mOrtho.f.y, l_half);
    // HD: projection state submission is omitted.
    orthoMatrixProjection(&mMtx44, mOrtho.i.y, bottom, mOrtho.i.x, mOrtho.f.x, mNear, mFar);
}

/* 027EDFC0 */
void J2DOrthoGraph::scissorBounds(Ortho::Box* output, const Ortho::Box* input) {
    f32 orthoX = mOrtho.i.x, left = mBounds.i.x;
    f32 orthoY = mOrtho.i.y, top = mBounds.i.y;
    f32 widthPower = f32(gabi::fsubs_ppc(mBounds.f.x, left) / gabi::fsubs_ppc(mOrtho.f.x, orthoX));
    f32 heightPower = f32(gabi::fsubs_ppc(mBounds.f.y, top) / gabi::fsubs_ppc(mOrtho.f.y, orthoY));
    f32 f0 = gabi::fmadds(gabi::fsubs_ppc(input->i.x, orthoX), widthPower, left);
    f32 f2 = gabi::fmadds(gabi::fsubs_ppc(input->f.x, orthoX), widthPower, left);
    f32 f1 = gabi::fmadds(gabi::fsubs_ppc(input->i.y, orthoY), heightPower, top);
    f32 f3 = gabi::fmadds(gabi::fsubs_ppc(input->f.y, orthoY), heightPower, top);
    output->i.x = f0;
    output->f.x = f2;
    output->i.y = f1;
    output->f.y = f3;
}

// HD: compiler-generated translation-unit initialization.
/* 027EE03C */
static void __sinit_J2DOrthoGraph_cpp() {
    l_staticFirstLow = 0;
    l_staticSecondLow = 0;
    l_staticFirstHigh = 0;
    l_staticSecondHigh = 0;
    orthoRegisterStatic(&l_staticRecord);
}

/* 027EE064 */
u32 J2DOrthoGraph::getGrafType() { return 1; }

// clang-format off
/* WWHD glue: HD inline constants, static storage and graphics bindings.
data l_zero 1016DEA0
data l_half 1016DEA4
data l_staticFirstLow 104B4530
data l_staticFirstHigh 104B4534
data l_staticSecondLow 104B4528
data l_staticSecondHigh 104B452C
data l_staticRecord 101F9838
ext 0273AD10 void* orthoAllocate(u32)
ext 027EDB54 void orthoBaseConstruct(J2DOrthoGraph*, f32, f32, f32, f32)
ext 027EDD50 void orthoBaseSetPort(J2DOrthoGraph*)
ext 028E9098 void orthoMatrixIdentity(Mtx34*)
ext 028E9A70 void orthoMatrixProjection(Ortho::Matrix44*, f32, f32, f32, f32, f32, f32)
ext 028F026C void orthoRegisterStatic(void*)
*/

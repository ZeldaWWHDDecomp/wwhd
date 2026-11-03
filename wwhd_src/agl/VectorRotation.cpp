// WWHD caller uses the canonical, verified vector/quaternion/matrix layouts.
#include "sead/RenderMath.h"
u32 rotationBetweenAglVectors(sead::RenderQuaternion*, const sead::RenderVector3*, const sead::RenderVector3*);
void setAglMatrixIdentity(sead::RenderMatrix34*);
/* 027AAFF4 */
static u32 setVectorRotation(sead::RenderMatrix34* matrix,
        const sead::RenderVector3* first, const sead::RenderVector3* second) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderQuaternion> rotation(0x8);
    if (rotationBetweenAglVectors(rotation.get(), first, second) != 0) {
        matrix->setQuaternion(rotation.get());
        return 1;
    }
    setAglMatrixIdentity(matrix);
    return 0;
}
/* WWHD glue
 ext 02750AF0 u32 rotationBetweenAglVectors(sead::RenderQuaternion*, const sead::RenderVector3*, const sead::RenderVector3*)
 ext 02750C2C void sead::RenderMatrix34::setQuaternion(const sead::RenderQuaternion*)
 ext 028E9098 void setAglMatrixIdentity(sead::RenderMatrix34*)
*/

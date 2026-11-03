// Own forwarding bodies; heap and descriptor roles follow the published original attribute-state ABI.
#include "agl/DrawPreparation.h"
#include "agl/PrimitiveShapes.h"
void configureDrawAttributeBinding(agl::VertexInputSet* inputs, u32 count, sead::AllocationHeap* heap);
void setDrawAttributeDescriptor(agl::VertexInputSet* inputs, u32 location, agl::VertexBufferLayout* buffer, u32 attribute);
namespace agl {
/* 027CC160 */
void agl::DrawResourceBindingState_l::preparePrimaryBinding(sead::AllocationHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    agl::PrimitiveShapes* shapes = sPrimitiveShapes;
    configureDrawAttributeBinding(&mInputs, 1, heap);
    setDrawAttributeDescriptor(&mInputs, 0, &shapes->mLodShapes[1].mVertexBuffer, 0);
    mInputs.prepare();
}

/* 027CF82C */
void agl::DrawResourceBindingState_l::prepareSelectedBinding(sead::AllocationHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    configureDrawAttributeBinding(&mInputs, 1, heap);
    setDrawAttributeDescriptor(&mInputs, 0, &sPrimitiveShapes->mLodShapes[1].mVertexBuffer, 0);
    mInputs.prepare();
}

/* 027CCC24 */
void agl::ExtendedDrawResourceBindingState_l::prepareExtendedBinding(sead::AllocationHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    configureDrawAttributeBinding(&mInputs, 1, heap);
    setDrawAttributeDescriptor(&mInputs, 0, &sPrimitiveShapes->mDualLodShapes[1].mVertexBuffer, 0);
    mInputs.prepare();
}

/* 027CDDE0 */
void agl::DrawResourceBindingState_l::prepareLargeBinding(sead::AllocationHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    configureDrawAttributeBinding(&mInputs, 1, heap);
    setDrawAttributeDescriptor(&mInputs, 0, &sPrimitiveShapes->mIndexedShapes[2].mVertexBuffer, 0);
    mInputs.prepare();
}

}
/* WWHD glue
ext 027BF960 void configureDrawAttributeBinding(agl::VertexInputSet* inputs, u32 count, sead::AllocationHeap* heap)
ext 027BFB60 void setDrawAttributeDescriptor(agl::VertexInputSet* inputs, u32 location, agl::VertexBufferLayout* buffer, u32 attribute)
ext 027BFC74 void agl::VertexInputSet::prepare()
*/

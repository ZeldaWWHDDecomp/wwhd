/* Material render-state selection and lazily initialized enum conversion tables. */
#include "nw/lyt/MaterialGpuState.h"
using namespace nw::lyt;
struct MaterialFactorTable { be<u32> values[8]; };
struct MaterialEquationTable { be<u32> values[6]; };
struct MaterialLogicTable { be<u32> values[17]; };
struct MaterialAlphaFunctions { be<u32> values[8]; };
struct MaterialStateGuard { be<u32> initialized; };
static MaterialStateGuard sMaterialFactorGuard;
static MaterialStateGuard sMaterialEquationGuard;
static MaterialStateGuard sMaterialLogicGuard;
static MaterialFactorTable sMaterialFactors;
static MaterialFactorTable sMaterialFactorDefaults;

static MaterialEquationTable sMaterialEquations;
static MaterialEquationTable sMaterialEquationDefaults;

static MaterialLogicTable sMaterialLogic;
static MaterialLogicTable sMaterialLogicDefaults;

static MaterialAlphaFunctions sMaterialAlphaFunctions;
static void initializeMaterialFactors() {
    if (sMaterialFactorGuard.initialized==0) {
        sMaterialFactorGuard.initialized=1;
        materialStateCopy(&sMaterialFactors,&sMaterialFactorDefaults,0x20);
    }
}
static void initializeMaterialEquations() {
    if (sMaterialEquationGuard.initialized==0) {
        sMaterialEquationGuard.initialized=1;
        materialStateCopy(&sMaterialEquations,&sMaterialEquationDefaults,0x18);
    }
}
/* 028836B8 */
void MaterialGpu::applyRenderState() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 flags=layoutFlags;
    u32 blendCount=(flags>>20)&3;
    if (blendCount) {
        flags=layoutFlags;
        u32 offset=(flags>>30)*28+((flags>>28)&3)*20+(((flags>>26)&3)+((flags>>22)&1))*8;
        auto* blend=storage.get()+offset;
        u32 logic=blend[3];
        if (logic) {
            if (sMaterialLogicGuard.initialized==0) {
                sMaterialLogicGuard.initialized=1;
                materialStateCopy(&sMaterialLogic,&sMaterialLogicDefaults,0x44);
            }
            materialColorControl(sMaterialLogic.values[logic],0,0,1);
        } else if (blend[0]==0) {
            materialColorControl(0xCC,0,0,1);
        } else {
            materialColorControl(0xCC,1,0,1);
            u32 source=blend[1];
            initializeMaterialFactors();
            u32 destination=blend[2];
            source=sMaterialFactors.values[source];
            initializeMaterialFactors();
            destination=sMaterialFactors.values[destination];
            u32 equation=blend[0];
            initializeMaterialEquations();
            equation=sMaterialEquations.values[equation];
            if (blendCount==2) {
                auto* alphaBlend=blend+4;
                u32 alphaSource=alphaBlend[1];
                initializeMaterialFactors();
                u32 alphaDestination=alphaBlend[2];
                alphaSource=sMaterialFactors.values[alphaSource];
                initializeMaterialFactors();
                alphaDestination=sMaterialFactors.values[alphaDestination];
                u32 alphaEquation=alphaBlend[0];
                initializeMaterialEquations();
                alphaEquation=sMaterialEquations.values[alphaEquation];
                materialBlendControl(0,source,destination,equation,1,alphaSource,alphaDestination,alphaEquation);
            } else {
                materialBlendControl(0,source,destination,equation,0,4,5,0);
            }
        }
    } else {
        materialColorControl(0xCC,1,0,1);
        materialBlendControl(0,4,5,0,0,4,5,0);
    }
    flags=layoutFlags;
    if ((flags>>22)&1) {
        flags=layoutFlags;
        u32 offset=(flags>>30)*28+((flags>>28)&3)*20+((flags>>26)&3)*8;
        auto* alpha=reinterpret_cast<MaterialAlphaTest*>(storage.get()+offset);
        u32 function=alpha->function;
        f32 reference=alpha->reference;
        materialAlphaTest(1,sMaterialAlphaFunctions.values[function],reference);
    } else {
        materialAlphaTest(0,0,0.0f);
    }
}
/* WWHD glue
data sMaterialFactorGuard 101FE1B0
data sMaterialEquationGuard 101FE1B4
data sMaterialLogicGuard 101FE1F0
data sMaterialFactors 101FE1D0
data sMaterialFactorDefaults 10181A30
data sMaterialEquations 101FE1B8
data sMaterialEquationDefaults 10181A18
data sMaterialLogic 101FE1F4
data sMaterialLogicDefaults 10181A50
data sMaterialAlphaFunctions 10181B00
ext C000A848 void materialStateCopy(void*,const void*,u32)
ext C00067D0 void materialColorControl(u32,u32,u32,u32)
ext C00067B0 void materialBlendControl(u32,u32,u32,u32,u32,u32,u32,u32)
ext C0006778 void materialAlphaTest(u32,u32,f32)
*/

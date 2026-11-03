// Data-only owner of the primitive shape singletons, their registered disposers and the shapes disposer's
// method table (declared in agl/PrimitiveShapes.h); the merged bootstrap and graphics-access units and both
// halves of the shapes translation unit (PrimitiveShapes.cpp, PrimitiveShapeBuilders.cpp) read these words.
#include "agl/PrimitiveShapes.h"
static agl::PrimitiveShapeInputs* sPrimitiveShapeInputs;
static agl::PrimitiveShapeInputsDisposer* sPrimitiveShapeInputsDisposer;
static agl::PrimitiveShapes* sPrimitiveShapes;
static agl::PrimitiveShapesDisposer* sPrimitiveShapesDisposer;
static sead::DisposerVtable sShapesDisposerMethods;
/* WWHD glue
data sPrimitiveShapeInputs 101F8DDC
data sPrimitiveShapeInputsDisposer 101F8DE0
data sPrimitiveShapes 101F8DE4
data sPrimitiveShapesDisposer 101F8DE8
data sShapesDisposerMethods 1016818C
*/

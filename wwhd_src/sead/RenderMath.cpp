// WWHD rendering matrix operations, reconstructed from the local game.
#include "sead/RenderMath.h"

// guest EA arithmetic for row/column-indexed matrix words: rows of 4 words,
// base + row * 16 + column * 4 wraps at 32 bits as on the console (a host index reached host addresses past 4 GB)
static inline be<u32>& matrixWord(const void* rows, u32 row, u32 column) {
    return *gabi::at<be<u32>>(gabi::ea(rows) + row * 16u + column * 4u);
}
/* 0274F300 */
static void copyMatrixRow(sead::RenderQuaternion* result, const sead::RenderMatrix44* matrix, u32 row) {
    // Pure lfs/stfs copies preserve signaling-NaN representations.
    reinterpret_cast<be<u32>&>(result->x) = (u32)matrixWord(matrix->mRows, row, 0);
    reinterpret_cast<be<u32>&>(result->y) = (u32)matrixWord(matrix->mRows, row, 1);
    reinterpret_cast<be<u32>&>(result->z) = (u32)matrixWord(matrix->mRows, row, 2);
    reinterpret_cast<be<u32>&>(result->w) = (u32)matrixWord(matrix->mRows, row, 3);
}

/* 0274F32C */
static void setMatrixRow(sead::RenderMatrix44* matrix, const sead::RenderQuaternion* source, u32 row) {
    matrixWord(matrix->mRows, row, 0) = (u32)reinterpret_cast<const be<u32>&>(source->x);
    matrixWord(matrix->mRows, row, 1) = (u32)reinterpret_cast<const be<u32>&>(source->y);
    matrixWord(matrix->mRows, row, 2) = (u32)reinterpret_cast<const be<u32>&>(source->z);
    matrixWord(matrix->mRows, row, 3) = (u32)reinterpret_cast<const be<u32>&>(source->w);
}

/* 02750C2C */
void sead::RenderMatrix34::setQuaternion(const RenderQuaternion* rotation) {
    f32 w = rotation->w, y = rotation->y;
    f32 twiceW = gabi::fadds_ppc(w, w);
    f32 x = rotation->x;
    f32 twiceY = gabi::fadds_ppc(y, y);
    f32 z = rotation->z;
    f32 twiceX = gabi::fadds_ppc(x, x);
    f32 twiceZ = gabi::fadds_ppc(z, z);
    f32 yy = gabi::fmuls_ppc(twiceY, y);
    f32 zz = gabi::fmuls_ppc(twiceZ, z);
    f32 oneMinusYY = gabi::fsubs_ppc(1.0f, yy);
    f32 wx = gabi::fmuls_ppc(twiceW, x);
    f32 oneMinusXX = gabi::fnmsubs(twiceX, x, 1.0f);
    f32 yz = gabi::fmuls_ppc(twiceY, z);
    f32 wz = gabi::fmuls_ppc(twiceW, z);
    f32 xz = gabi::fmuls_ppc(twiceX, z);
    f32 xy = gabi::fmuls_ppc(twiceX, y);
    f32 diagonalX = gabi::fsubs_ppc(oneMinusYY, zz);
    mRows[0][3] = 0.0f;
    f32 wy = gabi::fmuls_ppc(twiceW, y);
    mRows[2][3] = 0.0f;
    f32 xyMinusWZ = gabi::fsubs_ppc(xy, wz);
    mRows[0][0] = diagonalX;
    f32 xyPlusWZ = gabi::fadds_ppc(xy, wz);
    mRows[1][3] = 0.0f;
    f32 xzPlusWY = gabi::fadds_ppc(xz, wy);
    mRows[0][1] = xyMinusWZ;
    f32 diagonalY = gabi::fsubs_ppc(oneMinusXX, zz);
    mRows[1][0] = xyPlusWZ;
    f32 xzMinusWY = gabi::fsubs_ppc(xz, wy);
    mRows[0][2] = xzPlusWY;
    f32 yzMinusWX = gabi::fsubs_ppc(yz, wx);
    mRows[1][1] = diagonalY;
    f32 yzPlusWX = gabi::fadds_ppc(yz, wx);
    mRows[2][0] = xzMinusWY;
    mRows[1][2] = yzMinusWX;
    f32 diagonalZ = gabi::fsubs_ppc(oneMinusXX, yy);
    mRows[2][1] = yzPlusWX;
    mRows[2][2] = diagonalZ;
}

/* 02750CDC */
void sead::RenderMatrix34::setColumn(u32 column, const RenderVector3* vector) {
    matrixWord(mRows, 0, column) = (u32)reinterpret_cast<const be<u32>&>(vector->x);
    matrixWord(mRows, 1, column) = (u32)reinterpret_cast<const be<u32>&>(vector->y);
    matrixWord(mRows, 2, column) = (u32)reinterpret_cast<const be<u32>&>(vector->z);
}

/* 02750D00 */
void sead::RenderMatrix34::setTranslation(const RenderVector3* vector) {
    setColumn(3, vector);
}

void initializeSingularRenderMatrix(sead::RenderMatrix44* matrix);

/* 0274EDD4 */
void sead::RenderMatrix44::invert(const RenderMatrix44* source) {
    f32 rowOneColumnZero = source->mRows[1][0];
    f32 rowTwoColumnTwo = source->mRows[2][2];
    f32 rowOneColumnTwo = source->mRows[1][2];
    f32 minorProductA = gabi::fmuls_ppc(rowOneColumnZero, rowTwoColumnTwo);
    f32 rowTwoColumnThree = source->mRows[2][3];
    f32 rowOneColumnThree = source->mRows[1][3];
    f32 minorProductB = gabi::fmuls_ppc(rowOneColumnZero, rowTwoColumnThree);
    f32 rowOneColumnOne = source->mRows[1][1];
    f32 minorProductC = gabi::fmuls_ppc(rowOneColumnTwo, rowTwoColumnThree);
    f32 rowThreeColumnOne = source->mRows[3][1];
    f32 minorProductD = gabi::fmuls_ppc(rowOneColumnThree, rowTwoColumnTwo);
    f32 minorProductE = gabi::fmuls_ppc(rowOneColumnOne, rowTwoColumnTwo);
    f32 rowTwoColumnZero = source->mRows[2][0];
    f32 minorProductF = gabi::fmuls_ppc(rowOneColumnTwo, rowTwoColumnZero);
    f32 rowThreeColumnTwo = source->mRows[3][2];
    f32 minorProductG = gabi::fmuls_ppc(rowOneColumnOne, rowTwoColumnThree);
    f32 minorProductH = gabi::fmuls_ppc(minorProductB, rowThreeColumnTwo);
    f32 rowThreeColumnThree = source->mRows[3][3];
    f32 minorProductI = gabi::fmuls_ppc(minorProductC, rowThreeColumnOne);
    f32 cofactorSumA = gabi::fmadds(minorProductF, rowThreeColumnThree, minorProductH);
    f32 rowTwoColumnOne = source->mRows[2][1];
    f32 cofactorSumB = gabi::fmadds(minorProductE, rowThreeColumnThree, minorProductI);
    f32 rowThreeColumnZero = source->mRows[3][0];
    f32 minorProductJ = gabi::fmuls_ppc(rowOneColumnThree, rowTwoColumnOne);
    f32 cofactorSumC = gabi::fmadds(minorProductD, rowThreeColumnZero, cofactorSumA);
    f32 rowZeroColumnOne = source->mRows[0][1];
    f32 cofactorSumD = gabi::fmadds(minorProductJ, rowThreeColumnTwo, cofactorSumB);
    f32 minorProductK = gabi::fmuls_ppc(rowZeroColumnOne, cofactorSumC);
    f32 rowZeroColumnZero = source->mRows[0][0];
    f32 minorProductL = gabi::fmuls_ppc(rowOneColumnZero, rowTwoColumnOne);
    f32 cofactorSumE = gabi::fmadds(rowZeroColumnZero, cofactorSumD, minorProductK);
    f32 minorProductM = gabi::fmuls_ppc(minorProductG, rowThreeColumnZero);
    f32 minorProductN = gabi::fmuls_ppc(rowOneColumnThree, rowTwoColumnZero);
    f32 cofactorSumF = gabi::fmadds(minorProductL, rowThreeColumnThree, minorProductM);
    f32 minorProductO = gabi::fmuls_ppc(rowOneColumnTwo, rowTwoColumnOne);
    f32 rowZeroColumnTwo = source->mRows[0][2];
    f32 cofactorSumG = gabi::fmadds(minorProductN, rowThreeColumnOne, cofactorSumF);
    f32 minorProductP = gabi::fmuls_ppc(rowOneColumnOne, rowTwoColumnZero);
    f32 cofactorSumH = gabi::fmadds(rowZeroColumnTwo, cofactorSumG, cofactorSumE);
    f32 minorProductQ = gabi::fmuls_ppc(minorProductP, rowThreeColumnTwo);
    f32 minorProductR = gabi::fmuls_ppc(minorProductC, rowThreeColumnZero);
    f32 cofactorSumI = gabi::fmadds(minorProductA, rowThreeColumnOne, minorProductQ);
    f32 cofactorSumJ = gabi::fmadds(minorProductA, rowThreeColumnThree, minorProductR);
    f32 cofactorSumK = gabi::fmadds(minorProductO, rowThreeColumnZero, cofactorSumI);
    f32 rowZeroColumnThree = source->mRows[0][3];
    f32 cofactorSumL = gabi::fmadds(minorProductN, rowThreeColumnTwo, cofactorSumJ);
    f32 cofactorSumM = gabi::fmadds(rowZeroColumnThree, cofactorSumK, cofactorSumH);
    f32 minorProductS = gabi::fmuls_ppc(minorProductO, rowThreeColumnThree);
    f32 minorProductT = gabi::fmuls_ppc(minorProductP, rowThreeColumnThree);
    f32 cofactorSumN = gabi::fmadds(minorProductG, rowThreeColumnTwo, minorProductS);
    f32 cofactorSumO = gabi::fmadds(minorProductB, rowThreeColumnOne, minorProductT);
    f32 cofactorSumP = gabi::fmadds(minorProductD, rowThreeColumnOne, cofactorSumN);
    f32 cofactorSumQ = gabi::fmadds(minorProductJ, rowThreeColumnZero, cofactorSumO);
    f32 cofactorDifferenceA = gabi::fnmsubs(rowZeroColumnZero, cofactorSumP, cofactorSumM);
    f32 minorProductU = gabi::fmuls_ppc(minorProductE, rowThreeColumnZero);
    f32 cofactorDifferenceB = gabi::fnmsubs(rowZeroColumnOne, cofactorSumL, cofactorDifferenceA);
    f32 cofactorSumR = gabi::fmadds(minorProductL, rowThreeColumnTwo, minorProductU);
    f32 cofactorDifferenceC = gabi::fnmsubs(rowZeroColumnTwo, cofactorSumQ, cofactorDifferenceB);
    f32 cofactorSumS = gabi::fmadds(minorProductF, rowThreeColumnOne, cofactorSumR);
    f32 determinant = gabi::fnmsubs(rowZeroColumnThree, cofactorSumS, cofactorDifferenceC);
    if (determinant == 0.0f) {
        initializeSingularRenderMatrix(this);
        return;
    }

    f32 minorProductV = gabi::fmuls_ppc(rowOneColumnOne, rowThreeColumnThree);
    f32 minorDifferenceA = gabi::fsubs_ppc(minorProductA, minorProductF);
    f32 cofactorDifferenceD = gabi::fmsubs(rowOneColumnThree, rowThreeColumnOne, minorProductV);
    f32 minorDifferenceB = gabi::fsubs_ppc(minorProductD, minorProductC);
    f32 minorProductW = gabi::fmuls_ppc(rowTwoColumnZero, rowThreeColumnOne);
    f32 minorProductX = gabi::fmuls_ppc(rowOneColumnThree, rowThreeColumnTwo);
    f32 minorDifferenceC = gabi::fsubs_ppc(minorProductN, minorProductB);
    f32 cofactorDifferenceE = gabi::fmsubs(rowOneColumnTwo, rowThreeColumnThree, minorProductX);
    f32 cofactorDifferenceF = gabi::fmsubs(rowTwoColumnOne, rowThreeColumnZero, minorProductW);
    f32 minorProductY = gabi::fmuls_ppc(rowTwoColumnOne, rowThreeColumnTwo);
    f32 minorDifferenceD = gabi::fsubs_ppc(minorProductG, minorProductJ);
    f32 minorProductZ = gabi::fmuls_ppc(rowOneColumnTwo, rowThreeColumnOne);
    f32 minorProductAA = gabi::fmuls_ppc(rowTwoColumnThree, rowThreeColumnOne);
    f32 cofactorDifferenceG = gabi::fmsubs(rowOneColumnOne, rowThreeColumnTwo, minorProductZ);
    f32 minorProductAB = gabi::fmuls_ppc(rowOneColumnThree, rowThreeColumnZero);
    f32 minorProductAC = gabi::fmuls_ppc(rowOneColumnZero, rowThreeColumnTwo);
    f32 cofactorDifferenceH = gabi::fmsubs(rowOneColumnZero, rowThreeColumnThree, minorProductAB);
    f32 cofactorDifferenceI = gabi::fmsubs(rowOneColumnTwo, rowThreeColumnZero, minorProductAC);
    f32 cofactorDifferenceJ = gabi::fmsubs(rowTwoColumnOne, rowThreeColumnThree, minorProductAA);
    f32 minorProductAD = gabi::fmuls_ppc(rowOneColumnOne, rowThreeColumnZero);
    f32 minorDifferenceE = gabi::fsubs_ppc(minorProductO, minorProductE);
    f32 minorProductAE = gabi::fmuls_ppc(rowOneColumnTwo, cofactorDifferenceJ);
    f32 minorProductAF = gabi::fmuls_ppc(rowTwoColumnThree, rowThreeColumnZero);
    f32 minorProductAG = gabi::fmuls_ppc(rowTwoColumnThree, rowThreeColumnTwo);
    f32 cofactorDifferenceK = gabi::fmsubs(rowOneColumnZero, rowThreeColumnOne, minorProductAD);
    f32 cofactorDifferenceL = gabi::fmsubs(rowTwoColumnTwo, rowThreeColumnThree, minorProductAG);
    f32 minorDifferenceF = gabi::fsubs_ppc(minorProductP, minorProductL);
    f32 cofactorDifferenceM = gabi::fmsubs(rowTwoColumnTwo, rowThreeColumnOne, minorProductY);
    f32 cofactorDifferenceN = gabi::fmsubs(rowTwoColumnZero, rowThreeColumnThree, minorProductAF);
    f32 cofactorDifferenceO = gabi::fmsubs(rowOneColumnOne, cofactorDifferenceL, minorProductAE);
    f32 minorProductAH = gabi::fmuls_ppc(rowTwoColumnZero, rowThreeColumnTwo);
    f32 cofactorDifferenceP = gabi::fnmsubs(rowOneColumnThree, cofactorDifferenceM, cofactorDifferenceO);
    f32 cofactorDifferenceQ = gabi::fmsubs(rowTwoColumnTwo, rowThreeColumnZero, minorProductAH);
    f32 minorProductAI = gabi::fmuls_ppc(rowZeroColumnThree, cofactorDifferenceM);
    f32 inverseDeterminantA = gabi::fdivs_ppc(1.0f, determinant);
    f32 minorProductAJ = gabi::fmuls_ppc(rowZeroColumnTwo, cofactorDifferenceD);
    f32 cofactorSumT = gabi::fmadds(rowZeroColumnTwo, cofactorDifferenceJ, minorProductAI);
    f32 cofactorSumU = gabi::fmadds(rowZeroColumnOne, cofactorDifferenceE, minorProductAJ);
    f32 cofactorDifferenceR = gabi::fnmsubs(rowZeroColumnOne, cofactorDifferenceL, cofactorSumT);
    f32 cofactorSumV = gabi::fmadds(rowZeroColumnThree, cofactorDifferenceG, cofactorSumU);
    f32 minorProductAK = gabi::fmuls_ppc(rowZeroColumnTwo, minorDifferenceD);
    f32 minorProductAL = gabi::fmuls_ppc(rowOneColumnOne, cofactorDifferenceN);
    f32 cofactorSumW = gabi::fmadds(rowZeroColumnOne, minorDifferenceB, minorProductAK);
    f32 minorProductAM = gabi::fmuls_ppc(rowOneColumnThree, cofactorDifferenceQ);
    f32 cofactorSumX = gabi::fmadds(rowZeroColumnThree, minorDifferenceE, cofactorSumW);
    f32 minorProductAN = gabi::fmuls_ppc(rowZeroColumnZero, cofactorDifferenceD);
    f32 minorProductAO = gabi::fmuls_ppc(rowOneColumnOne, cofactorDifferenceQ);
    f32 cofactorDifferenceS = gabi::fmsubs(rowZeroColumnThree, cofactorDifferenceK, minorProductAN);
    f32 cofactorDifferenceT = gabi::fmsubs(rowOneColumnZero, cofactorDifferenceM, minorProductAO);
    f32 minorProductAP = gabi::fmuls_ppc(rowZeroColumnTwo, cofactorDifferenceK);
    f32 cofactorDifferenceU = gabi::fmsubs(rowOneColumnZero, cofactorDifferenceJ, minorProductAL);
    f32 minorProductAQ = gabi::fmuls_ppc(rowZeroColumnThree, minorDifferenceA);
    f32 cofactorSumY = gabi::fmadds(rowOneColumnTwo, cofactorDifferenceF, cofactorDifferenceT);
    f32 cofactorSumZ = gabi::fmadds(rowZeroColumnTwo, minorDifferenceC, minorProductAQ);
    f32 minorProductAR = gabi::fmuls_ppc(rowZeroColumnThree, cofactorDifferenceI);
    f32 minorProductAS = gabi::fmuls_ppc(rowZeroColumnTwo, cofactorDifferenceF);
    f32 cofactorSumAA = gabi::fmadds(rowZeroColumnTwo, cofactorDifferenceH, minorProductAR);
    f32 cofactorDifferenceV = gabi::fmsubs(rowZeroColumnOne, cofactorDifferenceQ, minorProductAS);
    f32 cofactorSumAB = gabi::fmadds(rowOneColumnTwo, cofactorDifferenceN, minorProductAM);
    f32 negatedMinorA = -minorProductAP;
    f32 cofactorDifferenceW = gabi::fnmsubs(rowZeroColumnOne, cofactorDifferenceH, cofactorDifferenceS);
    f32 cofactorDifferenceX = gabi::fnmsubs(rowOneColumnZero, cofactorDifferenceL, cofactorSumAB);
    f32 cofactorDifferenceY = gabi::fnmsubs(rowZeroColumnZero, cofactorDifferenceM, cofactorDifferenceV);
    f32 minorProductAT = gabi::fmuls_ppc(rowZeroColumnThree, cofactorDifferenceF);
    f32 cofactorDifferenceZ = gabi::fnmsubs(rowZeroColumnZero, cofactorDifferenceG, negatedMinorA);
    f32 cofactorDifferenceAA = gabi::fnmsubs(rowOneColumnThree, cofactorDifferenceF, cofactorDifferenceU);
    f32 cofactorDifferenceAB = gabi::fnmsubs(rowZeroColumnZero, minorDifferenceB, cofactorSumZ);
    f32 cofactorSumAC = gabi::fmadds(rowZeroColumnOne, cofactorDifferenceN, minorProductAT);
    f32 minorProductAU = gabi::fmuls_ppc(rowZeroColumnZero, minorDifferenceD);
    f32 minorProductAV = gabi::fmuls_ppc(rowZeroColumnTwo, cofactorDifferenceN);
    f32 cofactorDifferenceAC = gabi::fnmsubs(rowZeroColumnZero, cofactorDifferenceJ, cofactorSumAC);
    f32 cofactorDifferenceAD = gabi::fmsubs(rowZeroColumnThree, minorDifferenceF, minorProductAU);
    f32 cofactorDifferenceAE = gabi::fmsubs(rowZeroColumnZero, cofactorDifferenceL, minorProductAV);
    f32 minorProductAW = gabi::fmuls_ppc(cofactorDifferenceW, inverseDeterminantA);
    f32 cofactorDifferenceAF = gabi::fnmsubs(rowZeroColumnZero, cofactorDifferenceE, cofactorSumAA);
    f32 minorProductAX = gabi::fmuls_ppc(cofactorSumY, inverseDeterminantA);
    f32 minorProductAY = gabi::fmuls_ppc(cofactorDifferenceAF, inverseDeterminantA);
    f32 minorProductAZ = gabi::fmuls_ppc(rowZeroColumnTwo, minorDifferenceF);
    mRows[1][2] = minorProductAY;
    f32 cofactorDifferenceAG = gabi::fnmsubs(rowZeroColumnOne, minorDifferenceC, cofactorDifferenceAD);
    f32 cofactorDifferenceAH = gabi::fnmsubs(rowZeroColumnThree, cofactorDifferenceQ, cofactorDifferenceAE);
    f32 cofactorDifferenceAI = gabi::fnmsubs(rowZeroColumnOne, cofactorDifferenceI, cofactorDifferenceZ);
    f32 minorProductBA = gabi::fmuls_ppc(cofactorDifferenceAC, inverseDeterminantA);
    f32 negatedMinorB = -minorProductAZ;
    mRows[2][1] = minorProductBA;
    f32 minorProductBB = gabi::fmuls_ppc(cofactorDifferenceY, inverseDeterminantA);
    f32 minorProductBC = gabi::fmuls_ppc(cofactorDifferenceX, inverseDeterminantA);
    mRows[3][0] = minorProductAX;
    f32 minorProductBD = gabi::fmuls_ppc(cofactorSumX, inverseDeterminantA);
    mRows[3][1] = minorProductBB;
    f32 minorProductBE = gabi::fmuls_ppc(cofactorDifferenceAB, inverseDeterminantA);
    mRows[0][3] = minorProductBD;
    f32 minorProductBF = gabi::fmuls_ppc(cofactorDifferenceP, inverseDeterminantA);
    mRows[1][0] = minorProductBC;
    f32 minorProductBG = gabi::fmuls_ppc(cofactorDifferenceAG, inverseDeterminantA);
    mRows[0][0] = minorProductBF;
    f32 cofactorDifferenceAJ = gabi::fnmsubs(rowZeroColumnZero, minorDifferenceE, negatedMinorB);
    mRows[1][3] = minorProductBE;
    f32 minorProductBH = gabi::fmuls_ppc(cofactorDifferenceAA, inverseDeterminantA);
    f32 minorProductBI = gabi::fmuls_ppc(cofactorDifferenceAI, inverseDeterminantA);
    mRows[2][3] = minorProductBG;
    f32 minorProductBJ = gabi::fmuls_ppc(cofactorDifferenceAH, inverseDeterminantA);
    mRows[2][2] = minorProductAW;
    f32 minorProductBK = gabi::fmuls_ppc(cofactorDifferenceR, inverseDeterminantA);
    mRows[1][1] = minorProductBJ;
    f32 cofactorDifferenceAK = gabi::fnmsubs(rowZeroColumnOne, minorDifferenceA, cofactorDifferenceAJ);
    mRows[3][2] = minorProductBI;
    f32 minorProductBL = gabi::fmuls_ppc(cofactorSumV, inverseDeterminantA);
    mRows[0][1] = minorProductBK;
    f32 minorProductBM = gabi::fmuls_ppc(cofactorDifferenceAK, inverseDeterminantA);
    mRows[0][2] = minorProductBL;
    mRows[2][0] = minorProductBH;
    mRows[3][3] = minorProductBM;
}

static sead::RenderQuaternion sOppositeRotation;
void crossRenderVectors(const sead::RenderVector3* first, const sead::RenderVector3* second, sead::RenderVector3* result);

/* 02750AF0 */
static u32 rotationBetweenRenderVectors(sead::RenderQuaternion* result,
                                       const sead::RenderVector3* first,
                                       const sead::RenderVector3* second) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderVector3> cross(8);
    crossRenderVectors(first, second, cross);
    f32 firstX = first->x, firstY = first->y;
    f32 secondX = second->x, secondY = second->y;
    f32 firstZ = first->z;
    f32 productX = gabi::fmuls_ppc(firstX, secondX);
    f32 productY = gabi::fmuls_ppc(firstY, secondY);
    f32 secondZ = second->z;
    f32 planarDot = gabi::fadds_ppc(productX, productY);
    f32 dot = gabi::fmadds(firstZ, secondZ, planarDot);
    f32 onePlusDot = gabi::fadds_ppc(dot, 1.0f);
    if (!(onePlusDot > 1.1920928955078125e-7f)) {
        reinterpret_cast<be<u32>&>(result->x) = (u32)reinterpret_cast<const be<u32>&>(sOppositeRotation.x);
        reinterpret_cast<be<u32>&>(result->y) = (u32)reinterpret_cast<const be<u32>&>(sOppositeRotation.y);
        reinterpret_cast<be<u32>&>(result->z) = (u32)reinterpret_cast<const be<u32>&>(sOppositeRotation.z);
        reinterpret_cast<be<u32>&>(result->w) = (u32)reinterpret_cast<const be<u32>&>(sOppositeRotation.w);
        return 0;
    }
    f32 square = gabi::fadds_ppc(onePlusDot, onePlusDot);
    f32 inverseRoot = 0.0f;
    if (square > 0.0f) {
        f64 estimate = gabi::frsqrte(square);
        f32 squareInverse = (f32)(estimate * round25(estimate));
        f32 halfInverse = (f32)(estimate * 0.5);
        f32 correction = gabi::fnmsubs(squareInverse, square, 3.0f);
        inverseRoot = gabi::fmuls_ppc(correction, halfInverse);
    }
    f32 root = gabi::fmuls_ppc(inverseRoot, square);
    f32 crossZ = cross->z;
    f32 inverse = gabi::fdivs_ppc(1.0f, root);
    f32 crossY = cross->y;
    f32 y = gabi::fmuls_ppc(crossY, inverse);
    f32 crossX = cross->x;
    f32 w = gabi::fmuls_ppc(root, 0.5f);
    result->y = y;
    f32 x = gabi::fmuls_ppc(crossX, inverse);
    result->w = w;
    f32 z = gabi::fmuls_ppc(crossZ, inverse);
    result->x = x;
    result->z = z;
    return 1;
}

/* WWHD glue
 ext 028E892C void initializeSingularRenderMatrix(sead::RenderMatrix44*)
 data sOppositeRotation 104A0760
 ext 028E8D4C void crossRenderVectors(const sead::RenderVector3*,const sead::RenderVector3*,sead::RenderVector3*)
*/

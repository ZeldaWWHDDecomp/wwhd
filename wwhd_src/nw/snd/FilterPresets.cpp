/* Five sound-filter coefficient callbacks. Reconstructed from WWHD's instructions only.
 * Coefficients remain borrowed image data, with no extracted table initialisers. */
#include "nw/snd/FilterPresets.h"
using namespace nw::snd;

static const FilterTableA sPresetA;
static const FilterTableB sPresetB;
static const FilterTableC sPresetC;
static const FilterTableD sPresetD;
static const FilterTableE sPresetE;

/* 028A37D0 */
void FilterPresetA::coefficients(u32 type, FilterCoefficients* output, f64 amount) {
    s32 index = gabi::ftoi(gabi::fmuls_fpr(111.0, amount));
    if (index > 111) index = 111;
    else if (index < 0) index = 0;
    const FilterCoefficients* source = &sPresetA.entries[index];
    output->feedforward[0] = source->feedforward[0];
    output->feedforward[1] = source->feedforward[1];
    output->feedforward[2] = source->feedforward[2];
    output->feedback[0] = source->feedback[0];
    output->feedback[1] = source->feedback[1];
}

/* 028A3844 */
void FilterPresetB::coefficients(u32 type, FilterCoefficients* output, f64 amount) {
    s32 index = gabi::ftoi(gabi::fmuls_fpr(96.0, amount));
    if (index > 96) index = 96;
    else if (index < 0) index = 0;
    const FilterCoefficients* source = &sPresetB.entries[index];
    output->feedforward[0] = source->feedforward[0];
    output->feedforward[1] = source->feedforward[1];
    output->feedforward[2] = source->feedforward[2];
    output->feedback[0] = source->feedback[0];
    output->feedback[1] = source->feedback[1];
}

/* 028A38B8 */
void FilterPresetC::coefficients(u32 type, FilterCoefficients* output, f64 amount) {
    f32 inverse = gabi::fsubs_fpr(2.0, amount);
    f32 curved = gabi::fmuls_fpr(amount, inverse);
    s32 index = gabi::ftoi(gabi::fmuls_fpr(121.0, curved));
    if (index > 121) index = 121;
    else if (index < 0) index = 0;
    const FilterCoefficients* source = &sPresetC.entries[index];
    output->feedforward[0] = source->feedforward[0];
    output->feedforward[1] = source->feedforward[1];
    output->feedforward[2] = source->feedforward[2];
    output->feedback[0] = source->feedback[0];
    output->feedback[1] = source->feedback[1];
}

/* 028A393C */
void FilterPresetD::coefficients(u32 type, FilterCoefficients* output, f64 amount) {
    f32 inverse = gabi::fsubs_fpr(2.0, amount);
    f32 curved = gabi::fmuls_fpr(amount, inverse);
    s32 index = gabi::ftoi(gabi::fmuls_fpr(92.0, curved));
    if (index > 92) index = 92;
    else if (index < 0) index = 0;
    const FilterCoefficients* source = &sPresetD.entries[index];
    output->feedforward[0] = source->feedforward[0];
    output->feedforward[1] = source->feedforward[1];
    output->feedforward[2] = source->feedforward[2];
    output->feedback[0] = source->feedback[0];
    output->feedback[1] = source->feedback[1];
}

/* 028A39C0 */
void FilterPresetE::coefficients(u32 type, FilterCoefficients* output, f64 amount) {
    f32 inverse = gabi::fsubs_fpr(2.0, amount);
    f32 curved = gabi::fmuls_fpr(amount, inverse);
    s32 index = gabi::ftoi(gabi::fmuls_fpr(92.0, curved));
    if (index > 92) index = 92;
    else if (index < 0) index = 0;
    const FilterCoefficients* source = &sPresetE.entries[index];
    output->feedforward[0] = source->feedforward[0];
    output->feedforward[1] = source->feedforward[1];
    output->feedforward[2] = source->feedforward[2];
    output->feedback[0] = source->feedback[0];
    output->feedback[1] = source->feedback[1];
}

/* WWHD glue
 data sPresetA 10185814
 data sPresetB 10185C74
 data sPresetC 10186040
 data sPresetD 10186504
 data sPresetE 101868A8
*/

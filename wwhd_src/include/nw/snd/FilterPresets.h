/* Coefficient lookup callbacks from WWHD. Table letters identify distinct preset tables;
 * no particular analogue-filter response is inferred from their slot order. */
#pragma once
#include "wwhd.h"
namespace nw { namespace snd {
struct FilterCoefficients {
    be<u16> feedforward[3];
    be<u16> feedback[2];
};
WWHD_OFFSET(FilterCoefficients, feedback, 6);
WWHD_SIZE(FilterCoefficients, 10);
struct FilterTableA { FilterCoefficients entries[112]; };
WWHD_SIZE(FilterTableA, 1120);
struct FilterTableB { FilterCoefficients entries[97]; };
WWHD_SIZE(FilterTableB, 970);
struct FilterTableC { FilterCoefficients entries[122]; };
WWHD_SIZE(FilterTableC, 1220);
struct FilterTableD { FilterCoefficients entries[93]; };
WWHD_SIZE(FilterTableD, 930);
struct FilterTableE { FilterCoefficients entries[93]; };
WWHD_SIZE(FilterTableE, 930);
struct FilterPresetA { void coefficients(u32 type, FilterCoefficients* output, f64 amount); };
struct FilterPresetB { void coefficients(u32 type, FilterCoefficients* output, f64 amount); };
struct FilterPresetC { void coefficients(u32 type, FilterCoefficients* output, f64 amount); };
struct FilterPresetD { void coefficients(u32 type, FilterCoefficients* output, f64 amount); };
struct FilterPresetE { void coefficients(u32 type, FilterCoefficients* output, f64 amount); };
} }

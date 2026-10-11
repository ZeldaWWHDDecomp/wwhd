// HD: copy the weather projection defaults, then apply the current renderer bounds.
#include "d/d_kankyo_rain_kazanbai_move.h"
namespace RainMotion {
/* 02563DD8 */
void rainProjection(u32 output) {
    Local<ProjectionParameters> parameters;
    for (u32 i = 0; i < 5; i++) {
        parameters->words[i * 2] = (u32)at<ProjectionParameters>(ProjectionDefaults)->words[i * 2];
        parameters->words[i * 2 + 1] = (u32)at<ProjectionParameters>(ProjectionDefaults)->words[i * 2 + 1];
    }
    f32 upper = at<RendererBounds>(RendererAddress)->upper;
    f32 lower = at<RendererBounds>(RendererAddress)->lower;
    parameters->values[5] = upper;
    parameters->values[4] = lower;
    for (u32 i = 0; i < 5; i++) {
        at<ProjectionParameters>(output)->words[i * 2] = (u32)parameters->words[i * 2];
        at<ProjectionParameters>(output)->words[i * 2 + 1] = (u32)parameters->words[i * 2 + 1];
    }
}
} // namespace RainMotion
using namespace RainMotion;
/* WWHD glue (tools/verify/autoglue.py)
 */

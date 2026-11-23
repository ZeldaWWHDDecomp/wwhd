/* d_meter_t60_project.h (WWHD, true 60 build; 2026-11-22, magic bar placement): the HUD meter's
 * screen projection reads the frame's (exact) camera view. Decomp game build only.
 * Separate file: d_meter_00.cpp keeps one thin call.
 *
 * dMeter_flyGaugeMove 02592EE8 (under Medli / Makar control, the 50-frame move of the magic gauge to the flyer) projects
 * the partner's eyePos with mDoLib_projectScreen 025F0EA4 and stores the point in the HD HUD block (1047B03C +0xC / +0x10,
 * sHeartTimers.mProjectedEye = hd_screen_magic_bar sMagicPlacement mX / mY, read by hdHudMagicBar_c::updatePlacement
 * 02674EF0). mDoLib_projectScreen reads the HD renderer's 3D camera (*101F95D0 -> cameras[count > 1 ? 1 : 0]: its view
 * matrix, built by camera_draw's sead::ViewCamera::buildView). dMeter_Execute runs once a frame from the play scene's
 * draw, after camera_draw: in the original the view is the frame's camera. With frame interpolation the tick30 pass's
 * camera_draw runs twice (the port's exact pre-run, then the shown, blended in-between camera), so the meter projected
 * through the in-between view: mX / mY half a frame behind A on every transfer frame (uiMP1 / uiMX1: 211 records in both
 * arms; probe uiFB1 / uiFA1: Medli's eyePos A = B, the view at the call A = j3dSys's view, B's the blended one).
 *
 * Form (logic must not read the in-between picture; as f_op_actor_mng_t60_cull.h): around the
 * projection the camera's view matrix and projection state hold the port's copy of this pass's exact pre-run
 * (wwhd_t60_exact_hdcam; the blended draw also eases the fovy: with the view alone, ui9 / uiFP1, mX / mY were still
 * 0.11 / 0.63 apart on the transfer), and the live ones are put back after it. A port without that copy: the view only
 * (wwhd_t60_exact_view, the j3dSys view of the exact pre-run, = the HD camera's view in A), at N > 1 only. At N = 1 (the
 * 30 Hz fallback frames of true 60) the HD copy when the port has one (frame 56, uiZQ1: the fallback frame's
 * shown camera_draw is blended as well). Nothing when the port has no exact pre-run (no frame interpolation): the
 * original. */
#pragma once
#include "true60.h"

#if defined(GABI_GAME_BUILD) && GABI_T60
#include <cstring>
extern "C" __attribute__((visibility("default"))) int wwhd_t60_exact_view(void* out);
/* the port (interp.cpp): the HD 3D camera's view (0x30) and projection state (0xAC) as the exact pre-run left them */
extern "C" __attribute__((weak, visibility("default"))) int wwhd_t60_exact_hdcam(uint32_t* view_ea, void* view, uint32_t* proj_ea,
                                                                                 void* proj);
namespace meter_t60 {
/* the camera mDoLib_projectScreen 025F0EA4 reads: its view matrix and projection addresses (0: none) */
static inline void projectCamera(uint32_t* view, uint32_t* proj) {
    *view = *proj = 0;
    const uint32_t sys = gabi::load<uint32_t>(0x101F95D0);
    if (sys == 0) return;
    const uint32_t count = gabi::load<uint32_t>(sys + 0x1020), cams = gabi::load<uint32_t>(sys + 0x1024);
    if (cams == 0) return;
    const uint32_t cam = gabi::load<uint32_t>(cams + (count > 1 ? 4 : 0));
    if (cam == 0) return;
    const uint32_t flags = gabi::load<uint32_t>(cam + 0x50);
    *view = (flags & 0xC0u) == 0xC0u ? cam + 0x84 : gabi::load<uint32_t>(cam + 0x48); /* inline view / view pointer */
    *proj = (flags & 0x1000u) ? gabi::load<uint32_t>(cam + 0x164) : gabi::load<uint32_t>(cam + 0x4C);
}
struct ExactProjectView {
    uint32_t viewEa = 0, projEa = 0;
    uint8_t liveView[0x30], liveProj[0xAC];
    static void swapIn(uint32_t ea, uint8_t* save, const uint8_t* exact, size_t n) {
        std::memcpy(save, gabi::at<uint8_t>(ea), n);
        std::memcpy(gabi::at<uint8_t>(ea), exact, n);
    }
    ExactProjectView() {
        /* (N = 1 too, when the port has the HD camera copy: a 30 Hz fallback frame inside true 60, the B step 3556 of
         * uiZQ1, keeps the frame interpolation's blended camera_draw (and its exact pre-run), so the reader saw the
         * blended view there (frame 56); without a pre-run the port returns 0: the original) */
        if (t60::steps() <= 1 && !wwhd_t60_exact_hdcam) return;
        uint32_t view, proj;
        projectCamera(&view, &proj);
        uint8_t exactView[0x30], exactProj[0xAC];
        uint32_t exactViewEa = 0, exactProjEa = 0;
        if (wwhd_t60_exact_hdcam && wwhd_t60_exact_hdcam(&exactViewEa, exactView, &exactProjEa, exactProj)) {
            if (view != 0 && exactViewEa == view) { swapIn(view, liveView, exactView, sizeof exactView); viewEa = view; }
            if (proj != 0 && exactProjEa == proj) { swapIn(proj, liveProj, exactProj, sizeof exactProj); projEa = proj; }
        } else if (t60::steps() > 1 && view != 0 && wwhd_t60_exact_view(exactView)) { /* (a port without the HD camera copy: the view only) */
            swapIn(view, liveView, exactView, sizeof exactView);
            viewEa = view;
        }
    }
    ~ExactProjectView() {
        if (viewEa) std::memcpy(gabi::at<uint8_t>(viewEa), liveView, sizeof liveView);
        if (projEa) std::memcpy(gabi::at<uint8_t>(projEa), liveProj, sizeof liveProj);
    }
};
} // namespace meter_t60
#define T60_METER_EXACT_PROJECT_VIEW() meter_t60::ExactProjectView t60_exactProjectView_
#else
#define T60_METER_EXACT_PROJECT_VIEW() ((void)0)
#endif

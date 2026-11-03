#include "agl/UtilityDefaultBodies.h"
#include "agl/RenderLayerHooks.h"
/* 027C4C14 */
void agl::RenderLayerDefaultHooks_l::emptyHook() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 027C4C18 */
void agl::utility::ParameterDefaultBody_l::emptyHook() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 027CA9D4 */
void agl::utility::MatrixModeDefaultBodies_l::emptyHook() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 027CA9F8 */
u32 agl::utility::MatrixModeDefaultBodies_l::constantOne() {
    gabi::AutoFrame frame{gabi::current_function()};
    return 1;
}
/* 027CAA00 */
void agl::utility::ShadowDefaultBodies_l::emptyHookA() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 027CAA04 */
u32 agl::utility::ShadowDefaultBodies_l::constantOne() {
    gabi::AutoFrame frame{gabi::current_function()};
    return 1;
}
/* 027CAA0C */
void agl::utility::ShadowDefaultBodies_l::emptyHookB() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* WWHD glue
*/

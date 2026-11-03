/* Assigned texture surface update forwarding wrapper. */
#include "agl/TextureData.h"
using namespace agl;
static void updateTextureSurface(sead::GraphicsSurface*);
/* 027BE744 */
void TextureData::updateSurface() {
    gabi::AutoFrame frame{gabi::current_function()};
    updateTextureSurface(&surface);
}
/* WWHD glue
ext 0278635C void updateTextureSurface(sead::GraphicsSurface*)
*/

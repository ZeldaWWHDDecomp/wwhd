// Texture archive name accessor, starting from tww JPATexture.h.
#include "JSystem/JParticle/JPADraw_texture.h"
/* 028282FC */
const char* JPADrawTextureName(const JPATextureArc* self) {
    return self->mpData->mName;
}
/* WWHD glue
*/

/* Sampler settings defaults from the assigned WWHD constructor. */
#include "agl/TextureSamplerSettings.h"
using namespace agl;
static void* allocateSamplerSettings(u32);
/* 027BDE1C */
TextureSamplerSettings* TextureSamplerSettings::create(TextureSamplerSettings* settings) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!settings) settings = static_cast<TextureSamplerSettings*>(allocateSamplerSettings(0x40));
    if (settings) {
        settings->modes[0] = 1;
        settings->modes[1] = 1;
        settings->option = 0;
        settings->tail = 0;
        settings->scalarParameters[1] = 0.0f;
        settings->enabled = 0;
        settings->scalarParameters[0] = 0.0f;
        settings->scalarParameters[4] = 0.0f;
        settings->scalarParameters[2] = 0.0f;
        settings->scalarParameters[6] = 0.0f;
        settings->modes[3] = 2;
        settings->scalarParameters[5] = 14.0f;
        settings->modes[2] = 2;
        settings->modes[5] = 2;
        settings->modes[4] = 2;
        settings->scalarParameters[3] = 1.0f;
    }
    return settings;
}
/* WWHD glue
ext 0273AD10 void* allocateSamplerSettings(u32)
*/

/* Program object construction from the assigned WWHD constructor. */
#include "agl/ShaderProgram.h"
using namespace agl;
struct ProgramConfigurationPrefix_l {
    u8 bytes[0x1C];
};
WWHD_SIZE(ProgramConfigurationPrefix_l, 0x1C);
struct ProgramMethodIdentity_l {
    u8 byte;
};
static ProgramMethodIdentity_l sProgramMethods;
static ProgramMethodIdentity_l sVertexStageMethods;
static ProgramMethodIdentity_l sPixelStageMethods;
static ProgramMethodIdentity_l sGeometryStageMethods;
static void* allocateShaderProgram(u32);
static void constructProgramConfiguration(ProgramConfigurationPrefix_l*);
static void constructProgramStage(ShaderStage_l*);
/* 027B9F78 */
ShaderProgram* ShaderProgram::create(ShaderProgram* program) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!program) program = static_cast<ShaderProgram*>(allocateShaderProgram(0x84));
    if (program) {
        program->methods = &sProgramMethods;
        program->flags = 0;
        program->variantIndex = 0;
        constructProgramConfiguration(reinterpret_cast<ProgramConfigurationPrefix_l*>(&program->configuration));
        program->attributes = nullptr;
        program->attributeCount = 0;
        program->uniforms = nullptr;
        program->uniformCount = 0;
        program->uniformBlocks = nullptr;
        program->uniformBlockCount = 0;
        program->samplers = nullptr;
        program->samplerCount = 0;
        constructProgramStage(&program->vertex);
        program->vertex.methods = &sVertexStageMethods;
        constructProgramStage(&program->pixel);
        program->pixel.methods = &sPixelStageMethods;
        constructProgramStage(&program->geometry);
        program->binary = nullptr;
        program->geometry.methods = &sGeometryStageMethods;
    }
    return program;
}
/* WWHD glue
data sProgramMethods 10169CD8
data sVertexStageMethods 10169C30
data sPixelStageMethods 10169C58
data sGeometryStageMethods 10169C80
ext 0273AD10 void* allocateShaderProgram(u32)
ext 027C05F4 void constructProgramConfiguration(ProgramConfigurationPrefix_l*)
ext 027D4CCC void constructProgramStage(ShaderStage_l*)
*/

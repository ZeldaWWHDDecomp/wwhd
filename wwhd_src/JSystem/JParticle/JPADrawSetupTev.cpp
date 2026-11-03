// Cafe draw setup flags; tww's JPADrawSetupTev supplies the class/field roles.
#include "JSystem/JParticle/JPADrawSetupTev.h"
using namespace JPADrawExtension;
struct JPASetupStaticBlock { be<u32> words[4]; };
struct JPASetupRegistration { be<u32> words[3]; };
static JPASetupStaticBlock sSetupStaticBlock;
static JPASetupRegistration sSetupRegistration;
void JPADrawSetupRegisterGlobal(void*);
/* 028303D4 */
void JPADrawSetupTevDestroy(void*) {}
/* 028303D8 */
void JPADrawSetupTev_l::initialize() {
    mEnvColorEnabled = 1;
    mTextureEnabled = 0;
    mPrmColorEnabled = 1;
}
/* 028303F0 */
void JPADrawSetupTevStaticInitialize() {
    sSetupStaticBlock.words[2] = 0;
    sSetupStaticBlock.words[0] = 0;
    sSetupStaticBlock.words[3] = 0;
    sSetupStaticBlock.words[1] = 0;
    JPADrawSetupRegisterGlobal(&sSetupRegistration);
}
/* WWHD glue
data sSetupStaticBlock 104B5FDC
data sSetupRegistration 101FACF0
ext 028F026C void JPADrawSetupRegisterGlobal(void*)
*/

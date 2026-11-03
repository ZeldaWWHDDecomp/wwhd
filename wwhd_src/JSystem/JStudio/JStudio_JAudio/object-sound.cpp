// Sound adaptor setup and output objects, reconstructed from WWHD using the tww reference.
#include "JSystem/JStudio/JStudio_JAudio/object-sound.h"
#include "JSystem/JStudio/jstudio-object.h"
using JStudio_JAudio::SoundVariablePrefix;
void* JStudioSoundObject_allocate(u32);
void JStudioSoundObject_delete(void*);
void JStudioSoundObject_constructArray(void*, u32, u32, u32);
void JStudioSoundObject_destroyArray(void*, u32, u32, u32, u32, u32);
void JStudioSoundObject_clear(void*, u32);
void JStudioSoundObject_destroyBase(jst_adaptors::TAdaptor*, u32);
void JStudioSoundObject_destroyOutput(jst_timeline::TOutput*, u32);
void JStudioSoundObject_stop(jst_adaptors::JAISound*, u32);
void JStudioSoundObject_prepareSound(jst_adaptors::JAIBasic*, u32, gptr<jst_adaptors::JAISound>*, jst_adaptors::Vec*, u32, u32, u32);
void JStudioSoundObject_setImmediate(jst_adaptors::TAdaptor*, const void*);
void JStudioSoundObject_registerGlobal(void*);
jst_timeline::TVariableValue* TVariableValue_construct(jst_timeline::TVariableValue*);
void TVVOSetValue_destroy(JStudio_JAudio::TAdaptor_sound::TVVOSetValue_*, u32);
static f32 sJStudioQuietNaN;
namespace {
static u32 sBaseVTable;
static u32 sSoundVTable;
static u32 sBeginVTable;
static u32 sEndVTable;
static u32 sSetVTable;
struct Immediate { be<u32> index; union { be<f32> value; be<u32> bits; }; };
struct ImmediateTable { Immediate values[4]; };
static ImmediateTable sDefaults;
static u32 sDefaultsInitialized;
struct Outputs {
    JStudio_JAudio::TAdaptor_sound::TVVOSetValue_ values[1];
    JStudio_JAudio::TAdaptor_sound::TVVOOn_BEGIN_FADE_IN_ begin;
    JStudio_JAudio::TAdaptor_sound::TVVOOn_END_FADE_OUT_ end;
};
static Outputs sOutputs;
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
static u32 sBeginRegister;
static u32 sEndRegister;
static u32 sSetRegister;
static SoundVariablePrefix* prefix(JStudio_JAudio::TAdaptor_sound* adaptor) {
    return reinterpret_cast<SoundVariablePrefix*>(adaptor->pad04);
}
static jst_timeline::TVariableValue* values(JStudio_JAudio::TAdaptor_sound* adaptor) {
    return reinterpret_cast<jst_timeline::TVariableValue*>(adaptor->mVariableValues.get());
}
}
/* 0283DB40 */
JStudio_JAudio::TAdaptor_sound* TAdaptor_sound_construct(JStudio_JAudio::TAdaptor_sound* adaptor, jst_adaptors::JAIBasic* basic) {
    if (!adaptor) adaptor = static_cast<JStudio_JAudio::TAdaptor_sound*>(JStudioSoundObject_allocate(sizeof(JStudio_JAudio::TAdaptor_sound)));
    if (!adaptor) return adaptor;
    SoundVariablePrefix* state = prefix(adaptor);
    state->mCount = 11;
    state->__vtbl = &sBaseVTable;
    adaptor->mVariableValues = reinterpret_cast<jst_adaptors::TVariableValues*>(state->mValues);
    JStudioSoundObject_constructArray(state->mValues, 11, sizeof(jst_timeline::TVariableValue), gabi::guest_fn(reinterpret_cast<const void*>(&TVariableValue_construct)));
    adaptor->mpBasic = basic;
    adaptor->mpSound = nullptr;
    adaptor->mPosition = nullptr;
    state->__vtbl = &sSoundVTable;
    adaptor->mSoundID = 0xFFFFFFFF;
    return adaptor;
}
/* 0283DBDC */
void TAdaptor_sound_destroy(JStudio_JAudio::TAdaptor_sound* adaptor, u32 flags) {
    if (!adaptor) return;
    auto* sound = adaptor->mpSound.get();
    prefix(adaptor)->__vtbl = &sSoundVTable;
    if (sound) JStudioSoundObject_stop(sound, 0);
    JStudioSoundObject_destroyBase(adaptor, 0);
    if (flags & 1) JStudioSoundObject_delete(adaptor);
}
/* 0283DC50 */
void JStudio_JAudio::TAdaptor_sound::adaptor_do_prepare(const jst_adaptors::TObject* object) {
    if (!sDefaultsInitialized) {
        sDefaults.values[2].index = 2;
        sDefaults.values[3].index = 0xFFFFFFFF;
        sDefaults.values[0].index = 0;
        sDefaults.values[3].value = sJStudioQuietNaN;
        sDefaults.values[0].bits = 0;
        sDefaults.values[1].bits = 0;
        sDefaults.values[2].bits = 0;
        sDefaults.values[1].index = 1;
        sDefaultsInitialized = 1;
    }
    JStudioSoundObject_setImmediate(this, &sDefaults);
    values(this)[3].pOutput_ = &sOutputs.begin;
    values(this)[4].pOutput_ = &sOutputs.end;
    auto* output = sOutputs.values;
    while (s32(output->mValueIndex) != -1) {
        s32 index = output->mValueIndex;
        jst_timeline::TOutput* actual = output;
        if (gabi::ea(output) == 0) actual = &sJStudioOutputNone;
        values(this)[index].pOutput_ = actual;
        ++output;
    }
}
/* 0283DDC0 */
void JStudio_JAudio::TAdaptor_sound::adaptor_do_SOUND(u32 operation, const void* data, u32 size) {
    if (operation != 0x19) return;
    u32 id = *static_cast<const be<u32>*>(data);
    if (id & 0xC0000000) {
        auto* sound = mpSound.get();
        if (sound) JStudioSoundObject_stop(sound, 0);
        jst_adaptors::Vec* position = mPosition;
        auto* basic = mpBasic.get();
        mSoundID = id;
        JStudioSoundObject_prepareSound(basic, id, &mpSound, position, 0, 0, 4);
    } else {
        mSoundID = id;
    }
}
/* 0283DE58 */
void JStudio_JAudio::TAdaptor_sound::adaptor_do_LOCATED(u32 operation, const void* data, u32 size) {
    if (operation != 2) return;
    mPosition = nullptr;
    if (u32(*static_cast<const be<u32>*>(data))) mPosition = &_F8;
}
/* 0283E08C */
void JStudio_JAudio::TAdaptor_sound::TVVOSetValue_::output(f32 value, jst_adaptors::TAdaptor* adaptor) const {
    auto* sound = static_cast<JStudio_JAudio::TAdaptor_sound*>(adaptor)->mpSound.get();
    if (sound) (sound->*mSetFunc)(value, 0u);
}
/* 0283E0E0 */
static void initializeSoundAdaptor() {
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[1] = 0;
    sStaticBlock.words[0] = 0;
    JStudioSoundObject_registerGlobal(&sStaticRegister);
    sOutputs.begin.__vtbl = reinterpret_cast<jst_timeline::OutputVTable*>(&sBeginVTable);
    JStudioSoundObject_registerGlobal(&sBeginRegister);
    sOutputs.end.__vtbl = reinterpret_cast<jst_timeline::OutputVTable*>(&sEndVTable);
    JStudioSoundObject_registerGlobal(&sEndRegister);
    sOutputs.values[0].mSetFunc.d = 0;
    sOutputs.values[0].__vtbl = reinterpret_cast<jst_timeline::OutputVTable*>(&sSetVTable);
    sOutputs.values[0].mSetFunc.i = 0;
    sOutputs.values[0].mValueIndex = -1;
    sOutputs.values[0].mSetFunc.f = 0;
    JStudioSoundObject_registerGlobal(&sSetRegister);
}
/* 0283E198 */
static void destroySoundOutputs() {
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioSoundObject_destroyArray)), sOutputs.values, 1u, 0x10u, gabi::guest_fn(reinterpret_cast<const void*>(&TVVOSetValue_destroy)), 0u, 0u);
}
/* 0283E1BC */
jst_timeline::TVariableValue* TVariableValue_construct(jst_timeline::TVariableValue* value) {
    if (!value) value = static_cast<jst_timeline::TVariableValue*>(JStudioSoundObject_allocate(sizeof(jst_timeline::TVariableValue)));
    if (value) {
        value->field_0x4 = 0;
        value->field_0x8.v = 0;
        JStudioSoundObject_clear(&value->field_0xc, 4);
        value->pOutput_ = &sJStudioOutputNone;
    }
    return value;
}
/* 0283E220 */
void TVVOOn_BEGIN_FADE_IN_destroy(JStudio_JAudio::TAdaptor_sound::TVVOOn_BEGIN_FADE_IN_* output, u32 flags) {
    if (!output) return;
    JStudioSoundObject_destroyOutput(output, 0);
    if (flags & 1) JStudioSoundObject_delete(output);
}
/* 0283E274 */
void TVVOOn_END_FADE_OUT_destroy(JStudio_JAudio::TAdaptor_sound::TVVOOn_END_FADE_OUT_* output, u32 flags) {
    if (!output) return;
    JStudioSoundObject_destroyOutput(output, 0);
    if (flags & 1) JStudioSoundObject_delete(output);
}
/* 0283E2C8 */
void TVVOSetValue_destroy(JStudio_JAudio::TAdaptor_sound::TVVOSetValue_* output, u32 flags) {
    if (!output) return;
    JStudioSoundObject_destroyOutput(output, 0);
    if (flags & 1) JStudioSoundObject_delete(output);
}
/* WWHD glue
data sBaseVTable 101732BC
data sSoundVTable 10173314
data sBeginVTable 1017335C
data sEndVTable 10173374
data sSetVTable 1017338C
data sDefaults 104B60F8
data sDefaultsInitialized 104B6128
data sJStudioQuietNaN 101FCBC8
data sOutputs 104B612C
data sStaticBlock 104B6118
data sStaticRegister 101FADD4
data sBeginRegister 101FADE0
data sEndRegister 101FADEC
data sSetRegister 101FADF8
ext 0273AD10 void* JStudioSoundObject_allocate(u32)
ext 0273AF40 void JStudioSoundObject_delete(void*)
ext 028EFFD0 void JStudioSoundObject_constructArray(void*, u32, u32, u32)
ext 028F0164 void JStudioSoundObject_destroyArray(void*, u32, u32, u32, u32, u32) callback
ext 028F521C void JStudioSoundObject_clear(void*, u32)
ext 0283BCC4 void JStudioSoundObject_destroyBase(jst_adaptors::TAdaptor*, u32)
ext 02839E5C void JStudioSoundObject_destroyOutput(jst_timeline::TOutput*, u32)
ext 0280B1D8 void JStudioSoundObject_stop(jst_adaptors::JAISound*, u32)
ext 02802880 void JStudioSoundObject_prepareSound(jst_adaptors::JAIBasic*, u32, gptr<jst_adaptors::JAISound>*, jst_adaptors::Vec*, u32, u32, u32)
ext 0283A118 void JStudioSoundObject_setImmediate(jst_adaptors::TAdaptor*, const void*)
ext 028F026C void JStudioSoundObject_registerGlobal(void*)
*/

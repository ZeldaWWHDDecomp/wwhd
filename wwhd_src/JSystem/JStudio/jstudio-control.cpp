// JStudio control and object factories, adapted from tww jstudio-control.cpp.
#include "JSystem/JStudio/jstudio-control.h"
using namespace JStudio;
namespace {
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
static u32 sControlVTable;
static u32 sParseVTable;
struct TargetSignature { be<u8> bytes[8]; };
static const TargetSignature ga8cSignature = {{0x6A,0x73,0x74,0x75,0x64,0x69,0x6F,0}};
}
void* JStudio_allocate(u32);
void JStudio_delete(void*);
JStudio::fvb::TControl* TControl_construct(JStudio::fvb::TControl*);
void TControl_destroy(JStudio::fvb::TControl*, u32);
JStudio::fvb::TParse* TParse_construct(JStudio::fvb::TParse*, JStudio::fvb::TControl*);
void TParse_destroy(JStudio::fvb::TParse*, u32);
void TFactory_destroy(JStudio::fvb::TFactory*, u32);
void JStudio_insertNode(JGadget::TNodeLinkList*, gptr<JGadget::TLinkListNode>*, JGadget::TLinkListNode*);
void JStudio_destroyList(JGadget::TNodeLinkList*, u32);
void JStudio_constructSTBControl(TControl*);
void JStudio_destroySTBControl(TControl*, u32);
void JStudio_destroySTBFactory(TFactory*, u32);
void JStudio_constructSTBParse(TParse*, TControl*);
void JStudio_destroySTBParse(TParse*, u32);
jst_timeline::TObject* JStudio_createSTBObject(TFactory*, const stb::data::TParse_TBlock&);
u32 JStudio_parseSTBBlock(TParse*, const stb::data::TParse_TBlock&, u32);
u32 JStudio_parseFVB(JStudio::fvb::TParse*, gptr<const u8>*, u32);
void JStudio_controlRegisterGlobalObject(void*);
void PSMTXTrans(Mtx34*, f32, f32, f32);

/* 028394F4 */
void JStudio::TControl::transformOnSet_setOrigin(const Vec& origin, f32 rotationY) {
    const be<u32>* source = reinterpret_cast<const be<u32>*>(&origin);
    mTransformOnSet_Origin.words[0] = u32(source[0]);
    mTransformOnSet_Origin.words[1] = u32(source[1]);
    u32 z = source[2];
    mTransformOnSet_RotationY = rotationY;
    mTransformOnSet_Origin.words[2] = z;
    math::getTransformation_RyT(&mTransformOnSet_Matrix, rotationY, origin);
}
/* 02839518 */
void JStudio::TControl::transformOnGet_setOrigin(const Vec& origin, f32 rotationY) {
    const be<u32>* source = reinterpret_cast<const be<u32>*>(&origin);
    mTransformOnGet_Origin.words[0] = u32(source[0]);
    mTransformOnGet_Origin.words[1] = u32(source[1]);
    u32 z = source[2];
    mTransformOnGet_RotationY = rotationY;
    mTransformOnGet_Origin.words[2] = z;
    Mtx34 translation;
    PSMTXTrans(&translation, -f32(origin.x), -f32(origin.y), -f32(origin.z));
    math::rotate_y(&mTransformOnGet_Matrix, &translation, -rotationY);
}
/* 028395A8 */
TControl* JStudio_TControl_construct(TControl* control) {
    if (!control) control = static_cast<TControl*>(JStudio_allocate(sizeof(TControl)));
    if (!control) return control;
    JStudio_constructSTBControl(control);
    control->__vtbl = &sControlVTable;
    TControl_construct(&control->fvb_Control);
    control->mTransformOnGet = 0;
    control->mTransformOnSet = 0;
    Vec origin;
    origin.x = 0.0f;
    origin.y = 0.0f;
    origin.z = 0.0f;
    control->transformOnSet_setOrigin(origin, 0.0f);
    control->transformOnGet_setOrigin(origin, 0.0f);
    return control;
}
/* 0283965C */
void JStudio_TControl_destroy(TControl* control, u32 flags) {
    if (!control) return;
    TControl_destroy(&control->fvb_Control, 2);
    JStudio_destroySTBControl(control, 0);
    if (flags & 1) JStudio_delete(control);
}
/* 028396BC */
void JStudio_TCreateObject_destroy(TCreateObject* object, u32 flags) {
    if (object && (flags & 1)) JStudio_delete(object);
}
/* 028396D0 */
void JStudio_TFactory_destroy(TFactory* factory, u32 flags) {
    if (!factory) return;
    TFactory_destroy(&factory->fvb_Factory, 2);
    JStudio_destroyList(&factory->mList, 0);
    JStudio_destroySTBFactory(factory, 0);
    if (flags & 1) JStudio_delete(factory);
}
/* 0283973C */
void JStudio::TFactory::appendCreateObject(TCreateObject* object) {
    gabi::Local<gptr<JGadget::TLinkListNode>> end;
    *end = &mList.sentinel;
    JStudio_insertNode(&mList, end.get(), &object->mNode);
}
/* 02839774 */
jst_timeline::TObject* JStudio::TFactory::create(const stb::data::TParse_TBlock& block) {
    JGadget::TLinkListNode* end = &mList.sentinel;
    JGadget::TLinkListNode* node = mList.sentinel.mNext;
    gabi::Local<gptr<jst_timeline::TObject>> object;
    while (node != end) {
        TCreateObject* creator = reinterpret_cast<TCreateObject*>(node);
        node = node->mNext;
        *object = nullptr;
        u32 create = creator->__vtbl.get()->create.get();
        if (gabi::call_ptr<u32>(create, creator, object.get(), &block)) return object->get();
    }
    return JStudio_createSTBObject(this, block);
}
/* 0283980C */
TParse* JStudio_TParse_construct(TParse* parser, TControl* control) {
    if (!parser) parser = static_cast<TParse*>(JStudio_allocate(sizeof(TParse)));
    if (!parser) return parser;
    JStudio_constructSTBParse(parser, control);
    parser->__vtbl = &sParseVTable;
    TParse_construct(&parser->fvb_Parse, &control->fvb_Control);
    return parser;
}
/* 0283987C */
void JStudio_TParse_destroy(TParse* parser, u32 flags) {
    if (!parser) return;
    TParse_destroy(&parser->fvb_Parse, 2);
    JStudio_destroySTBParse(parser, 0);
    if (flags & 1) JStudio_delete(parser);
}
/* 028398DC */
bool JStudio::TParse::parseHeader(const stb::data::TParse_THeader& header, u32 flags) {
    const stb::data::THeader* raw = header.raw;
    for (u32 i = 0; i < 8; ++i)
        if (u8(raw->target[i]) != u8(ga8cSignature.bytes[i])) return false;
    return u32(u16(raw->target_version) - 2) < 2;
}
/* 0283992C */
u32 JStudio::TParse::parseBlock_block_fvb_(const stb::data::TParse_TBlock& block, u32 flags) {
    gabi::Local<gptr<const u8>> cursor;
    *cursor = reinterpret_cast<const u8*>(block.raw.get() + 1);
    return JStudio_parseFVB(&fvb_Parse, cursor.get(), flags & 0x7F);
}
/* 0283998C */
u32 JStudio::TParse::parseBlock_block(const stb::data::TParse_TBlock& block, u32 flags) {
    if (u32(block.raw.get()->type) == 0x4A465642) return parseBlock_block_fvb_(block, flags);
    return JStudio_parseSTBBlock(this, block, flags);
}
/* 028399AC */
static void __sinit_jstudio_control_cpp() {
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[0] = 0;
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[1] = 0;
    JStudio_controlRegisterGlobalObject(&sStaticRegister);
}

/* WWHD glue
data sStaticBlock 104B601C
data sStaticRegister 101FAD20
data sControlVTable 10172CC4
data sParseVTable 10172CFC
data ga8cSignature 10172D3C
ext 0283D228 void JStudio_constructSTBControl(TControl*)
ext 0283D2D0 void JStudio_destroySTBControl(TControl*, u32)
ext 0283D5C0 void JStudio_destroySTBFactory(TFactory*, u32)
ext 0283D5F0 void JStudio_constructSTBParse(TParse*, TControl*)
ext 0283D640 void JStudio_destroySTBParse(TParse*, u32)
ext 0283D9BC jst_timeline::TObject* JStudio_createSTBObject(TFactory*, const stb::data::TParse_TBlock&)
ext 0283D788 u32 JStudio_parseSTBBlock(TParse*, const stb::data::TParse_TBlock&, u32)
ext 027ED8C0 u32 JStudio_parseFVB(JStudio::fvb::TParse*, gptr<const u8>*, u32)
ext 028F026C void JStudio_controlRegisterGlobalObject(void*)
ext 028E93CC void PSMTXTrans(Mtx34*, f32, f32, f32)
*/

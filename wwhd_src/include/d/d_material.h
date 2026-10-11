/* HD replacement material state. */
#pragma once
#include "bindings.h"
namespace material_hd {
// HD: these render-state fields have no confirmed GameCube counterpart.
struct State {
    be<u32> field_0x0, field_0x4, field_0x8, field_0xc, field_0x10, field_0x14, field_0x18, field_0x1c;
    be<u32> field_0x20, field_0x24, field_0x28, field_0x2c, field_0x30, field_0x34, field_0x38;
    u8 _03c[0x90-0x3c];
};
struct ExtendedState {
    State base;
    u8 _090[0xd4-0x90];
    be<u32> field_0xd4, field_0xd8, field_0xdc;
    u8 _0e0[0x198-0xe0];
};
struct Control { State source[2]; ExtendedState target[2]; };
struct Model { u8 _000[0x78]; be<u8> mMaterialOverride; };
struct Morf { u8 _000[0x90]; gptr<Model> mpModel; };
struct Resource { u8 _000[0x20]; gptr<void> field_0x20; };
struct Play { u8 _0000[0x5d78]; be<u32> mDrawBufferOpa, mDrawBufferXlu; };
struct HeapVtable { u8 _000[0x24]; gfn destroy; u8 _028[4]; gfn adjustSize; };
struct Heap {
    u8 _000[0xc]; gptr<HeapVtable> __vtbl;
    void destroy() { gabi::call_ptr(__vtbl->destroy.get(), this); }
    void adjustSize() { gabi::call_ptr(__vtbl->adjustSize.get(), this); }
};
struct String { be<u32> mStringTop, __vtbl; };
WWHD_SIZE(State,0x90); WWHD_SIZE(ExtendedState,0x198); WWHD_SIZE(Control,0x450);
WWHD_OFFSET(Model,mMaterialOverride,0x78); WWHD_OFFSET(Morf,mpModel,0x90);
}
void copyState(material_hd::ExtendedState* dst, material_hd::State* src);
void initTexture(material_hd::State* state, void* texture);
void initState(material_hd::State* state);
void initExtendedState(material_hd::ExtendedState* state);
material_hd::Resource* getResource(void* resources, material_hd::String* name, u32 index);
material_hd::Heap* createHeap(u32 size, material_hd::String* name, material_hd::Heap* parent, u32 direction, u32 flags);
material_hd::Control* allocateControl(u32 size, material_hd::Heap* heap, s32 alignment);
void updateModel(material_hd::Model* model, u32 flags);
void entryModel(material_hd::Model* model);
void updateMorf(material_hd::Morf* morf);
void entryMorf(material_hd::Morf* morf);
void entryInvisible(void* invisible);
void entryInvisibleAlternate(void* invisible);

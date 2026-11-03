// Internal constructor boundary declarations reconstructed from AGL#12 callers.
#pragma once
#include "agl/RenderEntry.h"
#include "sead/FileDevice.h"
struct EntryLabel { be<u8> mFirst; };
void constructEntryName(agl::EntryNameObject_l*);
void constructEntryContainer(void*);
void constructEntryParameter(agl::ParameterHeader*);
void attachEntryParameter(agl::ParameterHeader*, const sead::SafeText*, const sead::SafeText*, const sead::SafeText*, void*);
void associateEntryContainer(agl::EntryNameObject_l*, void*, const sead::SafeText*);
void* allocateAglStorage(u32);
agl::RenderEntry* createRenderEntry(agl::RenderEntry*, u32, u32);

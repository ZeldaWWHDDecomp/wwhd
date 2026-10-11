/* f_pc (process framework), GameCube names -> WWHD, for GameCube-style engine sources: the functions of
 * f_pc_layer, f_pc_method, f_pc_profile, f_pc_layer_tag, f_pc_line_tag, f_pc_delete_tag, f_pc_priority
 * and f_pc_pause that other f_pc units call (by address: each is verified in its own unit). Layouts:
 * f_pc/f_pc_base.h.  [v f_pc_base] */
#pragma once
#include "f_pc/f_pc_base.h"
#include "gc_api/JSystem/JUTAssert.h"
#include "gc_api/SSystem/c_phase.h"

/* f_pc_layer */
inline layer_class* fpcLy_CurrentLayer() { return gabi::call<layer_class*>(0x025DED64); }
inline void fpcLy_SetCurrentLayer(layer_class* layer) { gabi::call(0x025DEAB4, layer); }
/* f_pc_method */
inline s32 fpcMtd_Execute(process_method_class* mtd, void* data) { return gabi::call<s32>(0x025DFCC4, mtd, data); }
inline s32 fpcMtd_IsDelete(process_method_class* mtd, void* data) { return gabi::call<s32>(0x025DFCCC, mtd, data); }
inline s32 fpcMtd_Delete(process_method_class* mtd, void* data) { return gabi::call<s32>(0x025DFCD4, mtd, data); }
inline s32 fpcMtd_Create(process_method_class* mtd, void* data) { return gabi::call<s32>(0x025DFCDC, mtd, data); }
/* f_pc_profile */
inline process_profile_definition* fpcPf_Get(s16 profName) { return gabi::call<process_profile_definition*>(0x025E10E0, profName); }
/* tags, priority, pause */
inline s32 fpcLyTg_Init(layer_management_tag_class* tag, u32 layerID, void* data) { return gabi::call<s32>(0x025DF178, tag, layerID, data); }
inline void fpcLnTg_Init(void* tag, void* data) { gabi::call(0x025DF7A8, tag, data); }
inline void fpcDtTg_Init(void* tag, void* data) { gabi::call(0x025DDEEC, tag, data); }
inline void fpcPi_Init(void* pi, void* data, u32 layer, u16 listID, u16 priority) { gabi::call(0x025E0FB0, pi, data, layer, listID, priority); }
inline void fpcPause_Init(void* process) { gabi::call(0x025E0C1C, process); }

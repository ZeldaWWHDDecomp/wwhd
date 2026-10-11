/** Debug map selection menu, using tww's scene and resource names.
 * HD: scene base grows by four bytes; resource requests use HD DVD command layouts.
 * SafeString comparisons and binary-resource endian dispatch are HD additions.
 */
#pragma once
#include "bindings.h"
#include "f_op/f_op_scene.h"

namespace MenuScene {
struct OffsetReaders {
    gabi::gfn entry[4];
};
WWHD_SIZE(OffsetReaders, 0x10);
struct Vtable {
    u8 prefix[0xC];
    gabi::gfn deletingDestructor;
    u8 reserved[4];
    gabi::gfn assureTermination;
};
struct SafeString {
    gptr<be<u8>> mString;
    gptr<Vtable> __vtbl;
};
struct DvdCommand {
    u8 prefix[0xC];
    be<u8> mSync;
    u8 padding[3];
    gptr<Vtable> __vtbl;
    u8 commandData[0x118 - 0x14];
    gptr<void> mMemAddress;
};
struct Font {
    u8 prefix[0x18];
    gptr<Vtable> __vtbl;
};
struct StartStage {
    be<char> mName[8];
    be<s16> mPoint;
    be<s8> mRoomNo, mLayer;
};
struct Play {
    u8 prefix[0x5140];
    StartStage mNextStage;
    be<u8> mEnableNextStage;
};
struct Save {
    u8 prefix[0x44];
    be<f32> mTime;
    be<s16> mDate;
    u8 padding[0x644 - 0x4A];
    u8 mEvent[0x1148 - 0x644];
    u8 mRestart[0x24];
    be<u32> mRestartRoomParam;
    u8 tail[0x12C0 - 0x1170];
    u8 mOption[8];
};
struct Environment {
    u8 prefix[0x1020];
    be<f32> mCurTime;
    u8 padding[4];
    be<f32> mTimeAdv;
};
struct Option {
    u8 prefix[4];
    be<u8> mLanguage;
};
// HD: the native execute frame keeps eight SafeString objects and its integer conversion slot.
struct ExecuteFrame {
    u8 linkage[8];
    SafeString needleLast, candidateFirst, candidateSecond, candidateThird, candidateLast, roomName,
        needleFirst, needleSecond, demoName, stageName;
    be<u32> conversionHigh, conversionLow;
    u8 savedRegisters[176 - 0x60];
};
enum {
    EVENT_DEMO23 = 0x2D01,
    EVENT_DEMO40 = 0x1E40,
    STRING_MAX_LENGTH = 0x40000,
    STRING_COMPARE_LIMIT = 0x40001,
    PLAY_SCENE = 7,
    OVERLAP0 = 0,
    REPEAT_DELAY = 20,
    REPEAT_INTERVAL = 4
};
} // namespace MenuScene
class menu_of_scene_class : public scene_class {
  public:
    struct room_inf {
        be<char> name[0x20];
        be<u8> field_0x20;
        be<char> stageName[8];
        be<s8> roomNo;
        be<u8> startCode;
        be<s8> layerNo;
    };
    struct stage_inf {
        be<char> name[0x20];
        be<u8> field_0x20, roomNum, field_0x22, field_0x23;
        gptr<room_inf> roomPtr;
    };
    struct menu_inf {
        be<u8> num;
        u8 padding[3];
        gptr<stage_inf> stage;
    };
    /* 0x1C8 */ u8 mPhs[8];
    /* 0x1D0 */ gptr<MenuScene::DvdCommand> command, fontCommand;
    /* 0x1D8 */ gptr<menu_inf> info;
    /* 0x1DC */ gptr<void> fontRes;
    /* 0x1E0 */ gptr<MenuScene::Font> font;
    /* 0x1E4 */ be<u8> startCode;
    /* 0x1E5 */ be<u8> field_0x1e1, field_0x1e2;
    u8 padding;
};
WWHD_OFFSET(menu_of_scene_class, mPhs, 0x1C8);
WWHD_OFFSET(menu_of_scene_class, command, 0x1D0);
WWHD_OFFSET(menu_of_scene_class, fontCommand, 0x1D4);
WWHD_OFFSET(menu_of_scene_class, info, 0x1D8);
WWHD_OFFSET(menu_of_scene_class, fontRes, 0x1DC);
WWHD_OFFSET(menu_of_scene_class, font, 0x1E0);
WWHD_OFFSET(menu_of_scene_class, startCode, 0x1E4);
WWHD_OFFSET(menu_of_scene_class, field_0x1e1, 0x1E5);
WWHD_OFFSET(menu_of_scene_class, field_0x1e2, 0x1E6);
WWHD_SIZE(menu_of_scene_class, 0x1E8);
WWHD_OFFSET(menu_of_scene_class::room_inf, stageName, 0x21);
WWHD_OFFSET(menu_of_scene_class::room_inf, roomNo, 0x29);
WWHD_OFFSET(menu_of_scene_class::room_inf, startCode, 0x2A);
WWHD_OFFSET(menu_of_scene_class::room_inf, layerNo, 0x2B);
WWHD_SIZE(menu_of_scene_class::room_inf, 0x2C);
WWHD_OFFSET(menu_of_scene_class::stage_inf, roomNum, 0x21);
WWHD_OFFSET(menu_of_scene_class::stage_inf, roomPtr, 0x24);
WWHD_SIZE(menu_of_scene_class::stage_inf, 0x28);
WWHD_OFFSET(menu_of_scene_class::menu_inf, stage, 4);
WWHD_SIZE(menu_of_scene_class::menu_inf, 8);
WWHD_OFFSET(MenuScene::Vtable, deletingDestructor, 0xC);
WWHD_OFFSET(MenuScene::Vtable, assureTermination, 0x14);
WWHD_SIZE(MenuScene::Vtable, 0x18);
WWHD_SIZE(MenuScene::SafeString, 8);
WWHD_OFFSET(MenuScene::DvdCommand, mSync, 0xC);
WWHD_OFFSET(MenuScene::DvdCommand, __vtbl, 0x10);
WWHD_OFFSET(MenuScene::DvdCommand, mMemAddress, 0x118);
WWHD_SIZE(MenuScene::DvdCommand, 0x11C);
WWHD_OFFSET(MenuScene::Font, __vtbl, 0x18);
WWHD_SIZE(MenuScene::Font, 0x1C);
WWHD_SIZE(MenuScene::StartStage, 0xC);
WWHD_OFFSET(MenuScene::Play, mNextStage, 0x5140);
WWHD_OFFSET(MenuScene::Play, mEnableNextStage, 0x514C);
WWHD_SIZE(MenuScene::Play, 0x514D);
WWHD_OFFSET(MenuScene::Save, mTime, 0x44);
WWHD_OFFSET(MenuScene::Save, mDate, 0x48);
WWHD_OFFSET(MenuScene::Save, mEvent, 0x644);
WWHD_OFFSET(MenuScene::Save, mRestart, 0x1148);
WWHD_OFFSET(MenuScene::Save, mRestartRoomParam, 0x116C);
WWHD_OFFSET(MenuScene::Save, mOption, 0x12C0);
WWHD_SIZE(MenuScene::Save, 0x12C8);
WWHD_OFFSET(MenuScene::Environment, mCurTime, 0x1020);
WWHD_OFFSET(MenuScene::Environment, mTimeAdv, 0x1028);
WWHD_SIZE(MenuScene::Environment, 0x102C);
WWHD_OFFSET(MenuScene::Option, mLanguage, 4);
WWHD_SIZE(MenuScene::Option, 5);
WWHD_OFFSET(MenuScene::ExecuteFrame, needleLast, 8);
WWHD_OFFSET(MenuScene::ExecuteFrame, candidateFirst, 0x10);
WWHD_OFFSET(MenuScene::ExecuteFrame, candidateSecond, 0x18);
WWHD_OFFSET(MenuScene::ExecuteFrame, candidateThird, 0x20);
WWHD_OFFSET(MenuScene::ExecuteFrame, candidateLast, 0x28);
WWHD_OFFSET(MenuScene::ExecuteFrame, roomName, 0x30);
WWHD_OFFSET(MenuScene::ExecuteFrame, needleFirst, 0x38);
WWHD_OFFSET(MenuScene::ExecuteFrame, needleSecond, 0x40);
WWHD_OFFSET(MenuScene::ExecuteFrame, demoName, 0x48);
WWHD_OFFSET(MenuScene::ExecuteFrame, stageName, 0x50);
WWHD_OFFSET(MenuScene::ExecuteFrame, conversionHigh, 0x58);
WWHD_SIZE(MenuScene::ExecuteFrame, 176);

// Unit-private bindings preserve this scene's full-register call signatures.
u32 MenuPad_holdUp(u32);
u32 MenuPad_holdDown(u32);
u32 MenuPad_trigUp(u32);
u32 MenuPad_trigDown(u32);
u32 MenuPad_holdB(u32);
u32 MenuPad_holdA(u32);
u32 MenuPad_trigB(u32);
u32 MenuPad_trigA(u32);
u32 MenuPad_trigStart(u32);
u32 MenuPad_trigY(u32);
u32 MenuPad_trigX(u32);
u32 MenuPad_trigLeft(u32);
u32 MenuPad_trigRight(u32);
u32 Menu_calcTimer(be<u8>*);
u32 MenuString_assureTermination(MenuScene::SafeString*);
u32 MenuString_terminateDirect();
u32 MenuEvent_off(void*, u32);
u32 MenuEvent_on(void*, u32);
MenuScene::Play* Menu_getPlay();
u32 Menu_setNextStage(void*, u32, s32, s32, u32, u32, u32, f64);
u32 Menu_changeScene(menu_of_scene_class*, u32, u32, u32, u32);
u32 Menu_setAudioScene(MenuScene::StartStage*, s32, s32);
MenuScene::Environment* Menu_getEnvironment();
void Menu_free(void*);
void Menu_setRestartOption(void*, u32);
void Menu_setFrontendMode(u32);
MenuScene::DvdCommand* Menu_createDvd(const char*, u32, u32);
void Menu_assert(const char*, u32, const char*);
be<s8>* Menu_newArray(u32);
void Menu_changeFont(void*, MenuScene::Font*);
be<u32>* Menu_getSystemClock();
void Menu_setTickRate(u32);
MenuScene::Option* Menu_getOption(void*);
s32 Menu_phaseHandler(void*, void*, menu_of_scene_class*);
void Menu_initColor(void*);
void Menu_initTypeA(void*);
void Menu_initTypeB(void*);
void Menu_delete(u32);

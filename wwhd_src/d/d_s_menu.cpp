/** Debug map selection scene. Local verification */
#include "d/d_s_menu.h"

using namespace MenuScene;
using SavePointer = gptr<Save>;
using SignedBytePointer = gptr<be<s8>>;
using FontManagerPointer = gptr<void>;
using SignedWord = be<s32>;
using SignedHalf = be<s16>;
using Byte = be<u8>;
using Word = be<u32>;
using FloatConstant = be<f32>;
using DoubleConstant = be<f64>;
static SignedWord l_cursolID;
static SignedWord l_timepat;
static SignedHalf l_weekpat;
static SignedBytePointer l_groupPoint;
static Byte l_languageType;
static Byte l_displayFlags;
static SavePointer l_save;
static Vtable l_safeStringVtable;
static FloatConstant l_zero;
static FloatConstant l_fastTimeAdv;
static FloatConstant l_hourStep;
static FloatConstant l_normalTime;
static FloatConstant l_normalTimeAdv;
static FloatConstant l_fastAdv;
static FloatConstant l_morning;
static FloatConstant l_noon;
static FloatConstant l_noonAdv;
static FloatConstant l_evening;
static FloatConstant l_eveningAdv;
static FloatConstant l_night;
static FloatConstant l_nightAdv;
static DoubleConstant l_integerBias;
static Word l_endianMode;
static Word l_clearColor;
static Word l_backColor;
static OffsetReaders l_decodeOffset;
static FontManagerPointer l_fontManager;
static u8 l_phaseMethod;
static Word l_headerZeroA;
static Word l_headerZeroB;
static Word l_headerZeroC;
static Word l_headerZeroD;
static FloatConstant l_headerFloatA;
static FloatConstant l_headerFloatB;
static FloatConstant l_headerZero;
static FloatConstant l_headerOne;
static u8 l_headerTypeA;
static u8 l_headerTypeB;
static u8 l_headerColorA;
static u8 l_headerColorB;
static u8 l_headerColorC;

using stage_inf = menu_of_scene_class::stage_inf;
using room_inf = menu_of_scene_class::room_inf;
using menu_inf = menu_of_scene_class::menu_inf;

static stage_inf* selectedStage(menu_inf* info) {
    u32 index = l_cursolID;
    return gabi::at<stage_inf>(gabi::ea(info->stage.get()) +
                               index * static_cast<u32>(sizeof(stage_inf)));
}
static be<s8>* groupPoint(u32 index) {
    return gabi::at<be<s8>>(gabi::ea(l_groupPoint.get()) + index);
}
static room_inf* selectedRoom(menu_inf* info) {
    u32 index = l_cursolID;
    s32 room = *groupPoint(index);
    stage_inf* stage = gabi::at<stage_inf>(gabi::ea(info->stage.get()) +
                                           index * static_cast<u32>(sizeof(stage_inf)));
    return gabi::at<room_inf>(gabi::ea(stage->roomPtr.get()) +
                              static_cast<u32>(room) * static_cast<u32>(sizeof(room_inf)));
}
static void setString(MenuScene::SafeString* string, void* text) {
    string->mString = static_cast<be<u8>*>(text);
    string->__vtbl = &l_safeStringVtable;
}
static void setString(MenuScene::SafeString* string, const char* text) {
    string->mString = gabi::at<be<u8>>(gabi::guest_addr(text));
    string->__vtbl = &l_safeStringVtable;
}
static void terminate(MenuScene::SafeString* string) {
    gabi::call_ptr<u32>(string->__vtbl->assureTermination.get(), string);
}
static u32 stringLength(be<u8>* string) {
    u32 length = 0;
    while (static_cast<u8>(*string) != 0) {
        ++length;
        string = gabi::at<be<u8>>(gabi::ea(string) + 1);
        if (length > STRING_MAX_LENGTH)
            return 0;
    }
    return length;
}
static bool equalBytes(be<u8>* left, be<u8>* right, u32 count) {
    for (u32 i = 0; i < count; ++i) {
        u8 a = *left, b = *right;
        if (a != b)
            return false;
        if (!a)
            return true;
        left = gabi::at<be<u8>>(gabi::ea(left) + 1);
        right = gabi::at<be<u8>>(gabi::ea(right) + 1);
    }
    return true;
}
// HD: SafeString::find keeps the native temporary slots and virtual termination calls.
static bool contains(MenuScene::SafeString* haystack, MenuScene::SafeString* needle,
                     MenuScene::SafeString* candidate) {
    terminate(haystack);
    u32 haystackLength = stringLength(haystack->mString);
    terminate(needle);
    u32 needleLength = stringLength(needle->mString);
    s32 last = static_cast<s32>(haystackLength - needleLength);
    if (last < 0)
        return false;
    for (u32 offset = 0; static_cast<s32>(offset) <= last; ++offset) {
        setString(candidate, gabi::at<void>(gabi::ea(haystack->mString.get()) + offset));
        // GHS's direct empty template call has no explicit argument register.
        MenuString_terminateDirect();
        terminate(candidate);
        be<u8>* left = candidate->mString;
        terminate(needle);
        be<u8>* right = needle->mString;
        if (gabi::ea(left) == gabi::ea(right) || static_cast<s32>(needleLength) <= 0 ||
            equalBytes(left, right, needleLength))
            return offset != 0xFFFFFFFFu;
    }
    return false;
}

/* 025AC790 */
static s32 dScnMenu_Draw(menu_of_scene_class* i_this) {
    // HD: the debug text draw is disabled.
    return 1;
}

/* 025AC798 */
static s32 dScnMenu_Execute(menu_of_scene_class* i_this) {
    gabi::Local<u8[176]> storage;
    ExecuteFrame* frame = reinterpret_cast<ExecuteFrame*>(storage.get());
    menu_inf* info = i_this->info;
    if (MenuPad_holdUp(0) || MenuPad_holdDown(0)) {
        bool trig = true;
        if (MenuPad_trigUp(0) || MenuPad_trigDown(0))
            i_this->field_0x1e1 = REPEAT_DELAY;
        else if (Menu_calcTimer(&i_this->field_0x1e1) == 0)
            i_this->field_0x1e1 = REPEAT_INTERVAL;
        else
            trig = false;
        if (trig) {
            bool up = MenuPad_holdUp(0);
            u32 id = l_cursolID;
            if (up) {
                id -= 1;
                l_cursolID = id;
                if (static_cast<s32>(id) < 0)
                    l_cursolID = static_cast<u32>(info->num) - 1;
            } else {
                id += 1;
                l_cursolID = id;
                if (static_cast<s32>(id) >= static_cast<s32>(info->num))
                    l_cursolID = 0;
            }
        }
    }
    if (MenuPad_holdB(0) || MenuPad_holdA(0)) {
        bool trig = true;
        if (MenuPad_trigB(0) || MenuPad_trigA(0))
            i_this->field_0x1e2 = REPEAT_DELAY;
        else if (Menu_calcTimer(&i_this->field_0x1e2) == 0)
            i_this->field_0x1e2 = REPEAT_INTERVAL;
        else
            trig = false;
        if (trig) {
            // GHS reloads selection and resource pointers after the byte store.
            bool back = MenuPad_holdB(0);
            u32 id = l_cursolID;
            be<s8>* point = groupPoint(id);
            s32 room;
            if (back) {
                room = static_cast<s8>(static_cast<u8>(*point) - 1);
                *point = room;
                if (room < 0) {
                    id = l_cursolID;
                    stage_inf* stage = gabi::at<stage_inf>(
                        gabi::ea(info->stage.get()) + id * static_cast<u32>(sizeof(stage_inf)));
                    u32 last = static_cast<u32>(stage->roomNum) - 1;
                    *groupPoint(id) = last;
                }
            } else {
                room = static_cast<s8>(static_cast<u8>(*point) + 1);
                *point = room;
                id = l_cursolID;
                stage_inf* stage = gabi::at<stage_inf>(gabi::ea(info->stage.get()) +
                                                       id * static_cast<u32>(sizeof(stage_inf)));
                if (room >= static_cast<s32>(stage->roomNum))
                    *groupPoint(id) = 0;
            }
        }
    }
    // HD: start-code shoulder-button adjustment is absent; startCode remains externally supplied.
    // HD: readable spellings below map to the original Shift-JIS menu strings.
    if (MenuPad_trigStart(0)) {
        setString(&frame->demoName, "demo");
        setString(&frame->stageName, selectedStage(info));
        MenuString_assureTermination(&frame->demoName);
        terminate(&frame->demoName);
        be<u8>* demo = frame->demoName.mString;
        terminate(&frame->stageName);
        be<u8>* stage = frame->stageName.mString;
        bool isDemo =
            gabi::ea(demo) == gabi::ea(stage) || equalBytes(demo, stage, STRING_COMPARE_LIMIT);
        if (isDemo) {
            setString(&frame->roomName, selectedRoom(info));
            u32 id = l_cursolID;
            s32 room = *groupPoint(id);
            bool demo23 = true;
            if (room > 23) {
                setString(&frame->needleFirst, "(44)");
                demo23 = contains(&frame->roomName, &frame->needleFirst, &frame->candidateFirst);
                if (!demo23) {
                    setString(&frame->needleSecond, "(49)");
                    demo23 =
                        contains(&frame->roomName, &frame->needleSecond, &frame->candidateSecond);
                }
                if (!demo23) {
                    setString(&frame->needleLast, "(50)");
                    demo23 = contains(&frame->roomName, &frame->needleLast, &frame->candidateThird);
                }
            }
            if (demo23)
                MenuEvent_off(l_save->mEvent, EVENT_DEMO23);
            else
                MenuEvent_on(l_save->mEvent, EVENT_DEMO23);
            setString(&frame->needleLast, "(19)");
            if (contains(&frame->roomName, &frame->needleLast, &frame->candidateLast))
                MenuEvent_on(l_save->mEvent, EVENT_DEMO40);
        }
        room_inf* room = selectedRoom(info);
        Menu_getPlay()->mEnableNextStage = 0;
        u32 start = i_this->startCode;
        s32 roomNo = room->roomNo;
        u32 startCode = start ? static_cast<u32>(static_cast<s16>(start - 1))
                              : static_cast<u32>(room->startCode);
        f64 time = l_zero;
        s32 layer = room->layerNo;
        Menu_setNextStage(room->stageName, startCode, roomNo, layer, 0, 1, 0, time);
        Menu_changeScene(i_this, PLAY_SCENE, OVERLAP0, 5, 0);
        l_save->mRestartRoomParam = 0;
        Play* next = Menu_getPlay();
        StartStage* name = &next->mNextStage;
        s32 nextRoom = Menu_getPlay()->mNextStage.mRoomNo;
        s32 nextLayer = Menu_getPlay()->mNextStage.mLayer;
        Menu_setAudioScene(name, nextRoom, nextLayer);
    }
    if (MenuPad_trigY(0)) {
        u32 time = static_cast<u32>(l_timepat) - 1;
        l_timepat = static_cast<s32>(time) < 0 ? 29 : time;
    } else if (MenuPad_trigX(0)) {
        u32 time = static_cast<u32>(l_timepat) + 1;
        l_timepat = static_cast<s32>(time) > 29 ? 0 : time;
    }
    if (MenuPad_trigLeft(0)) {
        s32 week = static_cast<s16>(static_cast<s32>(l_weekpat) - 1);
        l_weekpat = week < 0 ? 6 : week;
    } else if (MenuPad_trigRight(0)) {
        s32 week = static_cast<s16>(static_cast<s32>(l_weekpat) + 1);
        l_weekpat = week >= 7 ? 0 : week;
    }
    l_save->mDate = static_cast<s16>(l_weekpat);
    Environment* environment = Menu_getEnvironment();
    f64 zero = l_zero;
    f64 advance = l_fastTimeAdv;
    environment->mTimeAdv = zero;
    s32 timepat = l_timepat;
    if (timepat >= 6) {
        // HD: retain the native signed integer-to-double conversion and single rounding.
        frame->conversionHigh = 0x43300000u;
        frame->conversionLow = (static_cast<u32>(timepat) - 6) ^ 0x80000000u;
        f64 encoded = gabi::load<f64>(gabi::ea(&frame->conversionHigh));
        f64 hours = to_single(encoded - static_cast<f64>(l_integerBias));
        f64 step = l_hourStep;
        f64 time = to_single(hours * round25(step));
        l_save->mTime = time;
        Menu_getEnvironment()->mTimeAdv = advance;
    } else {
        switch (static_cast<u32>(timepat)) {
        case 0: {
            f64 time = l_normalTime;
            f64 adv = l_normalTimeAdv;
            l_save->mTime = time;
            Menu_getEnvironment()->mTimeAdv = adv;
        } break;
        case 1: {
            f64 time = l_normalTime;
            f64 adv = l_fastAdv;
            l_save->mTime = time;
            Menu_getEnvironment()->mTimeAdv = adv;
        } break;
        case 2: {
            f64 time = l_morning;
            l_save->mTime = time;
        }
            environment = Menu_getEnvironment();
            {
                f64 time = l_morning;
                environment->mCurTime = time;
            }
            Menu_getEnvironment()->mTimeAdv = advance;
            break;
        case 3: {
            f64 time = l_noon;
            l_save->mTime = time;
            environment = Menu_getEnvironment();
            f64 adv = l_noonAdv;
            environment->mCurTime = time;
            Menu_getEnvironment()->mTimeAdv = adv;
        } break;
        case 4: {
            f64 time = l_evening;
            l_save->mTime = time;
            environment = Menu_getEnvironment();
            f64 adv = l_eveningAdv;
            environment->mCurTime = time;
            Menu_getEnvironment()->mTimeAdv = adv;
        } break;
        case 5: {
            f64 time = l_night;
            l_save->mTime = time;
            environment = Menu_getEnvironment();
            f64 adv = l_nightAdv;
            environment->mCurTime = time;
            Menu_getEnvironment()->mTimeAdv = adv;
        } break;
        }
    }
    return 1;
}

/* 025AD550 */
static s32 dScnMenu_IsDelete(menu_of_scene_class* i_this) { return 1; }

/* 025AD558 */
static s32 dScnMenu_Delete(menu_of_scene_class* i_this) {
    Font* font = i_this->font;
    if (font)
        gabi::call_ptr(font->__vtbl->deletingDestructor.get(), font, 3);
    Menu_free(i_this->info);
    Menu_free(i_this->fontRes);
    u8 flags = static_cast<u8>(l_displayFlags) & ~2;
    Save* save = l_save;
    l_displayFlags = flags;
    Menu_setRestartOption(save->mRestart, 0);
    // HD: menu lifecycle also toggles the frontend mode.
    Menu_setFrontendMode(0);
    return 1;
}

/* 025AD5E0 */
static s32 phase_1(menu_of_scene_class* i_this) {
    DvdCommand* command = Menu_createDvd("/Common/Debug/DebugMenu/Menu1.dat", 0, 0);
    i_this->command = command;
    if (!command)
        Menu_assert("d_s_menu.cpp", 0x308, "i_this->command != NULL");
    command = Menu_createDvd("/Common/Debug/DebugMenu/kanfont_fix16.bfn", 0, 0);
    i_this->fontCommand = command;
    if (!command)
        Menu_assert("d_s_menu.cpp", 0x30B, "i_this->fontCommand != NULL");
    return cPhs_NEXT_e;
}

/* 025AD680 */
static s32 phase_2(menu_of_scene_class* i_this) {
    DvdCommand* command = i_this->command;
    if (!command->mSync)
        return cPhs_INIT_e;
    DvdCommand* fontCommand = i_this->fontCommand;
    if (!fontCommand->mSync)
        return cPhs_INIT_e;
    menu_inf* info = static_cast<menu_inf*>(command->mMemAddress.get());
    i_this->info = info;
    if (!info)
        Menu_assert("d_s_menu.cpp", 0x338, "i_this->info != NULL");
    command = i_this->command;
    if (command)
        gabi::call_ptr(command->__vtbl->deletingDestructor.get(), command, 3);
    // HD: resource-relative pointers are decoded through the selected endian reader.
    u32 endian = l_endianMode;
    gabi::gfn* decoder = gabi::at<gabi::gfn>(gabi::ea(&l_decodeOffset) + endian * 4);
    info = i_this->info;
    u32 offset = gabi::ea(info->stage.get());
    u32 decoded = gabi::call_ptr<u32>(decoder->get(), offset);
    u32 num = info->num;
    stage_inf* stages = gabi::at<stage_inf>(decoded + gabi::ea(info));
    info->stage = stages;
    if (num > 0) {
        u32 target = decoder->get();
        for (u32 i = 0;; ++i) {
            stage_inf* stage =
                gabi::at<stage_inf>(gabi::ea(stages) + i * static_cast<u32>(sizeof(stage_inf)));
            u32 room = gabi::ea(stage->roomPtr.get());
            room = gabi::call_ptr<u32>(target, room);
            stages = info->stage;
            stage = gabi::at<stage_inf>(gabi::ea(stages) + i * static_cast<u32>(sizeof(stage_inf)));
            stage->roomPtr = gabi::at<room_inf>(room + gabi::ea(info));
            num = info->num;
            if (i + 1 >= num)
                break;
            stages = info->stage;
        }
    }
    if (!l_groupPoint) {
        be<s8>* points = Menu_newArray(num);
        l_groupPoint = points;
        if (!points)
            Menu_assert("d_s_menu.cpp", 0x353, "l_groupPoint != NULL");
        for (u32 i = 0; i < static_cast<u32>(info->num); ++i)
            *groupPoint(i) = 0;
    }
    fontCommand = i_this->fontCommand;
    void* font = fontCommand->mMemAddress;
    i_this->fontRes = font;
    if (fontCommand) {
        gabi::call_ptr(fontCommand->__vtbl->deletingDestructor.get(), fontCommand, 3);
        font = i_this->fontRes;
    }
    // HD: this build uses an existing font instance rather than allocating myFontClass.
    if (font) {
        Font* instance = i_this->font;
        if (instance)
            Menu_changeFont(l_fontManager, instance);
    }
    be<u32>* system = Menu_getSystemClock();
    u32 clock = *system;
    u32 high = (static_cast<u64>(0x88888889u) * (clock >> 2)) >> 32;
    Menu_setTickRate(high >> 5);
    l_backColor = static_cast<u32>(l_clearColor);
    l_displayFlags = static_cast<u8>(l_displayFlags) | 2;
    return cPhs_COMPLEATE_e;
}

/* 025AD8AC */
static s32 dScnMenu_Create(menu_of_scene_class* i_this) {
    Save* save = l_save;
    Option* option = Menu_getOption(save->mOption);
    l_languageType = option->mLanguage;
    Menu_setFrontendMode(1);
    return Menu_phaseHandler(i_this->mPhs, &l_phaseMethod, i_this);
}

// HD: compiler-generated and template code of this translation unit.
/* 025AD90C */
static void __sinit_d_s_menu_cpp() {
    l_headerZeroA = 0;
    l_headerZeroB = 0;
    l_headerZeroC = 0;
    l_headerZeroD = 0;
    Menu_initColor(&l_headerColorA);
    f32 zero = l_headerZero, one = l_headerOne;
    l_headerFloatA = zero;
    l_headerFloatB = one;
    Menu_initTypeA(&l_headerTypeA);
    Menu_initColor(&l_headerColorB);
    Menu_initTypeB(&l_headerTypeB);
    Menu_initColor(&l_headerColorC);
}
/* 025AD9A0 */
static void SafeString_deletingDtor_d_s_menu(u32 p, u32 flags) {
    if (p && (flags & 1))
        Menu_delete(p);
}
/* 025AD9B4 */
static void SafeString_assureTerminationImpl_d_s_menu(u32 p) {}
/* WWHD glue
data l_cursolID 101EA6C4
data l_timepat 101EA6C8
data l_groupPoint 101EA6CC
data l_weekpat 101EA6D0
data l_languageType 101EA6D2
data l_save 101F84DC
data l_displayFlags 104873CF
data l_safeStringVtable 100528C4
str 1005291C "demo" renamed
str 10052924 "(44)" renamed
str 10052930 "(49)" renamed
str 1005293C "(50)" renamed
str 10052948 "(19)" renamed
data l_zero 100528DC
data l_fastTimeAdv 100528E0
data l_integerBias 100528E8
data l_hourStep 100528F0
data l_normalTime 100528F4
data l_normalTimeAdv 100528F8
data l_fastAdv 100528FC
data l_morning 10052900
data l_noon 10052904
data l_noonAdv 10052908
data l_evening 1005290C
data l_eveningAdv 10052910
data l_night 10052914
data l_nightAdv 10052918
data l_endianMode 104A0CD4
data l_decodeOffset 10147620
data l_fontManager 101F97D4
data l_backColor 101F4818
data l_clearColor 101D5E90
data l_phaseMethod 101EA6E8
data l_headerZeroA 1047B3AC
data l_headerZeroB 1047B3A4
data l_headerZeroC 1047B3B0
data l_headerZeroD 1047B3A8
data l_headerColorA 101EA6F0
data l_headerFloatA 1047B398
data l_headerFloatB 1047B39C
data l_headerZero 10052A24
data l_headerOne 10052A28
data l_headerTypeA 1047B3A0
data l_headerColorB 101EA6FC
data l_headerTypeB 1047B3A1
data l_headerColorC 101EA708
str 10052954 "/Common/Debug/DebugMenu/Menu1.dat"
str 10052978 "/Common/Debug/DebugMenu/kanfont_fix16.bfn"
str 100529A4 "d_s_menu.cpp" in 025AD5E0
str 10052A10 "d_s_menu.cpp" in 025AD680
str 100529B4 "i_this->command != NULL"
str 100529CC "i_this->fontCommand != NULL"
str 100529E8 "i_this->info != NULL"
str 100529FC "l_groupPoint != NULL"
ext 020075E0 u32 MenuPad_holdUp(u32)
ext 0200760C u32 MenuPad_holdDown(u32)
ext 02007764 u32 MenuPad_trigUp(u32)
ext 02007790 u32 MenuPad_trigDown(u32)
ext 020076E0 u32 MenuPad_holdB(u32)
ext 020076BC u32 MenuPad_holdA(u32)
ext 020078BC u32 MenuPad_trigB(u32)
ext 02007898 u32 MenuPad_trigA(u32)
ext 02007940 u32 MenuPad_trigStart(u32)
ext 02007914 u32 MenuPad_trigY(u32)
ext 020078E8 u32 MenuPad_trigX(u32)
ext 020077BC u32 MenuPad_trigLeft(u32)
ext 020077E8 u32 MenuPad_trigRight(u32)
ext 0207A9A0 u32 Menu_calcTimer(be<u8>*)
ext 025AD9B4 u32 MenuString_assureTermination(MenuScene::SafeString*)
ext 025AD9B4 u32 MenuString_terminateDirect()
ext 025B8B7C u32 MenuEvent_off(void*, u32)
ext 025B8B68 u32 MenuEvent_on(void*, u32)
ext 025200D4 MenuScene::Play* Menu_getPlay()
ext 0252012C u32 Menu_setNextStage(void*, u32, s32, s32, u32, u32, u32, f64)
ext 025DC86C u32 Menu_changeScene(menu_of_scene_class*, u32, u32, u32, u32)
ext 025E17CC u32 Menu_setAudioScene(MenuScene::StartStage*, s32, s32)
ext 02555D0C MenuScene::Environment* Menu_getEnvironment()
ext 027EC0B0 void Menu_free(void*)
ext 025B9864 void Menu_setRestartOption(void*, u32)
ext 0259170C void Menu_setFrontendMode(u32)
ext 025E255C MenuScene::DvdCommand* Menu_createDvd(const char*, u32, u32)
ext 0273AA24 void Menu_assert(const char*, u32, const char*)
ext 0273ADAC be<s8>* Menu_newArray(u32)
ext 027ECCC8 void Menu_changeFont(void*, MenuScene::Font*)
ext C0009C80 be<u32>* Menu_getSystemClock()
ext 025F096C void Menu_setTickRate(u32)
ext 027200D0 MenuScene::Option* Menu_getOption(void*)
ext 02525FE4 s32 Menu_phaseHandler(void*, void*, menu_of_scene_class*)
ext 028F026C void Menu_initColor(void*)
ext 028ED6F8 void Menu_initTypeA(void*)
ext 028EAB2C void Menu_initTypeB(void*)
ext 0273AF40 void Menu_delete(u32)
*/

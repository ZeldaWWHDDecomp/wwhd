/* hd_text_unit.h: shared inline helpers of the hd_text_unit sources (HD message text builder), WWHD.
 * sead string operations as GHS inlines them. */
#pragma once
#include "bindings.h"
#include "d/hd_msg_text.h"
#include "d/hd_text_tag.h"
#include "d/hd_msg_res_mgr.h"
#include "gc_api/d/d_com_inf_game.h"

using gabi::be;

using hdUnitTagHandler = u32 (*)(u32, u32, u32, u32, u32, u32);
u32 textUnitNative_putPlayerName_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putEventCount_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putRupeeCount_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putPlayMessage1_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putPlayMessage2_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putSwordGameDigits_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putBlows_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putEventMessage_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putPlayCount2_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putLetters_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putKorokCount_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putSaveTimer_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putMeters_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putEvent86FF_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putSavedPendants_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putPendantsEvent_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putPlayTimer_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putSavedBombs_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putSavedArrows_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putEvent8AFF_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putPlayCount_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putPlayCountX10_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putSeeds_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putNecklaces_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putChuchus_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putPendants_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putFeathers_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putCrests_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putStoredRupees_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putSeedDigits_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putNecklaceDigits_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putChuchuDigits_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putPendantDigits_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putFeatherDigits_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putCrestDigits_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_tagNoEndSe_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_tagEndSe_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_tagWait_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_tagWaitAuto_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_tagWaitInput_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_tagSpeed_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_tagChoice2_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_tagChoice3_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_tagRuby_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_tagRuby2_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_tagCapital_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_copyPaneStyle_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);

// HD: Private native bindings preserve every verified register argument.
void textUnitNative_registerGlobalObject_1(u32 arg0);
void textUnitNative_constructByte_1(u32 arg0);
void textUnitNative_constructFlag_1(u32 arg0);
u32 textUnitNative_onTextEnd_2(u32 arg0, u32 arg1);
void textUnitNative_playEndSe_1(u32 arg0);
void textUnitNative_setEndState_1(u32 arg0);
void textUnitNative_cutLines_2(u32 arg0, u32 arg1);
void textUnitNative_recordLineEnd_2(u32 arg0, u32 arg1);
u32 textUnitNative_putChar_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3);
u32 textUnitNative_newLine_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3);
u32 textUnitNative_boxFull_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3);
u32 textUnitNative_tagDispatch_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
void textUnitNative_advancePictograph_2(u32 arg0, u32 arg1);
u32 textUnitNative_convertPictograph_2(u32 arg0, u32 arg1);
u32 textUnitNative_group1_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_group0_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_getPlayInfo_0();
u32 textUnitNative_canStop_1(u32 arg0);
void textUnitNative_playMessageSound_3(u32 arg0, u32 arg1, u32 arg2);
u32 textUnitNative_getRoomReverb_1(u32 arg0);
u32 textUnitNative_putValueTag_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putNumber_7(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5, u32 arg6);
u32 textUnitNative_leadingZeros_2(u32 arg0, u32 arg1);
void textUnitNative_assureWideString_1(u32 arg0);
u32 textUnitNative_getLabelText_2(u32 arg0, u32 arg1);
u32 textUnitNative_findLabel_2(u32 arg0, u32 arg1);
u32 textUnitNative_getUnitSet_1(u32 arg0);
u32 textUnitNative_convertToWide_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3);
void textUnitNative_moveBytes_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3);
void textUnitNative_assureWideBuffer_1(u32 arg0);
u32 textUnitNative_getPlayerName_1(u32 arg0);
void textUnitNative_capitalize_2(u32 arg0, u32 arg1);
void textUnitNative_advanceCharacter_2(u32 arg0, u32 arg1);
u32 textUnitNative_mapChar_2(u32 arg0, u32 arg1);
void textUnitNative_formatString_3(u32 arg0, u32 arg1, u32 arg2);
void textUnitNative_formatString_2(u32 arg0, u32 arg1);
u32 textUnitNative_onlyTagsBetween_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3);
u32 textUnitNative_putDigitsLabel_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_getTimer_1(u32 arg0);
void textUnitNative_loadMessage_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3);
void textUnitNative_constructMessage_1(u32 arg0);
void textUnitNative_assureString_1(u32 arg0);
u32 textUnitNative_putCharacters_5(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4);
u32 textUnitNative_getPaneFont_1(u32 arg0);
u32 textUnitNative_getFont_2(u32 arg0, u32 arg1);
u32 textUnitNative_scaleTag_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3);
u32 textUnitNative_getProject_1(u32 arg0);
u32 textUnitNative_allocate_1(u32 arg0);
void textUnitNative_destroyArray_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putMessage_5(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4);
u32 textUnitNative_putTimer_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3);
u32 textUnitNative_putNumberWithUnit_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_putLocalizedUnit_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3);
u32 textUnitNative_putAsciiNumber_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
u32 textUnitNative_getEventRegister_2(u32 arg0, u32 arg1);


// HD: sead strings store the buffer before the vtable on this ABI.
struct hdUnitSafeString_c { be<u32> mBuffer; be<u32> mVtable; };
struct hdUnitBufferedString_c { hdUnitSafeString_c mString; be<s32> mCapacity; };
struct hdUnitInlineWideBuffer_c { hdUnitBufferedString_c mHeader; be<u16> mFirstCharacter; };
WWHD_OFFSET(hdUnitInlineWideBuffer_c, mFirstCharacter, 0xC);
WWHD_SIZE(hdUnitInlineWideBuffer_c, 0xE);
struct hdUnitWideFixed32_c { hdUnitBufferedString_c mHeader; be<u16> mBuffer[32]; };
WWHD_SIZE(hdUnitWideFixed32_c, 0x4C);
struct hdUnitFixedNarrow1_c { hdUnitBufferedString_c mHeader; be<u8> mBuffer[1]; u8 padding[3]; };
WWHD_SIZE(hdUnitSafeString_c, 8);
WWHD_SIZE(hdUnitBufferedString_c, 12);
WWHD_SIZE(hdUnitFixedNarrow1_c, 16);
struct hdUnitColor_c { be<u8> mRed, mGreen, mBlue, mAlpha; };
struct hdTextUnitStatics_c {
    /* 0x000 */ hdUnitSafeString_c paneName;
    /* 0x008 */ hdUnitSafeString_c defaultName;
    /* 0x010 */ hdUnitSafeString_c eventCountUnit;
    /* 0x018 */ hdUnitSafeString_c swordGameUnit;
    /* 0x020 */ hdUnitSafeString_c blowsRuby;
    /* 0x028 */ hdUnitSafeString_c lettersRuby;
    /* 0x030 */ hdUnitSafeString_c savedPendantsRuby;
    /* 0x038 */ hdUnitSafeString_c pendantsEventRuby;
    /* 0x040 */ hdUnitSafeString_c savedBombsRuby;
    /* 0x048 */ hdUnitSafeString_c seedsRuby;
    /* 0x050 */ hdUnitSafeString_c necklacesRuby;
    /* 0x058 */ hdUnitSafeString_c chuchusRuby;
    /* 0x060 */ hdUnitSafeString_c pendantsRuby;
    /* 0x068 */ hdUnitSafeString_c feathersRuby;
    /* 0x070 */ hdUnitSafeString_c crestsRuby;
    /* 0x078 */ hdUnitSafeString_c seedDigitsRuby;
    /* 0x080 */ hdUnitSafeString_c seedDigitsUnit;
    /* 0x088 */ hdUnitSafeString_c necklaceDigitsRuby;
    /* 0x090 */ hdUnitSafeString_c necklaceDigitsUnit;
    /* 0x098 */ hdUnitSafeString_c chuchuDigitsRuby;
    /* 0x0A0 */ hdUnitSafeString_c chuchuDigitsUnit;
    /* 0x0A8 */ hdUnitSafeString_c pendantDigitsRuby;
    /* 0x0B0 */ hdUnitSafeString_c pendantDigitsUnit;
    /* 0x0B8 */ hdUnitSafeString_c featherDigitsRuby;
    /* 0x0C0 */ hdUnitSafeString_c featherDigitsUnit;
    /* 0x0C8 */ hdUnitSafeString_c crestDigitsRuby;
    /* 0x0D0 */ hdUnitSafeString_c crestDigitsUnit;
    /* 0x0D8 */ hdUnitSafeString_c fallbackText;
    /* 0x0E0 */ be<f32> rangeLow;
    /* 0x0E4 */ be<f32> rangeHigh;
    /* 0x0E8 */ hdUnitColor_c redColor;
    /* 0x0EC */ hdUnitColor_c whiteColor;
    /* 0x0F0 */ be<u8> flag;
    /* 0x0F1 */ be<u8> byte;
    /* 0x0F2 */ u8 padding[2];
    /* 0x0F4 */ hdUnitSafeString_c rupeeUnits[2];
    /* 0x104 */ hdUnitSafeString_c blowUnits[2];
    /* 0x114 */ hdUnitSafeString_c countX10Units[2];
    /* 0x124 */ hdUnitSafeString_c letterUnits[2];
    /* 0x134 */ hdUnitSafeString_c korokUnits[2];
    /* 0x144 */ hdUnitSafeString_c meterUnits[2];
    /* 0x154 */ hdUnitSafeString_c event86FFUnits[2];
    /* 0x164 */ hdUnitSafeString_c savedPendantsUnits[2];
    /* 0x174 */ hdUnitSafeString_c pendantsEventUnits[2];
    /* 0x184 */ hdUnitSafeString_c savedBombsUnits[2];
    /* 0x194 */ hdUnitSafeString_c savedArrowUnits[2];
    /* 0x1A4 */ hdUnitSafeString_c seedsUnits[2];
    /* 0x1B4 */ hdUnitSafeString_c necklacesUnits[2];
    /* 0x1C4 */ hdUnitSafeString_c chuchusUnits[2];
    /* 0x1D4 */ hdUnitSafeString_c pendantsUnits[2];
    /* 0x1E4 */ hdUnitSafeString_c feathersUnits[2];
    /* 0x1F4 */ hdUnitSafeString_c crestsUnits[2];
    /* 0x204 */ hdUnitSafeString_c storedRupeeUnits[2];
    /* 0x214 */ hdUnitFixedNarrow1_c setName;
    /* 0x224 */ hdUnitSafeString_c minuteUnits[2];
    /* 0x234 */ hdUnitSafeString_c separators[2];
    /* 0x244 */ hdUnitSafeString_c secondUnits[2];
    /* 0x254 */ be<u32> zeroWords[4];
    /* 0x264 */ hdUnitSafeString_c fullWidthDigits[10];
    /* 0x2B4 */ hdUnitSafeString_c asciiDigits[10];
    /* 0x304 */ be<u32> palette[27];
    /* 0x370 */ be<u32> paneNameGuard;
    /* 0x374 */ be<u32> defaultNameGuard;
    /* 0x378 */ be<u32> asciiDigitsGuard;
    /* 0x37C */ be<u32> eventCountUnitGuard;
    /* 0x380 */ be<u32> rupeeUnitsGuard;
    /* 0x384 */ be<u32> fallbackTextGuard;
    /* 0x388 */ be<u32> setNameGuard;
    /* 0x38C */ be<u32> swordGameUnitGuard;
    /* 0x390 */ be<u32> blowsRubyGuard;
    /* 0x394 */ be<u32> blowUnitsGuard;
    /* 0x398 */ be<u32> countX10UnitsGuard;
    /* 0x39C */ be<u32> lettersRubyGuard;
    /* 0x3A0 */ be<u32> letterUnitsGuard;
    /* 0x3A4 */ be<u32> korokUnitsGuard;
    /* 0x3A8 */ be<u32> minuteUnitsGuard;
    /* 0x3AC */ be<u32> separatorsGuard;
    /* 0x3B0 */ be<u32> secondUnitsGuard;
    /* 0x3B4 */ be<u32> meterUnitsGuard;
    /* 0x3B8 */ be<u32> event86FFUnitsGuard;
    /* 0x3BC */ be<u32> savedPendantsRubyGuard;
    /* 0x3C0 */ be<u32> savedPendantsUnitsGuard;
    /* 0x3C4 */ be<u32> pendantsEventRubyGuard;
    /* 0x3C8 */ be<u32> pendantsEventUnitsGuard;
    /* 0x3CC */ be<u32> savedBombsRubyGuard;
    /* 0x3D0 */ be<u32> savedBombsUnitsGuard;
    /* 0x3D4 */ be<u32> savedArrowUnitsGuard;
    /* 0x3D8 */ be<u32> seedsRubyGuard;
    /* 0x3DC */ be<u32> seedsUnitsGuard;
    /* 0x3E0 */ be<u32> necklacesRubyGuard;
    /* 0x3E4 */ be<u32> necklacesUnitsGuard;
    /* 0x3E8 */ be<u32> chuchusRubyGuard;
    /* 0x3EC */ be<u32> chuchusUnitsGuard;
    /* 0x3F0 */ be<u32> pendantsRubyGuard;
    /* 0x3F4 */ be<u32> pendantsUnitsGuard;
    /* 0x3F8 */ be<u32> feathersRubyGuard;
    /* 0x3FC */ be<u32> feathersUnitsGuard;
    /* 0x400 */ be<u32> crestsRubyGuard;
    /* 0x404 */ be<u32> crestsUnitsGuard;
    /* 0x408 */ be<u32> storedRupeeUnitsGuard;
    /* 0x40C */ be<u32> seedDigitsRubyGuard;
    /* 0x410 */ be<u32> seedDigitsUnitGuard;
    /* 0x414 */ be<u32> necklaceDigitsRubyGuard;
    /* 0x418 */ be<u32> necklaceDigitsUnitGuard;
    /* 0x41C */ be<u32> chuchuDigitsRubyGuard;
    /* 0x420 */ be<u32> chuchuDigitsUnitGuard;
    /* 0x424 */ be<u32> pendantDigitsRubyGuard;
    /* 0x428 */ be<u32> pendantDigitsUnitGuard;
    /* 0x42C */ be<u32> featherDigitsRubyGuard;
    /* 0x430 */ be<u32> featherDigitsUnitGuard;
    /* 0x434 */ be<u32> crestDigitsRubyGuard;
    /* 0x438 */ be<u32> crestDigitsUnitGuard;
    /* 0x43C */ be<u32> fullWidthDigitsGuard;
};
WWHD_SIZE(hdTextUnitStatics_c, 0x440);
inline hdTextUnitStatics_c& textUnitStatics() { return gabi::object_at<hdTextUnitStatics_c>(0x1048D880); }

// HD: Explicit frames retain GHS stack sizes and construction order.
struct hdUnitFixedNarrow32_c { hdUnitBufferedString_c mHeader; be<u8> mBuffer[32]; };
struct hdUnitFormatFrame_c { hdUnitBufferedString_c mHeader; be<u8> mBuffer[256]; u8 padding[4]; };
struct hdUnitCharacterFrame_c { be<u16> mCharacter; u8 padding[2]; hdUnitSafeString_c mString; };
struct hdUnitPlayerNameFrame_c { hdUnitSafeString_c mName; hdUnitWideFixed32_c mWideName; };
struct hdUnitNumberFrame_c { hdUnitFixedNarrow32_c mNarrow; hdUnitWideFixed32_c mWide; be<u32> mDigits[10]; };
struct hdUnitDigitFrame_c { hdUnitBufferedString_c mHeader; be<u16> mBuffer[4]; be<u16> mDigitChars[4]; hdUnitSafeString_c mString; be<u32> mPadding; };
struct hdUnitPaneStyleFrame_c { be<u16> mSizeX; be<u16> mSizeY; be<u32> mColor1; be<u32> mColor2; };
struct hdUnitTag_c {
    enum Code_e { NEW_LINE = 0xA, BEGIN_TAG = 0xE, END_TAG = 0xF };
    be<u16> mCode, mGroup, mTag, mParamSize;
    be<u16> mParameters[32];
};
inline hdUnitTag_c& tagAt(u32 address) { return *gabi::at<hdUnitTag_c>(address); }
WWHD_SIZE(hdUnitFixedNarrow32_c, 0x2C);
WWHD_SIZE(hdUnitFormatFrame_c, 0x110);
WWHD_SIZE(hdUnitCharacterFrame_c, 0xC);
WWHD_SIZE(hdUnitPlayerNameFrame_c, 0x54);
WWHD_SIZE(hdUnitNumberFrame_c, 0xA0);
WWHD_SIZE(hdUnitDigitFrame_c, 0x28);
WWHD_SIZE(hdUnitPaneStyleFrame_c, 0xC);
WWHD_OFFSET(hdUnitTag_c, mParameters, 8);

struct hdUnitStaticRecord_c { u8 mOpaque[12]; };
WWHD_SIZE(hdUnitStaticRecord_c, 12);
enum hdUnitStaticRecordId_e {
    RECORD_paneName,
    RECORD_defaultName,
    RECORD_asciiDigits,
    RECORD_eventCountUnit,
    RECORD_rupeeUnits,
    RECORD_fallbackText,
    RECORD_setName,
    RECORD_swordGameUnit,
    RECORD_blowsRuby,
    RECORD_blowUnits,
    RECORD_countX10Units,
    RECORD_lettersRuby,
    RECORD_letterUnits,
    RECORD_korokUnits,
    RECORD_minuteUnits,
    RECORD_separators,
    RECORD_secondUnits,
    RECORD_meterUnits,
    RECORD_event86FFUnits,
    RECORD_savedPendantsRuby,
    RECORD_savedPendantsUnits,
    RECORD_pendantsEventRuby,
    RECORD_pendantsEventUnits,
    RECORD_savedBombsRuby,
    RECORD_savedBombsUnits,
    RECORD_savedArrowUnits,
    RECORD_seedsRuby,
    RECORD_seedsUnits,
    RECORD_necklacesRuby,
    RECORD_necklacesUnits,
    RECORD_chuchusRuby,
    RECORD_chuchusUnits,
    RECORD_pendantsRuby,
    RECORD_pendantsUnits,
    RECORD_feathersRuby,
    RECORD_feathersUnits,
    RECORD_crestsRuby,
    RECORD_crestsUnits,
    RECORD_storedRupeeUnits,
    RECORD_seedDigitsRuby,
    RECORD_seedDigitsUnit,
    RECORD_necklaceDigitsRuby,
    RECORD_necklaceDigitsUnit,
    RECORD_chuchuDigitsRuby,
    RECORD_chuchuDigitsUnit,
    RECORD_pendantDigitsRuby,
    RECORD_pendantDigitsUnit,
    RECORD_featherDigitsRuby,
    RECORD_featherDigitsUnit,
    RECORD_crestDigitsRuby,
    RECORD_crestDigitsUnit,
    RECORD_fullWidthDigits,
    RECORD_headerFloatPair,
    RECORD_headerFlag,
    RECORD_headerByte,
    RECORD_COUNT
};
inline u32 staticRecordAddress(hdUnitStaticRecordId_e record) {
    return gabi::ea(gabi::at<hdUnitStaticRecord_c>(0x101F4C04) + record);
}

struct hdUnitColorPair_c { be<u8> mFirst, mSecond; };
struct hdUnitPaletteProject_c { u8 padding[4]; be<u32> mColorCount, mColors; };
struct hdUnitLayoutHolder_c { u8 padding[0x124]; be<u32> mDescription; };
struct hdUnitLayoutDescription_c { u8 padding[12]; be<u32> mLayout; };
struct hdUnitPaneVtable_c {
    u8 padding[0x14]; be<u32> mGetType;
    u8 padding18[0x5C - 0x18]; be<u32> mFindPane;
};
struct hdUnitPane_c {
    u8 padding[8]; be<u32> mVtable;
    u8 paddingC[0xAC - 0xC]; hdUnitColor_c mColor1, mColor2;
    u8 paddingB4[4]; be<f32> mFontSizeX, mFontSizeY;
};
struct hdUnitFontVtable_c { u8 padding[0x1C]; be<u32> mGetWidth; u8 padding20[4]; be<u32> mGetHeight; };
struct hdUnitFont_c { u8 padding[4]; be<u32> mVtable; };
struct hdUnitPaneTypeStatics_c {
    be<u32> mGuard; u8 padding4[0x10]; be<u32> mTypeGuard;
    u8 padding18[0x58]; be<u32> mType, mTypeEnd;
};
WWHD_SIZE(hdUnitColor_c, 4);
WWHD_SIZE(hdUnitColorPair_c, 2);
WWHD_SIZE(hdUnitPaletteProject_c, 12);
WWHD_OFFSET(hdUnitLayoutHolder_c, mDescription, 0x124);
WWHD_OFFSET(hdUnitLayoutDescription_c, mLayout, 12);
WWHD_OFFSET(hdUnitPaneVtable_c, mGetType, 0x14);
WWHD_OFFSET(hdUnitPaneVtable_c, mFindPane, 0x5C);
WWHD_OFFSET(hdUnitPane_c, mFontSizeX, 0xB8);
WWHD_SIZE(hdUnitPaneTypeStatics_c, 0x78);
inline hdUnitPaneTypeStatics_c& paneTypeStatics() { return *gabi::at<hdUnitPaneTypeStatics_c>(0x101FD888); }
inline u32 fontManagerAddress() { return *gabi::at<be<u32>>(0x101F4A50); }
inline u32 resourceManagerAddress() { return *gabi::at<be<u32>>(0x101F4AE8); }

// HD: Partial views expose only fields read by message insertions.
struct hdUnitPlayInfo_c {
    u8 padding[0x5B54]; be<u32> mMessage1, mMessage2;
    u8 padding5B5C[0x44]; be<s16> mCounter, mDigitCounter;
    u8 padding5BA4[6]; be<s16> mTimer;
    u8 padding5BAC[0x1A]; be<u8> mCameraMode;
    u8 padding5BC7[0x69]; be<u32> mTextValue;
    u8 padding5C34[0xB8]; be<s16> mCounter2;
};
struct hdUnitSaveInfo_c {
    u8 padding[0x86]; u8 mTimer; u8 padding87[8]; be<u8> mArrowCount, mBombCount;
    u8 padding91[0x32]; be<u8> mPendantCount;
    u8 paddingC4[0x11FC]; u8 mOptions;
};
struct hdUnitItemCounters_c { be<s16> mStoredRupees; u8 padding2[12]; be<u8> mItemCount; };
struct hdUnitDigitSelection_c { u8 padding[0x948]; be<u32> mSelectedDigit; };
WWHD_OFFSET(hdUnitPlayInfo_c, mCounter, 0x5BA0);
WWHD_OFFSET(hdUnitPlayInfo_c, mTimer, 0x5BAA);
WWHD_OFFSET(hdUnitPlayInfo_c, mCameraMode, 0x5BC6);
WWHD_OFFSET(hdUnitPlayInfo_c, mTextValue, 0x5C30);
WWHD_OFFSET(hdUnitPlayInfo_c, mCounter2, 0x5CEC);
WWHD_OFFSET(hdUnitSaveInfo_c, mArrowCount, 0x8F);
WWHD_OFFSET(hdUnitSaveInfo_c, mPendantCount, 0xC3);
WWHD_OFFSET(hdUnitSaveInfo_c, mOptions, 0x12C0);
WWHD_SIZE(hdUnitItemCounters_c, 0xF);
WWHD_OFFSET(hdUnitItemCounters_c, mItemCount, 0xE);
WWHD_OFFSET(hdUnitDigitSelection_c, mSelectedDigit, 0x948);
inline hdUnitItemCounters_c& itemCounters() { return *gabi::at<hdUnitItemCounters_c>(0x101D5F3A); }
inline u32 selectedDigit() { return gabi::at<hdUnitDigitSelection_c>(*gabi::at<be<u32>>(0x101F4B5C))->mSelectedDigit; }
inline u32 messageLanguage() { return gabi::at<MsgConfig>(*gabi::at<be<u32>>(0x101F4BAC))->mLanguage; }

struct hdUnitSoundPosition_c { be<f32> mX, mY, mZ; };
struct hdUnitNumberTag_c { be<u16> mCode, mGroup, mTag, mParamSize; be<u8> mMaxDigits, mNoPadding, mAscii; };
struct hdUnitTagByte_c { u8 padding[5]; be<u8> mTagLow; };
WWHD_SIZE(hdUnitTagByte_c, 6);
struct hdUnitEntry_c { u8 padding; be<u8> mTag, mKind; };
WWHD_SIZE(hdUnitSoundPosition_c, 12);
WWHD_OFFSET(hdUnitNumberTag_c, mMaxDigits, 8);
WWHD_OFFSET(hdUnitNumberTag_c, mAscii, 10);
WWHD_SIZE(hdUnitEntry_c, 3);

struct hdTextUnit_c {
    enum State_e { STATE_BUILDING = 7, STATE_TWO_CHOICES = 8, STATE_THREE_CHOICES = 9,
                   STATE_END_WAIT = 0xA, STATE_END = 0xE, STATE_RUBY = 0x14, STATE_DIGITS = 0x15 };
    enum Insertion_e { INSERTION_NORMAL, INSERTION_CHOICE, INSERTION_RUBY, INSERTION_DIGITS };
    enum Stop_e { STOP_WAIT_FRAMES, STOP_WAIT_INPUT };
    gabi::gptr<HdMsgText> mpMessage;
    s32 putNumberFull(hdUnitBufferedString_c* i_string, s32 pos, s32 v, u32 maxDigits, u32 noPad);
    s32 putValueTag(hdUnitBufferedString_c* i_string, s32 pos, u32 tag);
    s32 group2(hdUnitBufferedString_c* i_string, s32 pos, u32 tag, u32 a7, u32 a8);
    s32 tagSound(u32 tag);
    s32 tagPlayValue(u32 tag);
    s32 tagPlayByte(u32 tag);
    s32 tagDispatch(hdUnitBufferedString_c* i_string, s32 pos, u32 tag, u32 a7, u32 a8);
    s32 pictTag(hdUnitBufferedString_c* i_string, be<u32>* i_position, u32 hdr, u32 tag);
    void collectRubyBase(u32 text);
    u32 copyRubyText(u32 dst, u32 text);
    u32 processTag(hdUnitBufferedString_c* i_string, be<u32>* i_position, u32 tag, u32 ctx);
    s32 insertLineTag(hdUnitBufferedString_c* i_string, s32 pos);
    void markLineStart(hdUnitBufferedString_c* i_string, be<u32>* i_position);
    u32 processText(hdUnitBufferedString_c* i_string, be<u32>* i_position, u32 text, u32 ctx);
    void finishText(u32 text);
    s32 build(hdUnitBufferedString_c* i_string, u32 text, u32 m, u32 ctx);
    u32 onlyTagsBetween(u32 buf, s32 start, s32 end);
    void cutLines(hdUnitBufferedString_c* i_string);
    u32 boxFull(hdUnitBufferedString_c* i_string);
    s32 newLine(hdUnitBufferedString_c* i_string, s32 pos, u32 next);
    u32 mapChar(u32 c);
    void capitalize(u32 chp);
    s32 putChar(hdUnitBufferedString_c* i_string, s32 pos, u32 chp);
    s32 putCharacters(hdUnitBufferedString_c* i_string, s32 pos, u32 chars, s32 n);
    s32 putPlayerName(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putNumber(hdUnitBufferedString_c* i_string, s32 pos, s32 v, u32 digits, u32 maxDigits, u32 noPad);
    s32 putAsciiNumber(hdUnitBufferedString_c* i_string, s32 pos, s32 v, u32 maxDigits, u32 noPad);
    s32 leadingZeros(u32 s);
    s32 putLocalizedUnit(hdUnitBufferedString_c* i_string, s32 pos, u32 label);
    s32 putEventCount(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putRupeeCount(hdUnitBufferedString_c* i_string, s32 pos);
    u32 putTag(hdUnitBufferedString_c* i_string, be<u32>* i_position, u32 tag, u32 ctx);
    s32 putText(hdUnitBufferedString_c* i_string, s32 pos, u32 text, u32 ctx);
    s32 putMessage(hdUnitBufferedString_c* i_string, s32 pos, u32 id, u32 ctx);
    s32 putPlayMessage1(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx);
    s32 putPlayMessage2(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx);
    s32 putSwordGameDigits(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putNumberWithUnit(hdUnitBufferedString_c* i_string, s32 pos, s32 v, u32 ruby, u32 units);
    s32 putBlows(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putEventMessage(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx);
    s32 putPlayCount2(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putEvent8AFF(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putPlayCount(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putPlayCountX10(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putLetters(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putKorokCount(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putTimer(hdUnitBufferedString_c* i_string, s32 pos, s32 frames);
    s32 putSaveTimer(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putPlayTimer(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putMeters(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putEvent86FF(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putSavedPendants(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putPendantsEvent(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putSavedBombs(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putSavedArrows(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putSeeds(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putNecklaces(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putChuchus(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putPendants(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putFeathers(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putCrests(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putStoredRupees(hdUnitBufferedString_c* i_string, s32 pos);
    s32 putDigitsLabel(hdUnitBufferedString_c* i_string, s32 pos, u32 ruby, u32 unit, u32 ctx);
    s32 putSeedDigits(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx);
    s32 putNecklaceDigits(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx);
    s32 putChuchuDigits(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx);
    s32 putPendantDigits(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx);
    s32 putFeatherDigits(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx);
    s32 putCrestDigits(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx);
    void playEndSe();
    void setEndState();
    u32 onTextEnd(u32 ch);
    s32 insertFontTag(hdUnitBufferedString_c* i_string, s32 pos, u32 id);
    s32 fontTag(hdUnitBufferedString_c* i_string, s32 pos, u32 tag);
    s32 scaleTag(u32 tag);
    s32 insertColorTag(hdUnitBufferedString_c* i_string, s32 pos, u32 rgba);
    s32 colorTag(hdUnitBufferedString_c* i_string, s32 pos, u32 tag, u32 mode);
    s32 colorTagAuto(hdUnitBufferedString_c* i_string, s32 pos, u32 tag);
    s32 group0(hdUnitBufferedString_c* i_string, s32 pos, u32 tag);
    s32 tagNoEndSe();
    s32 tagEndSe();
    u32 canStop();
    void stopHere(u32 tag, u32 flag);
    s32 tagWait(hdUnitBufferedString_c* i_string, s32 pos, u32 tag);
    s32 tagWaitAuto(hdUnitBufferedString_c* i_string, s32 pos, u32 tag);
    s32 tagWaitInput(hdUnitBufferedString_c* i_string, s32 pos, u32 tag);
    s32 tagSpeed(hdUnitBufferedString_c* i_string, s32 pos, u32 tag);
    s32 choiceTag(u32 mode, u32 state);
    s32 tagChoice2();
    s32 tagChoice3();
    s32 tagRuby();
    s32 tagRuby2();
    s32 tagCapital();
    s32 insertPaneTag(hdUnitBufferedString_c* i_string, s32 pos, u32 c1, u32 c2, u32 sx, u32 sy);
    s32 copyPaneStyle(hdUnitBufferedString_c* i_string, s32 pos, u32 holder);
    s32 group1(hdUnitBufferedString_c* i_string, s32 pos, u32 tag, u32 a7, u32 a8);
};
WWHD_SIZE(hdTextUnit_c, 4);


WWHD_SIZE(hdUnitTag_c, 0x48);
WWHD_SIZE(hdUnitLayoutHolder_c, 0x128);
WWHD_SIZE(hdUnitLayoutDescription_c, 0x10);
WWHD_SIZE(hdUnitPaneVtable_c, 0x60);
WWHD_SIZE(hdUnitPane_c, 0xC0);
WWHD_SIZE(hdUnitFontVtable_c, 0x28);
WWHD_SIZE(hdUnitFont_c, 8);
WWHD_SIZE(hdUnitPlayInfo_c, 0x5CEE);
WWHD_SIZE(hdUnitSaveInfo_c, 0x12C1);
WWHD_SIZE(hdUnitDigitSelection_c, 0x94C);
WWHD_SIZE(hdUnitNumberTag_c, 0xB);
WWHD_OFFSET(hdTextUnitStatics_c, fullWidthDigits, 0x264);
WWHD_OFFSET(hdTextUnitStatics_c, asciiDigits, 0x2B4);
WWHD_OFFSET(hdTextUnitStatics_c, paneNameGuard, 0x370);
static_assert(RECORD_COUNT == 55);

namespace hd_text_unit {

// HD: Native destructor tokens passed unchanged to the array cleanup routine.
enum StringDestructor_e { SAFE_STRING_DESTRUCTOR = 0x02601A70, WIDE_STRING_DESTRUCTOR = 0x02601A84 };
using namespace gabi;

// HD: Message addresses remain 32-bit to preserve iterator wrap and caller ABI.
inline HdMsgText& messageAt(u32 address) { return *gabi::at<HdMsgText>(address); }


/* vtables of the TU's sead string instances and their devirtualised assureTermination members */
static const u32 kBufferedString256Vt = 0x100E0FF0;
static const u32 kFixedString32Vt = 0x100E1020;
static const u32 kWideBufferedString32Vt = 0x100E1038;
static const u32 kWideFixedString4Vt = 0x100E1068;
static const u32 kBufferedString1Vt = 0x100E1080;
static const u32 kFixedString1Vt = 0x100E1098;
static const u32 kSafeVt = 0x100E0F90;    /* SafeString */
static const u32 kWSafeVt = 0x100E0FC0;   /* WSafeString */
static const u32 kWFixed32Vt = 0x100E1050; /* WFixedSafeString<32> */

/* the message text object of the builder: *self */
static inline u32 msg(u32 self) { return gabi::ea(gabi::at<hdTextUnit_c>(self)->mpMessage.get()); }

struct hdUnitStringVtable_c {
    u8 padding[0x14];
    be<u32> mAssureTermination;
};
WWHD_OFFSET(hdUnitStringVtable_c, mAssureTermination, 0x14);
WWHD_SIZE(hdUnitStringVtable_c, 0x18);
inline hdUnitSafeString_c& stringAt(u32 address) { return *gabi::at<hdUnitSafeString_c>(address); }
inline hdUnitBufferedString_c& bufferAt(u32 address) { return *gabi::at<hdUnitBufferedString_c>(address); }
static inline void vAssure(u32 address) {
    const hdUnitSafeString_c& string = stringAt(address);
    const hdUnitStringVtable_c* vtable = gabi::at<hdUnitStringVtable_c>(string.mVtable);
    gabi::call_ptr<void>(vtable->mAssureTermination, address);
}

// HD: String iterators stay 32-bit, including wrap at zero; sead caps lengths at 0x40000.
static inline s32 cstrLength(u32 p) {
    s32 n = 0;
    if (!load<u8>(p)) return 0;
    for (;;) {
        n++;
        if (n > 0x40000) return 0;
        p++;
        if (!load<u8>(p)) return n;
    }
}
static inline s32 wcsLength(u32 p) {
    s32 n = 0;
    if (!load<u16>(p)) return 0;
    for (;;) {
        n++;
        if (n > 0x40000) return 0;
        p += 2;
        if (!load<u16>(p)) return n;
    }
}

/* sead WBufferedSafeString::copyAt(pos, src, cnt) as inlined (cnt < 0: whole src) */
static inline void wcopyAt(u32 dst, s32 pos, u32 src, s32 cnt) {
    const u32 base = u32(stringAt(dst).mBuffer);
    vAssure(dst);
    const s32 len = wcsLength(u32(stringAt(dst).mBuffer));
    if (pos < 0) {
        pos = len + pos + 1;
        if (pos < 0) pos = 0;
    }
    if (cnt < 0) {
        vAssure(src);
        cnt = wcsLength(u32(stringAt(src).mBuffer));
    }
    const s32 avail = s32(u32(bufferAt(dst).mCapacity)) - pos;
    if (cnt >= avail) cnt = avail - 1;
    if (cnt <= 0) return;
    vAssure(src);
    textUnitNative_moveBytes_4(base + u32(pos) * 2, u32(stringAt(src).mBuffer), u32(cnt) * 2, 0u);
    if (pos + cnt > len) store<u16>(base + u32(pos + cnt) * 2, 0);
}

/* a stack WSafeString {top, WSafeString vtable} */
static inline void wtmp(u32 t, u32 top) {
    stringAt(t).mBuffer = top;
    stringAt(t).mVtable = kWSafeVt;
}

/* a stack WFixedSafeString<N> (header at s, buffer at s + 0xC), assured */
static inline void wfixed(u32 s, u32 vt, u32 n) {
    const u32 buf = gabi::ea(&gabi::at<hdUnitInlineWideBuffer_c>(s)->mFirstCharacter);
    store<u16>(buf + 2 * n - 2, 0);
    bufferAt(s).mCapacity = n;
    stringAt(s).mBuffer = buf;
    store<u16>(buf, 0);
    stringAt(s).mVtable = vt;
    textUnitNative_assureWideBuffer_1(s);
}

/* function-local static SafeString / WSafeString */
static inline void staticStr(u32 guard, u32 obj, u32 lit, u32 vt, u32 rec) {
    if (load<u32>(guard)) return;
    store<u32>(guard, 1);
    stringAt(obj).mVtable = vt;
    stringAt(obj).mBuffer = lit;
    textUnitNative_registerGlobalObject_1(rec);
}
/* function-local static string array (placement-new null checks after the first element) */
static inline void staticArr(u32 guard, u32 obj, const u32* lits, u32 n, u32 vt, u32 rec) {
    if (load<u32>(guard)) return;
    store<u32>(guard, 1);
    stringAt(obj).mVtable = vt;
    stringAt(obj).mBuffer = lits[0];
    for (u32 i = 1; i < n; i++) {
        u32 q = obj + 8 * i;
        if (!q) q = textUnitNative_allocate_1(8u);
        if (q) {
            stringAt(q).mVtable = vt;
            stringAt(q).mBuffer = lits[i];
        }
    }
    textUnitNative_registerGlobalObject_1(rec);
}
static inline void staticArr(u32 guard, u32 obj, const char* const* lits, u32 n, u32 vt, u32 rec) {
    if (load<u32>(guard)) return;
    store<u32>(guard, 1);
    stringAt(obj).mVtable = vt;
    stringAt(obj).mBuffer = gabi::guest_addr(lits[0]);
    for (u32 i = 1; i < n; i++) {
        u32 q = obj + 8 * i;
        if (!q) q = textUnitNative_allocate_1(8u);
        if (q) {
            stringAt(q).mVtable = vt;
            stringAt(q).mBuffer = gabi::guest_addr(lits[i]);
        }
    }
    textUnitNative_registerGlobalObject_1(rec);
}
static inline void staticPair(u32 guard, u32 obj, u32 s0, u32 s1, u32 rec) {
    const u32 lits[2] = {s0, s1};
    staticArr(guard, obj, lits, 2, kSafeVt, rec);
}

/* save info (101F84DC) and play info (025200D4) */
static inline u32 saveInfo() { return dComIfGs_ea(); }
static inline u32 eventReg(u32 reg) { return dComIfGs_getEventRegHD(reg); }

/* the builder's functions in the other source files (called through their addresses) */
static inline s32 putCharactersNative(u32 self, u32 w, s32 pos, u32 chars, s32 n) { return s32(textUnitNative_putCharacters_5(self, w, u32(pos), chars, u32(n))); }
static inline s32 putAsciiNumberNative(u32 self, u32 w, s32 pos, s32 v, u32 maxD, u32 noPad) { return s32(textUnitNative_putAsciiNumber_6(self, w, u32(pos), u32(v), maxD, noPad)); }
static inline s32 putLocalizedUnitNative(u32 self, u32 w, s32 pos, u32 label) { return s32(textUnitNative_putLocalizedUnit_4(self, w, u32(pos), label)); }
static inline s32 putNumberWithUnitNative(u32 self, u32 w, s32 pos, s32 v, u32 ruby, u32 units) { return s32(textUnitNative_putNumberWithUnit_6(self, w, u32(pos), u32(v), ruby, units)); }
static inline s32 putTimerNative(u32 self, u32 w, s32 pos, s32 frames) { return s32(textUnitNative_putTimer_4(self, w, u32(pos), u32(frames))); }
static inline s32 putMessageNative(u32 self, u32 w, s32 pos, u32 id, u32 ctx) { return s32(textUnitNative_putMessage_5(self, w, u32(pos), id, ctx)); }

} // namespace hd_text_unit

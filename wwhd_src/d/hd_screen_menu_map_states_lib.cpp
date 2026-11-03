/* sead string instance emitted in the hd_screen_menu_map_states translation unit (HDTU-LIB): the variadic
 * constructor of WFormatFixedSafeString<8> (the floor and chart numbers of the map menu), which formats into its own storage.
 * Reconstruction from the game instructions; */
#include "bindings.h"
#include "sead/HdTuInstances.h"

// WideTextBuffer::formatArguments (02759D24) is bound by the glue of hd_btn_map_icon_number_lib.cpp.

static u8 sFormatWideTextVtable;

/* 0269EE74 */
sead::instances::screen_menu_map_states::NumberText* sead::instances::screen_menu_map_states::NumberText::construct(NumberText* text, const be<u16>* format, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<VariadicSaveArea> save(8);
    const u32 registers[8] = {gabi::ea(text), gabi::ea(format), a, b, c, d, e, f};
    saveVariadicRegisters(save.get(), registers);
    if (text == NULL) {
        text = static_cast<NumberText*>(operator_new(sizeof(NumberText)));
        if (text == NULL) return NULL;
    }
    // the buffered, fixed and format constructors, merged by GHS into one set of stores
    text->mCapacity = 8;
    text->mText = text->mStorage;
    text->mStorage[0] = 0;
    text->mVtable = &sFormatWideTextVtable;
    text->mStorage[7] = 0;
    startVariadicArguments(save.get(), 0x80);
    text->formatArguments(format, &save->mArguments);
    return text;
}

/* WWHD glue
data sFormatWideTextVtable 100FB234 nocheck
*/

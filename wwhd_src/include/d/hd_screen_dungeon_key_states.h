/* HD dungeon-key states. Companion to the merged sparkle update source. */
#pragma once
#include "d/hd_parts_no_control.h"
#include "d/hd_parts.h"
#include "d/hd_parts_text_pane.h"
using hdDungeonKeyText_c = PaneString_l;
struct hdDungeonKeyTextBuffer_c { be<u16> mCharacters[3]; };
struct hdDungeonKeyPaneTable_c { u8 _00[0x5c]; gfn mFindPane; };
struct hdDungeonKeyRootPane_c { u8 _00[8]; gptr<hdDungeonKeyPaneTable_c> mVtable; };
struct hdDungeonKeyGame_c { u8 _00[0x7b8]; be<u8> mKeyCount; };
/* a no-control part (FW-parts_msg_base) */
struct hdDungeonKeyStates_c : NoControlPart_l {
    be<u32> mSparkleActive[2];
    gptr<hdTextPane_c> mpTextPane;
    be<u32> mSparkleTimer,mMinimumInterval,mRandomInterval;
    be<u16> mKeyCount; be<u8> mFlagA,mFlagB;
    static hdDungeonKeyStates_c* construct(hdDungeonKeyStates_c* object);
    static void destroy(hdDungeonKeyStates_c* object,u32 flags);
    void waitEnter(); void waitExecute(); void updateCountText(); void inEnter(); void inExecute();
    void displayExecute(); void outEnter(); void outExecute(); void findTextPane();
    u32 setup(u32 first,u32 second,u32 third); void toWait();
    u32 load(void* name,u32 first,u32 second,u32 third,void* desc);
};
WWHD_OFFSET(hdDungeonKeyStates_c,mpTextPane,0x58);
WWHD_OFFSET(hdDungeonKeyStates_c,mSparkleTimer,0x5c);
WWHD_OFFSET(hdDungeonKeyStates_c,mKeyCount,0x68);
WWHD_OFFSET(hdDungeonKeyStates_c,mFlagB,0x6b);
WWHD_SIZE(hdDungeonKeyStates_c,0x6c);

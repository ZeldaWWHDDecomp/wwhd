/* WWHD sead text delimiters: the static initialiser of the translation unit that owns the whitespace
 * delimiter text (constructed in place and registered for destruction at exit) and an interface object.
 * Reconstructed from this game's instructions. */
#include "sead/AssertText.h"

using namespace sead;

static void registerGlobalObject(GlobalObjectRecord* record);

struct InterfaceObjectB { gptr<void> mVtable; };

static InterfaceObjectB sTokenizerInterface;
static u32 sTokenizerInterfaceVtable;
static DelimiterText sDelimiters;
static GlobalObjectRecord sDelimitersRecord;

/* 0275F904 */
static void __sinit_TextDelimiters_cpp() {
    sTokenizerInterface.mVtable = &sTokenizerInterfaceVtable;
    DelimiterText::construct(&sDelimiters);
    registerGlobalObject(&sDelimitersRecord);
}

/* WWHD glue
 data sTokenizerInterface 104A0CEC
 data sTokenizerInterfaceVtable 1015F9B4 nocheck
 data sDelimiters 104A0CF0
 data sDelimitersRecord 101F8B70 nocheck
 ext 028F026C void registerGlobalObject(sead::GlobalObjectRecord*)
*/

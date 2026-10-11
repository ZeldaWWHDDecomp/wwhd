// Item-base methods imported by field, special-event and shop item actors.
// Implementations belong to d_a_itembase; this unit only binds their guest calls.
#include "d/actor/d_a_itembase.h"

// HD: retain the original guest ABI, including all five animation speeds.
// No verified function registrations belong to this binding translation unit.
/* WWHD glue
ext 02183788 BOOL daItemBase_c::DeleteBase(const char* resName)
ext 021837B0 void daItemBase_c::animPlay(f32 brk1Speed, f32 brk2Speed, f32 btk1Speed, f32 btk2Speed, f32 bckSpeed)
ext 021842B8 void daItemBase_c::hide()
ext 021842C8 void daItemBase_c::show()
ext 021842D8 bool daItemBase_c::chkDraw()
ext 021842E4 void daItemBase_c::changeDraw()
ext 02184350 void daItemBase_c::setLoadError()
*/

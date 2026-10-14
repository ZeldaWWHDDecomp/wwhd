/** Place-name state identity and allocation/header initialization companion. */
#include "d/d_menu_window_hd.h"
void* hdPlaceName_allocate(u32 size);
/* 026C9648 */
hdPlaceNameWork_c* hdPlaceNameWork_c::construct(hdPlaceNameWork_c* work) {
    if (!work)
        work = static_cast<hdPlaceNameWork_c*>(hdPlaceName_allocate(sizeof(hdPlaceNameWork_c)));
    if (work) {
        work->mHeaderWord = 0;
        work->mHeaderUpperHalf = 0;
        work->mHeaderLowerHalf = 0;
    }
    return work;
}
/* 026C96E4 */
u32 hdPlaceNameState_c::stateKey(hdPartsStateId_c* state) { return state->mNumber; }
/* WWHD glue (tools/verify/autoglue.py)
 ext 0273AD10 void* hdPlaceName_allocate(u32)
*/

/* CC0 tww JAudio 1 parameter constructors instantiated in the WWHD SeMgr TU. */
#include "JSystem/JAudio/JAISoundParams.h"
namespace JAInter {
/* 028078CC */
MoveParaSetInitHalf::MoveParaSetInitHalf() {
    mMoveCounter = 0;
    mTargetValue = 0.5f;
    mCurrentValue = 0.5f;
}
/* 02807918 */
MoveParaSetInitZero::MoveParaSetInitZero() {
    mMoveCounter = 0;
    mTargetValue = 0.0f;
    mCurrentValue = 0.0f;
}
/* 02807964 */
LinkSound::LinkSound() {
    constructLinkSoundCriticalSection(criticalSection);
}
}
using namespace JAInter;
/* WWHD glue
ext 02760084 void constructLinkSoundCriticalSection(void*)
*/

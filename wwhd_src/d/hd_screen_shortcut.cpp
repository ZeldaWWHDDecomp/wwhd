#include "d/hd_hud_b.h"
static HdHudState l_prepareState;
static HdHudState l_oneState;
static HdHudState l_returnState;
static HdHudState l_nextState;
static HdHudState l_activeState;
static u8 l_shortcutActive;

/* 026DC1E8 */
void hdShortcutPrepareState(HdHudShortcut* self, HdHudState* state) {

  self->mNextState = state;
  hdHudChangeState(&self->mState, &l_prepareState);
}
/* 026DC1FC */
s32 hdShortcutAdvanceOne(HdHudShortcut* self) {

  self->mTextOffset = self->mTextOffset + HD_HUD_COMMAND_SIZE;
  hdShortcutPrepareState(self, &l_oneState);
  return 0;
}
/* 026DC234 */
s32 hdShortcutAdvanceThree(HdHudShortcut* self) {

  self->mTextOffset = self->mTextOffset + 3 * HD_HUD_COMMAND_SIZE;
  hdShortcutPrepareState(self, &l_returnState);
  return 0;
}
/* 026DC29C */
s32 hdShortcutAdvanceToNext(HdHudShortcut* self) {

  self->mTextOffset = self->mTextOffset + HD_HUD_COMMAND_SIZE;
  hdShortcutPrepareState(self, &l_nextState);
  return 0;
}
/* 026DC2D4 */
s32 hdShortcutActivate(HdHudShortcut* self) {

  self->mTextOffset = self->mTextOffset + HD_HUD_COMMAND_SIZE;
  l_shortcutActive = 1;
  hdHudChangeState(&self->mState, &l_activeState);
  self->mActive = 1;
  return 0;
}
/* 026DC334 */
s32 hdShortcutActivateAlternate(HdHudShortcut* self) {

  self->mTextOffset = self->mTextOffset + HD_HUD_COMMAND_SIZE;
  l_shortcutActive = 1;
  hdHudChangeState(&self->mState, &l_activeState);
  self->mActive = 1;
  return 0;
}
/* 026DC414 */
s32 hdShortcutAdvanceReturn(HdHudShortcut* self) {

  self->mTextOffset = self->mTextOffset + HD_HUD_COMMAND_SIZE;
  hdShortcutPrepareState(self, &l_returnState);
  return 0;
}
/* 026DC45C */
s32 hdShortcutAdvanceReturnAlternate(HdHudShortcut* self) {

  self->mTextOffset = self->mTextOffset + HD_HUD_COMMAND_SIZE;
  hdShortcutPrepareState(self, &l_returnState);
  return 0;
}
/* 026DC4A4 */
s32 hdShortcutRetreatReturn(HdHudShortcut* self) {

  self->mTextOffset = self->mTextOffset - HD_HUD_COMMAND_SIZE;
  hdShortcutPrepareState(self, &l_returnState);
  return 0;
}
/* WWHD glue (tools/verify/autoglue.py)
data l_prepareState 1049B540
data l_oneState 1049B630
data l_returnState 1049B510
data l_nextState 1049B690
data l_activeState 1049B600
data l_shortcutActive 1047B07C
ext 020063C0 void hdHudChangeState(HdHudStateController*,HdHudState*)
*/

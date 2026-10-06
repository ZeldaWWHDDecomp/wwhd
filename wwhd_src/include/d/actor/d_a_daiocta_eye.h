#pragma once
#include "f_op/f_op_actor.h"

// HD Big Octo eye layout, recovered from the profile and field accesses.
struct daDaiocta_Eye_c : fopAc_ac_c {
  be<s32> eyeNumber;
  be<u8> dead, bombKilled, damaged, damagedByBomb, appeared;
  u8 pad3b5[3];
  be<s32> mode;
  request_of_phase_process_class phase;
  gptr<void> model, brkResource;
  u8 brkAnimation[0x78];
  gptr<void> btkResource;
  u8 btkAnimation[0x74];
  csXyz targetRotation, eyeRotation, rotationMinimum, rotationMaximum;
  u8 sphere[0x12c], status[0x3c];
  cXyz eyeScale;
  be<s32> scaleAnimationIndex;
  gptr<void> jointHit, parent;
  u8 particleCallback[0x14];
  cXyz particlePosition;
  csXyz particleRotation;
  u8 pad67a[2];
};
WWHD_OFFSET(daDaiocta_Eye_c, eyeNumber, 0x3ac);
WWHD_OFFSET(daDaiocta_Eye_c, mode, 0x3b8);
WWHD_OFFSET(daDaiocta_Eye_c, model, 0x3c4);
WWHD_OFFSET(daDaiocta_Eye_c, btkResource, 0x444);
WWHD_OFFSET(daDaiocta_Eye_c, eyeRotation, 0x4c2);
WWHD_OFFSET(daDaiocta_Eye_c, sphere, 0x4d4);
WWHD_OFFSET(daDaiocta_Eye_c, status, 0x600);
WWHD_OFFSET(daDaiocta_Eye_c, eyeScale, 0x63c);
WWHD_OFFSET(daDaiocta_Eye_c, parent, 0x650);
WWHD_OFFSET(daDaiocta_Eye_c, particlePosition, 0x668);
WWHD_SIZE(daDaiocta_Eye_c, 0x67c);

/* Rito postbox, readable WWHD actor source. */
#include "d/actor/d_a_obj_toripost.h"
namespace {
u32 play() { return gabi::call<u32>(0x025200D4); }
u32 save() { return gabi::load<u32>(0x101F84DC); }
u32 letter(s32 index) { return 0x10031504 + u32(index) * 12; }
u16 letterEvent(s32 index) { return gabi::load<u16>(letter(index) + 10); }
void resetEvent() {
  u32 p = play();
  gabi::store<u16>(p + 0x52B8, gabi::load<u16>(p + 0x52B8) | 8);
}
void count(s16 n) { gabi::store<s16>(play() + 0x5BA0, n); }
void cutEnd(s32 id) {
  gabi::call(0x02543280, gabi::at<u8>(play() + 0x52C4), id);
}
bool stopped(mDoExt_McaMorf *m) {
  u32 p = gabi::ea(m);
  return (gabi::load<u8>(p + 0xA7) & 1) || gabi::load<f32>(p + 0x98) == 0.0f;
}
bool talkXY(u32 p) { return u32(gabi::load<u8>(p + 0x52B0) - 1) <= 3; }
} // namespace
BOOL daObjTpost_c::createHeap() {
  WWHD_FUNC(0x023A3CC4, BOOL, this);

  struct ResourceName {
    be<u32> name, vt;
  };
  gabi::Local<ResourceName> name;
  // Keep caller linkage below live payloads for nonleaf GHS callees.
  gabi::Local<be<u32>> nameLinkage;
  name->name = 0x100314B4;
  name->vt = 0x10031248;

  u32 data = gabi::call<u32>(
      0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)), name.get(), 9);

  if (!data)
    gabi::call(0x0273AA24, STR(0x100312A4), 0x87, STR(0x100312BC));

  mMorf = gabi::at<mDoExt_McaMorf>(
      gabi::call<u32>(0x025E4F64, 0, gabi::at<u8>(data), 0, 0, 0, -1, 1.0f, 0,
                      -1, 1, 0, 0x80000, 0x11000022));

  if (!mMorf || !gabi::load<u32>(gabi::ea((mDoExt_McaMorf *)mMorf) + 0x90))
    return false;

  u32 model = gabi::load<u32>(gabi::ea((mDoExt_McaMorf *)mMorf) + 0x90);
  gabi::store<u32>(model + 0xB8, gabi::ea(this));
  return true;
}
VERIFY(0x023A3CC4, &daObjTpost_c::createHeap);

static BOOL createHeapCB(daObjTpost_c *a) {
  WWHD_FUNC(0x023A3DCC, BOOL, a);
  return a->createHeap();
}
VERIFY(0x023A3DCC, createHeapCB);

void daObjTpost_c::setAnm(s32 index, BOOL force) {
  WWHD_FUNC(0x023A3DD0, void, this, index, force);

  if (index != 5)
    mAnmPrmIdx = index;

  s32 bck = mBckIdx;
  auto *morf = (mDoExt_McaMorf *)mMorf;

  if (bck == 0) {
    if (gabi::load<f32>(gabi::ea(morf) + 0x9C) == 1.0f) {
      gabi::call(0x025E1988, 0x6973);
      bck = mBckIdx;
    } else
      goto set;
  }
  if (bck == 1) {
    gabi::Local<cXyz> size;
    gabi::Local<be<u32>> sizeLinkage;
    size->x = 1.0f;
    size->y = 1.0f;
    size->z = 1.0f;

    morf = mMorf; // reloaded here (023A3E4C): the bck==0 arm may have called 025E1988 since the first load
    if (gabi::load<f32>(gabi::ea(morf) + 0x9C) != 1.0f)
      goto set;

    u32 p = play();
    gabi::call(0x025A847C, gabi::at<u8>(gabi::load<u32>(p + 0x5AB0)), 0, 0x8190,
               &current.pos, &current.angle, size.get(), 255, 0, -1, 0, 0, 0);

    gabi::call(0x025E1988, 0x6974);
  }
  morf = mMorf;

set:
  gabi::call(0x02587A0C, STR(0x100314B4), morf, &mBckIdx, &mAnmPrmIdx,
             &mOldAnmPrmIdx, gabi::at<u8>(0x100312D0), gabi::at<u8>(0x100312DC),
             force, 0);
}
VERIFY(0x023A3DD0, &daObjTpost_c::setAnm);

void daObjTpost_c::setMtx() {
  WWHD_FUNC(0x023A3F08, void, this);

  u32 model = gabi::load<u32>(gabi::ea((mDoExt_McaMorf *)mMorf) + 0x90);

  gabi::store<f32>(model + 0xBC, scale.x);
  gabi::store<f32>(model + 0xC0, scale.y);
  gabi::store<f32>(model + 0xC4, scale.z);

  auto *matrix = gabi::at<Mtx34>(0x1048D0CC);

  gabi::call(0x028E93CC, matrix, (f32)current.pos.x, (f32)current.pos.y,
             (f32)current.pos.z);

  gabi::call(0x025F1C28, matrix, (s16)shape_angle.y);

  for (u32 i = 0; i < 12; i++)
    gabi::store<f32>(model + 0xC8 + i * 4, gabi::load<f32>(0x1048D0CC + i * 4));
}
VERIFY(0x023A3F08, &daObjTpost_c::setMtx);

u32 daObjTpost_c::checkSendPrice() {
  WWHD_FUNC(0x023A3FE8, u32, this);

  struct String {
    be<u32> text, vt;
  };
  gabi::Local<String> left, right;
  gabi::Local<be<u32>> stringLinkage;

  left->text = 0x1003132C;
  left->vt = 0x10031248;

  u32 p = play();
  right->text = p + 0x5134;
  right->vt = 0x10031248;

  auto terminate = [](String *s) {
    u32 vt = s->vt;
    gabi::call_ptr(gabi::load<u32>(vt + 0x14), s);
  };

  terminate(left.get());
  terminate(left.get());
  u32 first = left->text;
  terminate(right.get());

  bool equal = first == right->text;

  if (!equal) {
    u32 a = left->text, b = right->text;
    for (u32 i = 0; i < 0x40001; i++) {
      u8 c = gabi::load<u8>(a + i);
      if (c != gabi::load<u8>(b + i))
        break;
      if (!c) {
        equal = true;
        break;
      }
    }
  }
  return equal ? gabi::load<u8>(0x10031330 + u32((s32)(s8)current.roomNo)) : 0;
}
VERIFY(0x023A3FE8, &daObjTpost_c::checkSendPrice);

void daObjTpost_c::modeProc(s32 proc, s32 mode) {
  WWHD_FUNC(0x023A40E0, void, this, proc, mode);

  u32 entry;

  if (proc == 0) {
    mCurMode = mode;
    entry = 0x10031364 + u32(mode) * 20;
  } else if (proc == 1)
    entry = 0x1003136C + u32((u32)mCurMode) * 20;

  else
    return;

  s16 delta = gabi::load<s16>(entry), slot = gabi::load<s16>(entry + 2);
  u32 self = gabi::ea(this) + delta;

  u32 target;

  if (slot < 0)
    target = gabi::load<u32>(entry + 4);

  else {
    s16 vtOff = gabi::load<s16>(entry + 6);
    u32 vt = gabi::load<u32>(self + vtOff);
    target = gabi::load<u32>(vt + u32(slot) * 8 + 4);
  }
  gabi::call_ptr(target, gabi::at<daObjTpost_c>(self));
}
VERIFY(0x023A40E0, &daObjTpost_c::modeProc);

void daObjTpost_c::createInit() {
  WWHD_FUNC(0x023A418C, void, this);

  if (gabi::call<BOOL>(0x025B7D90, gabi::at<u8>(save() + 0xD4), 2))
    gabi::call(0x02586E94, 0xB503);

  if (gabi::call<BOOL>(0x02520C0C, 0x31))
    gabi::call(0x02586E94, 0x7D03);

  if (gabi::call<BOOL>(0x02586ED0, 0xAC03) && gabi::call<BOOL>(0x02520A84, 6))
    gabi::call(0x02586E94, 0x7C03);

  if (gabi::call<BOOL>(0x025B8B94, gabi::at<u8>(save() + 0x644), 0x1E80))
    gabi::call(0x02586E94, 0x7B03);

  u8 wallet = gabi::load<u8>(save() + 0x32);
  if (wallet == 1 || wallet == 2)
    gabi::call(0x02586E94, 0x7A03);

  mLetterOrdinal = 1;
  mCurrMsgBsPcId = 0xFFFFFFFF;

  gabi::store<u8>(gabi::ea(this) + 0x38B, 6);
  gabi::store<u8>(gabi::ea(this) + 0x389, 5);
  gabi::store<u32>(gabi::ea(this) + 0x39C, 0x2000000A);

  setAnm(1, false);
  setMtx();
  gabi::call(0x025E55A0, (mDoExt_McaMorf *)mMorf);

  u32 model = gabi::load<u32>(gabi::ea((mDoExt_McaMorf *)mMorf) + 0x90);
  cullMtx = model ? model + 0xC8 : 0;

  gabi::call(0x025D674C, this, -50.0f, 0.0f, -50.0f, 70.0f, 200.0f, 70.0f);
  gabi::store<f32>(gabi::ea(this) + 0x364, 10.0f);

  gabi::call(0x02515F14, &mStts, 255, 255, this);
  gabi::call(0x02516518, &mCyl, gabi::at<u8>(0x100314C0));
  gabi::store<u32>(gabi::ea(this) + 0x6D4, gabi::ea(&mStts));

  mPayType = checkSendPrice();
  modeProc(0, 0);
  gabi::call(0x02588BDC, &current.pos);
  mEventCut.setActorInfo2(STR(0x1003140C), this);
}
VERIFY(0x023A418C, &daObjTpost_c::createInit);

s32 daObjTpost_c::create() {
  WWHD_FUNC(0x023A4370, s32, this);

  u32 flags = actor_condition;

  if (!(flags & 8)) {
    if (gabi::ea(this)) {
      gabi::call(0x025A1458, this);
      __vtbl = 0x100315A0;
      gabi::call(0x024F0474, &mAcch);

      u32 a = gabi::ea(this);
      gabi::store<u32>(a + 0x800, 0x10031260);
      gabi::store<u32>(a + 0x804, 0x10031280);
      gabi::store<u8>(a + 0x808, 1);
      gabi::store<u32>(a + 0x810, 0x10031270);
      gabi::call(0x024EFE94, &mWall);
      flags = actor_condition;
    }
    actor_condition = flags | 8;
  }
  s32 phase = gabi::call<s32>(0x02520460, &mPhase, STR(0x100314B4));

  if (phase == 4) {
    if (!gabi::call<BOOL>(0x025D63E8, this, 0x023A3DCC, 0x7E0))
      return 5;
    createInit();
  }
  return phase;
}
VERIFY(0x023A4370, &daObjTpost_c::create);

static s32 createActor(daObjTpost_c *a) {
  WWHD_FUNC(0x023A4470, s32, a);
  return a->create();
}
VERIFY(0x023A4470, createActor);

BOOL daObjTpost_c::remove() {
  WWHD_FUNC(0x023A4474, BOOL, this);
  gabi::call(0x025204C8, &mPhase, STR(0x100314B4));
  return true;
}
VERIFY(0x023A4474, &daObjTpost_c::remove);

static BOOL removeActor(daObjTpost_c *a) {
  WWHD_FUNC(0x023A44A4, BOOL, a);
  return a->remove();
}
VERIFY(0x023A44A4, removeActor);

s32 daObjTpost_c::getReadableLetterNum() {
  WWHD_FUNC(0x023A44A8, s32, this);

  s32 start = mNumReadable;
  if (!start)
    start = 1;

  for (s32 i = start; i < 13; i++)
    if (gabi::call<BOOL>(0x02586DC8, letterEvent(i)))
      return i;
  return 0;
}
VERIFY(0x023A44A8, &daObjTpost_c::getReadableLetterNum);

void daObjTpost_c::checkOrder() {
  WWHD_FUNC(0x023A4548, void, this);

  u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8);

  if (command == 2) {
    mEventIdx = 0;
    return;
  }
  if (command == 1 && ((s8)mEventIdx == 1 || (s8)mEventIdx == 2)) {
    mEventIdx = 0;
    if (talkXY(play()))
      mTalkXYPending = 1;
    else
      mTalkPending = 1;
  }
}
VERIFY(0x023A4548, &daObjTpost_c::checkOrder);

void daObjTpost_c::setAttention() {
  WWHD_FUNC(0x023A45F4, void, this);

  f32 z = current.pos.z, y = current.pos.y;
  gabi::store<f32>(gabi::ea(this) + 0x398, z);
  gabi::store<f32>(gabi::ea(this) + 0x394, y);
  f32 x = current.pos.x;

  gabi::store<f32>(gabi::ea(this) + 0x390, x);
  eyePos.z = z;
  eyePos.y = y;
  eyePos.x = x;

  gabi::store<f32>(gabi::ea(this) + 0x394, y + gabi::load<f32>(0x1046C534));
  eyePos.y = y + gabi::load<f32>(0x1046C538);
}
VERIFY(0x023A45F4, &daObjTpost_c::setAttention);

void daObjTpost_c::cutSetAnmStart(s32 staff) {
  WWHD_FUNC(0x023A463C, void, this, staff);

  u32 name = gabi::call<u32>(0x0254487C, gabi::at<u8>(play() + 0x52C4), staff,
                             STR(0x10031414), 4);

  bool equal = false;

  if (name) {
    u32 i = 0;
    for (;; i++) {
      u8 a = gabi::load<u8>(name + i), b = gabi::load<u8>(0x1003141C + i);
      if (a != b)
        break;
      if (!a) {
        equal = true;
        break;
      }
    }
  }
  setAnm(equal ? 3 : 1, false);
}
VERIFY(0x023A463C, &daObjTpost_c::cutSetAnmStart);

void daObjTpost_c::cutDispLetterStart(s32 staff) {
  WWHD_FUNC(0x023A46E4, void, this, staff);
  mCurrMsgNo = gabi::load<u32>(letter(mNumReadable) + 4);
}
VERIFY(0x023A46E4, &daObjTpost_c::cutDispLetterStart);

void daObjTpost_c::cutPresentProc(s32 staff) {
  WWHD_FUNC(0x023A4700, void, this, staff);

  u8 item = gabi::load<u8>(letter(mNumReadable) + 8);

  s32 id = gabi::call<s32>(0x025D7DEC, &current.pos, item, 0, -1, -1, 0, 0);
  u32 p = play();

  if (id != -1) {
    gabi::store<u32>(p + 0x52A0, id);
    p = play();
  }
  gabi::call(0x02543280, gabi::at<u8>(p + 0x52C4), staff);
}
VERIFY(0x023A4700, &daObjTpost_c::cutPresentProc);

void daObjTpost_c::cutSetAnmProc(s32 staff) {
  WWHD_FUNC(0x023A4798, void, this, staff);
  if (stopped(mMorf))
    cutEnd(staff);
}
VERIFY(0x023A4798, &daObjTpost_c::cutSetAnmProc);

void daObjTpost_c::cutDispLetterProc(s32 staff) {
  WWHD_FUNC(0x023A47F4, void, this, staff);
  u16 status = talk(0);

  if (status == 5) {
    u32 player = gabi::load<u32>(play() + 0x5B2C);
    gabi::store<u32>(player + 0x3BC, gabi::load<u32>(player + 0x3BC) | 0x4000);
  } else if (status == 18)
    cutEnd(staff);
}
VERIFY(0x023A47F4, &daObjTpost_c::cutDispLetterProc);

void daObjTpost_c::cutProc() {
  WWHD_FUNC(0x023A486C, void, this);

  s32 staff = gabi::call<s32>(0x02542D88, gabi::at<u8>(play() + 0x52C4),
                              STR(0x10031428), 0, 0);
  if (staff == -1)
    return;

  s32 act = gabi::call<s32>(0x02542EDC, gabi::at<u8>(play() + 0x52C4), staff,
                            gabi::at<u8>(0x101CD458), 3, 1, 0);

  u32 p = play();
  if (act == -1) {
    gabi::call(0x02543280, gabi::at<u8>(p + 0x52C4), staff);
    return;
  }
  if (gabi::call<BOOL>(0x025447C8, gabi::at<u8>(p + 0x52C4), staff)) {
    if (act == 1)
      cutSetAnmStart(staff);
    else if (act == 2)
      cutDispLetterStart(staff);
  }
  if (act == 0)
    cutPresentProc(staff);
  else if (act == 1)
    cutSetAnmProc(staff);
  else if (act == 2)
    cutDispLetterProc(staff);
}
VERIFY(0x023A486C, &daObjTpost_c::cutProc);

void daObjTpost_c::eventOrder() {
  WWHD_FUNC(0x023A4A04, void, this);

  s32 event = mEventIdx;

  if (event == 1 || event == 2) {
    event = mEventIdx;
    u32 a = gabi::ea(this);
    gabi::store<u16>(a + 0xFA, gabi::load<u16>(a + 0xFA) | 0x21);
    if (event == 1)
      gabi::call(0x025D76A8, this);
  } else if (event >= 3)
    gabi::call(0x025D77DC, this,
               gabi::at<char>(gabi::load<u32>(0x101CD458 + u32(event) * 4)), 1,
               0x14F);
}
VERIFY(0x023A4A04, &daObjTpost_c::eventOrder);

BOOL daObjTpost_c::execute() {
  WWHD_FUNC(0x023A4A60, BOOL, this);

  s32 readable = gabi::call<BOOL>(0x025B7D90, gabi::at<u8>(save() + 0xD4), 1);
  if (readable)
    readable = getReadableLetterNum();
  mNumReadable = readable;

  checkOrder();
  setAttention();
  setCollision(40.0f, 140.0f);
  modeProc(1, 5);

  if (gabi::load<u8>(play() + 0x5292) && !mEventCut.cutProc())
    cutProc();
  eventOrder();

  gabi::call(0x02515E50, gabi::at<u8>(gabi::ea(this) + 0x670));

  if (gabi::call<BOOL>(0x025162A4, &mCyl))
    gabi::call(0x023129C4, &eyePos, (s32)(s8)current.roomNo, &mCyl, 11);

  gabi::call(0x02312C8C, this, &mCyl);
  gabi::call(0x025D69FC, this, 7, 40.0f);

  gabi::call(0x025E535C, (mDoExt_McaMorf *)mMorf, 0, 0, 0);
  gabi::call(0x025E55A0, (mDoExt_McaMorf *)mMorf);
  setAnm(5, false);
  return false;
}
VERIFY(0x023A4A60, &daObjTpost_c::execute);

static BOOL executeActor(daObjTpost_c *a) {
  WWHD_FUNC(0x023A4BB0, BOOL, a);
  return a->execute();
}
VERIFY(0x023A4BB0, executeActor);

static void debugDraw(daObjTpost_c *a) {
  WWHD_FUNC(0x023A4BB4, void, a);

  if (!gabi::load<u32>(0x101FDA50)) {
    gabi::store<u32>(0x101FDA50, 1);
    gabi::call(0xC000A848, gabi::at<u8>(0x101FEBF4), gabi::at<u8>(0x10031240),
               4);
  }
}
VERIFY(0x023A4BB4, debugDraw);

BOOL daObjTpost_c::draw() {
  WWHD_FUNC(0x023A4BE4, BOOL, this);

  if (gabi::load<u8>(0x1046C531))
    debugDraw(this);

  u32 model = gabi::load<u32>(gabi::ea((mDoExt_McaMorf *)mMorf) + 0x90);

  u32 light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, gabi::at<u8>(light), 1, &current.pos, &tevStr);

  light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, gabi::at<u8>(light), gabi::at<J3DModel>(model),
             &tevStr);

  gabi::call(0x025E5590, (mDoExt_McaMorf *)mMorf);
  return true;
}
VERIFY(0x023A4BE4, &daObjTpost_c::draw);

static BOOL drawActor(daObjTpost_c *a) {
  WWHD_FUNC(0x023A4C64, BOOL, a);
  return a->draw();
}
VERIFY(0x023A4C64, drawActor);

void daObjTpost_c::modeWaitInit() {
  WWHD_FUNC(0x023A4C68, void, this);
  mLetterOrdinal = 1;
  setAnm((s32)mNumReadable ? 4 : 1, false);
}
VERIFY(0x023A4C68, &daObjTpost_c::modeWaitInit);

BOOL daObjTpost_c::checkTalk() {
  WWHD_FUNC(0x023A4C88, BOOL, this);
  u32 player = gabi::load<u32>(play() + 0x5B2C);

  f32 distance = gabi::call<f32>(0x025D6958, this, gabi::at<u8>(player));
  return distance < gabi::load<f32>(0x1046C53C);
}
VERIFY(0x023A4C88, &daObjTpost_c::checkTalk);

void daObjTpost_c::modeWait() {
  WWHD_FUNC(0x023A4CD8, void, this);
  setAnm((s32)mNumReadable ? 4 : 1, false);
  if (mTalkPending)
    modeProc(0, 1);
  else if (mTalkXYPending)
    modeProc(0, 2);
  else if (checkTalk())
    mEventIdx = 2;
}
VERIFY(0x023A4CD8, &daObjTpost_c::modeWait);

void daObjTpost_c::modeTalkInit() {
  WWHD_FUNC(0x023A4D90, void, this);
  setAnm(1, false);
}
VERIFY(0x023A4D90, &daObjTpost_c::modeTalkInit);

void daObjTpost_c::modeTalk() {
  WWHD_FUNC(0x023A4D9C, void, this);
  if (talk(1) == 18) {
    if (mReceivePending) {
      modeProc(0, 3);
      mReceivePending = 0;
    } else
      modeProc(0, 0);
    resetEvent();
    mTalkPending = 0;
  }
}
VERIFY(0x023A4D9C, &daObjTpost_c::modeTalk);

void daObjTpost_c::modeTalkXYInit() {
  WWHD_FUNC(0x023A4E34, void, this);
  setAnm(1, false);
  mPreItem = gabi::load<u8>(play() + 0x52B1);
  mTalkTimer = gabi::load<s16>(0x1046C540);
  mPresentTimer = gabi::load<s16>(0x1046C542);
}
VERIFY(0x023A4E34, &daObjTpost_c::modeTalkXYInit);

void daObjTpost_c::modeTalkXY() {
  WWHD_FUNC(0x023A4E8C, void, this);

  u32 p = play();
  s32 index = mAnmPrmIdx;
  u32 player = gabi::load<u32>(p + 0x5B2C);

  auto demo = [&](u32 mode) {
    gabi::store<u32>(player + 0x428, 0);
    gabi::store<u32>(player + 0x430, mode);
    gabi::store<u16>(player + 0x420, 3);
  };

  if (index == 3) {
    if (mSpitPending) {
      mSpitPending = 0;
      mSurpriseTimer = 10;
    }
    if ((s32)mSurpriseTimer != -1 &&
        !gabi::call<s32>(0x0211D2F8, &mSurpriseTimer)) {
      demo(24);
      mSurpriseTimer = -1;
    }
    u32 msg = mCurrMsgNo;

    if (msg == 0xCE9 || msg == 0xCEA || msg == 0xCF0 || msg == 0xCF1) {
      u32 vt = gabi::load<u32>(player + 0xB4),
          target = gabi::load<u32>(vt + 0x9C);
      if (gabi::call_ptr<f32>(target, gabi::at<u8>(player)) == 0.0f)
        demo(1);
    }
    index = mAnmPrmIdx;
  }
  if (index == 1) {
    if (gabi::call<BOOL>(0x02544950, gabi::at<u8>(play() + 0x52C4)) &&
        !gabi::call<s32>(0x0211D2F8, &mPresentTimer)) {
      gabi::call(0x02544980, gabi::at<u8>(play() + 0x52C4));
      setAnm(2, false);
    }
    index = mAnmPrmIdx;
  }
  if ((index == 2 || index == 3) && stopped(mMorf) &&
      !gabi::call<s32>(0x0211D2F8, &mTalkTimer) && talk(1) == 18) {
    modeProc(0, 0);
    resetEvent();
    mTalkXYPending = 0;
  }
}
VERIFY(0x023A4E8C, &daObjTpost_c::modeTalkXY);

void daObjTpost_c::modeReceiveInit() {
  WWHD_FUNC(0x023A5070, void, this);
  resetEvent();
  mEventIdx = 3;
  setAnm(1, false);
}
VERIFY(0x023A5070, &daObjTpost_c::modeReceiveInit);

void daObjTpost_c::modeReceive() {
  WWHD_FUNC(0x023A50C0, void, this);
  modeProc(0, 4);
}
VERIFY(0x023A50C0, &daObjTpost_c::modeReceive);

void daObjTpost_c::modeReceiveDemo() {
  WWHD_FUNC(0x023A50CC, void, this);

  if (gabi::call<BOOL>(0x0254457C, gabi::at<u8>(play() + 0x52C4),
                       STR(0x10031464))) {
    gabi::call(0x02586E04, letterEvent(mNumReadable));
    mNumReadable = getReadableLetterNum();
    resetEvent();

    if ((s32)mNumReadable) {
      modeProc(0, 1);
      mEventIdx = 1;
      mNextLetter = 1;
      mLetterOrdinal = (u32)mLetterOrdinal + 1;
    } else {
      modeProc(0, 0);
      mLetterOrdinal = 1;
    }
  }
}
VERIFY(0x023A50CC, &daObjTpost_c::modeReceiveDemo);

void daObjTpost_c::deliverLetter() {
  WWHD_FUNC(0x023A5198, void, this);
  u8 item = mPreItem;
  if (item == 0x99)
    gabi::call(0x02586D5C, 0xAC03);
  else if (item == 0x9A)
    gabi::call(0x025B8B68, gabi::at<u8>(save() + 0x644), 0x1220);
}
VERIFY(0x023A5198, &daObjTpost_c::deliverLetter);

s16 daObjTpost_c::getReceiveLetterNum() {
  WWHD_FUNC(0x023A51CC, s16, this);
  s16 n = 0;
  for (s32 i = 1; i < 13; i++)
    if (gabi::call<BOOL>(0x02586DC8, letterEvent(i)))
      n++;
  return n;
}
VERIFY(0x023A51CC, &daObjTpost_c::getReceiveLetterNum);

static f32 colorGamma(u32 color) {
  WWHD_FUNC(0x023A5238, f32, color);

  f32 input = (f32)color / 255.0f;
  f32 out = gabi::call<f32>(0x028F4560, input, 2.2f);

  if (out < 0.0f)
    out = 0.0f;
  else if (out > 1.0f)
    out = 1.0f;
  return out;
}
VERIFY(0x023A5238, colorGamma);

u32 daObjTpost_c::getMsgXY() {
  WWHD_FUNC(0x023A52B8, u32, this);

  play();
  u32 regs = 0x1047B608;
  u8 item = mPreItem;

  gabi::Local<cXyz> pos;
  pos->y = gabi::load<f32>(regs + 0x6CC);
  pos->x = gabi::load<f32>(regs + 0x6C8);
  pos->z = gabi::load<f32>(regs + 0x6D0);

  gabi::Local<cXyz> size;
  size->x = 2.0f;
  size->y = 2.0f;
  size->z = 2.0f;
  gabi::Local<GXColor> color;
  gabi::Local<be<u32>> effectLinkage;
  color->a = 128;

  if (item == 0x99 || item == 0x9A) {
    u8 r = gabi::load<s16>(regs + 0x740) + 128,
       g = gabi::load<s16>(regs + 0x742) + 128,
       b = gabi::load<s16>(regs + 0x744) + 128;

    color->r = r;
    color->g = g;
    color->b = b;

    color->r = (u8)gabi::ftoi(colorGamma(r) * 255.0f);
    color->g = (u8)gabi::ftoi(colorGamma(color->g) * 255.0f);
    color->b = (u8)gabi::ftoi(colorGamma(color->b) * 255.0f);

    u32 model = gabi::load<u32>(gabi::ea((mDoExt_McaMorf *)mMorf) + 0x90),
        data = gabi::load<u32>(model + 0x2C);

    u16 flags = gabi::load<u16>(data + 4);
    u32 matrices = gabi::load<u32>(data + 0x10);
    gabi::store<u16>(data + 4, flags | 0x10);

    auto *matrix = gabi::at<Mtx34>(0x1048D0CC);
    gabi::call(0x028E90D4, gabi::at<Mtx34>(matrices + 0x60), matrix);
    gabi::call(0x028E8F64, matrix, pos.get(), pos.get());

    u32 control = gabi::load<u32>(play() + 0x5AB0);
    gabi::call(0x025A847C, gabi::at<u8>(control), 0, 0x57, pos.get(),
               &shape_angle, size.get(), 255, 0, -1, color.get(), 0, 0);
    return 0xCE8;
  }
  color->r = 0;
  color->g = 0;
  color->b = 0;
  setAnm(3, false);
  mSpitPending = 1;
  return (item == 0x98 || item == 0x9B) ? 0xCEA : 0xCE9;
}
VERIFY(0x023A52B8, &daObjTpost_c::getMsgXY);

u32 daObjTpost_c::getMsgNormal() {
  WWHD_FUNC(0x023A5500, u32, this);
  if (mNextLetter) {
    s16 n = gabi::load<s16>(gabi::ea(&mLetterOrdinal) + 2);
    count(n);
    mNextLetter = 0;
    return 0xCF7;
  }
  return gabi::call<BOOL>(0x02556D14) ? 0xCE6 : 0xCE5;
}
VERIFY(0x023A5500, &daObjTpost_c::getMsgNormal);

u32 daObjTpost_c::getMsg() {
  WWHD_FUNC(0x023A5570, u32, this);
  return talkXY(play()) ? getMsgXY() : getMsgNormal();
}
VERIFY(0x023A5570, &daObjTpost_c::getMsg);

u16 daObjTpost_c::nextMsg(be<u32> *message) {
  WWHD_FUNC(0x023A55BC, u16, this, message);

  u32 msgControl = gabi::load<u32>(0x101F4B5C);
  play();
  u16 status = 15;

  switch ((u32)*message) {
  case 0xCE5:
  case 0xCE6:
    if ((s32)mNumReadable) {
      s16 n = getReceiveLetterNum();
      count(n);
      *message = 0xCEB;
    } else
      *message = 0xCE7;
    break;

  case 0xCEB:
    count(gabi::load<s16>(gabi::ea(&mLetterOrdinal) + 2));
    *message = 0xCF7;
    break;

  case 0xCF7:
    if (gabi::load<u8>(0x1046C533) || gabi::load<u8>(letter(mNumReadable)))
      *message = 0xCF3;
    else {
      mReceivePending = 1;
      status = 16;
    }
    break;

  case 0xCE8:
    *message = gabi::load<u32>(0x1003148C + u32((u8)mPayType) * 4);
    break;

  case 0xCEC:
  case 0xCED:
  case 0xCEE:
    *message = 0xCEF;
    break;

  case 0xCEF:
    if (!gabi::load<u32>(msgControl + 0x948)) {
      u8 type = mPayType;
      u32 sv = save();
      s32 price = gabi::load<s32>(0x100314A8 + u32(type) * 4);
      u16 money = gabi::load<u16>(sv + 0x24);

      if (money >= price) {
        u32 p = play();
        gabi::store<u32>(p + 0x5B48, gabi::load<u32>(p + 0x5B48) - price);
        gabi::call(0x025B7270, gabi::at<u8>(save() + 0x96));
        deliverLetter();
        *message = 0xCF2;
      } else {
        setAnm(3, false);
        mSpitPending = 1;
        *message = 0xCF1;
      }
    } else {
      setAnm(3, false);
      mSpitPending = 1;
      *message = 0xCF0;
    }
    break;

  case 0xCF3:
    *message = letterEvent(mNumReadable) == 0xB203 ? 0xCF8 : 0xCF4;
    break;

  case 0xCF4:
  case 0xCF8:
    if (!gabi::load<u32>(msgControl + 0x948)) {
      s32 i = mNumReadable;
      u32 sv = save();
      u16 event = letterEvent(i), money = gabi::load<u16>(sv + 0x24);
      s32 price = event == 0xB203 ? 201 : 10;

      if (money >= price) {
        u32 p = play();
        gabi::store<u32>(p + 0x5B48, gabi::load<u32>(p + 0x5B48) - price);
        mReceivePending = 1;
        status = 16;
      } else
        *message = 0xCF6;

    } else
      *message = 0xCF5;
    break;

  case 0xCF5:
  case 0xCF6:
    mNumReadable = (s32)((u32)(s32)mNumReadable + 1);
    mLetterOrdinal = (u32)mLetterOrdinal + 1;
    mNumReadable = getReadableLetterNum();

    if ((s32)mNumReadable) {
      count(gabi::load<s16>(gabi::ea(&mLetterOrdinal) + 2));
      *message = 0xCF7;
    } else
      status = 16;
    break;

  default:
    status = 16;
    break;
  }
  s32 index = mNumReadable;
  u32 current = mCurrMsgNo;
  if (current == gabi::load<u32>(letter(index) + 4) && status == 16)
    status = 14;
  return status;
}
VERIFY(0x023A55BC, &daObjTpost_c::nextMsg);

struct TpostHIO {
  be<s8> number;
  be<u8> debug, unused, payAll;
  be<f32> attention, eye, distance;
  be<s16> talkTimer, presentTimer;
  be<u32> vt;
};

WWHD_SIZE(TpostHIO, 0x18);

static TpostHIO *createHIO(TpostHIO *h) {
  WWHD_FUNC(0x023A5AB4, TpostHIO *, h);

  if (!gabi::ea(h)) {
    h = gabi::call<TpostHIO *>(0x0273AD10, 0x18);
    if (!h)
      return h;
  }
  h->unused = 0;
  h->number = -1;
  h->eye = 100.0f;
  h->distance = 300.0f;
  h->presentTimer = 30;
  h->attention = 140.0f;
  h->payAll = 0;
  h->talkTimer = 30;
  h->debug = 0;
  h->vt = 0x10031290;
  return h;
}
VERIFY(0x023A5AB4, createHIO);

static void initializeStatics() {
  WWHD_FUNC(0x023A5B3C, void);

  gabi::store<u32>(0x1046C550, 0);
  gabi::store<u32>(0x1046C548, 0);
  gabi::store<u32>(0x1046C554, 0);
  gabi::store<u32>(0x1046C54C, 0);

  gabi::call(0x028F026C, gabi::at<u8>(0x101CD468));

  gabi::store<f32>(0x1046C524, -3.1415927410125732f);
  gabi::store<f32>(0x1046C528, 3.1415927410125732f);

  gabi::call(0x028ED6F8, gabi::at<u8>(0x1046C52C));
  gabi::call(0x028F026C, gabi::at<u8>(0x101CD474));

  gabi::call(0x028EAB2C, gabi::at<u8>(0x1046C52D));
  gabi::call(0x028F026C, gabi::at<u8>(0x101CD480));
  createHIO(gabi::at<TpostHIO>(0x1046C530));
}
VERIFY(0x023A5B3C, initializeStatics);

static void deleteHIO(TpostHIO *h, u32 flags) {
  WWHD_FUNC(0x023A5BDC, void, h, flags);
  if (gabi::ea(h) && (flags & 1))
    gabi::call(0x0273AF40, h);
}
VERIFY(0x023A5BDC, deleteHIO);

static BOOL isDelete(daObjTpost_c *a) {
  WWHD_FUNC(0x023A5BF0, BOOL, a);
  return true;
}
VERIFY(0x023A5BF0, isDelete);

static void receiveDemoInit(daObjTpost_c *a) { WWHD_FUNC(0x023A5BF8, void, a); }
VERIFY(0x023A5BF8, receiveDemoInit);

static void deleteInstance(daObjTpost_c *a, u32 flags) {
  WWHD_FUNC(0x023A5BFC, void, a, flags);

  if (!gabi::ea(a))
    return;
  u32 p = gabi::ea(a);

  gabi::call(0x02018034, gabi::at<u8>(p + 0x9C8), 2);
  gabi::store<u32>(p + 0x810, 0x10031270);
  gabi::store<u32>(p + 0x804, 0x10031280);
  gabi::call(0x024EFD9C, gabi::at<u8>(p + 0x7F0), 0);

  gabi::call(0x02515A70, gabi::at<u8>(p + 0x690), 2);
  gabi::call(0x02515860, gabi::at<u8>(p + 0x654), 2);
  gabi::call(0x02018034, gabi::at<u8>(p + 0x628), 2);

  gabi::store<u32>(p + 0x470, 0x10031270);
  gabi::store<u32>(p + 0x464, 0x10031280);
  gabi::call(0x024EFD9C, gabi::at<u8>(p + 0x450), 0);
  gabi::call(0x025D50BC, a, 0);
  if (flags & 1)
    gabi::call(0x0273AF40, a);
}
VERIFY(0x023A5BFC, deleteInstance);

static void getArg(daObjTpost_c *a) { WWHD_FUNC(0x023A5CC8, void, a); }
VERIFY(0x023A5CC8, getArg);

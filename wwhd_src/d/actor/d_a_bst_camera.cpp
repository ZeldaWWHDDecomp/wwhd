#include "d/actor/d_a_bst.h"

// Gohdan's introduction, nose-item, and defeat camera sequences.
static void demo_camera(bst_class *actor) {
  WWHD_FUNC(0x020EE200, void, actor);
  const u32 a = gabi::ea(actor);
  constexpr u32 tune = 0x1047B608;
  auto getPlay = [] { return gabi::call<u32>(0x025200D4); };
  u32 player = gabi::load<u32>(getPlay() + 0x5B2C);
  s8 cameraId = gabi::load<s8>(getPlay() + 0x5B30);
  u32 camera =
      gabi::load<u32>(getPlay() + (s32)cameraId * 0x34 + 0x5AF8) + 0x248;
  auto ptr = [](u32 p) { return gabi::at<void>(p); };
  auto f = [](u32 p) { return gabi::load<f32>(p); };
  auto F = [a](u32 o) { return gabi::load<f32>(a + o); };
  auto S = [a](u32 o) { return (s32)gabi::load<s16>(a + o); };
  auto B = [a](u32 o) { return (s32)gabi::load<s8>(a + o); };
  auto sf = [a](u32 o, f32 v) { gabi::store<f32>(a + o, v); };
  auto sh = [a](u32 o, s32 v) { gabi::store<s16>(a + o, (s16)v); };
  auto sb = [a](u32 o, s32 v) { gabi::store<s8>(a + o, (s8)v); };
  auto T = [f](u32 n) { return f(tune + 8 + 4 * n); };
  auto TS = [](u32 n) { return (s32)gabi::load<s16>(tune + 0x80 + 2 * n); };
  auto add = [](f32 x, f32 y) { return gabi::fadds_ppc(x, y); };
  auto sub = [](f32 x, f32 y) { return gabi::fsubs_ppc(x, y); };
  auto mul = [](f32 x, f32 y) { return gabi::fmuls_ppc(x, y); };
  auto boss = [] { return gabi::load<u32>(0x10462988); };
  auto hand = [](u32 n) { return gabi::load<u32>(0x10462978 + n * 4); };
  const f32 zero = f(0x1000B854), one = f(0x1000B860), hundred = f(0x1000B8B4),
            tenth = f(0x1000B8E4);
  auto chase = [a](u32 o, f32 target, f32 scale, f32 step) {
    cLib_addCalc2(gabi::at<be<f32>>(a + o), target, scale, step);
  };
  // Vector assignment uses integer loads/stores, preserving all payload bits.
  auto copy = [](u32 dst, u32 src) {
    for (u32 o = 0; o != 12; o += 4)
      gabi::store<u32>(dst + o, gabi::load<u32>(src + o));
  };
  gabi::Local<cXyz> offset, transformed, result, shakeEye, shakeCenter, scale,
      shock;
  gabi::Local<csXyz> itemAngle;
  gabi::Local<be<u32>> lookup;
  auto setVector = [](cXyz *p, f32 x, f32 y, f32 z) {
    p->x = x;
    p->y = y;
    p->z = z;
  };
  auto sound = [ptr](u32 owner, u32 position, u32 id) {
    if (owner && position) {
      s8 room = gabi::load<s8>(owner + 0x326);
      s8 reverb = gabi::call<s8>(0x02520540, room);
      gabi::call(0x025E1A40, id, ptr(position), 0, reverb);
    }
  };
  auto playerPos = [&](f32 x) {
    setVector(transformed.get(), x, zero, zero);
    u32 vt = gabi::load<u32>(player + 0xB4);
    u32 fn = gabi::load<u32>(vt + 0x114);
    gabi::call(fn, ptr(player), transformed.get(), 0x4000);
  };
  auto finish = [&] {
    gabi::call(0x02515280, ptr(camera), 0);
    gabi::call(0x02514F38, ptr(camera));
    gabi::call(0x0259169C);
    u32 play = getPlay();
    gabi::store<u16>(play + 0x52B8, gabi::load<u16>(play + 0x52B8) | 8);
  };
  auto message = [&](u32 id) { gabi::call(0x020E46A0, actor, id); };
  auto messageEnd = [] { gabi::call(0x020E471C); };
  auto initialHandCamera = [&](u32 n) {
    copy(a + 0x30E0, hand(n) + 0x314);
    sf(0x30E4, add(F(0x30E4), add(T(4), f(0x1000B874))));
    copy(a + 0x30D4, hand(n) + 0x314);
    sf(0x30D4, add(F(0x30D4), sub(T(5), f(0x1000B930))));
    sf(0x30D8, add(F(0x30D8), T(6)));
  };
  auto handCamera = [&](u32 n) {
    chase(0x30D4, sub(f(hand(n) + 0x314), f(0x1000B9AC)), tenth,
          mul(f(0x1000B88C), F(0x30F8)));
    chase(0x30F8, one, one, f(0x1000B984));
    chase(0x30E0, f(hand(n) + 0x314), f(0x1000B8EC), f(0x1000B890));
    chase(0x30E8, f(hand(n) + 0x31C), f(0x1000B8EC), f(0x1000B890));
    if (S(0x30D0) == 20) {
      gabi::store<u8>(hand(n) + 0x3F0, 1);
      gabi::store<u8>(hand(n) + 0x3E0, 1);
      u32 h = hand(n);
      sound(h, h + 0x314, 0x58D4);
    }
    if (S(0x30D0) == 59) {
      if (n)
        sf(0x3104, zero);
      sb(0x3108, 1);
    }
    if (S(0x30D0) == 60) {
      gabi::store<s16>(hand(n) + 0x130E, 2);
      u32 h = hand(n);
      sound(h, h + 0x314, 0x58D5);
    }
    if (S(0x30D0) == 90) {
      gabi::store<s16>(hand(n) + 0x130E, 4);
      u32 h = hand(n);
      sound(h, h + 0x314, 0x6983);
    }
    if (S(0x30D0) == 169)
      sb(0x3108, 0);
  };
  auto highCamera = [&] {
    sf(0x30E4, add(T(16), f(0x1000B9C4)));
    sf(0x30E0, zero);
    sf(0x30E8, zero);
    sf(0x30D4, add(T(13), f(0x1000B9C8)));
    sf(0x30D8, add(T(14), hundred));
    sf(0x30DC, add(T(15), f(0x1000B9BC)));
  };
  auto defeatCamera = [&] {
    setVector(transformed.get(), F(0x314), add(add(F(0x318), hundred), T(10)),
              F(0x31C));
    gabi::call(0x0200F164, ptr(a + 0x30E0), transformed.get(), f(0x1000B87C),
               add(f(tune + 0x5C8), f(0x1000B8BC)));
    setVector(transformed.get(), sub(F(0x314), f(0x1000B9D0)),
              sub(F(0x318), f(0x1000B9D4)), sub(F(0x31C), f(0x1000B8B0)));
    gabi::call(0x0200F164, ptr(a + 0x30D4), transformed.get(), f(0x1000B8EC),
               add(f(tune + 0x5CC), one));
  };
  auto findItem = [&] {
    u32 id = gabi::load<u32>(a + 0x30C0);
    *lookup = id;
    return id == 0xFFFFFFFF
               ? 0u
               : gabi::call<u32>(0x025D5218, ptr(0x025E1234), lookup.get());
  };
  auto particle = [&](u32 id, cXyz *pos, s8 room) {
    u32 control = gabi::load<u32>(getPlay() + 0x5AB0);
    return gabi::call<u32>(0x025A847C, ptr(control), 2, id, pos, ptr(a + 0x328),
                           0, 0xB9, ptr(a + 0x3134), room, 0, 0, 0);
  };
  s32 state = B(0x30CE);
  switch (state) {
  case 1:
  case 10:
  case 50:
    if (gabi::load<u16>(a + 0xF8) != 2) {
      gabi::call(0x025D7B24, actor, 2, 0xFFFF, 0);
      gabi::store<u16>(a + 0xFA, gabi::load<u16>(a + 0xFA) | 2);
      return;
    }
    sb(0x30CE, state + 1);
    gabi::call(0x02514F2C, ptr(camera));
    gabi::call(0x02515280, ptr(camera), 2);
    sh(0x30D0, 0);
    if (state == 1)
      sh(0x30CC, 100);
    sf(0x30FC, f(0x1000B98C));
    sf(0x30F8, zero);
    if (state == 1)
      goto noseCamera;
    if (state == 10) {
      sf(0x30D4, sub(add(f(boss() + 0x314), T(1)), f(0x1000B998)));
      sf(0x30D8, sub(add(f(boss() + 0x318), T(2)), f(0x1000B99C)));
      sf(0x30DC, sub(add(f(boss() + 0x31C), T(3)), f(0x1000B878)));
      // HD centers this first shot to the left/below the head, with separate
      // tuning fields.
      sf(0x30E0, add(sub(f(boss() + 0x314), f(0x1000B96C)), f(tune + 0x5B0)));
      sf(0x30E4, add(add(f(boss() + 0x318), f(0x1000B9A0)), f(tune + 0x5B4)));
      sf(0x30E8, f(boss() + 0x31C));
      gabi::call(0x025E1944);
      gabi::store<f32>(gabi::load<u32>(a + 0x332C), one);
      sh(0x30CC, 120);
      goto introduction;
    }
    sf(0x30D4, add(T(2), f(0x1000B9B8)));
    gabi::store<u32>(player + 0x428, 0);
    gabi::store<s16>(player + 0x420, 3);
    gabi::call(0x020E5068, actor);
    if (u32 emitter = gabi::load<u32>(a + 0x312C)) {
      u32 flags = gabi::load<u32>(emitter + 0x254);
      gabi::store<u32>(emitter + 0x5C, 0xFFFFFFFF);
      gabi::store<u32>(emitter + 0x254, flags | 1);
      gabi::store<u32>(a + 0x312C, 0);
      gabi::store<u8>(gabi::load<u32>(0x101FFC78) + 0x1E44, 0);
    }
    sb(0x3330, 0);
    sh(0x30CC, 150);
    goto defeatIntroduction;
  case 2:
  noseCamera:
    if (S(0x30D0) > (s16)(TS(4) + 33)) {
      chase(0x30F8, f(0x1000B990), f(0x1000B86C), add(T(3), f(0x1000B890)));
      if (S(0x30D0) > (s16)(TS(4) + 45))
        chase(0x30FC, f(0x1000B888), f(0x1000B86C), add(T(3), f(0x1000B8BC)));
    }
    gabi::call(0x025F1884, ptr(gabi::load<u32>(0x1018C7B0)),
               (s16)(S(0x32A) + TS(1) + 0xAF0));
    setVector(offset.get(), B(0x30D2) ? add(T(9), f(0x1000B8C4)) : T(10),
              add(T(11), F(0x30F8)), T(12));
    MtxPosition(offset.get(), transformed.get());
    gabi::call(0x0201AD78, ptr(a + 0x37C), result.get(), transformed.get());
    copy(a + 0x30E0, gabi::ea(result.get()));
    gabi::call(0x025F1BF4, ptr(gabi::load<u32>(0x1018C7B0)),
               (s16)(TS(2) + 4000));
    setVector(offset.get(), zero, T(7), add(T(8), f(0x1000B994)));
    MtxPosition(offset.get(), transformed.get());
    gabi::call(0x0201AD78, ptr(a + 0x314), result.get(), transformed.get());
    copy(a + 0x30D4, gabi::ea(result.get()));
    if (S(0x30D0) > 80) {
      sh(0x130A, 1);
      sh(0x130E, 0);
      finish();
      sb(0x30CE, 0);
      sh(0x30CC, 1);
    }
    break;
  case 11:
  introduction:
    playerPos(f(0x1000B9A4));
    chase(0x30DC, add(add(f(boss() + 0x31C), T(3)), f(0x1000B878)),
          f(0x1000B8EC), f(0x1000B9A8));
    if (S(0x30D0) == 60) {
      message(0x170D);
      gabi::store<u8>(0x1047B07B, 0xFF);
    }
    if (S(0x30D0) == 180)
      messageEnd();
    if (S(0x30D0) == TS(4) + 200)
      message(0x170E);
    if (S(0x30D0) == TS(4) + 290)
      messageEnd();
    if (S(0x30D0) == 2 * TS(4) + 310)
      message(0x170F);
    if (S(0x30D0) == 2 * TS(4) + 410)
      messageEnd();
    if (S(0x30D0) != 2 * TS(4) + 430)
      break;
    sh(0x30D0, 0);
    sb(0x30CE, 12);
    initialHandCamera(0);
    [[fallthrough]];
  case 12:
    handCamera(0);
    if (S(0x30D0) != 170)
      break;
    sh(0x30D0, 0);
    sf(0x30F8, zero);
    sb(0x30CE, 13);
    initialHandCamera(1);
    [[fallthrough]];
  case 13:
    handCamera(1);
    if (S(0x30D0) != 170)
      break;
    sh(0x30D0, 0);
    sf(0x30F8, zero);
    sb(0x30CE, 14);
    copy(a + 0x30E0, boss() + 0x314);
    sf(0x30E4, add(F(0x30E4), add(T(7), f(0x1000B878))));
    copy(a + 0x30D4, boss() + 0x314);
    sf(0x30D4, add(F(0x30D4), sub(T(8), f(0x1000B930))));
    sf(0x30D8, add(F(0x30D8), add(T(7), f(0x1000B878))));
    [[fallthrough]];
  case 14:
    if (S(0x30D0) < 30)
      break;
    if (S(0x30D0) <= 90) {
      chase(0x30D4, sub(f(boss() + 0x314), f(0x1000B96C)), f(0x1000B87C),
            mul(hundred, F(0x30F8)));
      chase(0x30F8, one, one, f(0x1000B864));
      if (S(0x30D0) == 30) {
        gabi::store<u8>(boss() + 0x3F0, 1);
        gabi::store<u8>(boss() + 0x3E0, 1);
        u32 b = boss();
        sound(b, b + 0x314, 0x58D6);
      }
      if (S(0x30D0) == 59) {
        sf(0x3104, zero);
        sb(0x3108, 1);
      }
      if (S(0x30D0) == 60) {
        gabi::store<s16>(boss() + 0x130E, 2);
        u32 b = boss();
        sound(b, b + 0x314, 0x58D7);
      }
      if (S(0x30D0) == 90) {
        gabi::store<s16>(boss() + 0x130E, 4);
        u32 b = boss();
        sound(b, b + 0x314, 0x6984);
      }
    }
    if (S(0x30D0) != 150)
      break;
    sb(0x30CE, 15);
    gabi::store<s16>(boss() + 0x130A, 1);
    gabi::store<s16>(hand(0) + 0x130A, 1);
    gabi::store<s16>(hand(1) + 0x130A, 1);
    gabi::store<s16>(boss() + 0x130E, 0);
    gabi::store<s16>(hand(1) + 0x130E, 0);
    gabi::store<s16>(hand(0) + 0x130E, 0);
    copy(a + 0x30E0, boss() + 0x37C);
    sf(0x30E4, add(F(0x30E4), T(0)));
    copy(a + 0x30D4, player + 0x314);
    sf(0x30D4, sub(F(0x30D4), add(T(1), f(0x1000B8B0))));
    sf(0x30D8, add(F(0x30D8), add(T(2), hundred)));
    sf(0x30FC, add(T(4), f(0x1000B9B0)));
    for (u32 n = 0; n != 6; ++n) {
      u32 b = n < 4 ? hand(n / 2) : boss();
      u32 model = gabi::load<u32>(b + 0x3E4);
      gabi::Local<be<u32>[2]> name;
      (*name)[0] = 0x1000B710;
      (*name)[1] = 0x1000B714;
      const u32 ids[6] = {0x4A, 0x64, 0x4D, 0x67, 0x42, 0x5C};
      u32 resource = gabi::call<u32>(
          0x026066C4, ptr(gabi::load<u32>(0x101F4F28)), name.get(), ids[n]);
      u32 data = gabi::load<u32>(model + 0xAC);
      b = n < 4 ? hand(n / 2) : boss();
      u32 animator = gabi::load<u32>(b + (n % 2 ? 0x3E8 : 0x3EC));
      gabi::call(n % 2 ? 0x025E7CE0 : 0x025E8154, ptr(animator), ptr(data),
                 ptr(resource), 1, 2, one, 0, -1, 1, 0);
    }
    sh(0x30B8, 500);
    gabi::store<u32>(player + 0x430, 23);
    gabi::store<u32>(player + 0x428, 2);
    gabi::store<s16>(player + 0x420, 3);
    break;
  case 15:
    chase(0x30E0, f(boss() + 0x314), tenth, f(0x1000B890));
    chase(0x30E4, add(f(boss() + 0x380), T(0)), tenth, f(0x1000B890));
    chase(0x30E8, f(boss() + 0x31C), tenth, f(0x1000B890));
    copy(a + 0x30D4, player + 0x314);
    sf(0x30D4, sub(F(0x30D4), add(T(1), f(0x1000B8B0))));
    sf(0x30D8, add(F(0x30D8), add(T(2), hundred)));
    chase(0x30FC, f(0x1000B98C), tenth, f(0x1000B9B4));
    if (S(0x30D0) == 200) {
      setVector(shock.get(), zero, zero, zero);
      u32 control = gabi::load<u32>(getPlay() + 0x5AB0);
      u32 emitter = gabi::call<u32>(0x025A847C, ptr(control), 0, 0x81E9,
                                    shock.get(), 0, 0, 0xFF, 0, -1, 0, 0, 0);
      gabi::store<u32>(a + 0x312C, emitter);
      gabi::store<u8>(gabi::load<u32>(0x101FFC78) + 0x1E44, 1);
    }
    if (S(0x30D0) == 300) {
      finish();
      sb(0x30CE, 0);
      sb(0x30B0, 10);
      gabi::call(0x025B9098, ptr(gabi::load<u32>(0x101F84DC) + 0x798), 5);
      gabi::call(0x025E18EC, 0x80000023u);
      sh(0x30CC, 1);
      sb(0x3330, 1);
      gabi::store<u8>(0x1047B07B, 0);
    }
    break;
  case 51:
  defeatIntroduction:
    playerPos(add(T(1), f(0x1000B9BC)));
    sf(0x31C, zero);
    sf(0x30E0, f(0x1000B878));
    sf(0x30E4, F(0x318));
    sf(0x30E8, zero);
    sh(0x32A, -0x4000);
    sf(0x314, f(0x1000B878));
    sf(0x30E4, add(F(0x30E4), add(T(0), f(0x1000B878))));
    sf(0x30D8, add(T(3), f(0x1000B874)));
    sf(0x30DC, zero);
    chase(0x30D4, add(T(2), f(0x1000B9C0)), f(0x1000B87C),
          mul(f(0x1000B8BC), F(0x30F8)));
    chase(0x30F8, one, one, f(0x1000B864));
    if (S(0x30D0) < 80)
      sf(0x3100, add(T(9), f(0x1000B8BC)));
    if (S(0x30D0) == 90) {
      gabi::store<s16>(boss() + 0x130E, 2);
      sf(0x3104, zero);
      sb(0x3108, 2);
    }
    if (S(0x30D0) == 100)
      sb(0x3108, 3);
    if (S(0x30D0) > 100 && S(0x30D0) < 530)
      sound(a, a + 0x37C, 0x703A);
    if (S(0x30D0) == 120) {
      message(0x1710);
      gabi::store<u8>(0x1047B07B, 0xFF);
    }
    if (S(0x30D0) == 240)
      messageEnd();
    if (S(0x30D0) == TS(4) + 260)
      message(0x1711);
    if (S(0x30D0) == TS(4) + 350)
      messageEnd();
    if (S(0x30D0) == 2 * TS(4) + 370)
      message(0x1712);
    if (S(0x30D0) == 2 * TS(4) + 520)
      messageEnd();
    if (S(0x30D0) == 3 * TS(4) + 540)
      message(0x1713);
    if (S(0x30D0) == 3 * TS(4) + 630)
      messageEnd();
    if (S(0x30D0) != 3 * TS(4) + 660)
      break;
    sh(0x30D0, 0);
    sf(0x30F8, zero);
    sb(0x30CE, B(0x30CE) + 1);
    highCamera();
    gabi::call(0x025E1944);
    sh(0x30CC, 1);
    [[fallthrough]];
  case 52:
    if (S(0x30D0) <= 30)
      break;
    sf(0x30F8, zero);
    sb(0x30CE, B(0x30CE) + 1);
    sh(0x30D0, 0);
    gabi::store<u32>(player + 0x430, 26);
    [[fallthrough]];
  case 53:
    copy(a + 0x30E0, player + 0x314);
    sf(0x30E4, add(F(0x30E4), add(T(10), hundred)));
    copy(a + 0x30D4, player + 0x314);
    sf(0x30D4, add(F(0x30D4), add(T(11), f(0x1000B898))));
    sf(0x30D8, add(F(0x30D8), add(T(12), f(0x1000B890))));
    if (S(0x30D0) <= (s16)(TS(3) + 65))
      break;
    sf(0x30F8, zero);
    sb(0x30CE, B(0x30CE) + 1);
    sh(0x30D0, 0);
    highCamera();
    sb(0x3108, 4);
    sh(0x130E, 10);
    sf(0x3104, one);
    gabi::call(0x025B9098, ptr(gabi::load<u32>(0x101F84DC) + 0x798), 3);
    setVector(offset.get(), zero, zero, zero);
    gabi::call(0x025D9874, offset.get(), 0, gabi::load<s8>(a + 0x326), 0);
    [[fallthrough]];
  case 54:
    if (S(0x30D0) == TS(4) + 22)
      gabi::store<u32>(player + 0x430, 24);
    if (S(0x30D0) == TS(5) + 150)
      gabi::store<u32>(player + 0x430, 29);
    chase(0x30E4, add(T(8), f(0x1000B874)), f(0x1000B9CC),
          add(T(9), f(0x1000B878)));
    if (S(0x30D0) != TS(6) + 250)
      break;
    copy(a + 0x30E0, a + 0x314);
    sh(0x30D0, 0);
    sb(0x30CE, B(0x30CE) + 1);
    sf(0x30F8, zero);
    sf(0x30E4, add(F(0x30E4), add(T(10), hundred)));
    // HD adds these tuning values after subtracting the shot offsets.
    sf(0x30D4, add(sub(F(0x314), f(0x1000B9D0)), T(11)));
    sf(0x30D8, add(sub(F(0x318), f(0x1000B9D4)), T(12)));
    sf(0x30DC, add(sub(F(0x31C), f(0x1000B8B0)), T(13)));
    sh(0x30CC, 100);
    [[fallthrough]];
  case 55:
    defeatCamera();
    gabi::call(0x0201ADE0, ptr(a + 0x2EC), result.get(), ptr(a + 0x314));
    copy(gabi::ea(offset.get()), gabi::ea(result.get()));
    if (!(std_sqrtf(PSVECSquareMag(offset.get())) < f(0x1000B8BC)))
      break;
    {
      u32 id = gabi::load<u16>(0x101928A0 + 2 * gabi::load<u8>(a + 0x3D0));
      s8 room = gabi::load<s8>(a + 0x326);
      particle(id, gabi::at<cXyz>(a + 0x314), room);
    }
    setVector(shock.get(), zero, one, zero);
    {
      u32 play = getPlay();
      s32 shockStrength = TS(2) + 3;
      gabi::call(0x025CB374, ptr(play + 0x599C), shockStrength, -0x21,
                 shock.get());
    }
    sound(a, a + 0x37C, 0x6990);
    sf(0x3100, add(T(9), f(0x1000B8BC)));
    sh(0x135E, TS(8) + 3);
    copy(a + 0x314, a + 0x2EC);
    sh(0x30D0, 0);
    sb(0x30CE, 56);
    gabi::call(0x020E5068, actor);
    break;
  case 56:
    defeatCamera();
    if (S(0x30D0) == TS(4) + 50) {
      gabi::call(0x020E47D0, actor, 20, f(0x1000B8B8), 0, one, -1);
      sound(a, a + 0x314, 0x58E2);
      sh(0x30CC, 1);
    }
    if (gabi::ftoi(f(gabi::load<u32>(a + 0x3D4) + 0x9C)) == 26) {
      auto jointMatrix = [&](u32 offsetBytes) {
        u32 model = gabi::load<u32>(gabi::load<u32>(a + 0x3D4) + 0x90);
        u32 block = gabi::load<u32>(model + 0x2C);
        u16 flags = gabi::load<u16>(block + 4);
        u32 matrices = gabi::load<u32>(block + 0x10);
        gabi::store<u16>(block + 4, flags | 0x10);
        gabi::call(0x028E90D4, ptr(matrices + offsetBytes),
                   ptr(gabi::load<u32>(0x1018C7B0)));
      };
      jointMatrix(0x150);
      setVector(offset.get(), add(T(2), f(0x1000B8D4)), T(3), T(4));
      sound(a, a + 0x37C, 0x6901);
      MtxPosition(offset.get(), transformed.get());
      setVector(scale.get(), one, one, one);
      itemAngle->x = gabi::load<s16>(a + 0x328);
      itemAngle->y = (s16)(S(0x32A) + TS(7) - 300);
      itemAngle->z = gabi::load<s16>(a + 0x32C);
      u32 id = gabi::call<u32>(0x025D8A5C, transformed.get(), 0,
                               gabi::load<s8>(a + 0x326), itemAngle.get(),
                               scale.get(), 1);
      gabi::store<u32>(a + 0x30C0, id);
      sound(a, a + 0x37C, 0x6987);
      jointMatrix(0);
      setVector(offset.get(), zero, zero, zero);
      MtxPosition(offset.get(), transformed.get());
      s8 room = gabi::load<s8>(a + 0x326);
      u32 smoke = gabi::load<u16>(0x1019285E);
      particle(smoke, transformed.get(), room);
    }
    if (gabi::ftoi(f(gabi::load<u32>(a + 0x3D4) + 0x9C)) > 26) {
      u32 item = findItem();
      if (item) {
        sb(0x30CE, 57);
        gabi::store<f32>(item + 0x370, add(T(6), f(0x1000B8DC)));
      }
    }
    break;
  case 57: {
    u32 item = findItem();
    if (item) {
      chase(0x30E0, f(item + 0x314), tenth, hundred);
      chase(0x30E4, f(item + 0x318), tenth, hundred);
      chase(0x30E8, f(item + 0x31C), tenth, hundred);
      sf(0x30FC, add(T(4), f(0x1000B98C)));
    }
    if (S(0x30D0) == 170) {
      finish();
      sb(0x30CE, 0);
      sb(0x30B0, 100);
      sh(0x30CC, 1);
      gabi::store<u8>(0x1047B07B, 0);
    }
    break;
  }
  }
  if (B(0x30CE) == 0)
    return;
  auto trig = [](s32 angle, bool cosine) {
    return gabi::load<f32>(0x104A44F8 + ((u16)angle >> 3) * 8 +
                           (cosine ? 4 : 0));
  };
  s32 frame = S(0x30D0);
  f32 magnitude = F(0x3100);
  f32 x = mul(trig(frame * 0x3300, false), magnitude),
      y = mul(trig(frame * 0x3000, true), magnitude),
      z = mul(trig(frame * 0x3500, true), magnitude);
  setVector(shakeEye.get(), sub(F(0x30E0), x), sub(F(0x30E4), y),
            sub(F(0x30E8), z));
  setVector(shakeCenter.get(), add(F(0x30D4), x), add(F(0x30D8), y),
            add(F(0x30DC), z));
  s16 bank = (s16)gabi::ftoi(
      mul(mul(trig(S(0x1308) * 0x1C00, true), magnitude), f(0x1000B9D8)));
  gabi::call(0x02514FE8, ptr(camera), shakeEye.get(), shakeCenter.get(), bank,
             F(0x30FC));
  cLib_addCalc0(gabi::at<be<f32>>(a + 0x3100), one, one);
  gabi::call(0x027EC9E8, 30, 410, ptr(0x1000B82C), B(0x30CE));
  gabi::call(0x027EC9E8, 30, 430, ptr(0x1000B840), S(0x30D0));
  sh(0x30D0, S(0x30D0) + 1);
}
VERIFY(0x020EE200, demo_camera);

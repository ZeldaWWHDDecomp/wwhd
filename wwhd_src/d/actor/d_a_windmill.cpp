#include "d/actor/d_a_windmill.h"
#include "bindings.h"

static Mtx34 *matrix() { return gabi::at<Mtx34>(0x1048D0CC); }
static u32 archive(u8 type) { return gabi::load<u32>(0x101D355C + 4u * type); }
static void translate(daWindMill_c *a) {
  gabi::call<void>(0x028E93CC, matrix(), (f32)a->current.pos.x,
                   (f32)a->current.pos.y, (f32)a->current.pos.z);
}
static void rotate(daWindMill_c *a, bool blade) {
  s16 x = a->current.angle.x, y = a->current.angle.y, z = a->current.angle.z;
  if (blade) {
    s16 sum = (s16)((s16)a->angle[0] + (s16)a->angle[1]);
    if ((u8)a->type == 0)
      y = sum;
    else
      z = sum;
  }
  gabi::call<void>(0x025F1B48, matrix(), x, y, z);
}
static void multiply(cXyz *v) { gabi::call<void>(0x028E8F64, matrix(), v, v); }
static void register_collision(void *obj) {
  void *play = gabi::call<void *>(0x025200D4);
  gabi::call<void>(0x0200E240, gabi::at<void>(gabi::ea(play) + 0x26A4), obj);
}

BOOL windmill_create_heap(daWindMill_c *a) {
  WWHD_FUNC(0x024E0BA0, BOOL, a);
  wind::Local<be<u32>[2]> name;
  (*name)[1] = 0x10042BF4;
  u8 type = a->type;
  void *controller = gabi::at<void>(gabi::load<u32>(0x101F4F28));
  s16 index = gabi::load<s16>(0x10042CD8 + 2u * type);
  (*name)[0] = archive(type);
  void *data = gabi::call<void *>(0x026066C4, controller, name.get(), index);
  if (!data)
    gabi::call<void>(0x0273AA24, STR(0x10042C2C), 405, STR(0x10042C40));
  J3DModel *model =
      gabi::call<J3DModel *>(0x025E38E0, data, 0x80000u, 0x11000222u);
  a->model = model;
  if (!model)
    return 0;
  type = a->type;
  index = gabi::load<s16>(0x10042CDC + 2u * type);
  if (index == -1)
    return 1;
  void *bg = gabi::call<void *>(0x024F23F4, (void *)nullptr);
  a->background = bg;
  if (!bg)
    return 0;
  wind::Local<be<u32>[2]> bgName;
  type = a->type;
  (*bgName)[1] = 0x10042BF4;
  controller = gabi::at<void>(gabi::load<u32>(0x101F4F28));
  (*bgName)[0] = archive(type);
  index = gabi::load<s16>(0x10042CDC + 2u * type);
  void *res = gabi::call<void *>(0x026066C4, controller, bgName.get(), index);
  bg = a->background;
  if (gabi::call<s32>(0x0200A030, bg, res, 1, &a->backgroundMatrix) != 0)
    return 0;
  bg = a->background;
  gabi::store<u32>(gabi::ea(bg) + 0xA8, 0x024EE708);
  return 1;
}
VERIFY(0x024E0BA0, windmill_create_heap);
BOOL windmill_heap_thunk(daWindMill_c *a) {
  WWHD_FUNC(0x024E0CF0, BOOL, a);
  return windmill_create_heap(a);
}
VERIFY(0x024E0CF0, windmill_heap_thunk);

BOOL windmill_node_callback(void *node, s32 timing) {
  WWHD_FUNC(0x024E0CF4, BOOL, node, timing);
  if (timing != 0)
    return 1;
  void *joint = gabi::call<void *>(0x027F7878, node);
  u32 model = gabi::load<u32>(0x104B462C);
  daWindMill_c *a = gabi::at<daWindMill_c>(gabi::load<u32>(model + 0xB8));
  u16 jointNo = gabi::load<u16>(gabi::ea(joint) + 4);
  if (!a)
    return 1;
  a->angle[0] = (s16)((s16)a->angle[0] + (s16)a->angle[1]);
  u32 buffer = gabi::load<u32>(model + 0x2C);
  u32 matrices = gabi::load<u32>(buffer + 0x10);
  u16 flags = gabi::load<u16>(buffer + 4);
  gabi::store<u16>(buffer + 4, flags | 0x10);
  gabi::call<void>(0x028E90D4, gabi::at<Mtx34>(matrices + jointNo * 48u),
                   matrix());
  u8 type = a->type;
  if (type == 0)
    gabi::call<void>(0x025F1C28, matrix(), (s16)a->angle[0]);
  else if (type == 1)
    gabi::call<void>(0x025F1C5C, matrix(), (s16)a->angle[0]);
  buffer = gabi::load<u32>(model + 0x2C);
  flags = gabi::load<u16>(buffer + 4);
  matrices = gabi::load<u32>(buffer + 0x10);
  gabi::store<u16>(buffer + 4, flags | 0x10);
  mtx_copy(gabi::at<Mtx34>(matrices + jointNo * 48u), matrix());
  gabi::call<void>(0x028E90D4, matrix(), gabi::at<Mtx34>(0x104B4868));
  gabi::call<void>(0x028E90D4, matrix(), &a->backgroundMatrix);
  a->shape_angle.y = a->angle[0];
  return 1;
}
VERIFY(0x024E0CF4, windmill_node_callback);

void windmill_set_matrix(daWindMill_c *a) {
  WWHD_FUNC(0x024E0F8C, void, a);
  f32 x = a->scale.x, y = a->scale.y, z = a->scale.z;
  u32 model = gabi::ea((J3DModel *)a->model);
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC0, y);
  gabi::store<f32>(model + 0xC4, z);
  translate(a);
  rotate(a, false);
  mtx_copy(gabi::at<Mtx34>(gabi::ea((J3DModel *)a->model) + 0xC8), matrix());
}
VERIFY(0x024E0F8C, windmill_set_matrix);
void windmill_search_wind(daWindMill_c *a) {
  WWHD_FUNC(0x024E106C, void, a);
  wind::Local<be<s16>> profile;
  *profile = 0x187;
  void *wind =
      gabi::call<void *>(0x025D5218, gabi::at<void>(0x025E121C), profile.get());
  a->windTagId = wind ? gabi::load<u32>(gabi::ea(wind) + 4) : 0xFFFFFFFFu;
}
VERIFY(0x024E106C, windmill_search_wind);
void windmill_init(daWindMill_c *a) {
  WWHD_FUNC(0x024E10C8, void, a);
  u32 model = gabi::ea((J3DModel *)a->model);
  u8 type = a->type;
  gabi::store<u32>(gabi::ea(a) + 0x348, model ? model + 0xC8 : 0);
  u32 cull = 0x10042CE4 + 24u * type;
  f32 loX = gabi::load<f32>(cull), loY = gabi::load<f32>(cull + 4),
      loZ = gabi::load<f32>(cull + 8);
  f32 hiX = gabi::load<f32>(cull + 12), hiY = gabi::load<f32>(cull + 16),
      hiZ = gabi::load<f32>(cull + 20);
  gabi::call<void>(0x025D674C, a, loX, loY, loZ, hiX, hiY, hiZ);
  gabi::store<f32>(gabi::ea(a) + 0x364, 3.f);
  gabi::call<void>(0x02515F14, &a->status, 255, 255, a);
  type = a->type;
  if (type <= 1) {
    for (int i = 0; i < 4; i++) {
      gabi::call<void>(0x025164C0, &a->capsules[i], gabi::at<void>(0x101D34A8));
      gabi::store<u32>(gabi::ea(&a->capsules[i]) + 0x44, gabi::ea(&a->status));
    }
    if (type == 1) {
      for (int i = 0; i < 9; i++) {
        gabi::call<void>(0x0251677C, &a->spheres[i],
                         gabi::at<void>(0x101D3448));
        gabi::store<u32>(gabi::ea(&a->spheres[i]) + 0x44, gabi::ea(&a->status));
        if (i < 4)
          gabi::call<void>(0x02018C8C,
                           gabi::at<void>(gabi::ea(&a->spheres[i]) + 0x118),
                           60.f);
      }
    } else {
      gabi::call<void>(0x02516518, &a->cylinder, gabi::at<void>(0x101D34F4));
      gabi::store<u32>(gabi::ea(&a->cylinder) + 0x44, gabi::ea(&a->status));
    }
  }
  model = gabi::ea((J3DModel *)a->model);
  gabi::store<u32>(model + 0xB8, gabi::ea(a));
  windmill_set_matrix(a);
  model = gabi::ea((J3DModel *)a->model);
  void *joints = gabi::call<void *>(
      0x027F3F94, gabi::at<void>(gabi::load<u32>(model + 0xAC)));
  u16 count = gabi::load<u16>(gabi::ea(joints) + 8);
  for (u16 i = 0; i < count;) {
    if (i == 2) {
      model = gabi::ea((J3DModel *)a->model);
      u32 data = gabi::load<u32>(model + 0xAC);
      u32 size = gabi::load<u32>(data + 4), joint = gabi::load<u32>(data + 8);
      if (size > 2)
        joint += 0x38;
      gabi::store<u32>(joint + 8, 0x024E0CF4);
      break;
    }
    model = gabi::ea((J3DModel *)a->model);
    i++;
    joints = gabi::call<void *>(0x027F3F94,
                                gabi::at<void>(gabi::load<u32>(model + 0xAC)));
    count = gabi::load<u16>(gabi::ea(joints) + 8);
  }
  gabi::call<void>(0x027F4D5C, (J3DModel *)a->model);
  windmill_search_wind(a);
  if (a->background) {
    translate(a);
    gabi::call<void>(0x025F1C28, matrix(), (s16)a->current.angle.y);
    gabi::call<void>(0x028E90D4, matrix(), &a->backgroundMatrix);
    void *play = gabi::call<void *>(0x025200D4);
    gabi::call<void>(0x024EEA6C, gabi::at<void>(gabi::ea(play) + 0x12A0),
                     (void *)a->background, a);
    gabi::call<void>(0x024F43DC, (void *)a->background);
  }
}
VERIFY(0x024E10C8, windmill_init);
s32 windmill_create(daWindMill_c *a) {
  WWHD_FUNC(0x024E1370, s32, a);
  if (!(gabi::load<u32>(gabi::ea(a) + 0x2E4) & 8)) {
    if (a) {
      gabi::call<void>(0x025D4ED0, a);
      gabi::store<u32>(gabi::ea(a) + 0xB4, 0x10042C1C);
      gabi::call<void>(0x0200BD2C, &a->status);
      gabi::call<void>(0x02515DA0, gabi::at<void>(gabi::ea(a) + 0x3D4));
      gabi::store<u32>(gabi::ea(a) + 0x3D0, 0x1004AE88);
      gabi::store<u32>(gabi::ea(a) + 0x3D4, 0x1004AEC0);
      gabi::call<void>(0x028EFFD0, a->spheres, 9, 0x12C,
                       gabi::at<void>(0x025166F0));
      gabi::call<void>(0x028EFFD0, a->capsules, 4, 0x138,
                       gabi::at<void>(0x024E1DA4));
      gabi::call<void>(0x02515FB8, &a->cylinder);
      gabi::store<u32>(gabi::ea(a) + 0x1518, 0x100015A8);
      gabi::store<u32>(gabi::ea(a) + 0x1514, 0x10042C0C);
      gabi::call<void>(0x02018590, gabi::at<void>(gabi::ea(a) + 0x151C));
      u32 condition = gabi::load<u32>(gabi::ea(a) + 0x2E4);
      gabi::store<u32>(gabi::ea(a) + 0x1518, 0x1004B160);
      gabi::store<u32>(gabi::ea(a) + 0x1440, 0x1004B108);
      gabi::store<u32>(gabi::ea(a) + 0x1530, 0x1004B150);
      gabi::store<u32>(gabi::ea(a) + 0x2E4, condition | 8);
    } else
      gabi::store<u32>(gabi::ea(a) + 0x2E4,
                       gabi::load<u32>(gabi::ea(a) + 0x2E4) | 8);
  }
  u8 type = gabi::load<u32>(gabi::ea(a) + 0xB0) & 15;
  a->type = type;
  if (type >= 2) {
    gabi::call<void>(0x0273AA24, STR(0x10042C74), 615, STR(0x10042C5C));
    type = a->type;
  }
  s32 phase = gabi::call<s32>(0x02520460, a->phase, STR(archive(type)));
  if (phase == 4) {
    type = a->type;
    s16 size = gabi::load<s16>(0x10042CE0 + 2u * type);
    if (!gabi::call<BOOL>(0x025D63E8, a, gabi::at<void>(0x024E0CF0), size))
      return 5;
    windmill_init(a);
  }
  return phase;
}
VERIFY(0x024E1370, windmill_create);
s32 windmill_create_thunk(daWindMill_c *a) {
  WWHD_FUNC(0x024E1518, s32, a);
  return windmill_create(a);
}
VERIFY(0x024E1518, windmill_create_thunk);
BOOL windmill_delete(daWindMill_c *a) {
  WWHD_FUNC(0x024E151C, BOOL, a);
  if (gabi::load<u32>(gabi::ea(a) + 0xF4) && a->background) {
    void *play = gabi::call<void *>(0x025200D4);
    gabi::call<void>(0x020087EC, gabi::at<void>(gabi::ea(play) + 0x12A0),
                     (void *)a->background);
  }
  gabi::call<void>(0x025204C8, a->phase, STR(archive(a->type)));
  return 1;
}
VERIFY(0x024E151C, windmill_delete);
BOOL windmill_delete_thunk(daWindMill_c *a) {
  WWHD_FUNC(0x024E158C, BOOL, a);
  return windmill_delete(a);
}
VERIFY(0x024E158C, windmill_delete_thunk);
BOOL windmill_draw(daWindMill_c *a) {
  WWHD_FUNC(0x024E1590, BOOL, a);
  void *light = gabi::call<void *>(0x02555D0C);
  gabi::call<void>(0x025626A4, light, 0, &a->current.pos,
                   gabi::at<void>(gabi::ea(a) + 0x110));
  light = gabi::call<void *>(0x02555D0C);
  gabi::call<void>(0x02562F5C, light, (J3DModel *)a->model,
                   gabi::at<void>(gabi::ea(a) + 0x110));
  u8 type = a->type;
  if (type == 0) {
    void *play = gabi::call<void *>(0x025200D4);
    gabi::store<u32>(0x104B4634, gabi::load<u32>(gabi::ea(play) + 0x5D70));
    play = gabi::call<void *>(0x025200D4);
    gabi::store<u32>(0x104B4638, gabi::load<u32>(gabi::ea(play) + 0x5D74));
    gabi::call<void>(0x025E2E5C, (J3DModel *)a->model);
    play = gabi::call<void *>(0x025200D4);
    gabi::store<u32>(0x104B4634, gabi::load<u32>(gabi::ea(play) + 0x5D78));
    play = gabi::call<void *>(0x025200D4);
    gabi::store<u32>(0x104B4638, gabi::load<u32>(gabi::ea(play) + 0x5D7C));
  } else if (type == 1)
    gabi::call<void>(0x025E2DE0, (J3DModel *)a->model, 0);
  return 1;
}
VERIFY(0x024E1590, windmill_draw);
BOOL windmill_draw_thunk(daWindMill_c *a) {
  WWHD_FUNC(0x024E1650, BOOL, a);
  return windmill_draw(a);
}
VERIFY(0x024E1650, windmill_draw_thunk);
void windmill_move_blades(daWindMill_c *a) {
  WWHD_FUNC(0x024E1654, void, a);
  f32 wind = 0;
  windmill_search_wind(a);
  u32 id = a->windTagId;
  if (id != 0xFFFFFFFFu) {
    wind::Local<be<u32>> searchId;
    *searchId = id;
    void *tag = gabi::call<void *>(0x025D5218, gabi::at<void>(0x025E1234),
                                   searchId.get());
    if (tag) {
      u32 addr = gabi::ea(tag);
      u8 type = gabi::load<u8>(addr + 0x6EC);
      f32 scale = gabi::load<f32>(addr + 0x6F4);
      f32 length = gabi::load<f32>(0x1004BD40 + 8u * type) * scale;
      wind = gabi::load<f32>(addr + 0x6F0) / length;
    }
  }
  s16 target = (s16)gabi::ftoi(wind * 2500.f);
  gabi::call<void>(0x0200F378, &a->angle[1], target, 15, 100, 10);
  s16 speed = a->angle[1], previous = a->angle[2];
  if (previous <= speed && speed != 0) {
    u8 type = a->type;
    if (type <= 1) {
      s8 room = gabi::load<s8>(gabi::ea(a) + 0x326);
      s32 reverb = gabi::call<s32>(0x02520540, room);
      gabi::call<void>(0x025E1A40, type == 0 ? 0x7043 : 0x7044,
                       gabi::at<void>(gabi::ea(a) + 0x37C), 0, reverb);
    }
  }
}
VERIFY(0x024E1654, windmill_move_blades);
void windmill_set_attack(daWindMill_c *a) {
  WWHD_FUNC(0x024E179C, void, a);
  wind::Local<cXyz[5]> small;
  (*small)[0].set(0, 0, 70);
  (*small)[1].set(450, 0, 70);
  (*small)[2].set(-450, 0, 70);
  (*small)[3].set(0, 450, 70);
  (*small)[4].set(0, -450, 70);
  wind::Local<cXyz[4]> start, end;
  (*start)[0].set(-1400, 290, 130);
  (*start)[1].set(-1400, 290, -130);
  (*start)[2].set(130, 290, -1400);
  (*start)[3].set(-130, 290, -1400);
  (*end)[0].set(1400, 290, 130);
  (*end)[1].set(1400, 290, -130);
  (*end)[2].set(130, 290, 1400);
  (*end)[3].set(-130, 290, 1400);
  u8 type = a->type;
  if (type > 1 || (s16)a->angle[1] <= 1000)
    return;
  translate(a);
  s16 sum = (s16)((s16)a->angle[0] + (s16)a->angle[1]);
  s16 x = a->current.angle.x, y, z;
  if (type == 1) {
    y = a->current.angle.y;
    z = sum;
  } else {
    y = sum;
    z = a->current.angle.z;
  }
  gabi::call<void>(0x025F1B48, matrix(), x, y, z);
  if (type == 1)
    multiply(&(*small)[0]);
  for (int i = 0; i < 4; i++) {
    WindmillCapsuleData *data = &a->capsuleData[i];
    if (type == 1) {
      multiply(&(*small)[i + 1]);
      data->start.copy((*small)[0]);
      data->end.copy((*small)[i + 1]);
      data->radius = 70.f;
    } else {
      multiply(&(*start)[i]);
      multiply(&(*end)[i]);
      data->start.copy((*start)[i]);
      data->end.copy((*end)[i]);
      data->radius = 170.f;
    }
    gabi::call<void>(0x020181FC,
                     gabi::at<void>(gabi::ea(&a->capsules[i]) + 0x118), data);
    if (type == 0)
      gabi::store<u8>(gabi::ea(&a->capsules[i]) + 0x6F, 10);
  }
  for (int i = 0; i < 4; i++)
    register_collision(&a->capsules[i]);
}
VERIFY(0x024E179C, windmill_set_attack);
void windmill_set_contact(daWindMill_c *a) {
  WWHD_FUNC(0x024E1ADC, void, a);
  wind::Local<cXyz[9]> points;
  (*points)[0].set(150, 0, 70);
  (*points)[1].set(-150, 0, 70);
  (*points)[2].set(0, 150, 70);
  (*points)[3].set(0, -150, 70);
  (*points)[4].set(0, 0, 70);
  (*points)[5].set(350, 0, 70);
  (*points)[6].set(-350, 0, 70);
  (*points)[7].set(0, 350, 70);
  (*points)[8].set(0, -350, 70);
  u8 type = a->type;
  if (type == 0) {
    gabi::call<void>(0x020182E0, gabi::at<void>(gabi::ea(&a->cylinder) + 0x118),
                     &a->current.pos);
    register_collision(&a->cylinder);
  } else if (type == 1 && (s16)a->angle[1] <= 1000) {
    translate(a);
    s16 sum = (s16)((s16)a->angle[0] + (s16)a->angle[1]);
    gabi::call<void>(0x025F1B48, matrix(), (s16)a->current.angle.x,
                     (s16)a->current.angle.y, sum);
    for (int i = 0; i < 9; i++)
      multiply(&(*points)[i]);
    for (int i = 0; i < 9; i++) {
      gabi::call<void>(0x02018D40,
                       gabi::at<void>(gabi::ea(&a->spheres[i]) + 0x118),
                       &(*points)[i]);
      register_collision(&a->spheres[i]);
    }
  }
}
VERIFY(0x024E1ADC, windmill_set_contact);
BOOL windmill_execute(daWindMill_c *a) {
  WWHD_FUNC(0x024E1C84, BOOL, a);
  windmill_move_blades(a);
  windmill_set_attack(a);
  windmill_set_contact(a);
  windmill_set_matrix(a);
  if ((u8)a->type == 0)
    gabi::call<void>(0x027F4D5C, (J3DModel *)a->model);
  if (a->background)
    gabi::call<void>(0x024F43DC, (void *)a->background);
  a->angle[2] = a->angle[1];
  return 1;
}
VERIFY(0x024E1C84, windmill_execute);
BOOL windmill_execute_thunk(daWindMill_c *a) {
  WWHD_FUNC(0x024E1CF8, BOOL, a);
  return windmill_execute(a);
}
VERIFY(0x024E1CF8, windmill_execute_thunk);
void windmill_static_init() {
  WWHD_FUNC(0x024E1CFC, void);
  gabi::store<u32>(0x1046EC14, 0);
  gabi::store<u32>(0x1046EC0C, 0);
  gabi::store<u32>(0x1046EC18, 0);
  gabi::store<u32>(0x1046EC10, 0);
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101D3538));
  gabi::store<f32>(0x1046EC00, -3.1415927410125732f);
  gabi::store<f32>(0x1046EC04, 3.1415927410125732f);
  gabi::call<void>(0x028ED6F8, gabi::at<void>(0x1046EC08));
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101D3544));
  gabi::call<void>(0x028EAB2C, gabi::at<void>(0x1046EC09));
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101D3550));
}
VERIFY(0x024E1CFC, windmill_static_init);
void windmill_local_delete(void *obj, u32 flags) {
  WWHD_FUNC(0x024E1D90, void, obj, flags);
  if (obj && (flags & 1))
    gabi::call<void>(0x0273AF40, obj);
}
VERIFY(0x024E1D90, windmill_local_delete);
dCcD_Cps *windmill_capsule_ctor(dCcD_Cps *obj) {
  WWHD_FUNC(0x024E1DA4, dCcD_Cps *, obj);
  if (!obj)
    obj = gabi::call<dCcD_Cps *>(0x0273AD10, 0x138);
  if (!obj)
    return obj;
  gabi::call<void>(0x02515FB8, obj);
  u32 addr = gabi::ea(obj);
  gabi::store<u32>(addr + 0x114, 0x100015A8);
  gabi::store<u32>(addr + 0x110, 0x10042C0C);
  gabi::call<void>(0x02018150, gabi::at<void>(addr + 0x118));
  gabi::store<u32>(addr + 0x3C, 0x1004AF18);
  gabi::store<u32>(addr + 0x130, 0x1004AF60);
  gabi::store<u32>(addr + 0x114, 0x1004AF70);
  return obj;
}
VERIFY(0x024E1DA4, windmill_capsule_ctor);
BOOL windmill_is_delete(void *obj) {
  WWHD_FUNC(0x024E1E30, BOOL, obj);
  return 1;
}
VERIFY(0x024E1E30, windmill_is_delete);
void windmill_destructor(daWindMill_c *a, u32 flags) {
  WWHD_FUNC(0x024E1E38, void, a, flags);
  if (!a)
    return;
  gabi::call<void>(0x02515A70, &a->cylinder, 2);
  gabi::call<void>(0x028F0164, a->capsules, 4, 0x138,
                   gabi::at<void>(0x02515980), 0, 0);
  gabi::call<void>(0x028F0164, a->spheres, 9, 0x12C, gabi::at<void>(0x02515AE8),
                   0, 0);
  gabi::call<void>(0x02515860, &a->status, 2);
  gabi::call<void>(0x025D50BC, a, 0);
  if (flags & 1)
    gabi::call<void>(0x0273AF40, a);
}
VERIFY(0x024E1E38, windmill_destructor);
void windmill_empty(void *obj) { WWHD_FUNC(0x024E1EE4, void, obj); }
VERIFY(0x024E1EE4, windmill_empty);

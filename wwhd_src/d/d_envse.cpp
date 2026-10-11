/* Environmental sounds along room paths, GameCube names with HD semantics.
 * Verified against WWHD. */
#include "d/d_envse.h"

// HD: the room-control byte is in BSS, beyond the loaded image data.
static be<s8>& l_stayRoom = *gabi::at<be<s8>>(0x1047E6C8);
static const f32 l_maxDistance = 3.4028234663852886e+38;

/* inlined into dEnvSe_getNearPathPos and dEnvSe_c::execute */
static void copyPosition(cXyz* destination, const cXyz* source) {
    dEnvSe_words* dst = (dEnvSe_words*)destination;
    const dEnvSe_words* src = (const dEnvSe_words*)source;
    // HD: all three words are loaded before the x/z/y stores.
    u32 x = src->x, y = src->y, z = src->z;
    dst->x = x;
    dst->z = z;
    dst->y = y;
}

/* 0252FCB4 */
static BOOL dEnvSe_Draw() {
    return TRUE;
}

/* 0252FCBC */
void dEnvSe_getNearPathPos(cXyz* out, cXyz* pos, path_layout::dPath* path) {
    gabi::Local<be<f32>> distance;
    gabi::Local<cXyz> first, second;
    gabi::Local<cM3dGLin> line;
    line->__vtbl = 0x1004CC7C;
    // HD: retain the unrounded distance result for every nearest-point comparison.
    f64 best = l_maxDistance;
    u32 nearest = 0;
    path_layout::dPnt* point = path->m_points;
    u32 count = path->m_num;
    for (u32 i = 0; i < count; ++i) {
        f64 d = dEnvSe_squareDistance(pos, &point->m_position);
        *distance = (f32)d;
        if (best > d) {
            best = d;
            nearest = i;
        }
        count = path->m_num;
        ++point;
    }
    point = &path->m_points[nearest];
    u32 hasFirst = 0, hasSecond = 0;
    if (nearest != 0) {
        dEnvSe_setLine(line.get(), &point[-1].m_position, &point->m_position);
        hasFirst = dEnvSe_projectLine(line.get(), pos, first.get(), distance.get());
        count = path->m_num;
    }
    if (nearest != count - 1) {
        dEnvSe_setLine(line.get(), &point->m_position, &point[1].m_position);
        hasSecond = dEnvSe_projectLine(line.get(), pos, second.get(), distance.get());
    }
    if (hasFirst) {
        if (!hasSecond || dEnvSe_squareDistance(second.get(), pos) > (f64)(f32)*distance) {
            copyPosition(out, first.get());
        } else {
            copyPosition(out, second.get());
        }
    } else if (hasSecond) {
        copyPosition(out, second.get());
    } else {
        out->x = point->m_position.x;
        out->y = point->m_position.y;
        out->z = point->m_position.z;
    }
}

/* 0252FEB8 */
BOOL dEnvSe_c::execute() {
    gabi::Local<cXyz> eye, temp1, temp2;
    s32 roomNo = l_stayRoom;
    dEnvSe_gameInfo* play = (dEnvSe_gameInfo*)dComIfGp_get();
    dEnvSe_roomDt* fili_p = dEnvSe_getStatusRoomDt(play->mRoomCtrl, roomNo);
    if (!fili_p)
        return TRUE;
    dEnvSe_soundInfo* soundInf = fili_p->getSoundInf();
    if (!soundInf)
        return TRUE;
    u32 count = soundInf->num;
    dEnvSe_soundEntry* sound_data = soundInf->m_entries;
    play = (dEnvSe_gameInfo*)dComIfGp_get();
    s32 cameraID = play->mPlayerCameraID;
    play = (dEnvSe_gameInfo*)dComIfGp_get();
    dEnvSe_camera* camera = play->mCameraInfo[cameraID].mpCamera;
    // HD: Eye includes the offset vector, via the original hidden-result call.
    dEnvSe_addEye(&camera->mEye, eye.get(), &camera->mEyeOffset);
    f64 maximum = l_maxDistance;
    for (; count != 0; --count, ++sound_data) {
        u32 type = sound_data->field_0x17;
        path_layout::dPath* path;
        if (type == dEnvSe_SEA_e || type == dEnvSe_RIVER_e || type == dEnvSe_WINDOW_e) {
            switch (type) {
            case dEnvSe_SEA_e:
                mDoAud_initSeaEnvPos();
                break;
            case dEnvSe_RIVER_e:
                mDoAud_initRiverPos();
                break;
            default:
                mDoAud_initWindowPos();
                break;
            }
            path = dEnvSe_getRoomPath((u32)sound_data->field_0x18, roomNo);
            while (path) {
                dEnvSe_getNearPathPos(&mPos, eye.get(), path);
                switch (type) {
                case dEnvSe_SEA_e:
                    mDoAud_registSeaEnvPos(&mPos);
                    break;
                case dEnvSe_RIVER_e:
                    mDoAud_registRiverPos(&mPos);
                    break;
                default:
                    mDoAud_registWindowPos(&mPos);
                    break;
                }
                path = dPath_GetNextRoomPath(path, roomNo);
            }
            if (type == dEnvSe_SEA_e) {
                mDoAud_seaEnvSePlay((u32)sound_data->field_0x14);
            } else if (type == dEnvSe_RIVER_e) {
                u32 reverb = dEnvSe_getReverb(roomNo);
                mDoAud_riverSePlay((u32)sound_data->field_0x14, reverb);
            }
        } else if (type == dEnvSe_SHORE_e || type == dEnvSe_WATERFALL_e) {
            copyPosition(&mPos, eye.get());
            f64 nearest = maximum;
            u32 arg = 0;
            path = dEnvSe_getRoomPath((u32)sound_data->field_0x18, roomNo);
            cXyz* tmp = type == dEnvSe_SHORE_e ? temp1.get() : temp2.get();
            while (path) {
                dEnvSe_getNearPathPos(tmp, eye.get(), path);
                f64 d = dEnvSe_squareDistance(tmp, eye.get());
                if (d < nearest) {
                    if (type == dEnvSe_SHORE_e)
                        arg = path->mArg0;
                    copyPosition(&mPos, tmp);
                    nearest = d;
                }
                path = dPath_GetNextRoomPath(path, roomNo);
            }
            if (type == dEnvSe_SHORE_e) {
                // HD: do not start a shore sound when no finite nearest path was selected.
                if (field_0xf8 == 0 && nearest != maximum) {
                    mDoAud_startSeaShoreSE(&mPos, arg);
                    field_0xfc = arg;
                } else {
                    mDoAud_continueSeaShoreSE(&mPos, field_0xfc);
                }
                // HD: the wave-frame store uses the low halfword, before reloading the counter.
                u16 frame = mWaveWords.mLow;
                play = (dEnvSe_gameInfo*)dComIfGp_get();
                play->mWaveFrame = frame;
                u32 ticks = field_0xf8;
                field_0xf8 = ticks == 99 ? 0 : ticks + 1;
            } else {
                u32 reverb = dEnvSe_getReverb(roomNo);
                mDoAud_waterfallSePlay((u32)sound_data->field_0x14, &mPos, reverb);
            }
        }
    }
    return TRUE;
}

/* 02530288 */
static BOOL dEnvSe_Execute(dEnvSe_c* i_this) {
    return i_this->execute();
}

/* 0253028C */
static BOOL dEnvSe_IsDelete() {
    return TRUE;
}

/* 02530294 */
static BOOL dEnvSe_Delete(dEnvSe_c* i_this) {
    mDoAud_seDeleteObject(&i_this->mPos);
    // HD: invoke the process base destructor after deleting the sound object.
    if (i_this)
        dEnvSe_processDelete(i_this, 0);
    return TRUE;
}

/* 025302DC */
static s32 dEnvSe_Create(dEnvSe_c* i_this) {
    // HD: the environment process has a C++ base constructor and vtable.
    if (i_this) {
        dEnvSe_processConstruct(i_this);
        i_this->__vtbl = 0x1004CC8C;
    }
    return cPhs_COMPLEATE_e;
}

/* ---- HD: compiler-generated header statics ---- */
/* 02530320 */
static void __sinit_d_envse_cpp() {
    sinit_header_statics_rodata(0x10475834, 0x101D61A8, 0x10475840, 0x1004CCA4);
}
/* WWHD glue
data l_maxDistance 1004CC70
ext 028E8DE8 f64 dEnvSe_squareDistance(const cXyz* a, const cXyz* b)
ext 0201883C void dEnvSe_setLine(cM3dGLin* line, const cXyz* start, const cXyz* end)
ext 02010AE4 u32 dEnvSe_projectLine(cM3dGLin* line, const cXyz* point, cXyz* out, be<f32>* distance)
ext 0201AD78 void dEnvSe_addEye(const cXyz* a, cXyz* out, const cXyz* b)
ext 025C11DC dEnvSe_roomDt* dEnvSe_getStatusRoomDt(void* control, s32 room)
ext 025AAF88 path_layout::dPath* dEnvSe_getRoomPath(u32 index, s32 room)
ext 025AB070 path_layout::dPath* dPath_GetNextRoomPath(path_layout::dPath* path, s32 room)
ext 02520540 u32 dEnvSe_getReverb(s32 room)
ext 025E1B70 void mDoAud_initSeaEnvPos()
ext 025E1B7C void mDoAud_registSeaEnvPos(cXyz* pos)
ext 025E1B8C void mDoAud_seaEnvSePlay(u32 value)
ext 025E1BA0 void mDoAud_startSeaShoreSE(cXyz* pos, u32 value)
ext 025E1BB8 void mDoAud_continueSeaShoreSE(cXyz* pos, u32 value)
ext 025E1BD0 void mDoAud_registRiverPos(cXyz* pos)
ext 025E1BE0 void mDoAud_riverSePlay(u32 value, u32 reverb)
ext 025E1BF8 void mDoAud_initRiverPos()
ext 025E1C04 void mDoAud_waterfallSePlay(u32 value, cXyz* pos, u32 reverb)
ext 025E1C20 void mDoAud_initWindowPos()
ext 025E1C2C void mDoAud_registWindowPos(cXyz* pos)
ext 025DD630 void dEnvSe_processDelete(base_process_class* object, u32 flags)
ext 025DD5F0 void dEnvSe_processConstruct(base_process_class* object)
*/

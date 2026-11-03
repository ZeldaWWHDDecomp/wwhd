// agl parameter archives (AAMP): list/object lookup by position and the byte-order conversion of a loaded
// archive, run once by the archive constructor. Written from WWHD's instructions; format facts: AAMP.
#include "agl/ResParameterArchive.h"
#include "agl/ShaderEndianConversions.h"
using namespace agl;

static void* allocateObject(u32 size);

/* inlined: the record behind a record of the same level */
template <class T> static T* nextRecord(T* record) {
    return reinterpret_cast<T*>(reinterpret_cast<u8*>(record) + record->mSize);
}

/* inlined: true for parameter types stored as 32-bit words (f32, int, vectors, colour, curves) */
static bool hasWordValue(u32 type) {
    if (type < ResParameterType_F32) return false;
    if (type <= ResParameterType_Curve4) return true;
    return type >= ResParameterType_Vec2 && type <= ResParameterType_Last;
}

/* 027A732C */
void ResParameterObj::modifyEndianObj(u32 byteOrder) {
    gabi::AutoFrame frame{gabi::current_function()};
    ShaderEndianConversions::convert(byteOrder, mData, sizeof(ResParameterObjData));
    ResParameterObjData* object = mData;
    u32 count = object->mParameterNum;
    ResParameterData* parameter = reinterpret_cast<ResParameterData*>(object + 1);
    for (u32 i = 0; i != count; ++i) {
        ShaderEndianConversions::convert(byteOrder, parameter, sizeof(ResParameterData));
        if (hasWordValue(parameter->mType)) {
            ShaderEndianConversions::convert(byteOrder, parameter + 1,
                parameter->mSize - sizeof(ResParameterData));
        }
        parameter = nextRecord(parameter);
    }
}

/* 027A73F4 */
ResParameterListData* ResParameterList::getResParameterList(u32 position) const {
    ResParameterListData* list = mData.get() + 1;
    for (u32 i = 0; i < position; ++i) list = nextRecord(list);
    return list;
}

/* 027A7424 */
ResParameterObjData* ResParameterList::getResParameterObj(u32 position) const {
    ResParameterListData* data = mData;
    ResParameterListData* list = data + 1;
    for (u32 i = 0; i < data->mListNum; ++i) list = nextRecord(list);
    ResParameterObjData* object = reinterpret_cast<ResParameterObjData*>(list);
    for (u32 i = 0; i < position; ++i) object = nextRecord(object);
    return object;
}

/* 027A747C */
void ResParameterList::modifyEndianList(u32 byteOrder) {
    gabi::AutoFrame frame{gabi::current_function()};
    ShaderEndianConversions::convert(byteOrder, mData, sizeof(ResParameterListData));
    ResParameterListData* entry = mData.get() + 1;
    for (u32 i = 0; i < mData->mListNum; ++i) {
        ResParameterList child;
        child.mData = entry;
        child.modifyEndianList(byteOrder);
        entry = nextRecord(child.mData.get());
    }
    ResParameterObjData* objectEntry = reinterpret_cast<ResParameterObjData*>(entry);
    for (u32 i = 0; i < mData->mObjNum; ++i) {
        ResParameterObj object;
        object.mData = objectEntry;
        object.modifyEndianObj(byteOrder);
        objectEntry = nextRecord(object.mData.get());
    }
}

/* 027A7558 */
ResParameterArchive* ResParameterArchive::create(ResParameterArchive* storage, void* data) {
    gabi::AutoFrame frame{gabi::current_function()};
    ResParameterArchive* archive = storage;
    if (archive == nullptr) archive = static_cast<ResParameterArchive*>(allocateObject(sizeof(ResParameterArchive)));
    if (archive == nullptr) return archive;
    archive->mData = static_cast<ResParameterArchiveData*>(data);
    if (archive->mData == nullptr) return archive;
    u32 otherOrder = (archive->mData->mFlags & kResParameterEndianFlags) != 0;
    if (!otherOrder) return archive;
    ShaderEndianConversions::convert(otherOrder, archive->mData, sizeof(ResParameterArchiveData));
    ResParameterArchiveData* header = archive->mData;
    ResParameterList root;
    root.mData = reinterpret_cast<ResParameterListData*>(reinterpret_cast<u8*>(header + 1) + header->mTypeNameSize);
    root.modifyEndianList((archive->mData->mFlags & kResParameterEndianFlags) != 0);
    // the flag now reads in this byte order: clearing it marks the archive converted
    archive->mData->mFlags = 1 - archive->mData->mFlags;
    return archive;
}

/* WWHD glue
ext 0273AD10 void* allocateObject(u32 size)
ext 027BF0CC void agl::ShaderEndianConversions::convert(u32 order, void* buffer, u32 bytes)
*/

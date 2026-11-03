// agl parameter archives (AAMP resources): the read-only views over a loaded parameter archive file and the
// in-place byte-order conversion its constructor runs. A file holds one root list; a list holds child lists
// followed by objects, an object holds parameters, and every record starts with its own byte size, so the
// records of one level are walked by adding sizes. Layouts from WWHD's instructions; format facts: AAMP.
#pragma once
#include "wwhd.h"
namespace agl {

// one parameter: header, then (size - sizeof header) value bytes; the parameters of an object follow each other
struct ResParameterData {
    be<u32> mSize;
    be<u32> mType;
    be<u32> mNameHash;
    ResParameterData* next() { return gabi::at<ResParameterData>(gabi::ea(this) + u32(mSize)); }
};
// one object: header, then its parameters
struct ResParameterObjData {
    be<u32> mSize;
    be<u32> mParameterNum;
    be<u32> mNameHash;
    be<u32> mReserved;
    ResParameterData* first() { return gabi::at<ResParameterData>(gabi::ea(this) + sizeof(ResParameterObjData)); }
    ResParameterData* find(u32 nameHash);
};
// one list: header, then its child lists, then its objects
struct ResParameterListData {
    be<u32> mSize;
    be<u32> mNameHash;
    be<u32> mListNum;
    be<u32> mObjNum;
};
// the file header (converted in place by the constructor); the root list follows the type name (mTypeNameSize bytes) behind it
struct ResParameterArchiveData {
    be<u32> mMagic;
    be<u32> mVersion;
    be<u32> mFlags;
    be<u32> mFileSize;
    be<u32> mReserved;
    be<u32> mTypeNameSize;
};

// parameter types whose value words are converted (bool and the fixed strings are bytes)
enum ResParameterType : u32 {
    ResParameterType_F32 = 1,
    ResParameterType_Curve4 = 6,
    ResParameterType_Vec2 = 9,
    ResParameterType_Last = 14,
};
// flag bit that marks a file still in the other byte order (tested in both byte orders)
static constexpr u32 kResParameterEndianFlags = 0x01000001;

struct ResParameterObj {
    gptr<ResParameterObjData> mData;
    void modifyEndianObj(u32 byteOrder);
};
struct ResParameterList {
    gptr<ResParameterListData> mData;
    ResParameterListData* getResParameterList(u32 position) const;
    ResParameterObjData* getResParameterObj(u32 position) const;
    void modifyEndianList(u32 byteOrder);
};
struct ResParameterArchive {
    gptr<ResParameterArchiveData> mData;
    static ResParameterArchive* create(ResParameterArchive* storage, void* data);
};
}
WWHD_SIZE(agl::ResParameterData, 0xC);
WWHD_OFFSET(agl::ResParameterData, mType, 4);
WWHD_OFFSET(agl::ResParameterData, mNameHash, 8);
WWHD_SIZE(agl::ResParameterObjData, 0x10);
WWHD_OFFSET(agl::ResParameterObjData, mParameterNum, 4);
WWHD_OFFSET(agl::ResParameterObjData, mNameHash, 8);
WWHD_SIZE(agl::ResParameterListData, 0x10);
WWHD_OFFSET(agl::ResParameterListData, mListNum, 8);
WWHD_OFFSET(agl::ResParameterListData, mObjNum, 0xC);
WWHD_OFFSET(agl::ResParameterArchiveData, mFlags, 8);
WWHD_OFFSET(agl::ResParameterArchiveData, mTypeNameSize, 0x14);
WWHD_SIZE(agl::ResParameterArchiveData, 0x18);
WWHD_SIZE(agl::ResParameterObj, 4);
WWHD_SIZE(agl::ResParameterList, 4);
WWHD_SIZE(agl::ResParameterArchive, 4);

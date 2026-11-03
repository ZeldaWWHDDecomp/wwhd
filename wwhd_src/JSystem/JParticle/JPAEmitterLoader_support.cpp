/** Cafe string teardown helpers emitted with the JPA archive loader. */
#include "JSystem/JParticle/JPAEmitterLoader_support.h"
using namespace jpa_loader;
/* 02847DD0 */
static void destroyString(LoaderBufferedString* object,s32 flags){if(object && (flags&1))JPALoaderDelete(object);}
/* 02847DE4 */
static void terminateString(LoaderBufferedString* object){auto* buffer=reinterpret_cast<const gptr<be<u8>>*>(&object->mTextPtr)->get();u32 length=object->mLength;buffer[length-1]=0;}
/* 02847DFC */
static void destroyStringBase(LoaderBufferedString* object,s32 flags){if(object && (flags&1))JPALoaderDelete(object);}
/* 02847E10 */
static void destroyStringVtable(void* object){}
/* 02847E14 */
static void destroyStringNode(void* object,s32 flags){if(object && (flags&1))JPALoaderDelete(object);}
/* 02847E28 */
static void destroyArchiveString(LoaderBufferedString* object,s32 flags){if(object && (flags&1))JPALoaderDelete(object);}
/* WWHD glue
 ext 0273AF40 void JPALoaderDelete(void*)
*/

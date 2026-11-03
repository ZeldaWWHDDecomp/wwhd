/* HD: compiler-emitted string adapter helpers used by the Cafe debug reporter. */
#include "JSystem/JUtility/JUTStringHelpers.h"
/* 027ECD0C */
static void JUTBufferedString_assureTermination(JUTReportBufferedString* string) {
    be<u8>* text=string->mText;
    u32 capacity=string->mCapacity;
    text[capacity-1]=0;
}
/* 027ECD24 */
static void JUTString_destroy(JUTReportString* string,u32 flags) {
    if(string && (flags & 1)) stringStorageDelete(string);
}
/* 027ECD38 */
static void JUTBufferedString_destroy(JUTReportBufferedString* string,u32 flags) {
    if(string && (flags & 1)) stringStorageDelete(string);
}
/* 027ECD4C */
static void JUTString_assureTermination(JUTReportString*) {}
/* 027ECD50 */
static void JUTStringBase_destroy(JUTReportString* string,u32 flags) {
    if(string && (flags & 1)) stringStorageDelete(string);
}
/* 027ECD64 */
static void JUTBufferedStringBase_destroy(JUTReportBufferedString* string,u32 flags) {
    if(string && (flags & 1)) stringStorageDelete(string);
}
/* WWHD glue
ext 0273AF40 void stringStorageDelete(void*)
*/

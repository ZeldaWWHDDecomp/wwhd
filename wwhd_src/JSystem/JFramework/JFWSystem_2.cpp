/* WWHD compiler initializer at the JFWSystem/JAudio boundary; assigned to the first JAudio unit. */
#include "JSystem/JFramework/JFWSystem_2.h"
static JFWBoundaryStatics headerStatics;
static JFWBoundaryDestructorRecord destructorRecord;
/* 02801444 */
void __sinit_JFWSystem_cpp() {
    headerStatics.mWords[2] = 0;
    headerStatics.mWords[0] = 0;
    headerStatics.mWords[3] = 0;
    headerStatics.mWords[1] = 0;
    registerJFWBoundaryStatics(&destructorRecord);
}
/* WWHD glue
data headerStatics 104B4EB4
data destructorRecord 101F9AE8
ext 028F026C void registerJFWBoundaryStatics(JFWBoundaryDestructorRecord*)
*/

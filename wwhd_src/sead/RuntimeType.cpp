// WWHD lazy runtime identities; reconstructed from the local game's instructions.
// The identity labels below are descriptive local names, not asserted original class names.
#include "sead/RuntimeType.h"

static u32 sControlBaseGuard;
static sead::RuntimeType sControlBaseType;
static u32 sControlDeviceGuard;
static sead::RuntimeType sControlDeviceType;
static u32 sInputAddonGuard;
static sead::RuntimeType sInputAddonType;
static u32 sObjectBaseGuard;
static sead::RuntimeType sObjectBaseType;
static u32 sTaskBaseGuard;
static sead::RuntimeType sTaskBaseType;
static u32 sSceneTaskGuard;
static sead::RuntimeType sSceneTaskType;
static u32 sChildSceneTaskGuard;
static sead::RuntimeType sChildSceneTaskType;
static u32 sLayoutTaskGuard;
static sead::RuntimeType sLayoutTaskType;
static u32 sOverlayTaskGuard;
static sead::RuntimeType sOverlayTaskType;
static u32 sWindowTaskGuard;
static sead::RuntimeType sWindowTaskType;
static u32 sPanelTaskGuard;
static sead::RuntimeType sPanelTaskType;
static u32 sDialogTaskGuard;
static sead::RuntimeType sDialogTaskType;
static u32 sFrameTaskGuard;
static sead::RuntimeType sFrameTaskType;
static u32 sReadStreamGuard;
static sead::RuntimeType sReadStreamType;
static u32 sControllerGuard;
static sead::RuntimeType sControllerType;

static u32 sControllerControllerTable;
static u32 sControlDeviceControlDeviceTable;
static u32 sControlDeviceControlBaseTable;
static u32 sInputAddonInputAddonTable;
static u32 sInputAddonControlBaseTable;
static u32 sSceneTaskSceneTaskTable;
static u32 sSceneTaskTaskBaseTable;
static u32 sSceneTaskObjectBaseTable;
static u32 sChildSceneTaskChildSceneTaskTable;
static u32 sChildSceneTaskSceneTaskTable;
static u32 sChildSceneTaskTaskBaseTable;
static u32 sChildSceneTaskObjectBaseTable;
static u32 sObjectChildSceneTaskChildSceneTaskTable;
static u32 sObjectChildSceneTaskSceneTaskTable;
static u32 sObjectChildSceneTaskTaskBaseTable;
static u32 sObjectChildSceneTaskObjectBaseTable;
static u32 sMenuChildSceneTaskChildSceneTaskTable;
static u32 sMenuChildSceneTaskSceneTaskTable;
static u32 sMenuChildSceneTaskTaskBaseTable;
static u32 sMenuChildSceneTaskObjectBaseTable;
static u32 sMapChildSceneTaskChildSceneTaskTable;
static u32 sMapChildSceneTaskSceneTaskTable;
static u32 sMapChildSceneTaskTaskBaseTable;
static u32 sMapChildSceneTaskObjectBaseTable;
static u32 sScreenChildSceneTaskChildSceneTaskTable;
static u32 sScreenChildSceneTaskSceneTaskTable;
static u32 sScreenChildSceneTaskTaskBaseTable;
static u32 sScreenChildSceneTaskObjectBaseTable;
static u32 sLayoutTaskLayoutTaskTable;
static u32 sLayoutTaskTaskBaseTable;
static u32 sLayoutTaskObjectBaseTable;
static u32 sOverlayTaskOverlayTaskTable;
static u32 sOverlayTaskTaskBaseTable;
static u32 sOverlayTaskObjectBaseTable;
static u32 sWindowTaskWindowTaskTable;
static u32 sWindowTaskTaskBaseTable;
static u32 sWindowTaskObjectBaseTable;
static u32 sPanelTaskPanelTaskTable;
static u32 sPanelTaskTaskBaseTable;
static u32 sPanelTaskObjectBaseTable;
static u32 sDialogTaskDialogTaskTable;
static u32 sDialogTaskTaskBaseTable;
static u32 sDialogTaskObjectBaseTable;
static u32 sFrameTaskFrameTaskTable;
static u32 sFrameTaskTaskBaseTable;
static u32 sFrameTaskObjectBaseTable;
static u32 sReadStreamReadStreamTable;
static u32 sReadStreamStreamTable;
static u32 sReadStreamStreamBaseTable;
static u32 sTaskBaseTaskBaseTable;
static u32 sTaskBaseObjectBaseTable;

static u32 sBufferedControllerDerivedGuard;
static sead::RuntimeType sBufferedControllerDerivedType;
static u32 sBufferedControllerDerivedTable;
static u32 sBufferedControllerParentGuard;
static sead::RuntimeType sBufferedControllerParentType;
static u32 sBufferedControllerParentTable;
static u32 sChannelControllerDerivedGuard;
static sead::RuntimeType sChannelControllerDerivedType;
static u32 sChannelControllerDerivedTable;
static u32 sChannelControllerParentGuard;
static sead::RuntimeType sChannelControllerParentType;
static u32 sChannelControllerParentTable;
static u32 sRemoteDeviceDerivedGuard;
static sead::RuntimeType sRemoteDeviceDerivedType;
static u32 sRemoteDeviceDerivedTable;
static u32 sRemoteDeviceParentGuard;
static sead::RuntimeType sRemoteDeviceParentType;
static u32 sRemoteDeviceParentTable;
static u32 sRemoteControllerDerivedGuard;
static sead::RuntimeType sRemoteControllerDerivedType;
static u32 sRemoteControllerDerivedTable;
static u32 sRemoteControllerParentGuard;
static sead::RuntimeType sRemoteControllerParentType;
static u32 sRemoteControllerParentTable;
static u32 sTabletDeviceDerivedGuard;
static sead::RuntimeType sTabletDeviceDerivedType;
static u32 sTabletDeviceDerivedTable;
static u32 sTabletDeviceParentGuard;
static sead::RuntimeType sTabletDeviceParentType;
static u32 sTabletDeviceParentTable;
static u32 sTabletControllerDerivedGuard;
static sead::RuntimeType sTabletControllerDerivedType;
static u32 sTabletControllerDerivedTable;
static u32 sTabletControllerParentGuard;
static sead::RuntimeType sTabletControllerParentType;
static u32 sTabletControllerParentTable;

static u32 sObjectChildSceneTaskGuard;
static sead::RuntimeType sObjectChildSceneTaskType;
static u32 sMenuChildSceneTaskGuard;
static sead::RuntimeType sMenuChildSceneTaskType;
static u32 sMapChildSceneTaskGuard;
static sead::RuntimeType sMapChildSceneTaskType;
static u32 sScreenChildSceneTaskGuard;
static sead::RuntimeType sScreenChildSceneTaskType;

/* 0273BB8C */
static u32 matchesController(void* object, const sead::RuntimeType* type) {
    if (sControllerGuard == 0) {
        sControllerGuard = 1;
        sControllerType.mTable = &sControllerControllerTable;
    }
    return type == &sControllerType;
}

/* 0273CD54 */
static u32 matchesControlDevice(void* object, const sead::RuntimeType* type) {
    if (sControlDeviceGuard == 0) {
        sControlDeviceGuard = 1;
        sControlDeviceType.mTable = &sControlDeviceControlDeviceTable;
    }
    if ( type == &sControlDeviceType) return 1;
    if (sControlBaseGuard == 0) {
        sControlBaseGuard = 1;
        sControlBaseType.mTable = &sControlDeviceControlBaseTable;
    }
    return type == &sControlBaseType;
}

/* 0272B1F0 */
static u32 matchesInputAddon(void* object, const sead::RuntimeType* type) {
    if (sInputAddonGuard == 0) {
        sInputAddonGuard = 1;
        sInputAddonType.mTable = &sInputAddonInputAddonTable;
    }
    if ( type == &sInputAddonType) return 1;
    if (sControlBaseGuard == 0) {
        sControlBaseGuard = 1;
        sControlBaseType.mTable = &sInputAddonControlBaseTable;
    }
    return type == &sControlBaseType;
}

/* 0272E544 */
static u32 matchesSceneTask(void* object, const sead::RuntimeType* type) {
    if (sSceneTaskGuard == 0) {
        sSceneTaskGuard = 1;
        sSceneTaskType.mTable = &sSceneTaskSceneTaskTable;
    }
    if ( type == &sSceneTaskType) return 1;
    if (sTaskBaseGuard == 0) {
        sTaskBaseGuard = 1;
        sTaskBaseType.mTable = &sSceneTaskTaskBaseTable;
    }
    if ( type == &sTaskBaseType) return 1;
    if (sObjectBaseGuard == 0) {
        sObjectBaseGuard = 1;
        sObjectBaseType.mTable = &sSceneTaskObjectBaseTable;
    }
    return type == &sObjectBaseType;
}

/* 0272E690 */
static u32 matchesChildSceneTask(void* object, const sead::RuntimeType* type) {
    if (sChildSceneTaskGuard == 0) {
        sChildSceneTaskGuard = 1;
        sChildSceneTaskType.mTable = &sChildSceneTaskChildSceneTaskTable;
    }
    if ( type == &sChildSceneTaskType) return 1;
    if (sSceneTaskGuard == 0) {
        sSceneTaskGuard = 1;
        sSceneTaskType.mTable = &sChildSceneTaskSceneTaskTable;
    }
    if ( type == &sSceneTaskType) return 1;
    if (sTaskBaseGuard == 0) {
        sTaskBaseGuard = 1;
        sTaskBaseType.mTable = &sChildSceneTaskTaskBaseTable;
    }
    if ( type == &sTaskBaseType) return 1;
    if (sObjectBaseGuard == 0) {
        sObjectBaseGuard = 1;
        sObjectBaseType.mTable = &sChildSceneTaskObjectBaseTable;
    }
    return type == &sObjectBaseType;
}

/* 0272EC00 */
static u32 matchesObjectChildSceneTask(void* object, const sead::RuntimeType* type) {
    if (sObjectChildSceneTaskGuard == 0) {
        sObjectChildSceneTaskGuard = 1;
        sObjectChildSceneTaskType.mTable = &sObjectChildSceneTaskChildSceneTaskTable;
    }
    if ( type == &sObjectChildSceneTaskType) return 1;
    if (sSceneTaskGuard == 0) {
        sSceneTaskGuard = 1;
        sSceneTaskType.mTable = &sObjectChildSceneTaskSceneTaskTable;
    }
    if ( type == &sSceneTaskType) return 1;
    if (sTaskBaseGuard == 0) {
        sTaskBaseGuard = 1;
        sTaskBaseType.mTable = &sObjectChildSceneTaskTaskBaseTable;
    }
    if ( type == &sTaskBaseType) return 1;
    if (sObjectBaseGuard == 0) {
        sObjectBaseGuard = 1;
        sObjectBaseType.mTable = &sObjectChildSceneTaskObjectBaseTable;
    }
    return type == &sObjectBaseType;
}

/* 0272F3F0 */
static u32 matchesMenuChildSceneTask(void* object, const sead::RuntimeType* type) {
    if (sMenuChildSceneTaskGuard == 0) {
        sMenuChildSceneTaskGuard = 1;
        sMenuChildSceneTaskType.mTable = &sMenuChildSceneTaskChildSceneTaskTable;
    }
    if ( type == &sMenuChildSceneTaskType) return 1;
    if (sSceneTaskGuard == 0) {
        sSceneTaskGuard = 1;
        sSceneTaskType.mTable = &sMenuChildSceneTaskSceneTaskTable;
    }
    if ( type == &sSceneTaskType) return 1;
    if (sTaskBaseGuard == 0) {
        sTaskBaseGuard = 1;
        sTaskBaseType.mTable = &sMenuChildSceneTaskTaskBaseTable;
    }
    if ( type == &sTaskBaseType) return 1;
    if (sObjectBaseGuard == 0) {
        sObjectBaseGuard = 1;
        sObjectBaseType.mTable = &sMenuChildSceneTaskObjectBaseTable;
    }
    return type == &sObjectBaseType;
}

/* 0272F8DC */
static u32 matchesMapChildSceneTask(void* object, const sead::RuntimeType* type) {
    if (sMapChildSceneTaskGuard == 0) {
        sMapChildSceneTaskGuard = 1;
        sMapChildSceneTaskType.mTable = &sMapChildSceneTaskChildSceneTaskTable;
    }
    if ( type == &sMapChildSceneTaskType) return 1;
    if (sSceneTaskGuard == 0) {
        sSceneTaskGuard = 1;
        sSceneTaskType.mTable = &sMapChildSceneTaskSceneTaskTable;
    }
    if ( type == &sSceneTaskType) return 1;
    if (sTaskBaseGuard == 0) {
        sTaskBaseGuard = 1;
        sTaskBaseType.mTable = &sMapChildSceneTaskTaskBaseTable;
    }
    if ( type == &sTaskBaseType) return 1;
    if (sObjectBaseGuard == 0) {
        sObjectBaseGuard = 1;
        sObjectBaseType.mTable = &sMapChildSceneTaskObjectBaseTable;
    }
    return type == &sObjectBaseType;
}

/* 02734940 */
static u32 matchesScreenChildSceneTask(void* object, const sead::RuntimeType* type) {
    if (sScreenChildSceneTaskGuard == 0) {
        sScreenChildSceneTaskGuard = 1;
        sScreenChildSceneTaskType.mTable = &sScreenChildSceneTaskChildSceneTaskTable;
    }
    if ( type == &sScreenChildSceneTaskType) return 1;
    if (sSceneTaskGuard == 0) {
        sSceneTaskGuard = 1;
        sSceneTaskType.mTable = &sScreenChildSceneTaskSceneTaskTable;
    }
    if ( type == &sSceneTaskType) return 1;
    if (sTaskBaseGuard == 0) {
        sTaskBaseGuard = 1;
        sTaskBaseType.mTable = &sScreenChildSceneTaskTaskBaseTable;
    }
    if ( type == &sTaskBaseType) return 1;
    if (sObjectBaseGuard == 0) {
        sObjectBaseGuard = 1;
        sObjectBaseType.mTable = &sScreenChildSceneTaskObjectBaseTable;
    }
    return type == &sObjectBaseType;
}

/* 0273381C */
static u32 matchesLayoutTask(void* object, const sead::RuntimeType* type) {
    if (sLayoutTaskGuard == 0) {
        sLayoutTaskGuard = 1;
        sLayoutTaskType.mTable = &sLayoutTaskLayoutTaskTable;
    }
    if ( type == &sLayoutTaskType) return 1;
    if (sTaskBaseGuard == 0) {
        sTaskBaseGuard = 1;
        sTaskBaseType.mTable = &sLayoutTaskTaskBaseTable;
    }
    if ( type == &sTaskBaseType) return 1;
    if (sObjectBaseGuard == 0) {
        sObjectBaseGuard = 1;
        sObjectBaseType.mTable = &sLayoutTaskObjectBaseTable;
    }
    return type == &sObjectBaseType;
}

/* 02733F0C */
static u32 matchesOverlayTask(void* object, const sead::RuntimeType* type) {
    if (sOverlayTaskGuard == 0) {
        sOverlayTaskGuard = 1;
        sOverlayTaskType.mTable = &sOverlayTaskOverlayTaskTable;
    }
    if ( type == &sOverlayTaskType) return 1;
    if (sTaskBaseGuard == 0) {
        sTaskBaseGuard = 1;
        sTaskBaseType.mTable = &sOverlayTaskTaskBaseTable;
    }
    if ( type == &sTaskBaseType) return 1;
    if (sObjectBaseGuard == 0) {
        sObjectBaseGuard = 1;
        sObjectBaseType.mTable = &sOverlayTaskObjectBaseTable;
    }
    return type == &sObjectBaseType;
}

/* 02734DFC */
static u32 matchesWindowTask(void* object, const sead::RuntimeType* type) {
    if (sWindowTaskGuard == 0) {
        sWindowTaskGuard = 1;
        sWindowTaskType.mTable = &sWindowTaskWindowTaskTable;
    }
    if ( type == &sWindowTaskType) return 1;
    if (sTaskBaseGuard == 0) {
        sTaskBaseGuard = 1;
        sTaskBaseType.mTable = &sWindowTaskTaskBaseTable;
    }
    if ( type == &sTaskBaseType) return 1;
    if (sObjectBaseGuard == 0) {
        sObjectBaseGuard = 1;
        sObjectBaseType.mTable = &sWindowTaskObjectBaseTable;
    }
    return type == &sObjectBaseType;
}

/* 027352D8 */
static u32 matchesPanelTask(void* object, const sead::RuntimeType* type) {
    if (sPanelTaskGuard == 0) {
        sPanelTaskGuard = 1;
        sPanelTaskType.mTable = &sPanelTaskPanelTaskTable;
    }
    if ( type == &sPanelTaskType) return 1;
    if (sTaskBaseGuard == 0) {
        sTaskBaseGuard = 1;
        sTaskBaseType.mTable = &sPanelTaskTaskBaseTable;
    }
    if ( type == &sTaskBaseType) return 1;
    if (sObjectBaseGuard == 0) {
        sObjectBaseGuard = 1;
        sObjectBaseType.mTable = &sPanelTaskObjectBaseTable;
    }
    return type == &sObjectBaseType;
}

/* 02735D28 */
static u32 matchesDialogTask(void* object, const sead::RuntimeType* type) {
    if (sDialogTaskGuard == 0) {
        sDialogTaskGuard = 1;
        sDialogTaskType.mTable = &sDialogTaskDialogTaskTable;
    }
    if ( type == &sDialogTaskType) return 1;
    if (sTaskBaseGuard == 0) {
        sTaskBaseGuard = 1;
        sTaskBaseType.mTable = &sDialogTaskTaskBaseTable;
    }
    if ( type == &sTaskBaseType) return 1;
    if (sObjectBaseGuard == 0) {
        sObjectBaseGuard = 1;
        sObjectBaseType.mTable = &sDialogTaskObjectBaseTable;
    }
    return type == &sObjectBaseType;
}

/* 0273816C */
static u32 matchesFrameTask(void* object, const sead::RuntimeType* type) {
    if (sFrameTaskGuard == 0) {
        sFrameTaskGuard = 1;
        sFrameTaskType.mTable = &sFrameTaskFrameTaskTable;
    }
    if ( type == &sFrameTaskType) return 1;
    if (sTaskBaseGuard == 0) {
        sTaskBaseGuard = 1;
        sTaskBaseType.mTable = &sFrameTaskTaskBaseTable;
    }
    if ( type == &sTaskBaseType) return 1;
    if (sObjectBaseGuard == 0) {
        sObjectBaseGuard = 1;
        sObjectBaseType.mTable = &sFrameTaskObjectBaseTable;
    }
    return type == &sObjectBaseType;
}

/* 02738C14 */
static u32 matchesReadStream(void* object, const sead::RuntimeType* type) {
    if (sReadStreamGuard == 0) {
        sReadStreamGuard = 1;
        sReadStreamType.mTable = &sReadStreamReadStreamTable;
    }
    if ( type == &sReadStreamType) return 1;
    if (sStreamGuard == 0) {
        sStreamGuard = 1;
        sStreamType.mTable = &sReadStreamStreamTable;
    }
    if ( type == &sStreamType) return 1;
    if (sStreamBaseGuard == 0) {
        sStreamBaseGuard = 1;
        sStreamBaseType.mTable = &sReadStreamStreamBaseTable;
    }
    return type == &sStreamBaseType;
}

/* 02734620 */
static u32 matchesTaskBase(void* object, const sead::RuntimeType* type) {
    if (sTaskBaseGuard == 0) {
        sTaskBaseGuard = 1;
        sTaskBaseType.mTable = &sTaskBaseTaskBaseTable;
    }
    if ( type == &sTaskBaseType) return 1;
    if (sObjectBaseGuard == 0) {
        sObjectBaseGuard = 1;
        sObjectBaseType.mTable = &sTaskBaseObjectBaseTable;
    }
    return type == &sObjectBaseType;
}

/* 0273D624 */
static u32 matchesBufferedController(void* object, const sead::RuntimeType* type) {
    if (sBufferedControllerDerivedGuard == 0) {
        sBufferedControllerDerivedGuard = 1;
        sBufferedControllerDerivedType.mTable = &sBufferedControllerDerivedTable;
    }
    if (type == &sBufferedControllerDerivedType) return 1;
    if (sBufferedControllerParentGuard == 0) {
        sBufferedControllerParentGuard = 1;
        sBufferedControllerParentType.mTable = &sBufferedControllerParentTable;
    }
    return type == &sBufferedControllerParentType;
}

/* 0273D728 */
static u32 matchesChannelController(void* object, const sead::RuntimeType* type) {
    if (sChannelControllerDerivedGuard == 0) {
        sChannelControllerDerivedGuard = 1;
        sChannelControllerDerivedType.mTable = &sChannelControllerDerivedTable;
    }
    if (type == &sChannelControllerDerivedType) return 1;
    if (sChannelControllerParentGuard == 0) {
        sChannelControllerParentGuard = 1;
        sChannelControllerParentType.mTable = &sChannelControllerParentTable;
    }
    return type == &sChannelControllerParentType;
}

/* 0273D96C */
static u32 matchesRemoteDevice(void* object, const sead::RuntimeType* type) {
    if (sRemoteDeviceDerivedGuard == 0) {
        sRemoteDeviceDerivedGuard = 1;
        sRemoteDeviceDerivedType.mTable = &sRemoteDeviceDerivedTable;
    }
    if (type == &sRemoteDeviceDerivedType) return 1;
    if (sRemoteDeviceParentGuard == 0) {
        sRemoteDeviceParentGuard = 1;
        sRemoteDeviceParentType.mTable = &sRemoteDeviceParentTable;
    }
    return type == &sRemoteDeviceParentType;
}

/* 0273E220 */
static u32 matchesRemoteController(void* object, const sead::RuntimeType* type) {
    if (sRemoteControllerDerivedGuard == 0) {
        sRemoteControllerDerivedGuard = 1;
        sRemoteControllerDerivedType.mTable = &sRemoteControllerDerivedTable;
    }
    if (type == &sRemoteControllerDerivedType) return 1;
    if (sRemoteControllerParentGuard == 0) {
        sRemoteControllerParentGuard = 1;
        sRemoteControllerParentType.mTable = &sRemoteControllerParentTable;
    }
    return type == &sRemoteControllerParentType;
}

/* 0273E43C */
static u32 matchesTabletDevice(void* object, const sead::RuntimeType* type) {
    if (sTabletDeviceDerivedGuard == 0) {
        sTabletDeviceDerivedGuard = 1;
        sTabletDeviceDerivedType.mTable = &sTabletDeviceDerivedTable;
    }
    if (type == &sTabletDeviceDerivedType) return 1;
    if (sTabletDeviceParentGuard == 0) {
        sTabletDeviceParentGuard = 1;
        sTabletDeviceParentType.mTable = &sTabletDeviceParentTable;
    }
    return type == &sTabletDeviceParentType;
}

/* 0273E9D4 */
static u32 matchesTabletController(void* object, const sead::RuntimeType* type) {
    if (sTabletControllerDerivedGuard == 0) {
        sTabletControllerDerivedGuard = 1;
        sTabletControllerDerivedType.mTable = &sTabletControllerDerivedTable;
    }
    if (type == &sTabletControllerDerivedType) return 1;
    if (sTabletControllerParentGuard == 0) {
        sTabletControllerParentGuard = 1;
        sTabletControllerParentType.mTable = &sTabletControllerParentTable;
    }
    return type == &sTabletControllerParentType;
}

/* WWHD glue
 data sObjectChildSceneTaskGuard 101FDDC4
 data sObjectChildSceneTaskType 101FDE30
 data sMenuChildSceneTaskGuard 101FDDD4
 data sMenuChildSceneTaskType 101FDE44
 data sMapChildSceneTaskGuard 101FDDCC
 data sMapChildSceneTaskType 101FDE58
 data sScreenChildSceneTaskGuard 101FDDDC
 data sScreenChildSceneTaskType 101FDEB4
 data sBufferedControllerDerivedGuard 101FD66C
 data sBufferedControllerDerivedType 101FDD74
 data sBufferedControllerDerivedTable 10143328
 data sBufferedControllerParentGuard 101FD65C
 data sBufferedControllerParentType 101FDD78
 data sBufferedControllerParentTable 10143318
 data sChannelControllerDerivedGuard 101FD67C
 data sChannelControllerDerivedType 101FDF28
 data sChannelControllerDerivedTable 10143398
 data sChannelControllerParentGuard 101FD65C
 data sChannelControllerParentType 101FDD78
 data sChannelControllerParentTable 10143388
 data sRemoteDeviceDerivedGuard 101FD698
 data sRemoteDeviceDerivedType 101FD978
 data sRemoteDeviceDerivedTable 10143400
 data sRemoteDeviceParentGuard 101FD654
 data sRemoteDeviceParentType 101FD96C
 data sRemoteDeviceParentTable 101433F0
 data sRemoteControllerDerivedGuard 101FD688
 data sRemoteControllerDerivedType 101FDF2C
 data sRemoteControllerDerivedTable 10143448
 data sRemoteControllerParentGuard 101FD65C
 data sRemoteControllerParentType 101FDD78
 data sRemoteControllerParentTable 10143438
 data sTabletDeviceDerivedGuard 101FD694
 data sTabletDeviceDerivedType 101FD968
 data sTabletDeviceDerivedTable 1014350C
 data sTabletDeviceParentGuard 101FD654
 data sTabletDeviceParentType 101FD96C
 data sTabletDeviceParentTable 101434FC
 data sTabletControllerDerivedGuard 101FD674
 data sTabletControllerDerivedType 101FDD70
 data sTabletControllerDerivedTable 10143558
 data sTabletControllerParentGuard 101FD65C
 data sTabletControllerParentType 101FDD78
 data sTabletControllerParentTable 10143548
 data sControllerControllerTable 101430D0
 data sControlDeviceControlDeviceTable 10143150
 data sControlDeviceControlBaseTable 10143140
 data sInputAddonInputAddonTable 101411D0
 data sInputAddonControlBaseTable 10141190
 data sSceneTaskSceneTaskTable 101416A0
 data sSceneTaskTaskBaseTable 10141680
 data sSceneTaskObjectBaseTable 10141670
 data sChildSceneTaskChildSceneTaskTable 101416B0
 data sChildSceneTaskSceneTaskTable 101416A0
 data sChildSceneTaskTaskBaseTable 10141680
 data sChildSceneTaskObjectBaseTable 10141670
 data sObjectChildSceneTaskChildSceneTaskTable 10141924
 data sObjectChildSceneTaskSceneTaskTable 10141914
 data sObjectChildSceneTaskTaskBaseTable 101418F4
 data sObjectChildSceneTaskObjectBaseTable 101418E4
 data sMenuChildSceneTaskChildSceneTaskTable 10141A9C
 data sMenuChildSceneTaskSceneTaskTable 10141A8C
 data sMenuChildSceneTaskTaskBaseTable 10141A6C
 data sMenuChildSceneTaskObjectBaseTable 10141A5C
 data sMapChildSceneTaskChildSceneTaskTable 10141C54
 data sMapChildSceneTaskSceneTaskTable 10141C44
 data sMapChildSceneTaskTaskBaseTable 10141C24
 data sMapChildSceneTaskObjectBaseTable 10141C14
 data sScreenChildSceneTaskChildSceneTaskTable 101422CC
 data sScreenChildSceneTaskSceneTaskTable 101422BC
 data sScreenChildSceneTaskTaskBaseTable 1014229C
 data sScreenChildSceneTaskObjectBaseTable 1014228C
 data sLayoutTaskLayoutTaskTable 10141E00
 data sLayoutTaskTaskBaseTable 10141DF0
 data sLayoutTaskObjectBaseTable 10141DB0
 data sOverlayTaskOverlayTaskTable 10142004
 data sOverlayTaskTaskBaseTable 10141FF4
 data sOverlayTaskObjectBaseTable 10141FE4
 data sWindowTaskWindowTaskTable 10142414
 data sWindowTaskTaskBaseTable 10142404
 data sWindowTaskObjectBaseTable 101423F4
 data sPanelTaskPanelTaskTable 10142574
 data sPanelTaskTaskBaseTable 10142564
 data sPanelTaskObjectBaseTable 10142554
 data sDialogTaskDialogTaskTable 101426CC
 data sDialogTaskTaskBaseTable 101426BC
 data sDialogTaskObjectBaseTable 1014269C
 data sFrameTaskFrameTaskTable 10142948
 data sFrameTaskTaskBaseTable 10142938
 data sFrameTaskObjectBaseTable 10142840
 data sReadStreamReadStreamTable 10142D10
 data sReadStreamStreamTable 10142D00
 data sReadStreamStreamBaseTable 10142CF0
 data sTaskBaseTaskBaseTable 10142190
 data sTaskBaseObjectBaseTable 10142150
 data sControlBaseGuard 101FD594
 data sControlBaseType 101FD980
 data sControlDeviceGuard 101FD598
 data sControlDeviceType 101FD97C
 data sInputAddonGuard 101FD71C
 data sInputAddonType 101FD9D8
 data sObjectBaseGuard 101FD7C0
 data sObjectBaseType 101FDCE8
 data sTaskBaseGuard 101FD984
 data sTaskBaseType 101FDCEC
 data sSceneTaskGuard 101FDDB8
 data sSceneTaskType 101FDE08
 data sChildSceneTaskGuard 101FDDBC
 data sChildSceneTaskType 101FDE0C
 data sLayoutTaskGuard 101FD9AC
 data sLayoutTaskType 101FDCE4
 data sOverlayTaskGuard 101FDDEC
 data sOverlayTaskType 101FDEA8
 data sWindowTaskGuard 101FD988
 data sWindowTaskType 101FDEBC
 data sPanelTaskGuard 101FDDF4
 data sPanelTaskType 101FDEC8
 data sDialogTaskGuard 101FDDE4
 data sDialogTaskType 101FDED8
 data sFrameTaskGuard 101FD9B4
 data sFrameTaskType 101FDEE4
 data sReadStreamGuard 101FD9F0
 data sReadStreamType 101FDCD4
 data sControllerGuard 101FD65C
 data sControllerType 101FDD78
*/

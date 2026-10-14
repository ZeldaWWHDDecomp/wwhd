/* HD main HUD child-screen composition. */
#pragma once
#include "d/hd_parts_heart.h"
#include "d/hd_parts_lyt.h"
struct hdHudMain_c : hdParts_c {
    gptr<hdPartsPaneSRT_c> mPanes[5]; // 50
    be<f32> mMagicOrigins[2][3]; // 64
    be<u32> mOriginalTranslation[3]; // 7C
    be<u32> mHeartLayout; // 88
    be<u32> mMagicAvailable; // 8C
    be<u32> mSwimTimePlacement; // 90
    be<u32> mPlayerMode; // 94
    be<u8> mFlags[17]; // 98..A8
    u8 mPadding[3];
    enum Flag { Reserved98,Reserved99,Reserved9A,Reserved9B,Reserved9C,Reserved9D,Reserved9E,Reserved9F,
        EnemyHpVisible,ArrowVisible,BatteryVisible,ReservedA3,AlternateLayout,HeartBattle,ReservedA6,EventActive,EventEnded };
    enum ChildIndex { Heart,Rupy,MagicBar,DungeonKey,CompassClock,SwimTime,Time,BossHp,EnemyHp,Arrow,Battery,RupySwordCounter,Shortcut,ChildCount };
    static void destroy(hdHudMain_c*,u32); static hdHudMain_c* construct(hdHudMain_c*);
    void setFrame(u32,u32); void captureMagicOrigins();void resetVisibility();
    void updateHeartLayout();void updateMagicAvailability();void updateSwimTimePlacement();void updateCounterPlacement();
    void updateLayout();void initializeValues();BOOL init(void*,u32,u32);void finalize();
    void updateEnemyHpVisible();void updateArrowVisibility();void updateBatteryVisibility();void updateVisibility();
    void updateBattle();void updateEvent();void execute();void reset();void beginSecondary();void endSecondary();
    BOOL createChild(void*);BOOL loadLayout(void*,u32,u32,u32,void*);
};
WWHD_SIZE(hdHudMain_c,0xAC);
WWHD_OFFSET(hdHudMain_c,mFlags,0x98);
struct hdHudMainStateNumber_c { be<u32> mNumber;u32 getNumber(); };

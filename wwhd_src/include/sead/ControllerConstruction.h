// WWHD controller construction entry points; addresses are bound in the companion source.
#pragma once
#include "sead/Controller.h"
sead::Controller* constructController(sead::Controller*, sead::ControllerManager*);
sead::ControllerWrapper* constructControllerWrapper(sead::ControllerWrapper*);
sead::BufferedController* constructBufferedController(sead::BufferedController*, sead::ControllerManager*, s32, void*);
sead::ChannelController* constructChannelController(sead::ChannelController*, sead::ControllerManager*, s32);
void* allocateControllerObject(u32);
void constructWrapperObject(void*);
void destroyWrapperObject(void*,u32);
void deleteControllerObject(void*);

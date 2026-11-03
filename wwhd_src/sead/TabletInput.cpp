// WWHD tablet buttons, stick and touch conversion, reconstructed from local instructions.
#include "sead/TabletInput.h"
#include "sead/ElementAt.h"

static sead::RuntimeType sTabletInputType;
static u32 sTabletInputTypeGuard;
static u32 sTabletInputTypeTable;
void calibrateTabletTouch(u32,sead::TabletTouch*,sead::TabletTouch*);

static u32 tabletButtons(u32 source) {
    u32 result = 0;
    if (source & 0x8000) result |= 1;
    if (source & 0x4000) result |= 2;
    if (source & 0x2000) result |= 8;
    if (source & 0x1000) result |= 16;
    if (source & 8) result |= 0xC00;
    if (source & 4) result |= 0x1200;
    if (source & 0x20) result |= 0x2000;
    if (source & 0x10) result |= 0x4000;
    if (source & 0x80) result |= 4;
    if (source & 0x40) result |= 0x20;
    if (source & 2) result |= 0x100;
    if (source & 0x200) result |= 0x10000;
    if (source & 0x100) result |= 0x20000;
    if (source & 0x800) result |= 0x40000;
    if (source & 0x400) result |= 0x80000;
    if (source & 0x20000) result |= 0x40;
    if (source & 0x40000) result |= 0x80;
    return result;
}

/* 0273E5D4 */
static void updateTabletController(TabletController* controller) {
    gabi::NativeFrame<80> frame;
    sead::TabletDevice* device = static_cast<sead::TabletDevice*>(controller->mManager->findDevice(8));
    if (sTabletInputTypeGuard == 0) {
        sTabletInputTypeGuard = 1;
        sTabletInputType.mTable = &sTabletInputTypeTable;
    }
    if (device == NULL) return;
    be<u32>* table = device->mTable.get();
    if (gabi::call_ptr<u32>(table[3],device,&sTabletInputType) == 0) return;
    s32 count = device->mBatch.mCount;
    sead::TabletBatch* batch = &device->mBatch;
    if (count <= 0) {
        if ((s32)batch->mError != -1 || controller->mConnected == 0 || (u32)controller->mTouchCount >= 180) {
            controller->mTouchCount = 0;
            controller->mConnected = 0;
            return;
        }
    }
    sead::TabletSample* sample = &batch->mSamples[0];
    controller->mButtons = tabletButtons(sample->mButtons);
    u32 x = sample->mStick.mX;
    u32 y = sample->mStick.mY;
    reinterpret_cast<be<u32>&>(controller->mStickX) = x;
    reinterpret_cast<be<u32>&>(controller->mStickY) = y;
    y = sample->mCross.mY;
    x = sample->mCross.mX;
    reinterpret_cast<be<u32>&>(controller->mCrossY) = y;
    reinterpret_cast<be<u32>&>(controller->mCrossX) = x;
    controller->mLeftTrigger = 0.0f;
    controller->mRightTrigger = 0.0f;
    u32 samples = batch->mCount;
    u32 capacity = controller->mTouchCapacity;
    bool haveX = false;
    bool haveY = false;
    if (samples == 0) samples = 1;
    else if (samples > capacity) samples = capacity;
    auto* point = gabi::at<TabletCoordinates>(frame + 8);
    auto* calibrated = gabi::at<sead::TabletTouch>(frame + 16);
    for (u32 index = 0; index < samples; ++index) {
        sead::TabletTouch* raw = &batch->mSamples[index].mTouches[0];
        if ((u16)raw->mValidity != 3) {
            calibrateTabletTouch(0,calibrated,raw);
            u16 validity = raw->mValidity;
            if ((validity & 1) == 0) {
                point->mX = (u16)calibrated->mX;
                haveX = true;
            }
            if ((validity & 2) == 0) {
                point->mY = (u16)calibrated->mY;
                haveY = true;
            }
            if (haveX && haveY) break;
        }
    }
    controller->mTouchPoints[0].update(haveX && haveY,(f32)(u16)point->mX,(f32)(u16)point->mY);
    auto* filtered = gabi::at<sead::TabletTouch>(frame + 32);
    calibrateTabletTouch(0,filtered,&sample->mTouches[1]);
    controller->mTouchPoints[1].update((u16)filtered->mTouched == 1,(f32)(u16)filtered->mX,(f32)(u16)filtered->mY);
    auto* smoothed = gabi::at<sead::TabletTouch>(frame + 40);
    calibrateTabletTouch(0,smoothed,&sample->mTouches[2]);
    controller->mTouchPoints[2].update((u16)smoothed->mTouched == 1,(f32)(u16)smoothed->mX,(f32)(u16)smoothed->mY);
    u32 selected = controller->mTouchIndex;
    TouchPoint* selectedPoint = &sead::elementAt(controller->mTouchPoints, selected);
    controller->updatePointer((u32)selectedPoint->mFlags & 1,1,&selectedPoint->mPosition);
    if ((s32)batch->mError == -1) controller->mTouchCount = (u32)controller->mTouchCount + 1;
    else controller->mTouchCount = 0;
    controller->mConnected = 1;
}

/* WWHD glue
 data sTabletInputType 101FD968
 data sTabletInputTypeGuard 101FD694
 data sTabletInputTypeTable 10143568
 ext C00075D0 void calibrateTabletTouch(u32,sead::TabletTouch*,sead::TabletTouch*)
*/

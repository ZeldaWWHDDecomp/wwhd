/** HD dungeon-key sparkle timing; private decomp source. */
#include "bindings.h"
#include "d/hd_screen_command_a.h"
static hdHudKeyAnimationIds sKeyAnimationIds;
static f32 sKeyAnimationRate;

static u32 keyRandomIndex(f32 value) {
    if (value < 2147483648.0f) {
        return u32(gabi::ftoi(value));
    }
    return u32(gabi::ftoi(gabi::fsubs_ppc(value, 2147483648.0f))) + 0x80000000u;
}

/* 02653C18 */
static void updateKeySparkles(hdHudDungeonKey* i_this) {
    // HD: keep the scratch indices in their original frame relative to nested random calls.
    auto frame = gabi::AutoFrame(0x02653C18);
    u32 timer = i_this->mSparkleTimer;
    if (timer != 0) {
        i_this->mSparkleTimer = timer - 1;
    } else {
        // HD: preserve the original scratch-index frame slot.
        gabi::FrameLocal<hdHudKeyIndices> indices(0x10);
        u32 count = 0;
        for (u32 i = 0; i < 2; i++) {
            if (i_this->mSparkleActive[i] == 0) {
                indices->mIndices[count] = i;
                count++;
            }
        }
        if (count != 0) {
            f32 random = cM_rndF(f32(f64(count)));
            u32 index = indices->mIndices[keyRandomIndex(random)];
            i_this->mSparkleActive[index < 2 ? index : 0] = 1;
            hdHudStartAnimation(i_this->animation(), index + 1, sKeyAnimationIds[index + 1], sKeyAnimationRate);
            random = cM_rndF(f32(f64(s32(i_this->mRandomInterval))));
            f32 base = f32(f64(s32(i_this->mMinimumInterval)));
            i_this->mSparkleTimer = u32(gabi::ftoi(gabi::fadds_ppc(random, base)));
        }
    }
    for (u32 i = 0; i < 2; i++) {
        if (i_this->mSparkleActive[i] != 0 && hdHudAnimationFinished(i_this->animation(), i + 1)) {
            i_this->mSparkleActive[i] = 0;
        }
    }
}

/* WWHD glue (tools/verify/autoglue.py): HD key animation data
 data sKeyAnimationIds 100EFDC4
 data sKeyAnimationRate 100EFE68
 ext 020053E4 void hdHudStartAnimation(void*, u32, u32, f32)
 ext 02005840 u32 hdHudAnimationFinished(void*, u32)
*/

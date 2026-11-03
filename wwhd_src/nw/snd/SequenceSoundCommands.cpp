/* Sequence command payload construction; command semantics remain identified by their ids. */
#include "nw/snd/SequenceSound.h"
using namespace nw::snd;
DriverCommandManager* sequenceCommandManager();
DriverCommand* allocateSequenceCommand(DriverCommandManager* manager,u32 size);
void submitSequenceCommand(DriverCommandManager* manager,DriverCommand* command);
struct SequenceSetupCommand : DriverCommand {
    gptr<SequenceSoundPlayer_l> player;
    be<u32> first,second,third;
    be<u8> flag,secondFlag;
    u8 padding[2];
    be<u32> fourth,fifth;
};
struct SequenceRangeCommand : DriverCommand {
    gptr<SequenceSoundPlayer_l> player;
    be<u32> first;
    be<s32> second;
};
struct SequencePrepareCopy { SequenceConfiguration config; be<u32> parameter; };
struct SequenceConfigurationCommand : DriverCommand { gptr<SequenceSoundPlayer_l> player; be<u32> payload[11]; };
struct SequenceReferenceCommand : DriverCommand {
    gptr<SequenceSoundPlayer_l> player;
    be<u32> parameters[3];
    be<u32> first,second,last;
    SequenceCommandPair pairs[5];
};
static_assert(sizeof(SequenceSetupCommand)==0x2C);
static_assert(sizeof(SequenceRangeCommand)==0x1C);
static_assert(sizeof(SequenceConfigurationCommand)==0x40);
static_assert(sizeof(SequenceReferenceCommand)==0x54);
static_assert(sizeof(SequencePrepareCopy)==0x2C);
/* 028ADC58 */
void SequenceSound::sendSetupValues(u32 first,u32 second,u32 third,u32 flag,u32 secondFlag,u32 fourth,u32 fifth) {
    gabi::AutoFrame frame{gabi::current_function()};
    DriverCommandManager* manager=sequenceCommandManager();
    SequenceSetupCommand* command=static_cast<SequenceSetupCommand*>(allocateSequenceCommand(manager,0x2C));
    command->player=&mPlayer;
    command->third=third;
    command->id=15;
    command->secondFlag=secondFlag;
    command->fifth=fifth;
    command->flag=flag;
    command->fourth=fourth;
    command->second=second;
    command->first=first;
    submitSequenceCommand(manager,command);
}
/* 028ADCE4 */
void SequenceSound::sendRange(u32 first,s32 second) {
    gabi::AutoFrame frame{gabi::current_function()};
    DriverCommandManager* manager=sequenceCommandManager();
    SequenceRangeCommand* command=static_cast<SequenceRangeCommand*>(allocateSequenceCommand(manager,0x1C));
    command->second=second;
    command->first=first;
    command->id=18;
    command->player=&mPlayer;
    submitSequenceCommand(manager,command);
}
/* 028ADD60 */
void SequenceSound::sendConfiguration(const SequenceConfiguration* config,const SequenceCommandParameters* parameters) {
    gabi::AutoFrame frame{gabi::current_function()};
    DriverCommandManager* manager=sequenceCommandManager();
    SequenceConfigurationCommand* command=static_cast<SequenceConfigurationCommand*>(allocateSequenceCommand(manager,0x40));
    command->id=17;
    command->player=&mPlayer;
    gabi::FrameLocal<SequencePrepareCopy> copy(8);
    for (u32 index=0;index<4;++index) {
        copy->config.mFirst[index]=0;
        copy->config.mSecond[index]=0;
        copy->config.mFlags[index]=0;
    }
    copy->config.mHeader=config->mHeader;
    for (u32 index=0;index<4;++index) {
        copy->config.mFirst[index]=config->mFirst[index];
        copy->config.mSecond[index]=config->mSecond[index];
        copy->config.mFlags[index]=config->mFlags[index];
    }
    copy->parameter=parameters->mValue;
    const be<u32>* words=reinterpret_cast<const be<u32>*>(copy.get());
    for (u32 index=0;index<11;++index) command->payload[index]=words[index];
    submitSequenceCommand(manager,command);
    s32 count=parameters->mCount;
    if (count>0) sendRange(parameters->mOption,count);
}
/* 028ADE8C */
void SequenceSound::sendReferenceConfiguration(const SequenceReferenceConfiguration* config,const SequenceCommandParameters* parameters) {
    gabi::AutoFrame frame{gabi::current_function()};
    DriverCommandManager* manager=sequenceCommandManager();
    SequenceReferenceCommand* command=static_cast<SequenceReferenceCommand*>(allocateSequenceCommand(manager,0x54));
    command->id=16;
    command->player=&mPlayer;
    command->parameters[0]=parameters->mValue;
    command->parameters[1]=parameters->mOption;
    command->parameters[2]=u32(parameters->mCount);
    command->first=config->mFirst;
    command->second=config->mSecond;
    command->last=config->mLast;
    const SequenceCommandPair* pair=config->mPairs[0];
    command->pairs[0].mFirst=pair->mFirst;
    command->pairs[0].mSecond=pair->mSecond;
    for (u32 index=1;index<5;++index) {
        pair=config->mPairs[index];
        command->pairs[index].mFirst=pair->mFirst;
        command->pairs[index].mSecond=pair->mSecond;
    }
    submitSequenceCommand(manager,command);
}
/* WWHD glue
ext 02885F10 nw::snd::DriverCommandManager* sequenceCommandManager()
ext 02887248 nw::snd::DriverCommand* allocateSequenceCommand(nw::snd::DriverCommandManager*,u32)
ext 028870F0 void submitSequenceCommand(nw::snd::DriverCommandManager*,nw::snd::DriverCommand*)
*/

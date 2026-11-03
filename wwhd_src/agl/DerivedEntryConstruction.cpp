// WWHD scalar-mode entry constructors with shared source-level initialization.
#include "agl/EntryConstruction.h"
#include "agl/EntryParameters.h"
#include "agl/EmbeddedEntry.h"
static agl::RenderEntryMethods sFirstScalarMethods;
static agl::RenderEntryMethods sSecondScalarMethods;
static u32 sFirstScalarVtable;
static u32 sSecondScalarVtable;
static u32 sFloatValueVtable;
static u32 sWordValueVtable;
static sead::SafeTextVtable sDerivedTextVtable;
static EntryLabel sFirstScalarName;
static EntryLabel sFirstScalarLabel;
static EntryLabel sFirstModeName;
static EntryLabel sFirstModeLabel;
static EntryLabel sSecondScalarName;
static EntryLabel sSecondScalarLabel;
static EntryLabel sSecondModeName;
static EntryLabel sSecondModeLabel;
static EntryLabel sScalarHelp;
static EntryLabel sModeHelp;
static f32 sScalarInitial;

static agl::ScalarModeEntry* initializeScalarEntry(agl::ScalarModeEntry* entry, u32 index, u32 kind,
        agl::RenderEntryMethods* methods, u32 vtable, EntryLabel* scalarNameText,
        EntryLabel* scalarLabelText, EntryLabel* modeNameText, EntryLabel* modeLabelText) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (entry == NULL) entry = (agl::ScalarModeEntry*)allocateAglStorage(sizeof(agl::ScalarModeEntry));
    if (entry != NULL) {
        createRenderEntry(entry, kind, index);
        entry->mMethods = methods;
        entry->mVtable = vtable;
        gabi::FrameLocal<sead::SafeText> scalarName(0x8);
        gabi::FrameLocal<sead::SafeText> scalarLabel(0x10);
        gabi::FrameLocal<sead::SafeText> scalarHelp(0x28);
        scalarLabel->mVtable = &sDerivedTextVtable;
        scalarName->mVtable = &sDerivedTextVtable;
        scalarName->mText = &scalarNameText->mFirst;
        scalarLabel->mText = &scalarLabelText->mFirst;
        constructEntryParameter(&entry->mScalar);
        entry->mScalar.mVtable = gabi::ea(&sFloatValueVtable);
        scalarHelp->mVtable = &sDerivedTextVtable;
        scalarHelp->mText = &sScalarHelp.mFirst;
        attachEntryParameter(&entry->mScalar, scalarName.get(), scalarLabel.get(), scalarHelp.get(), entry->mParameterContainer);
        entry->mScalar.mValue = sScalarInitial;
        gabi::FrameLocal<sead::SafeText> modeName(0x18);
        gabi::FrameLocal<sead::SafeText> modeLabel(0x20);
        gabi::FrameLocal<sead::SafeText> modeHelp(0x30);
        modeName->mVtable = &sDerivedTextVtable;
        modeLabel->mVtable = &sDerivedTextVtable;
        modeName->mText = &modeNameText->mFirst;
        modeLabel->mText = &modeLabelText->mFirst;
        constructEntryParameter(&entry->mMode);
        entry->mMode.mVtable = gabi::ea(&sWordValueVtable);
        modeHelp->mVtable = &sDerivedTextVtable;
        modeHelp->mText = &sModeHelp.mFirst;
        attachEntryParameter(&entry->mMode, modeName.get(), modeLabel.get(), modeHelp.get(), entry->mParameterContainer);
        entry->mMode.mValue = 2;
    }
    return entry;
}
/* 027AB294 */
static agl::ScalarModeEntry* createFirstScalarEntry(agl::ScalarModeEntry* entry, u32 index) {
    return initializeScalarEntry(entry, index, 0, &sFirstScalarMethods, gabi::ea(&sFirstScalarVtable),
        &sFirstScalarName, &sFirstScalarLabel, &sFirstModeName, &sFirstModeLabel);
}
/* 027AB414 */
static agl::ScalarModeEntry* createSecondScalarEntry(agl::ScalarModeEntry* entry, u32 index) {
    return initializeScalarEntry(entry, index, 1, &sSecondScalarMethods, gabi::ea(&sSecondScalarVtable),
        &sSecondScalarName, &sSecondScalarLabel, &sSecondModeName, &sSecondModeLabel);
}
static agl::RenderEntryMethods sTripleSelectorMethods;
static u32 sTripleSelectorVtable;
static EntryLabel sTripleFirstName;
static EntryLabel sTripleFirstLabel;
static EntryLabel sTripleSecondName;
static EntryLabel sTripleSecondLabel;
static EntryLabel sTripleThirdName;
static EntryLabel sTripleThirdLabel;
struct ThreeNamePairs { sead::SafeText mTexts[6]; };
struct ThreeHelpTexts { sead::SafeText mTexts[3]; };
/* 027AB594 */
static agl::TripleSelectorEntry* createTripleSelectorEntry(agl::TripleSelectorEntry* entry, u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (entry == NULL) entry = (agl::TripleSelectorEntry*)allocateAglStorage(sizeof(agl::TripleSelectorEntry));
    if (entry != NULL) {
        createRenderEntry(entry, 2, index);
        entry->mMethods = &sTripleSelectorMethods;
        entry->mVtable = gabi::ea(&sTripleSelectorVtable);
        gabi::FrameLocal<ThreeNamePairs> descriptors(0x8);
        gabi::FrameLocal<ThreeHelpTexts> helps(0x38);
        EntryLabel* names[] = {&sTripleFirstName, &sTripleSecondName, &sTripleThirdName};
        EntryLabel* labels[] = {&sTripleFirstLabel, &sTripleSecondLabel, &sTripleThirdLabel};
        u32 defaults[] = {4, 1, 9};
        for (u32 parameterIndex = 0; parameterIndex < 3; ++parameterIndex) {
            sead::SafeText* name = &descriptors->mTexts[parameterIndex * 2];
            sead::SafeText* label = name + 1;
            sead::SafeText* help = &helps->mTexts[parameterIndex];
            name->mVtable = &sDerivedTextVtable;
            label->mVtable = &sDerivedTextVtable;
            name->mText = &names[parameterIndex]->mFirst;
            label->mText = &labels[parameterIndex]->mFirst;
            agl::WordParameter* parameter = &entry->mSelectors[parameterIndex];
            constructEntryParameter(parameter);
            parameter->mVtable = gabi::ea(&sWordValueVtable);
            help->mVtable = &sDerivedTextVtable;
            help->mText = &sModeHelp.mFirst;
            attachEntryParameter(parameter, name, label, help, entry->mParameterContainer);
            parameter->mValue = defaults[parameterIndex];
        }
    }
    return entry;
}
static u32 sPairValueVtable;
static EntryLabel sPairHelp;
static f32 sPairFirst;
static f32 sPairSecond;
static f32 sPairUniform;
static agl::RenderEntryMethods sChannelMethods;
static u32 sChannelVtable;
static EntryLabel sChannelName0;
static EntryLabel sChannelLabel0;
static EntryLabel sChannelName1;
static EntryLabel sChannelLabel1;
static EntryLabel sChannelName2;
static EntryLabel sChannelLabel2;
static EntryLabel sChannelName3;
static EntryLabel sChannelLabel3;
static EntryLabel sChannelName4;
static EntryLabel sChannelLabel4;
struct ChannelNames { sead::SafeText mTexts[10]; };
struct ChannelHelps { sead::SafeText mTexts[5]; };
/* 027AB7D8 */
static agl::ChannelSelectorEntry* createChannelEntry(agl::ChannelSelectorEntry* entry, u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (entry == NULL) entry = (agl::ChannelSelectorEntry*)allocateAglStorage(sizeof(agl::ChannelSelectorEntry));
    if (entry != NULL) {
        createRenderEntry(entry, 4, index);
        entry->mMethods = &sChannelMethods;
        entry->mVtable = gabi::ea(&sChannelVtable);
        gabi::FrameLocal<ChannelNames> descriptors(0x8);
        gabi::FrameLocal<ChannelHelps> helps(0x58);
        EntryLabel* names[] = {&sChannelName0, &sChannelName1, &sChannelName2, &sChannelName3, &sChannelName4};
        EntryLabel* labels[] = {&sChannelLabel0, &sChannelLabel1, &sChannelLabel2, &sChannelLabel3, &sChannelLabel4};
        u32 defaults[] = {4, 0, 1, 2, 5};
        for (u32 parameterIndex = 0; parameterIndex < 5; ++parameterIndex) {
            sead::SafeText* name = &descriptors->mTexts[parameterIndex * 2];
            sead::SafeText* label = name + 1;
            sead::SafeText* help = &helps->mTexts[parameterIndex];
            name->mVtable = &sDerivedTextVtable;
            label->mVtable = &sDerivedTextVtable;
            name->mText = &names[parameterIndex]->mFirst;
            label->mText = &labels[parameterIndex]->mFirst;
            agl::WordParameter* parameter = &entry->mSelectors[parameterIndex];
            constructEntryParameter(parameter);
            parameter->mVtable = gabi::ea(&sWordValueVtable);
            help->mVtable = &sDerivedTextVtable;
            help->mText = &sModeHelp.mFirst;
            attachEntryParameter(parameter, name, label, help, entry->mParameterContainer);
            parameter->mValue = defaults[parameterIndex];
        }
    }
    return entry;
}
static agl::RenderEntryMethods sTriplePairMethods;
static u32 sTriplePairVtable;
static EntryLabel sTriplePairName0;
static EntryLabel sTriplePairLabel0;
static EntryLabel sTriplePairName1;
static EntryLabel sTriplePairLabel1;
static EntryLabel sTriplePairName2;
static EntryLabel sTriplePairLabel2;
struct TriplePairNames { sead::SafeText mTexts[6]; };
struct TriplePairHelps { sead::SafeText mTexts[3]; };
/* 027ABA78 */
static agl::TriplePairEntry* createTriplePairEntry(agl::TriplePairEntry* entry, u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (entry == NULL) entry = (agl::TriplePairEntry*)allocateAglStorage(sizeof(agl::TriplePairEntry));
    if (entry != NULL) {
        createRenderEntry(entry, 5, index);
        entry->mVtable = gabi::ea(&sTriplePairVtable);
        entry->mMethods = &sTriplePairMethods;
        gabi::FrameLocal<TriplePairNames> descriptors(0x8);
        gabi::FrameLocal<TriplePairHelps> helps(0x38);
        EntryLabel* names[] = {&sTriplePairName0, &sTriplePairName1, &sTriplePairName2};
        EntryLabel* labels[] = {&sTriplePairLabel0, &sTriplePairLabel1, &sTriplePairLabel2};
        for (u32 parameterIndex = 0; parameterIndex < 3; ++parameterIndex) {
            sead::SafeText* name = &descriptors->mTexts[parameterIndex * 2];
            sead::SafeText* label = name + 1;
            sead::SafeText* help = &helps->mTexts[parameterIndex];
            name->mVtable = &sDerivedTextVtable;
            label->mVtable = &sDerivedTextVtable;
            name->mText = &names[parameterIndex]->mFirst;
            label->mText = &labels[parameterIndex]->mFirst;
            agl::PairParameter* parameter = &entry->mPairs[parameterIndex];
            constructEntryParameter(parameter);
            parameter->mVtable = gabi::ea(&sPairValueVtable);
            help->mVtable = &sDerivedTextVtable;
            help->mText = &sPairHelp.mFirst;
            attachEntryParameter(parameter, name, label, help, entry->mParameterContainer);
            parameter->mValue[0] = sPairFirst;
            parameter->mValue[1] = sPairSecond;
        }
    }
    return entry;
}
static agl::RenderEntryMethods sDoublePairMethods;
static u32 sDoublePairVtable;
static EntryLabel sDoublePairName0;
static EntryLabel sDoublePairLabel0;
static EntryLabel sDoublePairName1;
static EntryLabel sDoublePairLabel1;
struct DoublePairNames { sead::SafeText mTexts[4]; };
struct DoublePairHelps { sead::SafeText mTexts[2]; };
/* 027ABC60 */
static agl::DoublePairEntry* createDoublePairEntry(agl::DoublePairEntry* entry, u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (entry == NULL) entry = (agl::DoublePairEntry*)allocateAglStorage(sizeof(agl::DoublePairEntry));
    if (entry != NULL) {
        createRenderEntry(entry, 6, index);
        entry->mMethods = &sDoublePairMethods;
        entry->mVtable = gabi::ea(&sDoublePairVtable);
        gabi::FrameLocal<DoublePairNames> descriptors(0x8);
        gabi::FrameLocal<DoublePairHelps> helps(0x28);
        EntryLabel* names[] = {&sDoublePairName0, &sDoublePairName1};
        EntryLabel* labels[] = {&sDoublePairLabel0, &sDoublePairLabel1};
        for (u32 parameterIndex = 0; parameterIndex < 2; ++parameterIndex) {
            sead::SafeText* name = &descriptors->mTexts[parameterIndex * 2];
            sead::SafeText* label = name + 1;
            sead::SafeText* help = &helps->mTexts[parameterIndex];
            name->mVtable = &sDerivedTextVtable;
            label->mVtable = &sDerivedTextVtable;
            name->mText = &names[parameterIndex]->mFirst;
            label->mText = &labels[parameterIndex]->mFirst;
            agl::PairParameter* parameter = &entry->mPairs[parameterIndex];
            constructEntryParameter(parameter);
            parameter->mVtable = gabi::ea(&sPairValueVtable);
            help->mVtable = &sDerivedTextVtable;
            help->mText = &sPairHelp.mFirst;
            attachEntryParameter(parameter, name, label, help, entry->mParameterContainer);
            if (parameterIndex == 0) {
                parameter->mValue[0] = sPairFirst;
                parameter->mValue[1] = sPairSecond;
            } else {
                f32 value = sPairUniform;
                parameter->mValue[0] = value;
                parameter->mValue[1] = value;
            }
        }
    }
    return entry;
}
void* allocateAglHeapStorage(u32, void*, s32);
/* 027ABDD0 */
static agl::RenderEntry* createEntryByKind(u32 kind, u32 index, void* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (kind > 6) return NULL;
    u32 sizes[] = {
        sizeof(agl::ScalarModeEntry), sizeof(agl::ScalarModeEntry),
        sizeof(agl::TripleSelectorEntry), sizeof(agl::EmbeddedStorageEntry),
        sizeof(agl::ChannelSelectorEntry), sizeof(agl::TriplePairEntry), sizeof(agl::DoublePairEntry)
    };
    void* memory = allocateAglHeapStorage(sizes[kind], heap, 4);
    if (memory == NULL) return NULL;
    switch (kind) {
    case 0: return createFirstScalarEntry((agl::ScalarModeEntry*)memory, index);
    case 1: return createSecondScalarEntry((agl::ScalarModeEntry*)memory, index);
    case 2: return createTripleSelectorEntry((agl::TripleSelectorEntry*)memory, index);
    case 3: return createEmbeddedEntry((agl::EmbeddedStorageEntry*)memory, index);
    case 4: return createChannelEntry((agl::ChannelSelectorEntry*)memory, index);
    case 5: return createTriplePairEntry((agl::TriplePairEntry*)memory, index);
    case 6: return createDoublePairEntry((agl::DoublePairEntry*)memory, index);
    }
    return NULL;
}
/* WWHD glue
 ext 0273B050 void* allocateAglHeapStorage(u32, void*, s32)
 data sPairValueVtable 101687CC
 data sPairHelp 1016858B
 data sPairFirst 104A0908
 data sPairSecond 104A090C
 data sPairUniform 101689DC
 data sChannelMethods 10168D6C
 data sChannelVtable 10168DB4
 data sChannelName0 10168964
 data sChannelLabel0 1016896C
 data sChannelName1 1016897C
 data sChannelLabel1 1016895C
 data sChannelName2 10168988
 data sChannelLabel2 1016895E
 data sChannelName3 10168994
 data sChannelLabel3 10168960
 data sChannelName4 101689A0
 data sChannelLabel4 10168962
 data sTriplePairMethods 10168DC4
 data sTriplePairVtable 10168E0C
 data sTriplePairName0 101689AC
 data sTriplePairLabel0 101689C4
 data sTriplePairName1 101689B4
 data sTriplePairLabel1 101689CC
 data sTriplePairName2 101689BC
 data sTriplePairLabel2 101689D4
 data sDoublePairMethods 10168C0C
 data sDoublePairVtable 10168C54
 data sDoublePairName0 101689E0
 data sDoublePairLabel0 101689EC
 data sDoublePairName1 101689F8
 data sDoublePairLabel1 10168A04
 data sTripleSelectorMethods 10168D14
 data sTripleSelectorVtable 10168D5C
 data sTripleFirstName 10168910
 data sTripleFirstLabel 1016891C
 data sTripleSecondName 1016892C
 data sTripleSecondLabel 10168938
 data sTripleThirdName 10168900
 data sTripleThirdLabel 10168948
 data sFirstScalarMethods 10168C64
 data sSecondScalarMethods 10168CBC
 data sFirstScalarVtable 10168CAC
 data sSecondScalarVtable 10168D04
 data sFloatValueVtable 10168754
 data sWordValueVtable 101686DC
 data sDerivedTextVtable 1016858C
 data sFirstScalarName 10168878
 data sFirstScalarLabel 10168888
 data sFirstModeName 1016889C
 data sFirstModeLabel 101688AC
 data sSecondScalarName 101688BC
 data sSecondScalarLabel 101688CC
 data sSecondModeName 101688E0
 data sSecondModeLabel 101688F0
 data sScalarHelp 1016858A
 data sModeHelp 10168589
 data sScalarInitial 10168874
*/

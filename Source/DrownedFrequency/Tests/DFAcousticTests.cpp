#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "GameplayTagsManager.h"
#include "Acoustics/DFAcousticTypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticTagsTest,"TDF.Acoustic.A01.Tags",EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)
bool FTDFAcousticTagsTest::RunTest(const FString&)
{
    TestEqual(TEXT("Exactly seven fixed categories"), TDFAcoustics::CategoryNames().Num(),7);
    for (const FName Name:TDFAcoustics::CategoryNames()) TestTrue(*Name.ToString(), TDFAcoustics::IsCategory(FGameplayTag::RequestGameplayTag(Name,false)));
    const auto Children=UGameplayTagsManager::Get().RequestGameplayTagChildren(TDFAcoustics::Tag(TEXT("Acoustic.Category")));
    TestEqual(TEXT("Taxonomy has no extra categories"),Children.Num(),7);
    for(const TCHAR* Name:{TEXT("Acoustic.Fidelity.Clean"),TEXT("Acoustic.Fidelity.Degraded"),TEXT("Acoustic.Fidelity.Contaminated"),TEXT("Acoustic.Route.Direct"),TEXT("Acoustic.Route.Pipe"),TEXT("Acoustic.Route.Speaker"),TEXT("Acoustic.Route.PuckProjected"),TEXT("Acoustic.Route.Overdub"),TEXT("Acoustic.Puzzle.Required"),TEXT("Acoustic.Puzzle.Optional"),TEXT("Acoustic.Puzzle.Decoy"),TEXT("Acoustic.Debug.Visible"),TEXT("Acoustic.Debug.LogOnly")}) TestTrue(Name,TDFAcoustics::Tag(Name).IsValid());
    TestFalse(TEXT("No Anomaly category"),TDFAcoustics::IsCategory(TDFAcoustics::Tag(TEXT("Acoustic.Category.Anomaly"))));
    TestFalse(TEXT("Route is not a category"),TDFAcoustics::IsCategory(TDFAcoustics::Tag(TEXT("Acoustic.Route.Direct"))));
    return !HasAnyErrors();
}
#include "Sound/SoundWave.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticTypesTest,"TDF.Acoustic.A02.Types",EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)
bool FTDFAcousticTypesTest::RunTest(const FString&)
{
    TestEqual(TEXT("Tier3"),int32(ETDFRecordingTier::Tier3),3);
    TestEqual(TEXT("Tier4"),int32(ETDFRecordingTier::Tier4),4);
    TestEqual(TEXT("Tier5"),int32(ETDFRecordingTier::Tier5),5);
    TestEqual(TEXT("Stable fidelity name"),StaticEnum<ETDFAcousticFidelity>()->GetNameStringByValue(0),FString(TEXT("Clean")));
    FTDFAcousticClipVariant V;
    TestFalse(TEXT("Missing sound invalid"),V.IsValid());
    TestEqual(TEXT("Safe radius"),V.EffectiveRadius,1200.f);
    V.Sound=NewObject<USoundWave>(); TestTrue(TEXT("Sound makes defaults valid"),V.IsValid());
    V.EffectiveRadius=-1; TestFalse(TEXT("Negative radius invalid"),V.IsValid());
    return !HasAnyErrors();
}
#include "Acoustics/DFAudioAnomalyDataAsset.h"
static UTDFAudioAnomalyDataAsset* TestAnomaly()
{
    auto* A=NewObject<UTDFAudioAnomalyDataAsset>();
    A->StableId=FPrimaryAssetId(TEXT("TDFAnomaly"),TEXT("TestBell"));
    A->CategoryTag=TDFAcoustics::Tag(TEXT("Acoustic.Category.Bell"));
    A->bCanSatisfyReceiver=true; A->ReceiverKeys.AddTag(TDFAcoustics::Tag(TEXT("Acoustic.Key.ServiceBell")));
    FTDFAcousticClipVariant V; V.Sound=NewObject<USoundWave>(); A->Variants.Add(ETDFAcousticFidelity::Clean,V);
    return A;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticAssetTest,"TDF.Acoustic.A03.AssetValidation",EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)
bool FTDFAcousticAssetTest::RunTest(const FString&)
{
    auto* A=TestAnomaly(); TestTrue(TEXT("Clean asset valid"),A->HasValidDefinition());
    A->Variants[ETDFAcousticFidelity::Clean].Sound=nullptr; TestFalse(TEXT("Null clip rejected"),A->HasValidDefinition());
    A=TestAnomaly(); FTDFAcousticClipVariant V; A->Variants.Add(ETDFAcousticFidelity::Degraded,V); TestFalse(TEXT("Optional variants validated"),A->HasValidDefinition());
    A=TestAnomaly(); A->CategoryTag=TDFAcoustics::Tag(TEXT("Acoustic.Route.Direct")); TestFalse(TEXT("Invalid category"),A->HasValidDefinition());
    A=TestAnomaly(); A->ReceiverKeys.Reset(); TestFalse(TEXT("Puzzle asset needs key"),A->HasValidDefinition());
    A->bCanSatisfyReceiver=false; TestTrue(TEXT("Non-puzzle cue allowed"),A->HasValidDefinition());
    return !HasAnyErrors();
}
#include "Acoustics/DFAudioAnomalyActor.h"
#include "Acoustics/DFAcousticWorldSubsystem.h"
#include "Engine/World.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticSourceTest,"TDF.Acoustic.A04.Source",EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)
bool FTDFAcousticSourceTest::RunTest(const FString&)
{
    UWorld* W=UWorld::CreateWorld(EWorldType::Game,false);
    auto* A=W->SpawnActor<ATDFAudioAnomalyActor>();
    TestFalse(TEXT("Null source invalid"),A->MakeRecordedSignal().IsValid());
    TestFalse(TEXT("Null source cannot emit"),A->EmitWorldStimulus());
    A->AnomalyData=TestAnomaly(); const auto S=A->MakeRecordedSignal();
    TestTrue(TEXT("Valid recording"),S.IsValid());
    TestEqual(TEXT("Stable identity"),S.SourceAnomalyId,A->AnomalyData->StableId);
    TestEqual(TEXT("Category preserved"),S.CategoryTag,A->AnomalyData->CategoryTag);
    auto* B=W->GetSubsystem<UTDFAcousticWorldSubsystem>(); int32 Events=0;
    B->OnStimulusNative.AddLambda([&](const auto& E){++Events; TestEqual(TEXT("Source origin"),E.WorldLocation,A->GetActorLocation());});
    TestTrue(TEXT("World emit"),A->EmitWorldStimulus()); TestEqual(TEXT("Single event"),Events,1);
    W->DestroyWorld(false); return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticSignalTest,"TDF.Acoustic.A05.Signal",EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)
bool FTDFAcousticSignalTest::RunTest(const FString&)
{
    FTDFRecordedSignal S; TestFalse(TEXT("Default invalid"),S.IsValid());
    S.InstanceGuid=FGuid::NewGuid(); S.SourceAnomalyId=FPrimaryAssetId(TEXT("TDFAnomaly"),TEXT("Test")); S.CategoryTag=TDFAcoustics::Tag(TEXT("Acoustic.Category.Bell"));
    TestTrue(TEXT("Non-puzzle signal still recordable"),S.IsValid()); TestFalse(TEXT("Not puzzle eligible"),S.CanSatisfyReceiver());
    S.bPuzzleEligible=true; TestFalse(TEXT("Eligible without keys invalid"),S.IsValid());
    S.ReceiverKeysSnapshot.AddTag(TDFAcoustics::Tag(TEXT("Acoustic.Key.ServiceBell"))); TestTrue(TEXT("Puzzle record valid"),S.CanSatisfyReceiver());
    S.PlayCount=-1; TestFalse(TEXT("Negative count invalid"),S.IsValid());
    return !HasAnyErrors();
}
#include "Acoustics/DFRecordingSlots.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticSlotsTest,"TDF.Acoustic.A06.Slots",EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)
bool FTDFAcousticSlotsTest::RunTest(const FString&)
{
    FTDFRecorderSlotSet Slots; Slots.InitializeToTier(ETDFRecordingTier::Tier3);
    TestEqual(TEXT("Three slots unlocked"),Slots.GetUnlockedSlotCount(),3);
    FTDFRecordedSignal S; S.InstanceGuid=FGuid::NewGuid(); S.SourceAnomalyId=FPrimaryAssetId(TEXT("TDFAnomaly"),TEXT("Test")); S.CategoryTag=TDFAcoustics::Tag(TEXT("Acoustic.Category.Water"));
    TestTrue(TEXT("Write unlocked"),Slots.PutSignal(0,S)); TestFalse(TEXT("Locked write rejected"),Slots.PutSignal(3,S));
    TestFalse(TEXT("No tier2"),Slots.SetUnlockedSlotCount(2)); TestFalse(TEXT("No tier6"),Slots.SetUnlockedSlotCount(6));
    TestTrue(TEXT("Tier4"),Slots.SetUnlockedSlotCount(4)); TestEqual(TEXT("Four unlocked"),Slots.GetUnlockedSlotCount(),4);
    TestTrue(TEXT("Tier5"),Slots.SetUnlockedSlotCount(5)); TestEqual(TEXT("Five unlocked"),Slots.GetUnlockedSlotCount(),5);
    TestEqual(TEXT("Upgrade preserves clip"),Slots.GetSignal(0)->InstanceGuid,S.InstanceGuid);
    TestFalse(TEXT("No downgrade loses clips"),Slots.SetUnlockedSlotCount(3));
    FTDFRecordedSignal Old; TestTrue(TEXT("Overwrite"),Slots.PutSignal(0,S,&Old)); TestEqual(TEXT("Overwrite returns metadata"),Old.InstanceGuid,S.InstanceGuid);
    TestTrue(TEXT("Delete"),Slots.DeleteSignal(0,&Old)); TestEqual(TEXT("Delete returns metadata"),Old.InstanceGuid,S.InstanceGuid);
    TestNull(TEXT("Slot cleared"),Slots.GetSignal(0)); TestFalse(TEXT("Cannot delete twice"),Slots.DeleteSignal(0));
    return !HasAnyErrors();
}
#include "Acoustics/DFRecorderComponent.h"
#include "Acoustics/DFAcousticPuzzleLedgerComponent.h"
struct FTDFRecorderFixture
{
    UWorld* World=UWorld::CreateWorld(EWorldType::Game,false);
    AActor* Player=World->SpawnActor<AActor>();
    ATDFAudioAnomalyActor* Source=World->SpawnActor<ATDFAudioAnomalyActor>();
    UTDFRecorderComponent* Recorder=NewObject<UTDFRecorderComponent>(Player);
    UTDFAcousticPuzzleLedgerComponent* Ledger=NewObject<UTDFAcousticPuzzleLedgerComponent>(Player);
    FTDFRecorderFixture()
    {
        Player->AddInstanceComponent(Recorder); Recorder->RegisterComponent();
        Player->AddInstanceComponent(Ledger); Ledger->RegisterComponent();
        Source->SetActorLocation(FVector(100,0,0)); Source->AnomalyData=TestAnomaly();
    }
    ~FTDFRecorderFixture(){World->DestroyWorld(false);}
};
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticRecorderTest,"TDF.Acoustic.A07.Recorder",EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)
bool FTDFAcousticRecorderTest::RunTest(const FString&)
{
    FTDFRecorderFixture F; int32 Events=0; F.Recorder->OnSlotsChangedNative.AddLambda([&]{++Events;});
    TestEqual(TEXT("Default tier3"),F.Recorder->GetSlots().GetUnlockedSlotCount(),3);
    TestFalse(TEXT("Invalid source"),F.Recorder->TryRecordAnomaly(nullptr,0)); TestEqual(TEXT("No failed mutation event"),Events,0);
    TestTrue(TEXT("Record into selected slot"),F.Recorder->TryRecordAnomaly(F.Source,0)); TestEqual(TEXT("One mutation event"),Events,1);
    TestTrue(TEXT("Ledger registered"),F.Ledger->HasEverHeldReceiverKey(TDFAcoustics::Tag(TEXT("Acoustic.Key.ServiceBell"))));
    TestFalse(TEXT("Cannot skip tier4"),F.Recorder->UnlockTier(ETDFRecordingTier::Tier5));
    TestTrue(TEXT("Tier4 unlock"),F.Recorder->UnlockTier(ETDFRecordingTier::Tier4)); TestTrue(TEXT("Tier5 unlock"),F.Recorder->UnlockTier(ETDFRecordingTier::Tier5));
    F.Source->SetActorLocation(FVector(1000,0,0)); TestFalse(TEXT("Out of range rejected"),F.Recorder->TryRecordAnomaly(F.Source,1));
    TestTrue(TEXT("Delete valid slot"),F.Recorder->DeleteSlot(0)); TestEqual(TEXT("Four total successful mutations"),Events,4);
    TestTrue(TEXT("Delete preserves key history"),F.Ledger->HasEverHeldReceiverKey(TDFAcoustics::Tag(TEXT("Acoustic.Key.ServiceBell"))));
    TestFalse(TEXT("Unconfigured fallback denied"),F.Ledger->CanPuzzleUseFallback(TDFAcoustics::Tag(TEXT("Acoustic.Key.ServiceBell"))));
    return !HasAnyErrors();
}
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Components/AudioComponent.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticPlaybackTest,"TDF.Acoustic.A08.Playback",EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)
bool FTDFAcousticPlaybackTest::RunTest(const FString&)
{
    FTDFRecorderFixture F;
    auto* Asset=UTDFRecorderPlaybackComponent::Resolve(FPrimaryAssetId(TEXT("TDFAnomaly"),TEXT("ServiceBell")));
    if(!TestNotNull(TEXT("Authored bell resolves through asset manager"),Asset)) return false;
    F.Source->AnomalyData=Asset; TestTrue(TEXT("Record real asset"),F.Recorder->TryRecordAnomaly(F.Source,0));
    auto* Playback=NewObject<UTDFRecorderPlaybackComponent>(F.Player); F.Player->AddInstanceComponent(Playback); Playback->RegisterComponent();
    auto* Bus=F.World->GetSubsystem<UTDFAcousticWorldSubsystem>();
    FTDFRecorderPlaybackRequest R; R.SlotIndex=0; R.bEmitWorldStimulus=false;
    TestTrue(TEXT("Local preview succeeds"),Playback->PlayRecordedSlot(R));
    TestEqual(TEXT("Preview has no world stimulus"),Bus->GetRecentStimuli().Num(),0);
    TestEqual(TEXT("Presentation sound is authored variant"),Playback->ActiveAudio->Sound.Get(),Asset->FindVariant(ETDFAcousticFidelity::Clean)->Sound.Get());
    TestEqual(TEXT("Count increments once"),F.Recorder->GetSlots().GetSignal(0)->PlayCount,1);
    R.bEmitWorldStimulus=true; TestTrue(TEXT("World playback"),Playback->PlayRecordedSlot(R)); TestEqual(TEXT("One world event"),Bus->GetRecentStimuli().Num(),1);
    TestEqual(TEXT("Signal instance preserved"),Playback->LastPlayback.SignalInstanceGuid,F.Recorder->GetSlots().GetSignal(0)->InstanceGuid);
    R.SlotIndex=2; TestFalse(TEXT("Empty slot fails"),Playback->PlayRecordedSlot(R));
    TestEqual(TEXT("No false presentation request"),Playback->PresentationRequests,2);
    TestNull(TEXT("Missing asset safe"),UTDFRecorderPlaybackComponent::Resolve(FPrimaryAssetId(TEXT("TDFAnomaly"),TEXT("Missing"))));
    return !HasAnyErrors();
}
#include "Acoustics/DFAcousticPuckActor.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticPuckTest,"TDF.Acoustic.A09.Puck",EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)
bool FTDFAcousticPuckTest::RunTest(const FString&)
{
    FTDFRecorderFixture F; F.Source->AnomalyData=UTDFRecorderPlaybackComponent::Resolve(FPrimaryAssetId(TEXT("TDFAnomaly"),TEXT("ServiceBell")));
    TestTrue(TEXT("Record bell"),F.Recorder->TryRecordAnomaly(F.Source,0));
    auto* P=F.World->SpawnActor<ATDFAcousticPuckActor>(); P->SetActorLocation(FVector(1800,1200,50));
    TestFalse(TEXT("Empty puck slot cannot emit"),P->ProjectRecordedSignal(F.Recorder,2));
    TestTrue(TEXT("Puck playback"),P->ProjectRecordedSignal(F.Recorder,0));
    const auto& E=P->Playback->LastPlayback;
    TestEqual(TEXT("Origin at puck"),E.WorldLocation,P->GetActorLocation());
    TestTrue(TEXT("Origin differs from player"),!E.WorldLocation.Equals(F.Player->GetActorLocation()));
    TestTrue(TEXT("Projected route"),E.Route==ETDFAcousticRoute::PuckProjected);
    TestEqual(TEXT("Same signal identity"),E.SignalInstanceGuid,F.Recorder->GetSlots().GetSignal(0)->InstanceGuid);
    TestTrue(TEXT("Keys preserved"),E.ReceiverKeys.HasAllExact(F.Source->AnomalyData->ReceiverKeys));
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticStimulusTest,"TDF.Acoustic.A10.Stimulus",EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)
bool FTDFAcousticStimulusTest::RunTest(const FString&)
{
    FTDFAcousticStimulus E; TestFalse(TEXT("Default invalid"),E.IsValid());
    E.StimulusGuid=FGuid::NewGuid(); E.CategoryTag=TDFAcoustics::Tag(TEXT("Acoustic.Category.Footstep")); TestTrue(TEXT("Non-anomaly noise valid"),E.IsValid());
    E.Radius=-30; E.Loudness=99; E.Contamination=-1; TestFalse(TEXT("Unsafe ranges rejected"),E.IsValid()); E.ClampSafeRanges(); TestTrue(TEXT("Clamped safe ranges"),E.IsValid());
    TestEqual(TEXT("Loudness clamp"),E.Loudness,4.f); TestEqual(TEXT("Radius clamp"),E.Radius,1.f);
    E.bPuzzleEligible=true; TestFalse(TEXT("Noise cannot pretend puzzle eligible"),E.IsValid());
    E.bPuzzleEligible=false; E.CategoryTag=TDFAcoustics::Tag(TEXT("Acoustic.Route.Direct")); TestFalse(TEXT("Noncategory rejected"),E.IsValid());
    return !HasAnyErrors();
}
#include "Acoustics/DFAcousticReceiverComponent.h"
#include "Tests/DFAcousticTestProbe.h"
#include "Components/SceneComponent.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticBusTest,"TDF.Acoustic.A11.WorldBus",EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)
bool FTDFAcousticBusTest::RunTest(const FString&)
{
    FTDFRecorderFixture F; auto* Bus=F.World->GetSubsystem<UTDFAcousticWorldSubsystem>();
    auto* Probe=NewObject<UTDFAcousticTestProbe>(); Bus->OnStimulus.AddDynamic(Probe,&UTDFAcousticTestProbe::ReceiveStimulus);
    auto* R=NewObject<UTDFAcousticReceiverComponent>(F.Player); F.Player->AddInstanceComponent(R); R->RegisterComponent();
    FTDFAcousticReceiverRule Rule; Rule.RequiredCategory=TDFAcoustics::Tag(TEXT("Acoustic.Category.Bell")); Rule.RequiredReceiverKeys=F.Source->AnomalyData->ReceiverKeys; R->Rules.Add(Rule);
    R->OnRuleSatisfied.AddDynamic(Probe,&UTDFAcousticTestProbe::ReceiveRule); Bus->RegisterReceiver(R); Bus->RegisterReceiver(R);
    auto E=FTDFAcousticStimulus::FromSignal(F.Source->MakeRecordedSignal(),*F.Source->AnomalyData->FindVariant(ETDFAcousticFidelity::Clean),FVector(100,0,0),F.Player,ETDFAcousticRoute::Direct);
    TestTrue(TEXT("Valid emission"),Bus->EmitStimulus(E)); TestEqual(TEXT("Blueprint listener called"),Probe->Stimuli,1); TestEqual(TEXT("Registered receiver called once"),Probe->Satisfied,1);
    R->ResetReceiver(); E.WorldLocation=FVector(10000,0,0); Bus->EmitStimulus(E); TestFalse(TEXT("Outside radius excluded"),R->IsRuleSatisfied(0));
    Bus->UnregisterReceiver(R); E.WorldLocation=FVector::ZeroVector; Bus->EmitStimulus(E); TestFalse(TEXT("Unregistered receiver excluded"),R->IsRuleSatisfied(0));
    for(int32 I=0;I<80;++I) {E.StimulusGuid=FGuid::NewGuid(); Bus->EmitStimulus(E);}
    TestEqual(TEXT("Bounded history"),Bus->GetRecentStimuli().Num(),Bus->RecentLimit);
    E.CategoryTag={}; TestFalse(TEXT("Invalid emission rejected"),Bus->EmitStimulus(E)); TestEqual(TEXT("Invalid never broadcast"),Probe->Stimuli,83);
    return !HasAnyErrors();
}
#include "Acoustics/DFPlayerNoiseEmitterComponent.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticNoiseTest,"TDF.Acoustic.A12.PlayerNoise",EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)
bool FTDFAcousticNoiseTest::RunTest(const FString&)
{
    FTDFRecorderFixture F; auto* Noise=NewObject<UTDFPlayerNoiseEmitterComponent>(F.Player); F.Player->AddInstanceComponent(Noise); Noise->RegisterComponent();
    Noise->Tuning=NewObject<UTDFPlayerNoiseTuning>();
    for(const FName Name:TDFAcoustics::CategoryNames()) {FTDFAcousticClipVariant V; V.Sound=NewObject<USoundWave>(); V.EffectiveRadius=333; V.Loudness=.7f; Noise->Tuning->Categories.Add(FGameplayTag::RequestGameplayTag(Name),V);}
    TestTrue(TEXT("Seven category tuning valid"),Noise->Tuning->HasValidDefinition());
    const FVector L(70,0,0); auto* Bus=F.World->GetSubsystem<UTDFAcousticWorldSubsystem>();
    TestTrue(TEXT("Footstep"),Noise->EmitFootstep(L,.5f)); TestEqual(TEXT("Data driven radius and surface scale"),Bus->GetRecentStimuli().Last().Radius,166.5f);
    TestTrue(TEXT("Impact"),Noise->EmitImpact(L)); TestTrue(TEXT("Machine"),Noise->EmitMachine(L)); TestTrue(TEXT("Bell"),Noise->EmitBell(L));
    TestTrue(TEXT("Water"),Noise->EmitWater(L)); TestTrue(TEXT("Static"),Noise->EmitStatic(L));
    TestTrue(TEXT("Authored voice"),Noise->EmitAuthoredVoice(L,FPrimaryAssetId(TEXT("TDFAnomaly"),TEXT("Voice"))));
    TestEqual(TEXT("All seven helper paths emit"),Bus->GetRecentStimuli().Num(),7);
    for(const auto& E:Bus->GetRecentStimuli()) {TestTrue(TEXT("Valid noise"),E.IsValid()); TestFalse(TEXT("Noise not puzzle eligible"),E.bPuzzleEligible);}
    TestFalse(TEXT("Wrong voice identity"),Noise->EmitAuthoredVoice(L,FPrimaryAssetId(TEXT("TDFAnomaly"),TEXT("ServiceBell"))));
    auto* R=NewObject<UTDFAcousticReceiverComponent>(F.Player); F.Player->AddInstanceComponent(R); R->RegisterComponent();
    FTDFAcousticReceiverRule Rule; Rule.RequiredCategory=TDFAcoustics::Tag(TEXT("Acoustic.Category.Footstep")); Rule.bRequirePuzzleEligible=false; R->Rules.Add(Rule); Bus->RegisterReceiver(R);
    Noise->EmitFootstep(L); TestTrue(TEXT("Footstep can trigger explicitly non-puzzle receiver"),R->IsRuleSatisfied(0));
    return !HasAnyErrors();
}
#include "Acoustics/DFAcousticReceiverActor.h"
#include "World/DFDoorActor.h"
#include "UObject/Script.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticReceiverTest,"TDF.Acoustic.A13.Receiver",EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)
bool FTDFAcousticReceiverTest::RunTest(const FString&)
{
    FTDFRecorderFixture F; auto* R=NewObject<UTDFAcousticReceiverComponent>(F.Player); F.Player->AddInstanceComponent(R); R->RegisterComponent();
    FTDFAcousticReceiverRule Rule; Rule.RequiredCategory=F.Source->AnomalyData->CategoryTag; Rule.RequiredReceiverKeys=F.Source->AnomalyData->ReceiverKeys; Rule.MaxContamination=.2f; Rule.MinimumFidelity=ETDFAcousticFidelity::Clean; Rule.MaxAcceptedRepetitionIndex=2; R->Rules.Add(Rule);
    const auto Good=FTDFAcousticStimulus::FromSignal(F.Source->MakeRecordedSignal(),*F.Source->AnomalyData->FindVariant(ETDFAcousticFidelity::Clean),FVector(10,0,0),F.Player,ETDFAcousticRoute::Direct);
    auto E=Good; E.CategoryTag=TDFAcoustics::Tag(TEXT("Acoustic.Category.Water")); TestFalse(TEXT("Wrong category"),R->EvaluateStimulus(E));
    E=Good; E.Contamination=.5f; TestFalse(TEXT("Contamination rejected"),R->EvaluateStimulus(E));
    E=Good; E.Route=ETDFAcousticRoute::PuckProjected; TestFalse(TEXT("Disallowed route"),R->EvaluateStimulus(E));
    E=Good; E.Fidelity=ETDFAcousticFidelity::Degraded; TestFalse(TEXT("Fidelity rejected"),R->EvaluateStimulus(E));
    E=Good; E.RepetitionIndex=3; TestFalse(TEXT("Repetition rejected"),R->EvaluateStimulus(E));
    E=Good; E.bPuzzleEligible=false; TestFalse(TEXT("Noise cannot solve puzzle"),R->EvaluateStimulus(E));
    TestTrue(TEXT("Correct signal accepted"),R->EvaluateStimulus(Good)); TestFalse(TEXT("Consume once"),R->EvaluateStimulus(Good));
    FEditorScriptExecutionGuard Guard;
    auto* Station=F.World->SpawnActor<ATDFAcousticReceiverActor>(); auto* Door=F.World->SpawnActor<ATDFDoorActor>();
    Station->Receiver->Rules.Add(Rule); Station->LinkedActor=Door; Station->DispatchBeginPlay();
    TestFalse(TEXT("Door starts shut"),Door->bIsOpen); TestTrue(TEXT("Expected metadata solves receiver"),Station->Receiver->EvaluateStimulus(Good)); TestTrue(TEXT("Linked door opens"),Door->bIsOpen);
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticLedgerTest,"TDF.Acoustic.A14.NoSoftlock",EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)
bool FTDFAcousticLedgerTest::RunTest(const FString&)
{
    FTDFRecorderFixture F; const auto Key=TDFAcoustics::Tag(TEXT("Acoustic.Key.ServiceBell"));
    auto* Rules=NewObject<UTDFAcousticPuzzleSafetyRules>(); Rules->AllowedFallbackKeys.AddTag(Key); F.Ledger->SafetyRules=Rules;
    auto S=F.Source->MakeRecordedSignal(); S.bPuzzleEligible=false; F.Ledger->RegisterRecordedSignal(S); TestFalse(TEXT("Decoy cannot pollute ledger"),F.Ledger->CanPuzzleUseFallback(Key));
    TestTrue(TEXT("Record required source"),F.Recorder->TryRecordAnomaly(F.Source,0)); TestTrue(TEXT("Delete every carried signal"),F.Recorder->DeleteSlot(0));
    TestTrue(TEXT("History survives deletion"),F.Ledger->CanPuzzleUseFallback(Key));
    auto* R=NewObject<UTDFAcousticReceiverComponent>(F.Player); FTDFAcousticReceiverRule Rule; Rule.RequiredCategory=F.Source->AnomalyData->CategoryTag; Rule.RequiredReceiverKeys.AddTag(Key); R->Rules.Add(Rule);
    TestFalse(TEXT("No silent automatic puzzle solve"),R->TryLedgerFallback(F.Ledger,0));
    R->Rules[0].bAllowLedgerFallback=true; TestTrue(TEXT("Explicitly opted in fallback works"),R->TryLedgerFallback(F.Ledger,0)); TestFalse(TEXT("Fallback consumed once"),R->TryLedgerFallback(F.Ledger,0));
    TestTrue(TEXT("Source remains recordable after all clips deleted"),F.Recorder->TryRecordAnomaly(F.Source,0));
    return !HasAnyErrors();
}
#endif

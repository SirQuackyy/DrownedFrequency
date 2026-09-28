#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Acoustics/DFAcousticProcessingRules.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticProcessingTest,"TDF.Acoustic.A15.Processing",EAutomationTestFlags::EditorContext|EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFAcousticProcessingTest::RunTest(const FString&)
{
    auto* R=NewObject<UTDFAcousticProcessingRules>(); FTDFRecordedSignal S;
    TestTrue(TEXT("First play stays clean"),R->ComputeEffectiveFidelity(S)==ETDFAcousticFidelity::Clean);
    S.PlayCount=2; TestTrue(TEXT("Authored degradation threshold"),R->ComputeEffectiveFidelity(S)==ETDFAcousticFidelity::Degraded);
    TestTrue(TEXT("Credibility decreases deterministically"),FMath::IsNearlyEqual(R->ComputeCredibility(S,1,.25f),.5f));
    S.PlayCount=4; TestTrue(TEXT("Contaminated threshold"),R->ComputeEffectiveFidelity(S)==ETDFAcousticFidelity::Contaminated);
    S.PlayCount=0; S.Fidelity=ETDFAcousticFidelity::Contaminated; TestTrue(TEXT("No self cleaning"),R->ComputeEffectiveFidelity(S)==ETDFAcousticFidelity::Contaminated);
    S.PlayCount=999; TestTrue(TEXT("Radius remains positive"),R->ComputeEffectiveRadiusMultiplier(S)>0);
    return !HasAnyErrors();
}
#include "Acoustics/DFAcousticOverdubComponent.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFAudioAnomalyDataAsset.h"
static FTDFRecordedSignal DepthSignal(const TCHAR* Name)
{
    FTDFRecordedSignal S; auto* A=UTDFRecorderPlaybackComponent::Resolve(FPrimaryAssetId(TEXT("TDFAnomaly"),Name)); if(!A) return S;
    S.InstanceGuid=FGuid::NewGuid(); S.SourceAnomalyId=A->StableId; S.CategoryTag=A->CategoryTag; S.ReceiverKeysSnapshot=A->ReceiverKeys; S.bPuzzleEligible=A->bCanSatisfyReceiver; return S;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticOverdubTest,"TDF.Acoustic.A16.Overdub",EAutomationTestFlags::EditorContext|EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFAcousticOverdubTest::RunTest(const FString&)
{
    auto* C=NewObject<UTDFAcousticOverdubComponent>(); const auto Bell=DepthSignal(TEXT("ServiceBell")), Voice=DepthSignal(TEXT("Voice"));
    TestFalse(TEXT("Overdub inaccessible in slice"),C->TryOverdub(Bell,Voice).bSuccess);
    C->SetCampaignAct(5); auto Result=C->TryOverdub(Bell,Voice); TestTrue(TEXT("Late-game data path works"),Result.bSuccess);
    TestEqual(TEXT("Two stable source identities"),Result.ResultSignal.OverdubbedSourceIds.Num(),2);
    TestTrue(TEXT("Puzzle keys retained without duplicates"),Result.ResultSignal.ReceiverKeysSnapshot==Bell.ReceiverKeysSnapshot);
    TestEqual(TEXT("Generation increment"),Result.ResultSignal.Generation,1);
    auto* Asset=UTDFRecorderPlaybackComponent::Resolve(Voice.SourceAnomalyId); if(!TestNotNull(TEXT("Voice asset exists"),Asset)) return false;
    {TGuardValue<bool> Guard(Asset->bCanBeOverdubbed,false); TestFalse(TEXT("Source permission enforced"),C->TryOverdub(Bell,Voice).bSuccess);}
    auto Dirty=C->ApplyContamination(Voice,5); TestEqual(TEXT("Contamination upper clamp"),Dirty.Contamination,1.f); TestTrue(TEXT("Authored contaminated take selected"),Dirty.Fidelity==ETDFAcousticFidelity::Contaminated);
    TestEqual(TEXT("Lower clamp"),C->ApplyContamination(Dirty,-5).Contamination,0.f);
    auto Machine=DepthSignal(TEXT("Machine")); TestEqual(TEXT("Natural/machine permission respected"),C->ApplyContamination(Machine,.5f).Contamination,0.f);
    return !HasAnyErrors();
}
#include "Engine/World.h"
#include "Acoustics/DFAudioAnomalyActor.h"
#include "Acoustics/DFRecorderComponent.h"
#include "Acoustics/DFAcousticPuzzleLedgerComponent.h"
#include "Components/AudioComponent.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticVariantsTest,"TDF.Acoustic.A15.AuthoredVariants",EAutomationTestFlags::EditorContext|EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFAcousticVariantsTest::RunTest(const FString&)
{
    auto* A=UTDFRecorderPlaybackComponent::Resolve(FPrimaryAssetId(TEXT("TDFAnomaly"),TEXT("ServiceBell")));
    if(!TestNotNull(TEXT("Bell definition"),A)) return false;
    for(const auto F:{ETDFAcousticFidelity::Clean,ETDFAcousticFidelity::Degraded,ETDFAcousticFidelity::Contaminated}) TestNotNull(TEXT("Each authored take exists"),A->FindVariant(F));
    if(HasAnyErrors()) return false;
    UWorld* W=UWorld::CreateWorld(EWorldType::Game,false); auto* P=W->SpawnActor<AActor>(); auto* Source=W->SpawnActor<ATDFAudioAnomalyActor>(); Source->SetActorLocation(FVector(100,0,0)); Source->AnomalyData=A;
    auto* Recorder=NewObject<UTDFRecorderComponent>(P); P->AddInstanceComponent(Recorder); Recorder->RegisterComponent();
    auto* Ledger=NewObject<UTDFAcousticPuzzleLedgerComponent>(P); P->AddInstanceComponent(Ledger); Ledger->RegisterComponent();
    auto* Playback=NewObject<UTDFRecorderPlaybackComponent>(P); P->AddInstanceComponent(Playback); Playback->RegisterComponent(); Playback->ProcessingRules=NewObject<UTDFAcousticProcessingRules>();
    TestTrue(TEXT("Record"),Recorder->TryRecordAnomaly(Source,0)); FTDFRecorderPlaybackRequest Request; Request.SlotIndex=0;
    for(int32 I=0;I<5;++I)
    {
        TestTrue(TEXT("Playback succeeds with authored take"),Playback->PlayRecordedSlot(Request));
        const auto Expected=I<2?ETDFAcousticFidelity::Clean:I<4?ETDFAcousticFidelity::Degraded:ETDFAcousticFidelity::Contaminated;
        TestTrue(TEXT("Stimulus fidelity follows authored thresholds"),Playback->LastPlayback.Fidelity==Expected);
        TestEqual(TEXT("Correct sound asset actually assigned"),Playback->ActiveAudio->Sound.Get(),A->FindVariant(Expected)->Sound.Get());
        TestEqual(TEXT("Source identity never changes"),Playback->LastPlayback.SourceAnomalyId,A->StableId);
    }
    W->DestroyWorld(false); return !HasAnyErrors();
}
#include "Acoustics/DFAcousticPipeNode.h"
#include "Acoustics/DFAcousticPipeSubsystem.h"
#include "Acoustics/DFAcousticWorldSubsystem.h"
#include "Acoustics/DFAcousticReceiverActor.h"
#include "Acoustics/DFAcousticReceiverComponent.h"
#include "World/DFDoorActor.h"
#include "UObject/Script.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticPipeTest,"TDF.Acoustic.A17.Pipe",EAutomationTestFlags::EditorContext|EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFAcousticPipeTest::RunTest(const FString&)
{
    FEditorScriptExecutionGuard Guard;
    UWorld* W=UWorld::CreateWorld(EWorldType::Game,false); auto* Pipes=W->GetSubsystem<UTDFAcousticPipeSubsystem>(); auto* Bus=W->GetSubsystem<UTDFAcousticWorldSubsystem>();
    auto* In=W->SpawnActor<ATDFAcousticPipeNode>(); In->NetworkId=TEXT("TestPipe"); In->bIsInput=true;
    auto* Out=W->SpawnActor<ATDFAcousticPipeNode>(); Out->NetworkId=In->NetworkId; Out->bIsOutput=true; Out->SetActorLocation(FVector(5000,0,0)); Out->AttenuationMultiplier=.5f;
    Pipes->RegisterNode(In); Pipes->RegisterNode(Out);
    auto S=DepthSignal(TEXT("ServiceBell")); auto* A=UTDFRecorderPlaybackComponent::Resolve(S.SourceAnomalyId); if(!TestNotNull(TEXT("Source available"),A)) {W->DestroyWorld(false); return false;}
    const auto E=FTDFAcousticStimulus::FromSignal(S,*A->FindVariant(S.Fidelity),FVector::ZeroVector,nullptr,ETDFAcousticRoute::Direct);
    auto* Station=W->SpawnActor<ATDFAcousticReceiverActor>(); Station->SetActorLocation(FVector(5100,0,0)); auto* Door=W->SpawnActor<ATDFDoorActor>(); Station->LinkedActor=Door;
    FTDFAcousticReceiverRule Rule; Rule.RequiredCategory=S.CategoryTag; Rule.RequiredReceiverKeys=S.ReceiverKeysSnapshot; Rule.AcceptedRoutes={ETDFAcousticRoute::Pipe}; Station->Receiver->Rules.Add(Rule); Station->DispatchBeginPlay();
    TestTrue(TEXT("Input event"),Bus->EmitStimulus(E));
    const auto Routed=Bus->GetRecentStimuli().Last();
    TestTrue(TEXT("Pipe route"),Routed.Route==ETDFAcousticRoute::Pipe); TestEqual(TEXT("Outlet origin"),Routed.WorldLocation,Out->GetActorLocation());
    TestEqual(TEXT("Source identity preserved"),Routed.SourceAnomalyId,S.SourceAnomalyId); TestEqual(TEXT("Instance identity preserved"),Routed.SignalInstanceGuid,S.InstanceGuid);
    TestTrue(TEXT("Keys preserved"),Routed.ReceiverKeys==S.ReceiverKeysSnapshot); TestEqual(TEXT("Authored attenuation"),Routed.Radius,E.Radius*.5f);
    TestEqual(TEXT("Traceable history"),Routed.RouteHistory.Num(),2); TestTrue(TEXT("Remote pipe-only gate opened"),Door->bIsOpen);
    auto Feedback=Routed; Feedback.WorldLocation=FVector::ZeroVector; TestFalse(TEXT("Visited network cannot feedback"),Pipes->TryRouteThroughPipe(Feedback,In->NetworkId));
    W->DestroyWorld(false); return !HasAnyErrors();
}
#include "Acoustics/DFAcousticSpeakerActor.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticSpeakerTest,"TDF.Acoustic.A18.Speaker",EAutomationTestFlags::EditorContext|EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFAcousticSpeakerTest::RunTest(const FString&)
{
    FEditorScriptExecutionGuard Guard;
    UWorld* W=UWorld::CreateWorld(EWorldType::Game,false); auto* Bus=W->GetSubsystem<UTDFAcousticWorldSubsystem>();
    auto* Speaker=W->SpawnActor<ATDFAcousticSpeakerActor>(); Speaker->SetActorLocation(FVector(4500,0,0)); Speaker->DispatchBeginPlay();
    auto S=DepthSignal(TEXT("ServiceBell")); auto* A=UTDFRecorderPlaybackComponent::Resolve(S.SourceAnomalyId); if(!TestNotNull(TEXT("Source available"),A)) {W->DestroyWorld(false); return false;}
    auto E=FTDFAcousticStimulus::FromSignal(S,*A->FindVariant(S.Fidelity),FVector::ZeroVector,nullptr,ETDFAcousticRoute::Direct);
    auto* Station=W->SpawnActor<ATDFAcousticReceiverActor>(); Station->SetActorLocation(FVector(4600,0,0)); auto* Door=W->SpawnActor<ATDFDoorActor>(); Station->LinkedActor=Door;
    FTDFAcousticReceiverRule Rule; Rule.RequiredCategory=S.CategoryTag; Rule.RequiredReceiverKeys=S.ReceiverKeysSnapshot; Rule.AcceptedRoutes={ETDFAcousticRoute::Speaker}; Station->Receiver->Rules.Add(Rule); Station->DispatchBeginPlay();
    TestTrue(TEXT("Rebroadcast"),Speaker->RebroadcastStimulus(E)); const auto Out=Bus->GetRecentStimuli().Last();
    TestEqual(TEXT("Speaker origin"),Out.WorldLocation,Speaker->GetActorLocation()); TestTrue(TEXT("Speaker route"),Out.Route==ETDFAcousticRoute::Speaker);
    TestEqual(TEXT("Identity preserved"),Out.SourceAnomalyId,S.SourceAnomalyId); TestEqual(TEXT("Radius multiplier"),Out.Radius,E.Radius*1.5f);
    TestTrue(TEXT("Speaker-only gate opened"),Door->bIsOpen); TestFalse(TEXT("Self feedback blocked"),Speaker->RebroadcastStimulus(Out));
    auto* Other=W->SpawnActor<ATDFAcousticSpeakerActor>(); Other->SetActorLocation(Speaker->GetActorLocation()); Other->bRebroadcastNearby=true; Other->DispatchBeginPlay(); Speaker->bRebroadcastNearby=true;
    E.WorldLocation=Speaker->GetActorLocation(); const int32 Before=Bus->GetRecentStimuli().Num(); Bus->EmitStimulus(E);
    TestTrue(TEXT("Two-speaker feedback remains bounded"),Bus->GetRecentStimuli().Num()-Before<=5);
    W->DestroyWorld(false); return !HasAnyErrors();
}
#include "Acoustics/DFAcousticCaptionSubsystem.h"
#include "Engine/GameInstance.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticCaptionTest,"TDF.Acoustic.A19.Captions",EAutomationTestFlags::EditorContext|EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFAcousticCaptionTest::RunTest(const FString&)
{
    auto* C=NewObject<UTDFAcousticCaptionSubsystem>(NewObject<UGameInstance>()); auto S=DepthSignal(TEXT("ServiceBell"));
    C->PushCaptionsForSignal(S,ETDFAcousticRoute::Pipe,true);
    auto Lines=C->GetActiveCaptions(); TestTrue(TEXT("Authored caption appears"),Lines.Num()>0);
    if(Lines.Num()) {TestTrue(TEXT("Private preview metadata"),Lines[0].bLocalPreview); TestTrue(TEXT("Route metadata"),Lines[0].Route==ETDFAcousticRoute::Pipe); TestTrue(TEXT("Category metadata"),Lines[0].CategoryTag==S.CategoryTag);}
    C->Advance(30); TestEqual(TEXT("Captions expire"),C->GetActiveCaptions().Num(),0);
    auto* A=UTDFRecorderPlaybackComponent::Resolve(S.SourceAnomalyId); if(!A) return false;
    auto V=*A->FindVariant(S.Fidelity); V.Captions.Reset(); C->PushVariant(V,S.CategoryTag,ETDFAcousticRoute::Direct,FVector::ZeroVector,false);
    TestEqual(TEXT("Missing captions remain nonblocking"),C->GetActiveCaptions().Num(),0);
    S.Fidelity=ETDFAcousticFidelity::Contaminated; C->PushCaptionsForSignal(S,ETDFAcousticRoute::Speaker,false);
    Lines=C->GetActiveCaptions(); TestTrue(TEXT("Alternate authored caption appears"),Lines.Num()>0);
    if(Lines.Num()) TestEqual(TEXT("Selected variant text"),Lines[0].Text.ToString(),A->FindVariant(S.Fidelity)->Captions[0].Text.ToString());
    C->Clear(); return !HasAnyErrors();
}
#include "Acoustics/DFAcousticBlueprintLibrary.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticHelperTest,"TDF.Acoustic.A20.Presentation",EAutomationTestFlags::EditorContext|EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFAcousticHelperTest::RunTest(const FString&)
{
    auto S=DepthSignal(TEXT("ServiceBell")); S.SourceAnomalyId=FPrimaryAssetId(TEXT("TDFAnomaly"),TEXT("Missing"));
    TestEqual(TEXT("Missing source has fallback"),UTDFAcousticBlueprintLibrary::GetSignalDisplayName(S).ToString(),FString(TEXT("Unavailable signal")));
    TArray<FLinearColor> Colors; for(const auto& N:TDFAcoustics::CategoryNames()) Colors.AddUnique(UTDFAcousticBlueprintLibrary::GetCategoryColor(TDFAcoustics::Tag(*N.ToString())).GetSpecifiedColor());
    TestEqual(TEXT("Seven distinct category colors"),Colors.Num(),7); return !HasAnyErrors();
}
#include "Acoustics/DFAcousticDebug.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticDebugTest,"TDF.Acoustic.A21.Debug",EAutomationTestFlags::EditorContext|EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFAcousticDebugTest::RunTest(const FString&)
{
    for(const auto& N:TDFAcoustics::CategoryNames()) {const auto S=TDFAcousticDebug::MakeTest(TDFAcoustics::Tag(*N.ToString()),900,FVector(4,5,6)); TestTrue(TEXT("Valid debug category"),S.IsValid()); const FString D=TDFAcousticDebug::Describe(S); for(const auto* Key:{TEXT("category="),TEXT("route="),TEXT("fidelity="),TEXT("contamination="),TEXT("location=")}) TestTrue(TEXT("Inspectable dump"),D.Contains(Key));}
    TestEqual(TEXT("Asset validation passes"),TDFAcousticDebug::ValidateAssets(),0);
    return !HasAnyErrors();
}
#include "Acoustics/DFAcousticSaveTypes.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFAcousticSaveTest,"TDF.Acoustic.A22.SaveContract",EAutomationTestFlags::EditorContext|EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFAcousticSaveTest::RunTest(const FString&)
{
    auto* R=NewObject<UTDFRecorderComponent>(); auto* L=NewObject<UTDFAcousticPuzzleLedgerComponent>(); auto Empty=FTDFAcousticSaveData::Capture(R,L);
    TestTrue(TEXT("Empty tier3 restore"),Empty.Restore(R,L));
    auto S=DepthSignal(TEXT("ServiceBell")); S.Fidelity=ETDFAcousticFidelity::Contaminated; S.Contamination=.6f; S.PlayCount=13; S.Generation=2; S.OverdubbedSourceIds={S.SourceAnomalyId,FPrimaryAssetId(TEXT("TDFAnomaly"),TEXT("Voice"))};
    L->RegisterDeletedOrOverwrittenSignal(S); auto Data=FTDFAcousticSaveData::Capture(R,L); Data.UnlockTier=ETDFRecordingTier::Tier5; Data.SlotSignals[4]=S; Data.SelectedSlot=4;
    TArray<uint8> Bytes; FMemoryWriter Writer(Bytes); FObjectAndNameAsStringProxyArchive Out(Writer,false); Out.ArIsSaveGame=true; FTDFAcousticSaveData::StaticStruct()->SerializeItem(Out,&Data,nullptr);
    FTDFAcousticSaveData Loaded; FMemoryReader Reader(Bytes); FObjectAndNameAsStringProxyArchive In(Reader,true); In.ArIsSaveGame=true; FTDFAcousticSaveData::StaticStruct()->SerializeItem(In,&Loaded,nullptr);
    TestTrue(TEXT("Restores full serialized payload"),Loaded.Restore(R,L)); const auto* Restored=R->GetSlots().GetSignal(4); if(!TestNotNull(TEXT("Slot five restored"),Restored)) return false;
    TestEqual(TEXT("Count survives"),Restored->PlayCount,13); TestEqual(TEXT("Overdub identity survives"),Restored->OverdubbedSourceIds.Num(),2); TestEqual(TEXT("Contamination survives"),Restored->Contamination,.6f); TestTrue(TEXT("Deleted ledger survives"),L->GetEverHeldKeys()==S.ReceiverKeysSnapshot);
    Loaded.SlotSignals[4].SourceAnomalyId=FPrimaryAssetId(TEXT("TDFAnomaly"),TEXT("RemovedAsset")); TestTrue(TEXT("Unknown source retains metadata"),Loaded.Restore(R,L)); TestNull(TEXT("Unknown source fails playback resolution safely"),UTDFRecorderPlaybackComponent::Resolve(Loaded.SlotSignals[4].SourceAnomalyId));
    Loaded.Version=99; TestFalse(TEXT("Unsupported contract rejected"),Loaded.Restore(R,L)); return !HasAnyErrors();
}
#endif

#include "Acoustics/DFAcousticDebug.h"
#include "Acoustics/DFAcousticWorldSubsystem.h"
#include "Acoustics/DFAcousticReceiverComponent.h"
#include "Acoustics/DFAudioAnomalyDataAsset.h"
#include "Engine/AssetManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "HAL/IConsoleManager.h"
#include "DrawDebugHelpers.h"
#include "UObject/UObjectIterator.h"
DEFINE_LOG_CATEGORY(LogAcoustics);
DEFINE_LOG_CATEGORY(LogAcousticReceiver);
DEFINE_LOG_CATEGORY(LogRecorder);
FString TDFAcousticDebug::Describe(const FTDFAcousticStimulus& S)
{
    return FString::Printf(TEXT("id=%s source=%s category=%s route=%s fidelity=%s contamination=%.3f repetition=%d radius=%.1f location=%s"),*S.SignalInstanceGuid.ToString(),*S.SourceAnomalyId.ToString(),*S.CategoryTag.ToString(),*StaticEnum<ETDFAcousticRoute>()->GetNameStringByValue(int64(S.Route)),*StaticEnum<ETDFAcousticFidelity>()->GetNameStringByValue(int64(S.Fidelity)),S.Contamination,S.RepetitionIndex,S.Radius,*S.WorldLocation.ToString());
}
FTDFAcousticStimulus TDFAcousticDebug::MakeTest(FGameplayTag Category,float Radius,FVector Location)
{
    FTDFAcousticStimulus S; S.StimulusGuid=FGuid::NewGuid(); S.CategoryTag=Category; S.Radius=Radius; S.WorldLocation=Location; S.bPuzzleEligible=false; S.ClampSafeRanges(); return S;
}
int32 TDFAcousticDebug::ValidateAssets()
{
    TArray<FPrimaryAssetId> Ids; UAssetManager::Get().GetPrimaryAssetIdList(TEXT("TDFAnomaly"),Ids); int32 Invalid=0;
    for(const auto& Id:Ids) {auto* A=Cast<UTDFAudioAnomalyDataAsset>(UAssetManager::Get().GetPrimaryAssetPath(Id).TryLoad()); if(!A || !A->HasValidDefinition()) {++Invalid; UE_LOG(LogAcoustics,Warning,TEXT("Invalid anomaly %s"),*Id.ToString());}}
    UE_LOG(LogAcoustics,Display,TEXT("Validated %d anomalies; %d invalid"),Ids.Num(),Invalid); return Invalid;
}
#if !UE_BUILD_SHIPPING
static TAutoConsoleVariable<int32> DrawStimuli(TEXT("acoustic.Debug.DrawStimuli"),0,TEXT("Draw emitted stimulus radius: 0/1"),ECVF_Cheat);
static FAutoConsoleCommandWithWorld ListRecent(TEXT("acoustic.Debug.ListRecent"),TEXT("Print recent acoustic events"),FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* W){if(W) if(auto* B=W->GetSubsystem<UTDFAcousticWorldSubsystem>()) for(const auto& S:B->GetRecentStimuli()) UE_LOG(LogAcoustics,Display,TEXT("%s"),*TDFAcousticDebug::Describe(S));}));
static FAutoConsoleCommandWithWorldAndArgs EmitTest(TEXT("acoustic.Debug.EmitTest"),TEXT("EmitTest Category Radius"),FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& A,UWorld* W){
    if(!W || A.Num()!=2) return; const auto Tag=TDFAcoustics::Tag(*(TEXT("Acoustic.Category.")+A[0])); float Radius=0; if(!LexTryParseString(Radius,*A[1]) || !TDFAcoustics::IsCategory(Tag)) return;
    FVector Location=FVector::ZeroVector; if(auto* PC=W->GetFirstPlayerController()) if(APawn* P=PC->GetPawn()) Location=P->GetActorLocation(); W->GetSubsystem<UTDFAcousticWorldSubsystem>()->EmitStimulus(TDFAcousticDebug::MakeTest(Tag,Radius,Location));
}));
static FAutoConsoleCommand Validate(TEXT("acoustic.Debug.ValidateAssets"),TEXT("Validate anomaly assets"),FConsoleCommandDelegate::CreateLambda([](){TDFAcousticDebug::ValidateAssets();}));
static FAutoConsoleCommandWithWorld Receivers(TEXT("acoustic.Debug.DumpReceivers"),TEXT("Show receiver results"),FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* W){for(TObjectIterator<UTDFAcousticReceiverComponent> I;I;++I) if(I->GetWorld()==W && !I->IsTemplate()) UE_LOG(LogAcousticReceiver,Display,TEXT("%s: %s"),*I->GetPathName(),*I->LastEvaluation);}));
#endif
void TDFAcousticDebug::Draw(UWorld* W,const FTDFAcousticStimulus& S)
{
#if !UE_BUILD_SHIPPING
    if(W && DrawStimuli.GetValueOnGameThread()) {DrawDebugSphere(W,S.WorldLocation,S.Radius,24,S.bPuzzleEligible?FColor::Cyan:FColor::Silver,false,3.f); DrawDebugString(W,S.WorldLocation,Describe(S),nullptr,FColor::White,3.f);}
#endif
}

#include "Acoustics/DFAudioAnomalyActor.h"
#include "Acoustics/DFAudioAnomalyDataAsset.h"
#include "Acoustics/DFAcousticWorldSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Sound/SoundAttenuation.h"
ATDFAudioAnomalyActor::ATDFAudioAnomalyActor()
{
    StimulusOrigin=CreateDefaultSubobject<USceneComponent>(TEXT("StimulusOrigin")); SetRootComponent(StimulusOrigin);
    SourceMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SourceMesh")); SourceMesh->SetupAttachment(StimulusOrigin);
    SourceAudio=CreateDefaultSubobject<UAudioComponent>(TEXT("SourceAudio")); SourceAudio->SetupAttachment(StimulusOrigin); SourceAudio->bAutoActivate=false;
}
void ATDFAudioAnomalyActor::BeginPlay()
{
    Super::BeginPlay();
    if (bAutoEmitOnBeginPlay) EmitWorldStimulus();
    if (bAutoEmitOnBeginPlay && RepeatInterval>0) GetWorldTimerManager().SetTimer(RepeatTimer,FTimerDelegate::CreateWeakLambda(this,[this]{EmitWorldStimulus();}),FMath::Max(RepeatInterval,1.f),true);
}
void ATDFAudioAnomalyActor::EndPlay(const EEndPlayReason::Type Reason)
{
    if(GetWorld()) GetWorldTimerManager().ClearTimer(RepeatTimer); SourceAudio->Stop(); Super::EndPlay(Reason);
}
bool ATDFAudioAnomalyActor::CanBeRecordedBy(AActor* RecorderOwner) const
{
    if (!RecorderOwner || RecorderOwner->GetWorld()!=GetWorld() || !AnomalyData || !AnomalyData->HasValidDefinition() || !FMath::IsFinite(InteractionRecordRange) || InteractionRecordRange<=0) return false;
    FVector Eye; FRotator Rot; RecorderOwner->GetActorEyesViewPoint(Eye,Rot);
    const FVector Delta=StimulusOrigin->GetComponentLocation()-Eye;
    if (Delta.SizeSquared()>FMath::Square(InteractionRecordRange) || FVector::DotProduct(Rot.Vector(),Delta.GetSafeNormal())<.5f) return false;
    FHitResult Hit; FCollisionQueryParams Params(SCENE_QUERY_STAT(RecordSource),false,RecorderOwner); Params.AddIgnoredActor(this);
    return !GetWorld()->LineTraceSingleByChannel(Hit,Eye,StimulusOrigin->GetComponentLocation(),ECC_Visibility,Params);
}
FTDFRecordedSignal ATDFAudioAnomalyActor::MakeRecordedSignal(ETDFAcousticFidelity Fidelity) const
{
    FTDFRecordedSignal S;
    if (!AnomalyData || !AnomalyData->FindVariant(Fidelity)) return S;
    S.InstanceGuid=FGuid::NewGuid(); S.SourceAnomalyId=AnomalyData->GetPrimaryAssetId(); S.CategoryTag=AnomalyData->CategoryTag;
    S.Fidelity=Fidelity; S.ReceiverKeysSnapshot=AnomalyData->ReceiverKeys; S.bPuzzleEligible=AnomalyData->bCanSatisfyReceiver;
    return S;
}
bool ATDFAudioAnomalyActor::EmitWorldStimulus()
{
    const auto S=MakeRecordedSignal(); auto* Bus=GetWorld()->GetSubsystem<UTDFAcousticWorldSubsystem>();
    if (!S.IsValid() || !Bus) return false;
    const auto* V=AnomalyData->FindVariant(S.Fidelity);
    if (!Bus->EmitStimulus(FTDFAcousticStimulus::FromSignal(S,*V,StimulusOrigin->GetComponentLocation(),this,ETDFAcousticRoute::Direct))) return false;
    FSoundAttenuationSettings Att; Att.bSpatialize=true; Att.bAttenuate=true; Att.AttenuationShapeExtents=FVector(80);
    Att.FalloffDistance=FMath::Max(1.f,V->EffectiveRadius-80); Att.bEnableOcclusion=true; Att.OcclusionVolumeAttenuation=.5f; Att.OcclusionLowPassFilterFrequency=1800;
    SourceAudio->AdjustAttenuation(Att); SourceAudio->SetSound(V->Sound); SourceAudio->SetVolumeMultiplier(V->Loudness); SourceAudio->Play();
    return true;
}

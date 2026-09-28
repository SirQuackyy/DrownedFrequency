#include "Acoustics/DFPlayerNoiseEmitterComponent.h"
#include "Acoustics/DFAcousticWorldSubsystem.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFAudioAnomalyDataAsset.h"
#include "Player/DFPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundAttenuation.h"
#include "Engine/World.h"
bool UTDFPlayerNoiseTuning::HasValidDefinition() const
{
    if(Categories.Num()!=7 || !FMath::IsFinite(WalkStride) || WalkStride<=0 || !FMath::IsFinite(SprintStride) || SprintStride<=0 || !FMath::IsFinite(CrouchStride) || CrouchStride<=0 || !FMath::IsFinite(CrouchNoiseMultiplier) || CrouchNoiseMultiplier<=0 || !FMath::IsFinite(SprintNoiseMultiplier) || SprintNoiseMultiplier<=0) return false;
    for(const auto& P:Categories) if(!TDFAcoustics::IsCategory(P.Key) || !P.Value.IsValid()) return false;
    return true;
}
UTDFPlayerNoiseEmitterComponent::UTDFPlayerNoiseEmitterComponent() {PrimaryComponentTick.bCanEverTick=true;}
void UTDFPlayerNoiseEmitterComponent::BeginPlay()
{
    Super::BeginPlay(); PreviousLocation=GetOwner()->GetActorLocation(); SetComponentTickEnabled(Cast<ATDFPlayerCharacter>(GetOwner())!=nullptr);
}
bool UTDFPlayerNoiseEmitterComponent::EmitCategory(FGameplayTag Category,FVector Location,float Strength)
{
    if(!GetWorld() || !Tuning || !Tuning->HasValidDefinition() || !FMath::IsFinite(Strength) || Strength<=0) return false;
    const auto* V=Tuning->Categories.Find(Category); if(!V) return false;
    return EmitVariant(Category,*V,Location,Strength);
}
bool UTDFPlayerNoiseEmitterComponent::EmitVariant(FGameplayTag Category,const FTDFAcousticClipVariant& Variant,FVector Location,float Strength,FPrimaryAssetId SourceId)
{
    if(!GetWorld() || !Variant.IsValid()) return false;
    const auto* V=&Variant;
    FTDFAcousticStimulus S; S.StimulusGuid=FGuid::NewGuid(); S.WorldLocation=Location; S.InstigatorActor=GetOwner(); S.CategoryTag=Category;
    S.SourceAnomalyId=SourceId;
    S.Loudness=V->Loudness*Strength; S.Radius=V->EffectiveRadius*Strength; S.bPuzzleEligible=false; S.ClampSafeRanges();
    auto* Bus=GetWorld()->GetSubsystem<UTDFAcousticWorldSubsystem>(); if(!Bus || !Bus->EmitStimulus(S)) return false;
    // Playback copies these attenuation settings into the active sound.
    auto* Att=NewObject<USoundAttenuation>(this); Att->Attenuation.bSpatialize=true; Att->Attenuation.bAttenuate=true;
    Att->Attenuation.AttenuationShapeExtents=FVector(60); Att->Attenuation.FalloffDistance=FMath::Max(1.f,S.Radius-60);
    Att->Attenuation.bEnableOcclusion=true; Att->Attenuation.OcclusionVolumeAttenuation=.5f; Att->Attenuation.OcclusionLowPassFilterFrequency=1800;
    UGameplayStatics::PlaySoundAtLocation(this,V->Sound,Location,FRotator::ZeroRotator,S.Loudness,1.f,0.f,Att);
    return true;
}
bool UTDFPlayerNoiseEmitterComponent::EmitFootstep(FVector L,float S){return EmitCategory(TDFAcoustics::Tag(TEXT("Acoustic.Category.Footstep")),L,S);}
bool UTDFPlayerNoiseEmitterComponent::EmitImpact(FVector L,float S){return EmitCategory(TDFAcoustics::Tag(TEXT("Acoustic.Category.Impact")),L,S);}
bool UTDFPlayerNoiseEmitterComponent::EmitMachine(FVector L){return EmitCategory(TDFAcoustics::Tag(TEXT("Acoustic.Category.Machine")),L);}
bool UTDFPlayerNoiseEmitterComponent::EmitBell(FVector L){return EmitCategory(TDFAcoustics::Tag(TEXT("Acoustic.Category.Bell")),L);}
bool UTDFPlayerNoiseEmitterComponent::EmitWater(FVector L){return EmitCategory(TDFAcoustics::Tag(TEXT("Acoustic.Category.Water")),L);}
bool UTDFPlayerNoiseEmitterComponent::EmitStatic(FVector L){return EmitCategory(TDFAcoustics::Tag(TEXT("Acoustic.Category.Static")),L);}
bool UTDFPlayerNoiseEmitterComponent::EmitAuthoredVoice(FVector L,FPrimaryAssetId Id)
{
    auto* Asset=UTDFRecorderPlaybackComponent::Resolve(Id); if(!Asset || Asset->CategoryTag!=TDFAcoustics::Tag(TEXT("Acoustic.Category.Voice"))) return false;
    const auto* V=Asset->FindVariant(ETDFAcousticFidelity::Clean);
    return V && EmitVariant(Asset->CategoryTag,*V,L,1.f,Id);
}
void UTDFPlayerNoiseEmitterComponent::TickComponent(float Delta,ELevelTick TickType,FActorComponentTickFunction* TickFunction)
{
    Super::TickComponent(Delta,TickType,TickFunction);
    const auto* P=Cast<ATDFPlayerCharacter>(GetOwner()); if(!P || !Tuning) return;
    const FVector Location=P->GetActorLocation(); const float Travel=FVector::Dist2D(Location,PreviousLocation); PreviousLocation=Location;
    if(!P->GetCharacterMovement()->IsMovingOnGround() || Travel>300 || P->GetVelocity().Size2D()<5) {DistanceSinceStep=0; return;}
    DistanceSinceStep+=Travel;
    const auto& State=P->CurrentMovementState;
    const float Stride=P->bIsCrouched?Tuning->CrouchStride:State.bIsSprinting?Tuning->SprintStride:Tuning->WalkStride;
    if(DistanceSinceStep>=FMath::Max(50.f,Stride))
    {
        DistanceSinceStep=FMath::Fmod(DistanceSinceStep,FMath::Max(50.f,Stride));
        EmitFootstep(Location-FVector(0,0,85),P->bIsCrouched?Tuning->CrouchNoiseMultiplier:State.bIsSprinting?Tuning->SprintNoiseMultiplier:1.f);
    }
}

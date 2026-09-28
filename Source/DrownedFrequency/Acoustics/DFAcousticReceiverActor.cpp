#include "Acoustics/DFAcousticReceiverActor.h"
#include "Acoustics/DFAcousticReceiverComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "World/DFSwitchReceiverInterface.h"
#include "Player/DFPlayerCharacter.h"
#include "Acoustics/DFAcousticPuzzleLedgerComponent.h"
bool ATDFAcousticReceiverActor::CanInteract_Implementation(const ATDFPlayerCharacter* P) const
{
    if(!P || bIsSatisfied) return false;
    for(const auto& R:Receiver->Rules) if(R.bAllowLedgerFallback) for(const auto& Key:R.RequiredReceiverKeys) if(P->AcousticLedger->CanPuzzleUseFallback(Key)) return true;
    return false;
}
void ATDFAcousticReceiverActor::Interact_Implementation(ATDFPlayerCharacter* P)
{
    if(!CanInteract_Implementation(P)) return;
    for(int32 I=0;I<Receiver->Rules.Num();++I) if(Receiver->TryLedgerFallback(P->AcousticLedger,I)) break;
}
ATDFAcousticReceiverActor::ATDFAcousticReceiverActor()
{
    ReceiverMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ReceiverMesh")); SetRootComponent(ReceiverMesh);
    Receiver=CreateDefaultSubobject<UTDFAcousticReceiverComponent>(TEXT("Receiver"));
    StatusLight=CreateDefaultSubobject<UPointLightComponent>(TEXT("StatusLight")); StatusLight->SetupAttachment(ReceiverMesh);
    StatusLight->SetRelativeLocation(FVector(-30,0,20)); StatusLight->SetLightColor(FLinearColor(.12f,.55f,.32f)); StatusLight->SetIntensity(25); StatusLight->SetAttenuationRadius(100); StatusLight->SetCastShadows(false); StatusLight->SetVisibility(false);
}
void ATDFAcousticReceiverActor::BeginPlay()
{
    Super::BeginPlay(); Receiver->OnRuleSatisfied.AddDynamic(this,&ThisClass::HandleSatisfied); OnReceiverVisualChanged(bIsSatisfied);
}
void ATDFAcousticReceiverActor::HandleSatisfied(int32 RuleIndex)
{
    if(bIsSatisfied) return;
    bIsSatisfied=true; OnReceiverVisualChanged(true); CompleteAssignedObjective();
    if(IsValid(LinkedActor) && LinkedActor->Implements<UTDFSwitchReceiverInterface>()) ITDFSwitchReceiverInterface::Execute_ReceiveSwitchState(LinkedActor,this,true);
}

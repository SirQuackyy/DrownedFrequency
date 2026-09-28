#include "Acoustics/DFAcousticReceiverComponent.h"
#include "Acoustics/DFAcousticWorldSubsystem.h"
#include "Acoustics/DFAcousticPuzzleLedgerComponent.h"
#include "Engine/World.h"
void UTDFAcousticReceiverComponent::BeginPlay() {Super::BeginPlay(); if(auto* Bus=GetWorld()->GetSubsystem<UTDFAcousticWorldSubsystem>()) Bus->RegisterReceiver(this);}
void UTDFAcousticReceiverComponent::EndPlay(const EEndPlayReason::Type Reason) {if(auto* World=GetWorld()) if(auto* Bus=World->GetSubsystem<UTDFAcousticWorldSubsystem>()) Bus->UnregisterReceiver(this); Super::EndPlay(Reason);}
bool UTDFAcousticReceiverComponent::EvaluateStimulus(const FTDFAcousticStimulus& S)
{
    if(!S.IsValid() || !GetOwner() || S.Loudness<=0 || FVector::DistSquared(GetOwner()->GetActorLocation(),S.WorldLocation)>FMath::Square(S.Radius)) {LastEvaluation=TEXT("Invalid, silent or outside radius"); return false;}
    bool Matched=false;
    for(int32 I=0;I<Rules.Num();++I)
    {
        const auto R=Rules[I];
        if(R.bConsumeOnce && SatisfiedRules.Contains(I)) {LastEvaluation=TEXT("Already consumed"); continue;}
        if(!TDFAcoustics::IsCategory(R.RequiredCategory) || S.CategoryTag!=R.RequiredCategory) {LastEvaluation=TEXT("Category mismatch"); continue;}
        if((R.bRequirePuzzleEligible && !S.bPuzzleEligible) || !S.ReceiverKeys.HasAllExact(R.RequiredReceiverKeys)) {LastEvaluation=TEXT("Puzzle eligibility or keys mismatch"); continue;}
        if(!R.AcceptedRoutes.Contains(S.Route)) {LastEvaluation=TEXT("Route not accepted"); continue;}
        if(uint8(S.Fidelity)>uint8(R.MinimumFidelity) || !FMath::IsFinite(R.MaxContamination) || S.Contamination>R.MaxContamination || S.RepetitionIndex>R.MaxAcceptedRepetitionIndex) {LastEvaluation=TEXT("Fidelity, contamination or repetition rejected"); continue;}
        SatisfiedRules.Add(I); LastEvaluation=TEXT("Satisfied"); Matched=true; OnRuleSatisfied.Broadcast(I);
    }
    return Matched;
}
bool UTDFAcousticReceiverComponent::TryLedgerFallback(UTDFAcousticPuzzleLedgerComponent* Ledger,int32 I)
{
    if(!Ledger || !Rules.IsValidIndex(I)) return false;
    const auto& R=Rules[I];
    if(!R.bAllowLedgerFallback || R.RequiredReceiverKeys.IsEmpty() || (R.bConsumeOnce && SatisfiedRules.Contains(I))) return false;
    for(const auto Key:R.RequiredReceiverKeys) if(!Ledger->CanPuzzleUseFallback(Key)) return false;
    SatisfiedRules.Add(I); LastEvaluation=TEXT("Explicit configured ledger fallback"); OnRuleSatisfied.Broadcast(I); return true;
}

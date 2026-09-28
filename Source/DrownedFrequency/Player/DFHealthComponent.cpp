#include "Player/DFHealthComponent.h"
#include "Player/DFDifficultyProfile.h"
#include "Save/DFCheckpointSubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
float UTDFHealthComponent::GetRespawnHealth() const
{
    auto* Profile=Cast<UTDFDifficultyProfile>(UAssetManager::Get().GetPrimaryAssetPath(FPrimaryAssetId(TEXT("TDFDifficulty"),DifficultyId)).TryLoad());
    return Profile && FMath::IsFinite(Profile->CheckpointRespawnHealth)?FMath::Clamp(Profile->CheckpointRespawnHealth,1.f,100.f):100.f;
}
void UTDFHealthComponent::ApplyDamage(float Amount)
{
    if(IsDead() || !FMath::IsFinite(Amount) || Amount<=0) return; Health=FMath::Max(0.f,Health-Amount);
    if(IsDead()) {OnDeath.Broadcast(); if(auto* W=GetWorld()) if(auto* GI=W->GetGameInstance()) GI->GetSubsystem<UTDFCheckpointSubsystem>()->HandlePlayerDeath();}
}

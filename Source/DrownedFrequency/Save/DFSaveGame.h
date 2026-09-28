#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Acoustics/DFAcousticSaveTypes.h"
#include "DFSaveGame.generated.h"
// Version 2 adds recoverable placed pucks. Version 1 fixtures migrate without changing existing progress.
namespace TDFSave {inline constexpr int32 CurrentVersion=2;}
UENUM(BlueprintType)
enum class ETDFSaveReason:uint8 {Manual,Checkpoint,ChapterStart,Debug};
UENUM(BlueprintType)
enum class ETDFObjectiveState:uint8 {Locked,Active,Completed,Failed,Hidden};
USTRUCT(BlueprintType)
struct DROWNEDFREQUENCY_API FTDFPuckSaveData
{
    GENERATED_BODY()
    UPROPERTY(SaveGame) FGuid InstanceId;
    UPROPERTY(SaveGame) FTransform Transform=FTransform::Identity;
};
USTRUCT(BlueprintType)
struct DROWNEDFREQUENCY_API FTDFPlayerSaveData
{
    GENERATED_BODY()
    UPROPERTY(SaveGame,BlueprintReadOnly) FVector Location=FVector::ZeroVector;
    UPROPERTY(SaveGame,BlueprintReadOnly) FRotator Rotation=FRotator::ZeroRotator;
    UPROPERTY(SaveGame,BlueprintReadOnly) float Health=100.f;
    UPROPERTY(SaveGame) TArray<FName> ItemIds;
    UPROPERTY(SaveGame) TArray<FName> KeyIds;
    UPROPERTY(SaveGame) FName DifficultyId=TEXT("Standard");
    UPROPERTY(SaveGame) FTDFAcousticSaveData Acoustics;
    UPROPERTY(SaveGame) TArray<FTDFPuckSaveData> PlacedPucks;
    FTDFPlayerSaveData() {Acoustics.SlotSignals.SetNum(5);}
};
USTRUCT(BlueprintType)
struct DROWNEDFREQUENCY_API FTDFActorSaveData
{
    GENERATED_BODY()
    UPROPERTY(SaveGame,BlueprintReadOnly) FGuid StableActorId;
    UPROPERTY(SaveGame) FTransform Transform=FTransform::Identity;
    UPROPERTY(SaveGame) FName StateName;
    UPROPERTY(SaveGame) FName SectorId;
    UPROPERTY(SaveGame) TMap<FName,FString> SerializedFields;
};
USTRUCT(BlueprintType)
struct DROWNEDFREQUENCY_API FTDFObjectiveSaveData
{
    GENERATED_BODY()
    UPROPERTY(SaveGame) FName ObjectiveId;
    UPROPERTY(SaveGame) ETDFObjectiveState State=ETDFObjectiveState::Locked;
    UPROPERTY(SaveGame) FGuid TargetActorId;
};
USTRUCT(BlueprintType)
struct DROWNEDFREQUENCY_API FTDFSettingsSaveData
{
    GENERATED_BODY()
    // Progress metadata only. User input/audio/accessibility settings own separate slots.
    UPROPERTY(SaveGame) FName DifficultyId=TEXT("Standard");
};
UCLASS()
class DROWNEDFREQUENCY_API UTDFSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(SaveGame,BlueprintReadOnly) int32 SaveVersion=TDFSave::CurrentVersion;
    UPROPERTY(SaveGame,BlueprintReadOnly) FString BuildVersion=TEXT("Phase6");
    UPROPERTY(SaveGame,BlueprintReadOnly) FDateTime SavedAtUtc;
    UPROPERTY(SaveGame,BlueprintReadOnly) FName CurrentChapterId;
    UPROPERTY(SaveGame,BlueprintReadOnly) FName CurrentCheckpointId;
    UPROPERTY(SaveGame) FTransform CheckpointRespawnTransform=FTransform::Identity;
    UPROPERTY(SaveGame) TArray<FName> CheckpointRequiredSectors;
    UPROPERTY(SaveGame) bool bManualSaveAllowed=true;
    UPROPERTY(SaveGame,BlueprintReadOnly) FName CurrentPersistentMap;
    UPROPERTY(SaveGame,BlueprintReadOnly) TArray<FName> LoadedSectorIds;
    UPROPERTY(SaveGame) ETDFSaveReason Reason=ETDFSaveReason::Manual;
    UPROPERTY(SaveGame) FTDFPlayerSaveData Player;
    UPROPERTY(SaveGame) TArray<FTDFActorSaveData> Actors;
    UPROPERTY(SaveGame) TArray<FTDFObjectiveSaveData> Objectives;
    UPROPERTY(SaveGame) FTDFSettingsSaveData Settings;
};

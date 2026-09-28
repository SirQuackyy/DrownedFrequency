#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DFAcousticTypes.generated.h"

UENUM(BlueprintType)
enum class ETDFAcousticFidelity : uint8 { Clean, Degraded, Contaminated };
UENUM(BlueprintType)
enum class ETDFRecordingTier : uint8 { Invalid=0 UMETA(Hidden), Tier3=3, Tier4=4, Tier5=5 };
UENUM(BlueprintType)
enum class ETDFAcousticRoute : uint8 { Direct, Pipe, Speaker, PuckProjected, Overdub };

USTRUCT(BlueprintType)
struct DROWNEDFREQUENCY_API FTDFAcousticCaptionLine
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText Text;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float StartTime=0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float Duration=2.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag SpeakerOrSourceTag;
};
USTRUCT(BlueprintType)
struct DROWNEDFREQUENCY_API FTDFAcousticClipVariant
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<class USoundBase> Sound=nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) ETDFAcousticFidelity Fidelity=ETDFAcousticFidelity::Clean;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0",ClampMax="4")) float Loudness=1.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1",ClampMax="100000")) float EffectiveRadius=1200.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FTDFAcousticCaptionLine> Captions;
    bool IsValid() const;
};

namespace TDFAcoustics
{
    DROWNEDFREQUENCY_API FGameplayTag Tag(const TCHAR* Name);
    DROWNEDFREQUENCY_API const TArray<FName>& CategoryNames();
    DROWNEDFREQUENCY_API bool IsCategory(FGameplayTag Category);
}

#include "Acoustics/DFAcousticTypes.h"
#include "Sound/SoundBase.h"

bool FTDFAcousticClipVariant::IsValid() const
{
    if (!Sound || !FMath::IsFinite(Loudness) || Loudness<0 || Loudness>4 || !FMath::IsFinite(EffectiveRadius) || EffectiveRadius<=0 || EffectiveRadius>100000 || uint8(Fidelity)>2) return false;
    for (const auto& C:Captions) if (!FMath::IsFinite(C.StartTime) || C.StartTime<0 || !FMath::IsFinite(C.Duration) || C.Duration<=0) return false;
    return true;
}

FGameplayTag TDFAcoustics::Tag(const TCHAR* Name) { return FGameplayTag::RequestGameplayTag(FName(Name), false); }
const TArray<FName>& TDFAcoustics::CategoryNames()
{
    static const TArray<FName> Names={TEXT("Acoustic.Category.Voice"),TEXT("Acoustic.Category.Footstep"),TEXT("Acoustic.Category.Machine"),TEXT("Acoustic.Category.Bell"),TEXT("Acoustic.Category.Water"),TEXT("Acoustic.Category.Static"),TEXT("Acoustic.Category.Impact")};
    return Names;
}
bool TDFAcoustics::IsCategory(FGameplayTag Category) { return Category.IsValid() && CategoryNames().Contains(Category.GetTagName()); }

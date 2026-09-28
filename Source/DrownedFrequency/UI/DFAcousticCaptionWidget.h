#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DFAcousticCaptionWidget.generated.h"
UCLASS(Blueprintable)
class DROWNEDFREQUENCY_API UTDFAcousticCaptionWidget : public UUserWidget
{
    GENERATED_BODY()
protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    UFUNCTION() void Refresh();
    TWeakObjectPtr<class UTDFAcousticCaptionSubsystem> Captions;
};

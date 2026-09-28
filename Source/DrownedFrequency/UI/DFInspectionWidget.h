#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DFInspectionWidget.generated.h"
UCLASS(BlueprintType, Blueprintable)
class DROWNEDFREQUENCY_API UTDFInspectionWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="Inspection") void ShowItem(class UTDFItemDataAsset* Item);
    UFUNCTION(BlueprintCallable, Category="Inspection") void UpdateControls(const FText& Text);
    UFUNCTION(BlueprintCallable, Category="Inspection") void RotatePreview(FVector2D Delta);
    UPROPERTY(BlueprintReadOnly, Category="Inspection") FRotator PreviewRotation = FRotator(-15.f,25.f,0.f);
    UPROPERTY(Transient) TObjectPtr<class ATDFInspectionPreviewActor> PreviewActor;
    UPROPERTY(BlueprintReadOnly, Category="Inspection") TObjectPtr<UTDFItemDataAsset> DisplayedItem;
    UFUNCTION(BlueprintImplementableEvent, Category="Inspection") void OnItemChanged(UTDFItemDataAsset* Item);
protected:
    virtual void NativeDestruct() override;
};

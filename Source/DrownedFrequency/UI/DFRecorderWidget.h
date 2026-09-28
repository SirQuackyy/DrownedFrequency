#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Input/DFInputGlyphTypes.h"
#include "DFRecorderWidget.generated.h"
UCLASS(Abstract,Blueprintable)
class DROWNEDFREQUENCY_API UTDFRecorderWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    void BindPlayer(class ATDFPlayerCharacter* Player);
    UFUNCTION(BlueprintCallable) void Refresh();
protected:
    virtual void NativeDestruct() override;
private:
    UFUNCTION() void DeviceChanged(ETDFInputDevice Device) {Refresh();}
    void Unbind();
    TWeakObjectPtr<class ATDFPlayerCharacter> BoundPlayer;
};

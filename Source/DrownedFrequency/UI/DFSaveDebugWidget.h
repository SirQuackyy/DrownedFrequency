#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Save/DFSaveGame.h"
#include "DFSaveDebugWidget.generated.h"
UCLASS(Blueprintable)
class DROWNEDFREQUENCY_API UTDFSaveDebugWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable) void SaveSlot();
    UFUNCTION(BlueprintCallable) void LoadSlot();
    UFUNCTION(BlueprintCallable) void ToggleControls();
    void CloseControls();
protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FReply NativeOnKeyDown(const FGeometry& Geometry,const FKeyEvent& Event) override;
private:
    UFUNCTION() void DeleteSlot();
    UFUNCTION() void Intro();
    UFUNCTION() void Radio();
    UFUNCTION() void Die();
    UFUNCTION() void Saved(FName SlotName);
    UFUNCTION() void Failed(FName SlotName,FText Reason);
    UFUNCTION() void ObjectiveChanged(FName Id,ETDFObjectiveState State);
    void Refresh();
    bool bControlsOpen=false;
    TWeakObjectPtr<class UTDFSaveSubsystem> Saves;
    TWeakObjectPtr<class UTDFObjectiveSubsystem> Objectives;
};

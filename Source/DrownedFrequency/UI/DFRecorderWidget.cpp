#include "UI/DFRecorderWidget.h"
#include "Acoustics/DFAcousticBlueprintLibrary.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFPlayerController.h"
#include "Input/DFInputConfig.h"
#include "Acoustics/DFRecorderComponent.h"
#include "Acoustics/DFRecorderOperatorComponent.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFAudioAnomalyDataAsset.h"
#include "Components/TextBlock.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
void UTDFRecorderWidget::Unbind()
{
    if(auto* P=BoundPlayer.Get()) {P->Recorder->OnSlotsChanged.RemoveDynamic(this,&ThisClass::Refresh); P->RecorderOperator->OnFeedback.RemoveDynamic(this,&ThisClass::Refresh);}
    if(auto* PC=Cast<ATDFPlayerController>(GetOwningPlayer()))
    {
        PC->OnInputDeviceChanged.RemoveDynamic(this,&ThisClass::DeviceChanged);
        if(auto* Local=PC->GetLocalPlayer()) if(auto* Input=Local->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) Input->ControlMappingsRebuiltDelegate.RemoveDynamic(this,&ThisClass::Refresh);
    }
    BoundPlayer.Reset();
}
void UTDFRecorderWidget::BindPlayer(ATDFPlayerCharacter* P)
{
    Unbind(); BoundPlayer=P;
    if(P) {P->Recorder->OnSlotsChanged.AddDynamic(this,&ThisClass::Refresh); P->RecorderOperator->OnFeedback.AddDynamic(this,&ThisClass::Refresh);}
    if(auto* PC=Cast<ATDFPlayerController>(GetOwningPlayer()))
    {
        PC->OnInputDeviceChanged.AddDynamic(this,&ThisClass::DeviceChanged);
        if(auto* Local=PC->GetLocalPlayer()) if(auto* Input=Local->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) Input->ControlMappingsRebuiltDelegate.AddDynamic(this,&ThisClass::Refresh);
    }
    Refresh();
}
void UTDFRecorderWidget::NativeDestruct() {Unbind(); Super::NativeDestruct();}
void UTDFRecorderWidget::Refresh()
{
    auto* P=BoundPlayer.Get(); auto* PC=Cast<ATDFPlayerController>(GetOwningPlayer()); if(!P || !PC) return;
    auto Set=[this](const TCHAR* Name,const FString& Value){if(auto* T=Cast<UTextBlock>(GetWidgetFromName(Name))) T->SetText(FText::FromString(Value));};
    FString Slots;
    for(const auto& Entry:P->Recorder->GetSlots().Slots)
    {
        if(!Entry.bUnlocked) continue;
        const auto* Asset=Entry.Signal.IsValid()?UTDFRecorderPlaybackComponent::Resolve(Entry.Signal.SourceAnomalyId):nullptr;
        Slots+=FString::Printf(TEXT("%s %d  %s\n"),P->Recorder->SelectedSlot==Entry.SlotIndex?TEXT(">"):TEXT(" "),Entry.SlotIndex+1,*UTDFAcousticBlueprintLibrary::GetSignalDisplayName(Entry.Signal).ToString());
    }
    Set(TEXT("RecorderSlots"),Slots.TrimEnd());
    Set(TEXT("RecorderTitle"),FString::Printf(TEXT("FIELD RECORDER     /     PUCKS %d / 3"),P->RecorderOperator->GetPucksAvailable()));
    Set(TEXT("RecorderStatus"),P->RecorderOperator->StatusText.IsEmpty()?TEXT("Face a source and hold record."):P->RecorderOperator->StatusText.ToString());
    if(const auto* C=PC->InputConfig.Get())
    {
        const auto Label=[PC](const UInputAction* A){return PC->GetActionInputLabel(A).ToString();};
        Set(TEXT("RecorderControls"),FString::Printf(TEXT("Hold %s: record   %s: play   %s: puck\n%s / %s: select   %s: erase"),*Label(C->RecordAction),*Label(C->PlaybackAction),*Label(C->ProjectAction),*Label(C->PreviousClipAction),*Label(C->NextClipAction),*Label(C->DeleteClipAction)));
    }
}

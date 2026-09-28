#include "UI/DFAcousticCaptionWidget.h"
#include "Acoustics/DFAcousticCaptionSubsystem.h"
#include "Acoustics/DFAcousticBlueprintLibrary.h"
#include "Engine/GameInstance.h"
#include "Components/TextBlock.h"
void UTDFAcousticCaptionWidget::NativeConstruct()
{
    Super::NativeConstruct(); if(auto* GI=GetGameInstance()) Captions=GI->GetSubsystem<UTDFAcousticCaptionSubsystem>();
    if(Captions.IsValid()) Captions->OnCaptionsChanged.AddDynamic(this,&ThisClass::Refresh); Refresh();
}
void UTDFAcousticCaptionWidget::NativeDestruct()
{
    if(Captions.IsValid()) Captions->OnCaptionsChanged.RemoveDynamic(this,&ThisClass::Refresh); Captions.Reset(); Super::NativeDestruct();
}
void UTDFAcousticCaptionWidget::Refresh()
{
    FString Lines;
    if(Captions.IsValid()) for(const auto& C:Captions->GetActiveCaptions())
    {
        const auto Route=UTDFAcousticBlueprintLibrary::GetRouteDisplayName(C.Route).ToString();
        if(!Lines.IsEmpty()) Lines+=TEXT("\n");
        Lines+=(C.bLocalPreview?TEXT("[Recorder] "):Route.IsEmpty()?FString():TEXT("[")+Route+TEXT("] "))+C.Text.ToString();
    }
    if(auto* T=Cast<UTextBlock>(GetWidgetFromName(TEXT("CaptionText")))) T->SetText(FText::FromString(Lines));
    if(auto* B=GetWidgetFromName(TEXT("CaptionBackdrop"))) B->SetVisibility(Lines.IsEmpty()?ESlateVisibility::Collapsed:ESlateVisibility::HitTestInvisible);
}

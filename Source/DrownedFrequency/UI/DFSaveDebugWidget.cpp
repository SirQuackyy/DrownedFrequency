#include "UI/DFSaveDebugWidget.h"
#include "Save/DFSaveSubsystem.h"
#include "World/DFObjectiveSubsystem.h"
#include "World/DFLevelStreamingSubsystem.h"
#include "Player/DFPlayerController.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFHealthComponent.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Engine/GameInstance.h"
#include "HAL/FileManager.h"
void UTDFSaveDebugWidget::NativeConstruct()
{
    Super::NativeConstruct(); SetIsFocusable(true); Saves=GetGameInstance()->GetSubsystem<UTDFSaveSubsystem>(); Objectives=GetGameInstance()->GetSubsystem<UTDFObjectiveSubsystem>();
    if(Saves.IsValid()) {Saves->OnSaveFinished.AddDynamic(this,&ThisClass::Saved); Saves->OnSaveFailed.AddDynamic(this,&ThisClass::Failed); Saves->OnLoadFinished.AddDynamic(this,&ThisClass::Saved); Saves->OnLoadFailed.AddDynamic(this,&ThisClass::Failed);}
    if(Objectives.IsValid()) Objectives->OnObjectiveChanged.AddDynamic(this,&ThisClass::ObjectiveChanged);
    if(auto* B=Cast<UButton>(GetWidgetFromName(TEXT("SaveButton")))) B->OnClicked.AddDynamic(this,&ThisClass::SaveSlot);
    if(auto* B=Cast<UButton>(GetWidgetFromName(TEXT("LoadButton")))) B->OnClicked.AddDynamic(this,&ThisClass::LoadSlot);
    if(auto* B=Cast<UButton>(GetWidgetFromName(TEXT("DeleteButton")))) B->OnClicked.AddDynamic(this,&ThisClass::DeleteSlot);
    if(auto* B=Cast<UButton>(GetWidgetFromName(TEXT("IntroButton")))) B->OnClicked.AddDynamic(this,&ThisClass::Intro);
    if(auto* B=Cast<UButton>(GetWidgetFromName(TEXT("RadioButton")))) B->OnClicked.AddDynamic(this,&ThisClass::Radio);
    if(auto* B=Cast<UButton>(GetWidgetFromName(TEXT("DeathButton")))) B->OnClicked.AddDynamic(this,&ThisClass::Die);
    if(auto* Panel=GetWidgetFromName(TEXT("SaveControls"))) Panel->SetVisibility(ESlateVisibility::Collapsed); Refresh();
}
void UTDFSaveDebugWidget::NativeDestruct()
{
    CloseControls(); if(Saves.IsValid()) {Saves->OnSaveFinished.RemoveDynamic(this,&ThisClass::Saved); Saves->OnSaveFailed.RemoveDynamic(this,&ThisClass::Failed); Saves->OnLoadFinished.RemoveDynamic(this,&ThisClass::Saved); Saves->OnLoadFailed.RemoveDynamic(this,&ThisClass::Failed);}
    if(Objectives.IsValid()) Objectives->OnObjectiveChanged.RemoveDynamic(this,&ThisClass::ObjectiveChanged); Super::NativeDestruct();
}
void UTDFSaveDebugWidget::Refresh()
{
    if(auto* T=Cast<UTextBlock>(GetWidgetFromName(TEXT("ObjectiveText")))) T->SetText(Objectives.IsValid()?Objectives->GetActiveObjectiveText():FText::GetEmpty());
    if(auto* T=Cast<UTextBlock>(GetWidgetFromName(TEXT("SaveStatus")))) T->SetText(Saves.IsValid()?Saves->LastMessage:FText::GetEmpty());
}
void UTDFSaveDebugWidget::Saved(FName) {Refresh();}
void UTDFSaveDebugWidget::Failed(FName,FText Reason) {if(auto* T=Cast<UTextBlock>(GetWidgetFromName(TEXT("SaveStatus")))) T->SetText(Reason);}
void UTDFSaveDebugWidget::ObjectiveChanged(FName,ETDFObjectiveState) {Refresh();}
void UTDFSaveDebugWidget::SaveSlot() {CloseControls(); if(Saves.IsValid()) Saves->WriteSave(TEXT("Manual0"),ETDFSaveReason::Manual);}
void UTDFSaveDebugWidget::LoadSlot() {CloseControls(); if(Saves.IsValid()) Saves->RequestLoadSlot(TEXT("Manual0"));}
void UTDFSaveDebugWidget::DeleteSlot() {CloseControls(); if(IFileManager::Get().Delete(*UTDFSaveSubsystem::SlotPath(TEXT("Manual0")))) Failed(NAME_None,NSLOCTEXT("Save","DeletedManual","Manual slot 0 deleted."));}
void UTDFSaveDebugWidget::Intro() {CloseControls(); GetGameInstance()->GetSubsystem<UTDFLevelStreamingSubsystem>()->RequestSectorSet({TEXT("Intro")},TEXT("DebugUI"));}
void UTDFSaveDebugWidget::Radio() {CloseControls(); GetGameInstance()->GetSubsystem<UTDFLevelStreamingSubsystem>()->RequestSectorSet({TEXT("RadioRoom")},TEXT("DebugUI"));}
void UTDFSaveDebugWidget::Die() {CloseControls(); if(auto* PC=GetOwningPlayer()) if(auto* P=Cast<ATDFPlayerCharacter>(PC->GetPawn())) P->Health->ApplyDamage(200);}
void UTDFSaveDebugWidget::CloseControls() {if(bControlsOpen) ToggleControls();}
void UTDFSaveDebugWidget::ToggleControls()
{
    auto* PC=Cast<ATDFPlayerController>(GetOwningPlayer()); if(!PC || (!bControlsOpen && PC->IsWorldTransitionLocked())) return;
    bControlsOpen=!bControlsOpen; PC->SetIgnoreMoveInput(bControlsOpen); PC->SetIgnoreLookInput(bControlsOpen); PC->bShowMouseCursor=bControlsOpen;
    if(auto* Panel=GetWidgetFromName(TEXT("SaveControls"))) Panel->SetVisibility(bControlsOpen?ESlateVisibility::Visible:ESlateVisibility::Collapsed);
    if(bControlsOpen) {FInputModeGameAndUI Mode; Mode.SetWidgetToFocus(TakeWidget()); Mode.SetHideCursorDuringCapture(false); PC->SetInputMode(Mode); SetKeyboardFocus();} else PC->SetInputMode(FInputModeGameOnly());
}
FReply UTDFSaveDebugWidget::NativeOnKeyDown(const FGeometry& G,const FKeyEvent& E) {if(E.GetKey()==EKeys::F10) {ToggleControls(); return FReply::Handled();} return Super::NativeOnKeyDown(G,E);}

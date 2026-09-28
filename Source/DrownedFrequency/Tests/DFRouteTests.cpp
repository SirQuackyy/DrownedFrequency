#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#if WITH_EDITOR
#include "Tests/AutomationEditorCommon.h"
#include "Editor.h"
#endif
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "InputKeyEventArgs.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFPlayerController.h"
#include "Player/DFPlayerCameraComponent.h"
#include "Inventory/DFInventoryComponent.h"
#include "Inventory/DFItemDataAsset.h"
#include "World/DFDoorActor.h"
#include "World/DFSwitchActor.h"
#include "World/DFWorldItemActor.h"
#include "UI/DFInspectionWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace
{
UWorld* RouteWorld()
{
#if WITH_EDITOR
    if (GEditor && GEditor->PlayWorld) return GEditor->PlayWorld;
#endif
    for (const auto& C:GEngine->GetWorldContexts()) if (C.WorldType==EWorldType::Game) return C.World();
    return nullptr;
}
enum class ERouteOp { Walk,Aim,Interact,Crouch,Stand,Locked,Key,Inspect,Rotate,Back,Unlocked,Switched,Wait,Finish };
struct FRouteStep { ERouteOp Op; FVector Point=FVector::ZeroVector; float Duration=.3f; };
class FRouteFlow : public IAutomationLatentCommand
{
public:
    FRouteFlow(FAutomationTestBase* InTest,bool bController):Test(InTest),bGamepad(bController)
    {
        auto Walk=[this](FVector V){Steps.Add({ERouteOp::Walk,V});};
        auto Aim=[this](FVector V){Steps.Add({ERouteOp::Aim,V});};
        auto Op=[this](ERouteOp O,float D=.3f){Steps.Add({O,FVector::ZeroVector,D});};
        Walk({-900,150,0}); Walk({-200,150,0}); Walk({120,0,0}); Aim({300,0,150}); Op(ERouteOp::Interact); Op(ERouteOp::Locked);
        Walk({-200,150,0}); Walk({-535,300,0}); Op(ERouteOp::Crouch,.5f); Walk({-535,620,0}); Op(ERouteOp::Stand,.5f);
        Walk({-310,765,0}); Aim({-170,765,124}); Op(ERouteOp::Interact); Op(ERouteOp::Key);
        Walk({-535,620,0}); Op(ERouteOp::Crouch,.5f); Walk({-535,280,0}); Op(ERouteOp::Stand,.5f);
        Walk({-1000,200,0}); Walk({-1000,-450,0}); Walk({-550,-490,0}); Aim({-550,-650,132}); Op(ERouteOp::Interact); Op(ERouteOp::Inspect);
        Op(ERouteOp::Rotate,.6f); Op(ERouteOp::Back);
        Walk({-1000,-400,0}); Walk({-1000,150,0}); Walk({-200,150,0}); Walk({120,0,0}); Aim({300,0,150}); Op(ERouteOp::Interact);
        Op(ERouteOp::Wait,1.4f); Op(ERouteOp::Unlocked); Walk({500,0,0}); Walk({530,-410,0}); Aim({690,-410,145}); Op(ERouteOp::Interact);
        Op(ERouteOp::Wait,1.4f); Op(ERouteOp::Switched); Walk({650,0,0}); Walk({1110,0,0}); Op(ERouteOp::Finish);
    }
    bool Update() override
    {
        UWorld* W=RouteWorld();
        PC=W ? Cast<ATDFPlayerController>(W->GetFirstPlayerController()) : nullptr;
        P=PC ? Cast<ATDFPlayerCharacter>(PC->GetPawn()) : nullptr;
        if (!P) { Test->AddError(TEXT("Route requires a possessed player")); return true; }
        if (!bInitialized)
        {
            for (TActorIterator<AActor> It(W);It;++It)
            {
                if (It->ActorHasTag(TEXT("Interaction_LockedDoor"))) Door=Cast<ATDFDoorActor>(*It);
                if (It->ActorHasTag(TEXT("Interaction_SwitchDoor"))) Exit=Cast<ATDFDoorActor>(*It);
                if (It->ActorHasTag(TEXT("Interaction_ExitSwitch"))) Switch=Cast<ATDFSwitchActor>(*It);
                if (It->ActorHasTag(TEXT("Interaction_Relay"))) Relay=Cast<ATDFWorldItemActor>(*It);
            }
            if (!Door || !Exit || !Switch || !Relay) { Test->AddError(TEXT("Authored interaction route is incomplete")); return true; }
            PC->ResetBindingsToDefault(false);
            PC->FlushPressedKeys();
            bInitialized=true;
        }
        const auto& S=Steps[Stage];
        const bool bFirst=Started<0.;
        if (bFirst) Started=FPlatformTime::Seconds();
        const double Elapsed=FPlatformTime::Seconds()-Started;
        Axis(EKeys::Gamepad_RightX,0); Axis(EKeys::Gamepad_RightY,0); Axis(EKeys::Gamepad_LeftY,0);
        if (S.Op==ERouteOp::Walk)
        {
            const FVector Delta=S.Point-P->GetActorLocation();
            if (Delta.Size2D()<22.) { StopWalking(); Next(); return false; }
            if (Elapsed>12.) { StopWalking(); Test->AddError(FString::Printf(TEXT("Route blocked at step %d: pawn %s, goal %s"),Stage,*P->GetActorLocation().ToString(),*S.Point.ToString())); return true; }
            PC->SetControlRotation(FRotator(0,Delta.Rotation().Yaw,0));
            if (bGamepad) Axis(EKeys::Gamepad_LeftY,1.f); else if (bFirst) Key(EKeys::W,true);
            return false;
        }
        if (bFirst)
        {
            switch (S.Op)
            {
            case ERouteOp::Aim: PC->SetControlRotation((S.Point-P->PlayerCamera->GetComponentLocation()).Rotation()); break;
            case ERouteOp::Interact: Key(InteractKey(),true); break;
            case ERouteOp::Crouch: Key(bGamepad ? EKeys::Gamepad_FaceButton_Right : EKeys::LeftControl,true); break;
            case ERouteOp::Stand: Key(bGamepad ? EKeys::Gamepad_FaceButton_Right : EKeys::LeftControl,false); break;
            case ERouteOp::Locked: Test->TestTrue(TEXT("Route refuses locked door before key"),Door->bIsLocked && !Door->bIsOpen); break;
            case ERouteOp::Key: Test->TestTrue(TEXT("Crouch route key collected"),P->Inventory->HasKey(TEXT("BasementKey"))); break;
            case ERouteOp::Inspect:
                Test->TestTrue(TEXT("Mapped interact opens inspection"),PC->IsInspectingItem() && PC->InspectionWidget!=nullptr);
                if (PC->InspectionWidget)
                {
                    auto* Title=Cast<UTextBlock>(PC->InspectionWidget->GetWidgetFromName(TEXT("ItemTitle")));
                    Test->TestTrue(TEXT("Blueprint inspection title displays item data"),Title && Title->GetText().EqualTo(Relay->ItemData->DisplayName));
                }
                break;
            case ERouteOp::Rotate:
                Position=P->GetActorLocation(); CameraRotation=PC->GetControlRotation(); WorldItemTransform=Relay->GetActorTransform();
                PreviewYaw=PC->InspectionWidget ? PC->InspectionWidget->PreviewRotation.Yaw : 0.;
                if (!bGamepad) Key(EKeys::W,true);
                Key(InteractKey(),true); break;
            case ERouteOp::Back: Key(bGamepad ? EKeys::Gamepad_Special_Left : EKeys::Tab,true); break;
            case ERouteOp::Unlocked: Test->TestTrue(TEXT("Key unlocks bulkhead with mapped interact"),!Door->bIsLocked && Door->bIsOpen && !Door->bIsAnimating); break;
            case ERouteOp::Switched: Test->TestTrue(TEXT("Mapped switch opens linked exit"),Switch->bIsOn && Exit->bIsOpen); break;
            case ERouteOp::Finish: PC->LoadInputSettings(); Test->AddInfo(bGamepad ? TEXT("Controller route traversed end-to-end.") : TEXT("Keyboard route traversed end-to-end.")); return true;
            default: break;
            }
        }
        if (S.Op==ERouteOp::Rotate)
        {
            if (bGamepad) { Axis(EKeys::Gamepad_RightX,.7f); Axis(EKeys::Gamepad_LeftY,1.f); }
            else Axis(EKeys::MouseX,2.f);
        }
        if (Elapsed<S.Duration) return false;
        if (S.Op==ERouteOp::Interact) Key(InteractKey(),false);
        if (S.Op==ERouteOp::Rotate)
        {
            Key(InteractKey(),false); Key(EKeys::W,false);
            Axis(EKeys::Gamepad_RightX,0); Axis(EKeys::Gamepad_LeftY,0);
            Test->TestTrue(TEXT("Inspection rotates preview"),PC->InspectionWidget && FMath::Abs(PC->InspectionWidget->PreviewRotation.Yaw-PreviewYaw)>3.f);
            Test->TestTrue(TEXT("Inspection blocks mapped movement"),FVector::Dist(P->GetActorLocation(),Position)<2.f);
            Test->TestTrue(TEXT("Inspection leaves camera unchanged"),PC->GetControlRotation().Equals(CameraRotation,.1f));
            Test->TestTrue(TEXT("Inspection leaves world item unchanged"),Relay->GetActorTransform().Equals(WorldItemTransform));
        }
        if (S.Op==ERouteOp::Back)
        {
            Key(bGamepad ? EKeys::Gamepad_Special_Left : EKeys::Tab,false);
            Test->TestFalse(TEXT("Mapped back closes inspection"),PC->IsInspectingItem());
            Test->TestTrue(TEXT("Input restored after inspection"),!PC->IsMoveInputIgnored() && !PC->IsLookInputIgnored());
        }
        Next(); return false;
    }
private:
    void Key(FKey K,bool bDown) { PC->InputKey(FInputKeyEventArgs::CreateSimulated(K,bDown ? IE_Pressed : IE_Released,bDown ? 1.f : 0.f)); }
    void Axis(FKey K,float V) { PC->InputKey(FInputKeyEventArgs::CreateSimulated(K,IE_Axis,V)); }
    FKey InteractKey() const { return bGamepad ? EKeys::Gamepad_FaceButton_Left : EKeys::LeftMouseButton; }
    void StopWalking() { Key(EKeys::W,false); Axis(EKeys::Gamepad_LeftY,0); P->GetCharacterMovement()->StopMovementImmediately(); }
    void Next() { Started=-1.; ++Stage; }
    FAutomationTestBase* Test; bool bGamepad=false,bInitialized=false; int32 Stage=0; double Started=-1.;
    TArray<FRouteStep> Steps;
    ATDFPlayerController* PC=nullptr; ATDFPlayerCharacter* P=nullptr;
    ATDFDoorActor* Door=nullptr; ATDFDoorActor* Exit=nullptr; ATDFSwitchActor* Switch=nullptr; ATDFWorldItemActor* Relay=nullptr;
    FVector Position; FRotator CameraRotation; FTransform WorldItemTransform; double PreviewYaw=0.;
};
}
#if WITH_EDITOR
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFRouteKeyboard,"TDF.World.PIE.RouteKeyboard",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FTDFRouteKeyboard::RunTest(const FString& Parameters)
{
    AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_PlayerInteraction"),true);
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f));
    ADD_LATENT_AUTOMATION_COMMAND(FRouteFlow(this,false));
    ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand()); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFRouteController,"TDF.World.PIE.RouteController",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FTDFRouteController::RunTest(const FString& Parameters)
{
    AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_PlayerInteraction"),true);
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f));
    ADD_LATENT_AUTOMATION_COMMAND(FRouteFlow(this,true));
    ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand()); return true;
}
#endif
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFRouteStandaloneKeyboard,"TDF.World.Standalone.RouteKeyboard",EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFRouteStandaloneKeyboard::RunTest(const FString& Parameters)
{
    AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_PlayerInteraction"),true);
    ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(2.f));
    ADD_LATENT_AUTOMATION_COMMAND(FRouteFlow(this,false)); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFRouteStandaloneController,"TDF.World.Standalone.RouteController",EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFRouteStandaloneController::RunTest(const FString& Parameters)
{
    AutomationOpenMap(TEXT("/Game/Game/Maps/Test/M_Test_PlayerInteraction"),true);
    ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(2.f));
    ADD_LATENT_AUTOMATION_COMMAND(FRouteFlow(this,true)); return true;
}
#endif

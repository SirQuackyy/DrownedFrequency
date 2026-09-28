#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "World/DFDoorActor.h"
#include "UObject/Script.h"
#include "Components/SceneComponent.h"
#include "World/DFSwitchActor.h"
#include "Inventory/DFItemDataAsset.h"
#include "Inventory/DFInventoryComponent.h"
#include "World/DFWorldItemActor.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFPlayerController.h"
#include "UI/DFInspectionWidget.h"
#include "Save/DFSaveSubsystem.h"
#include "Engine/GameInstance.h"
#include "Input/DFPlayerInputSettingsSave.h"
#include "Kismet/GameplayStatics.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFDoorRulesTest, "TDF.World.DoorRules", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTDFDoorRulesTest::RunTest(const FString& Parameters)
{
    FEditorScriptExecutionGuard Guard;
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    auto* Door = World->SpawnActor<ATDFDoorActor>();
    TestFalse(TEXT("Door begins closed"), Door->bIsOpen);
    Door->Interact_Implementation(nullptr);
    TestTrue(TEXT("Interaction opens an unlocked door"), Door->bIsOpen);
    TestFalse(TEXT("Animation blocks repeated interaction"), Door->CanInteract_Implementation(nullptr));
    Door->FinishDoorAnimation();
    Door->Interact_Implementation(nullptr);
    TestFalse(TEXT("Second interaction closes door"), Door->bIsOpen);
    Door->FinishDoorAnimation();
    Door->bIsLocked = true;
    Door->Interact_Implementation(nullptr);
    TestFalse(TEXT("Missing key cannot open locked door"), Door->bIsOpen);
    TestTrue(TEXT("Locked door remains available for feedback"), Door->CanInteract_Implementation(nullptr));
    World->DestroyWorld(false);
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFDoorPresentationTest, "TDF.World.DoorBlueprint", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTDFDoorPresentationTest::RunTest(const FString& Parameters)
{
    FEditorScriptExecutionGuard Guard;
    UClass* Class = LoadClass<ATDFDoorActor>(nullptr, TEXT("/Game/Game/World/BP_Door_Base.BP_Door_Base_C"));
    if (!TestNotNull(TEXT("Door Blueprint exists"), Class)) return false;
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    auto* Door = World->SpawnActor<ATDFDoorActor>(Class);
    UFunction* Tick = Door->FindFunction(TEXT("ReceiveTick"));
    float Delta = .1f;
    Door->SetOpen(true);
    for (int32 I=0; I<20; ++I) Door->ProcessEvent(Tick, &Delta);
    TestTrue(TEXT("Blueprint opens the hinge to authored angle"), FMath::IsNearlyEqual(Door->Hinge->GetRelativeRotation().Yaw, Door->OpenAngle));
    TestFalse(TEXT("Blueprint releases interaction after animation"), Door->bIsAnimating);
    Door->SetOpen(false);
    for (int32 I=0; I<20; ++I) Door->ProcessEvent(Tick, &Delta);
    TestTrue(TEXT("Blueprint closes hinge"), FMath::IsNearlyZero(Door->Hinge->GetRelativeRotation().Yaw));
    World->DestroyWorld(false);
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFSwitchTest, "TDF.World.SwitchRules", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTDFSwitchTest::RunTest(const FString& Parameters)
{
    FEditorScriptExecutionGuard Guard;
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    auto* Switch = World->SpawnActor<ATDFSwitchActor>();
    auto* Door = World->SpawnActor<ATDFDoorActor>();
    Switch->LinkedActors.Add(Door);
    Switch->LinkedActors.Add(nullptr);
    Switch->Interact_Implementation(nullptr);
    TestTrue(TEXT("Switch notifies door through receiver contract"), Switch->bIsOn && Door->bIsOpen);
    Switch->Interact_Implementation(nullptr);
    TestFalse(TEXT("Reusable switch toggles linked door off"), Door->bIsOpen);
    Switch->bOneShot = true;
    Switch->bHasBeenUsed = false;
    Switch->Interact_Implementation(nullptr);
    Switch->Interact_Implementation(nullptr);
    TestTrue(TEXT("One-shot stays on"), Switch->bIsOn);
    TestFalse(TEXT("Used one-shot rejects input"), Switch->CanInteract_Implementation(nullptr));
    Door->bIsLocked = true;
    Door->SetOpen(false);
    ITDFSwitchReceiverInterface::Execute_ReceiveSwitchState(Door, Switch, true);
    TestFalse(TEXT("Switch cannot bypass a keyed lock"), Door->bIsOpen);
    World->DestroyWorld(false);
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFItemDefinitionTest, "TDF.World.ItemDefinition", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTDFItemDefinitionTest::RunTest(const FString& Parameters)
{
    auto* Item = NewObject<UTDFItemDataAsset>();
    TestFalse(TEXT("Empty definition rejected"), Item->HasValidDefinition());
    Item->ItemId = TEXT("Evidence.Test"); Item->DisplayName = FText::FromString(TEXT("Test relay"));
    TestTrue(TEXT("Inspectable definition is data driven"), Item->HasValidDefinition());
    Item->ItemType = ETDFItemType::KeyItem;
    TestFalse(TEXT("Key must have key id and permanent pickup flags"), Item->HasValidDefinition());
    Item->KeyId = TEXT("TestKey"); Item->bCanPickUp = true; Item->bPersistsInInventory = true;
    TestTrue(TEXT("Complete key is valid"), Item->HasValidDefinition());
    TestEqual(TEXT("Primary asset identity uses stable item id"), Item->GetPrimaryAssetId().PrimaryAssetName, Item->ItemId);
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFInventoryTest, "TDF.World.Inventory", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTDFInventoryTest::RunTest(const FString& Parameters)
{
    auto* Inventory = NewObject<UTDFInventoryComponent>();
    auto* Key = NewObject<UTDFItemDataAsset>();
    TestFalse(TEXT("Null pickup rejected"), Inventory->AddItem(nullptr));
    TestFalse(TEXT("Invalid pickup rejected"), Inventory->AddItem(Key));
    Key->ItemId=TEXT("Key.Test"); Key->DisplayName=FText::FromString(TEXT("Service key"));
    Key->KeyId=TEXT("ServiceKey"); Key->ItemType=ETDFItemType::KeyItem;
    Key->bCanPickUp=true; Key->bPersistsInInventory=true;
    TestTrue(TEXT("First pickup succeeds"), Inventory->AddItem(Key));
    TestTrue(TEXT("Inventory grants item id"), Inventory->HasItem(Key->ItemId));
    TestTrue(TEXT("Inventory grants key id"), Inventory->HasKey(Key->KeyId));
    TestFalse(TEXT("Duplicate pickup has no effect"), Inventory->AddItem(Key));
    TestFalse(TEXT("Unrelated key is not granted"), Inventory->HasKey(TEXT("OtherKey")));
    TestEqual(TEXT("Owned IDs are unique"), Inventory->GetOwnedItemIds().Num(), 1);
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFWorldPickupTest, "TDF.World.Pickup", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTDFWorldPickupTest::RunTest(const FString& Parameters)
{
    FEditorScriptExecutionGuard Guard;
    UWorld* World=UWorld::CreateWorld(EWorldType::Game,false);
    auto* Player=World->SpawnActor<ATDFPlayerCharacter>();
    auto* Item=World->SpawnActor<ATDFWorldItemActor>();
    Item->ItemData=LoadObject<UTDFItemDataAsset>(nullptr,TEXT("/Game/Game/Inventory/DA_Item_BasementKey.DA_Item_BasementKey"));
    TestNotNull(TEXT("Authored key definition loads"), Item->ItemData.Get());
    Item->Interact_Implementation(Player);
    TestTrue(TEXT("Pickup grants authored key"),Player->Inventory->HasKey(TEXT("BasementKey")));
    TestTrue(TEXT("Pickup retains actor state and hides it"),Item->bHasBeenPickedUp && Item->IsHidden());
    TestFalse(TEXT("Picked actor has no collision"),Item->GetActorEnableCollision());
    TestFalse(TEXT("Picked actor cannot be collected again"),Item->CanInteract_Implementation(Player));
    World->DestroyWorld(false);
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFInspectionStateTest, "TDF.World.InspectionState", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTDFInspectionStateTest::RunTest(const FString& Parameters)
{
    FEditorScriptExecutionGuard Guard;
    UWorld* World=UWorld::CreateWorld(EWorldType::Game,false);
    auto* PC=World->SpawnActor<ATDFPlayerController>();
    auto* Player=World->SpawnActor<ATDFPlayerCharacter>();
    PC->Possess(Player);
    auto* Item=LoadObject<UTDFItemDataAsset>(nullptr,TEXT("/Game/Game/Inventory/DA_Item_ArrayRelay.DA_Item_ArrayRelay"));
    TestFalse(TEXT("Null inspection is rejected"),PC->OpenInspection(nullptr));
    TestTrue(TEXT("Authored inspectable enters mode"),PC->OpenInspection(Item));
    TestTrue(TEXT("Inspection blocks motion and camera"),PC->IsMoveInputIgnored() && PC->IsLookInputIgnored());
    TestFalse(TEXT("Repeated open cannot stack input locks"),PC->OpenInspection(Item));
    PC->HandleBackAction();
    PC->CloseInspection();
    TestFalse(TEXT("Back exits inspection"),PC->IsInspectingItem());
    TestFalse(TEXT("Close restores movement and camera"),PC->IsMoveInputIgnored() || PC->IsLookInputIgnored());
    PC->SetIgnoreMoveInput(true);
    PC->OpenInspection(Item); PC->CloseInspection();
    TestTrue(TEXT("External input locks survive inspection"),PC->IsMoveInputIgnored());
    PC->SetIgnoreMoveInput(false);
    World->DestroyWorld(false);
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFInspectionRotationTest, "TDF.World.InspectionRotation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTDFInspectionRotationTest::RunTest(const FString& Parameters)
{
    auto* Widget=NewObject<UTDFInspectionWidget>();
    Widget->RotatePreview(FVector2D(400.f,500.f));
    TestTrue(TEXT("Yaw wraps safely"),FMath::IsNearlyEqual(Widget->PreviewRotation.Yaw,65.f));
    TestEqual(TEXT("Pitch clamps at upper bound"),Widget->PreviewRotation.Pitch,80.0);
    Widget->RotatePreview(FVector2D(-30.f,-1000.f));
    TestEqual(TEXT("Pitch clamps at lower bound"),Widget->PreviewRotation.Pitch,-80.0);
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFDoorKeyTest, "TDF.World.DoorKey", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTDFDoorKeyTest::RunTest(const FString& Parameters)
{
    FEditorScriptExecutionGuard Guard;
    UWorld* World=UWorld::CreateWorld(EWorldType::Game,false);
    auto* Player=World->SpawnActor<ATDFPlayerCharacter>();
    auto* Door=World->SpawnActor<ATDFDoorActor>();
    Door->bIsLocked=true; Door->RequiredKeyId=TEXT("BasementKey");
    Door->Interact_Implementation(Player);
    TestTrue(TEXT("Missing key keeps lock closed"),Door->bIsLocked && !Door->bIsOpen);
    auto* Key=LoadObject<UTDFItemDataAsset>(nullptr,TEXT("/Game/Game/Inventory/DA_Item_BasementKey.DA_Item_BasementKey"));
    Player->Inventory->AddItem(Key);
    TestEqual(TEXT("Owned key changes prompt"),Door->GetInteractionPrompt_Implementation(Player).ToString(),FString(TEXT("Unlock")));
    Door->Interact_Implementation(Player);
    TestTrue(TEXT("Matching stable key unlocks and opens"),!Door->bIsLocked && Door->bIsOpen);
    TestTrue(TEXT("Permanent key is retained"),Player->Inventory->HasKey(TEXT("BasementKey")));
    World->DestroyWorld(false);
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFPersistentStateTest, "TDF.World.PersistentState", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTDFPersistentStateTest::RunTest(const FString& Parameters)
{
    FEditorScriptExecutionGuard Guard;
    UWorld* World=UWorld::CreateWorld(EWorldType::Game,false);
    auto* Door=World->SpawnActor<ATDFDoorActor>();
    auto* Other=World->SpawnActor<ATDFDoorActor>();
    auto* Switch=World->SpawnActor<ATDFSwitchActor>();
    auto* Item=World->SpawnActor<ATDFWorldItemActor>();
    TestFalse(TEXT("Runtime temporary actors opt out"),Door->ShouldPersistInSave_Implementation());
    Door->StableIdentity->EnsurePlacedEditorId();
    TestFalse(TEXT("Runtime never manufactures IDs"),Door->GetStableActorId_Implementation().IsValid());
    const FGuid Id=FGuid::NewGuid();
    Door->SetStableActorId_Implementation(Id);
    auto* Registry=NewObject<UTDFSaveSubsystem>(NewObject<UGameInstance>());
    TestTrue(TEXT("Unique registration succeeds"),Registry->RegisterStableActor(Door,Id));
    AddExpectedError(TEXT("TDF Persistence: duplicate stable actor ID"),EAutomationExpectedErrorFlags::Contains,1);
    TestFalse(TEXT("Duplicate ID rejected"),Registry->RegisterStableActor(Other,Id));
    Door->SetOpen(true);
    const auto Saved=Door->GetPersistentState();
    Door->SetOpen(false);
    TestTrue(TEXT("Matching door state restores"),Door->ApplyPersistentState(Saved));
    TestTrue(TEXT("Open truth restored"),Door->bIsOpen);
    TestFalse(TEXT("Foreign record rejected"),Other->ApplyPersistentState(Saved));
    TestEqual(TEXT("Restoring cannot change identity"),Door->GetStableActorId_Implementation(),Id);
    Switch->SetStableActorId_Implementation(FGuid::NewGuid());
    Switch->LinkedActors.Add(Door);
    const auto SwitchState=Switch->GetPersistentState();
    Switch->bIsOn=true;
    Switch->ApplyPersistentState(SwitchState);
    TestTrue(TEXT("Restore does not replay linked commands over door record"),Door->bIsOpen);
    Item->SetStableActorId_Implementation(FGuid::NewGuid());
    auto ItemState=Item->GetPersistentState(); ItemState.bHasBeenPickedUp=true;
    TestTrue(TEXT("Pickup state imports"),Item->ApplyPersistentState(ItemState));
    TestTrue(TEXT("Restored pickup stays hidden"),Item->IsHidden() && !Item->GetActorEnableCollision());
    World->DestroyWorld(false);
    UWorld* EditorWorld=UWorld::CreateWorld(EWorldType::Editor,false);
    auto* Placed=EditorWorld->SpawnActor<ATDFDoorActor>();
    Placed->StableIdentity->EnsurePlacedEditorId();
    TestTrue(TEXT("New editor placement gets an ID"),Placed->GetStableActorId_Implementation().IsValid());
    const FGuid PlacedId=Placed->GetStableActorId_Implementation();
    Placed->StableIdentity->EnsurePlacedEditorId();
    TestEqual(TEXT("Construction preserves existing ID"),Placed->GetStableActorId_Implementation(),PlacedId);
    Placed->SetFlags(RF_WasLoaded);
    Placed->StableIdentity->StableActorId.Invalidate();
    Placed->StableIdentity->EnsurePlacedEditorId();
    TestFalse(TEXT("Loading missing identity never silently regenerates it"),Placed->GetStableActorId_Implementation().IsValid());
    EditorWorld->DestroyWorld(false);
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFInputSettingsTest, "TDF.World.InputSettingsRoundTrip", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTDFInputSettingsTest::RunTest(const FString& Parameters)
{
    auto* Settings=NewObject<UTDFPlayerInputSettingsSave>();
    Settings->KeyboardOverrides.Add(TEXT("Interact"),EKeys::F);
    Settings->GamepadOverrides.Add(TEXT("Interact"),EKeys::Gamepad_DPad_Up);
    TArray<uint8> Bytes;
    TestTrue(TEXT("Binding settings serialize"),UGameplayStatics::SaveGameToMemory(Settings,Bytes));
    auto* Restored=Cast<UTDFPlayerInputSettingsSave>(UGameplayStatics::LoadGameFromMemory(Bytes));
    if (TestNotNull(TEXT("Settings load"),Restored))
    {
        TestEqual(TEXT("Keyboard override preserved"),Restored->KeyboardOverrides.FindRef(TEXT("Interact")),EKeys::F);
        TestEqual(TEXT("Gamepad override preserved"),Restored->GamepadOverrides.FindRef(TEXT("Interact")),EKeys::Gamepad_DPad_Up);
    }
    return !HasAnyErrors();
}
#endif

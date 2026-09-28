#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Core/DFGameMode.h"
#include "Player/DFPlayerCharacter.h"
#include "Player/DFPlayerController.h"
#include "Player/DFMovementStateTypes.h"
#include "Tests/DFInteractionTestTarget.h"
#include "UObject/Script.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFInteractionContractTest, "TDF.Player.Foundation.InteractionContract",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTDFInteractionContractTest::RunTest(const FString& Parameters)
{
    // This isolated world has not entered play; permit reflected event dispatch
    // for the contract test. The PIE suite exercises the normal runtime path.
    FEditorScriptExecutionGuard ScriptExecution;
    for (const FName Name : {FName("CanInteract"), FName("GetInteractionPrompt"), FName("Interact"), FName("WantsPromptVisible"), FName("GetInteractionId")})
    {
        const UFunction* Function = UTDFInteractableInterface::StaticClass()->FindFunctionByName(Name);
        if (TestNotNull(*Name.ToString(), Function)) TestTrue(TEXT("Contract is Blueprint overrideable"), Function->HasAnyFunctionFlags(FUNC_BlueprintEvent));
    }
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    auto* Target = World->SpawnActor<ATDFInteractionTestTarget>();
    TestTrue(TEXT("Fixture implements interface"), Target->Implements<UTDFInteractableInterface>());
    TestTrue(TEXT("Fixture enabled default"), Target->bInteractionEnabled);
    TestTrue(TEXT("Native fixture implementation"), Target->CanInteract_Implementation(nullptr));
    TestNotNull(TEXT("Native interface address"), Target->GetNativeInterfaceAddress(UTDFInteractableInterface::StaticClass()));
    TestTrue(TEXT("Enabled fixture accepts interaction"), ITDFInteractableInterface::Execute_CanInteract(Target, nullptr));
    ITDFInteractableInterface::Execute_Interact(Target, nullptr);
    TestEqual(TEXT("Interface dispatch executes interaction"), Target->InteractionCount, 1);
    Target->bInteractionEnabled = false;
    TestFalse(TEXT("Disabled fixture rejects interaction"), ITDFInteractableInterface::Execute_CanInteract(Target, nullptr));
    World->DestroyWorld(false);
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFMovementRulesTest, "TDF.Player.Foundation.MovementExclusions",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTDFMovementRulesTest::RunTest(const FString& Parameters)
{
    FTDFPlayerMovementState State;
    State.bWantsSprint = true;
    State.Resolve(false, true, 100.f, 10.f);
    TestTrue(TEXT("Standing motion can sprint"), State.bIsSprinting);
    State.Resolve(true, true, 100.f, 10.f);
    TestFalse(TEXT("Crouch excludes sprint"), State.bIsSprinting);
    State.bHoldingBreath = true;
    State.Resolve(false, true, 100.f, 10.f);
    TestFalse(TEXT("Held breath excludes sprint"), State.bIsSprinting);
    State.bHoldingBreath = false;
    State.Resolve(false, true, 0.f, 10.f);
    TestFalse(TEXT("Stationary player cannot sprint"), State.bIsSprinting);
    State.Resolve(false, false, 100.f, 10.f);
    TestFalse(TEXT("Falling player cannot sprint"), State.bIsSprinting);
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFPlayerSkeletonTest, "TDF.Player.Foundation.SpawnAndPossess",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTDFPlayerSkeletonTest::RunTest(const FString& Parameters)
{
    const ATDFGameMode* Defaults = GetDefault<ATDFGameMode>();
    TestEqual(TEXT("Game mode uses project pawn"), Defaults->DefaultPawnClass.Get(), ATDFPlayerCharacter::StaticClass());
    TestEqual(TEXT("Game mode uses project controller"), Defaults->PlayerControllerClass.Get(), ATDFPlayerController::StaticClass());
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    if (!TestNotNull(TEXT("Test world"), World)) return false;
    ATDFPlayerCharacter* Pawn = World->SpawnActor<ATDFPlayerCharacter>();
    ATDFPlayerController* Controller = World->SpawnActor<ATDFPlayerController>();
    if (TestNotNull(TEXT("Project pawn spawns"), Pawn) && TestNotNull(TEXT("Project controller spawns"), Controller))
    {
        Controller->Possess(Pawn);
        TestTrue(TEXT("Controller possesses project pawn"), Controller->GetPawn() == Pawn);
    }
    World->DestroyWorld(false);
    return !HasAnyErrors();
}
#endif

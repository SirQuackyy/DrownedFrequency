#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Player/DFPlayerController.h"
#include "Inventory/DFItemDataAsset.h"
#include "UI/DFInspectionWidget.h"
#include "UI/DFInspectionPreviewActor.h"

class FTDFRouteCaptures : public IAutomationLatentCommand
{
public:
    explicit FTDFRouteCaptures(FAutomationTestBase* T):Test(T){}
    bool Update() override
    {
        UWorld* W=nullptr;
        for (const auto& C:GEngine->GetWorldContexts()) if (C.WorldType==EWorldType::Game) W=C.World();
        auto* PC=W ? Cast<ATDFPlayerController>(W->GetFirstPlayerController()) : nullptr;
        if (!PC || !PC->GetPawn()) { Test->AddError(TEXT("Capture needs a game world")); return true; }
        if (Started<0.)
        {
            Started=FPlatformTime::Seconds();
            if (Stage==0) { PC->GetPawn()->SetActorLocation(FVector(-1100,0,92)); PC->SetControlRotation(FRotator(-2,5,0)); }
            if (Stage==2) { PC->GetPawn()->SetActorLocation(FVector(-750,-280,92)); PC->SetControlRotation(FRotator(-2,25,0)); }
            if (Stage==4) { PC->GetPawn()->SetActorLocation(FVector(-535,260,92)); PC->SetControlRotation(FRotator(-8,90,0)); }
            if (Stage==6)
            {
                auto* Item=LoadObject<UTDFItemDataAsset>(nullptr,TEXT("/Game/Game/Inventory/DA_Item_ArrayRelay.DA_Item_ArrayRelay"));
                Test->TestTrue(TEXT("Inspection opens for render"),PC->OpenInspection(Item));
            }
        }
        if (FPlatformTime::Seconds()-Started<(Stage==0 ? 18. : 4.)) return false;
        if (Stage%2==0)
        {
            if (Stage==6) Test->TestTrue(TEXT("Inspection has a real preview render target"),PC->InspectionWidget && PC->InspectionWidget->PreviewActor && PC->InspectionWidget->PreviewActor->RenderTarget);
            const FString Name=Names[Stage/2];
            FScreenshotRequest::RequestScreenshot(FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()/TEXT("Screenshots")/Name),true,false);
        }
        if (Stage==7)
        {
            PC->CloseInspection();
            for (const TCHAR* Name:Names) Test->TestTrue(TEXT("Phase 3 render written"),IFileManager::Get().FileExists(*(FPaths::ProjectSavedDir()/TEXT("Screenshots")/Name)));
            return true;
        }
        Started=-1.; ++Stage; return false;
    }
private:
    FAutomationTestBase* Test; int32 Stage=0; double Started=-1.;
    const TCHAR* Names[4]={TEXT("Phase3Entrance.png"),TEXT("Phase3Corner.png"),TEXT("Phase3Crawl.png"),TEXT("Phase3Inspection.png")};
};
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTDFRouteVisualTest,"TDF.Visual.Phase3Captures",EAutomationTestFlags::ClientContext|EAutomationTestFlags::EngineFilter)
bool FTDFRouteVisualTest::RunTest(const FString& Parameters)
{
    ADD_LATENT_AUTOMATION_COMMAND(FTDFRouteCaptures(this)); return true;
}
#endif

#include "UI/DFInspectionPreviewActor.h"
#include "Inventory/DFItemDataAsset.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/PointLightComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
ATDFInspectionPreviewActor::ATDFInspectionPreviewActor()
{
    RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    PreviewMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh"));
    PreviewMesh->SetupAttachment(RootComponent);
    PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    PreviewMesh->SetSimulatePhysics(false);
    Capture=CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture"));
    Capture->SetupAttachment(RootComponent);
    Capture->SetRelativeLocation(FVector(-240,0,0));
    Capture->FOVAngle=35.f;
    Capture->bCaptureEveryFrame=true;
    Capture->bAlwaysPersistRenderingState=true;
    Capture->bCaptureOnMovement=false;
    Capture->CaptureSource=ESceneCaptureSource::SCS_FinalToneCurveHDR;
    Capture->PrimitiveRenderMode=ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
    Capture->ShowFlags.SetFog(false);
    Capture->ShowFlags.SetVolumetricFog(false);
    Capture->ShowFlags.SetAtmosphere(false);
    Capture->ShowFlags.SetSkyLighting(false);
    Capture->ShowFlags.SetTemporalAA(true);
    Capture->PostProcessSettings.bOverride_FilmGrainIntensity=true;
    Capture->PostProcessSettings.FilmGrainIntensity=0.f;
    Capture->PostProcessSettings.bOverride_AmbientOcclusionIntensity=true;
    Capture->PostProcessSettings.AmbientOcclusionIntensity=0.f;
    Capture->PostProcessSettings.bOverride_DynamicGlobalIlluminationMethod=true;
    Capture->PostProcessSettings.DynamicGlobalIlluminationMethod=EDynamicGlobalIlluminationMethod::None;
    Capture->PostProcessSettings.bOverride_ReflectionMethod=true;
    Capture->PostProcessSettings.ReflectionMethod=EReflectionMethod::None;
    Capture->PostProcessSettings.bOverride_AutoExposureMinBrightness=true;
    Capture->PostProcessSettings.bOverride_AutoExposureMaxBrightness=true;
    Capture->PostProcessSettings.AutoExposureMinBrightness=1.f;
    Capture->PostProcessSettings.AutoExposureMaxBrightness=1.f;
    Capture->PostProcessSettings.bOverride_MotionBlurAmount=true;
    Capture->PostProcessSettings.MotionBlurAmount=0.f;
    KeyLight=CreateDefaultSubobject<UPointLightComponent>(TEXT("KeyLight"));
    KeyLight->SetupAttachment(RootComponent);
    KeyLight->SetRelativeLocation(FVector(-100,-90,100));
    KeyLight->SetIntensity(4000.f);
    KeyLight->SetAttenuationRadius(500.f);
    KeyLight->SetLightColor(FLinearColor(.75f,.85f,.8f));
    RimLight=CreateDefaultSubobject<UPointLightComponent>(TEXT("RimLight"));
    RimLight->SetupAttachment(RootComponent);
    RimLight->SetRelativeLocation(FVector(30,100,60));
    RimLight->SetIntensity(2000.f);
    RimLight->SetAttenuationRadius(400.f);
    RimLight->SetLightColor(FLinearColor(.8f,.5f,.2f));
}
void ATDFInspectionPreviewActor::Initialize(UTDFItemDataAsset* Item)
{
    if (!Item) return;
    UStaticMesh* Mesh=Item->InspectionMesh.LoadSynchronous();
    if (!Mesh) return;
    PreviewMesh->SetStaticMesh(Mesh);
    if (UMaterialInterface* Material=Item->InspectionMaterialOverride.LoadSynchronous())
        for (int32 I=0; I<PreviewMesh->GetNumMaterials(); ++I) PreviewMesh->SetMaterial(I,Material);
    const FVector Bounds=Mesh->GetBounds().BoxExtent*Item->InspectionScale;
    const FVector Scale=Item->InspectionScale*(50.f/FMath::Max(Bounds.GetMax(),1.f));
    PreviewMesh->SetRelativeScale3D(Scale);
    PreviewMesh->SetRelativeLocation(-Mesh->GetBounds().Origin*Scale);
    RenderTarget=NewObject<UTextureRenderTarget2D>(this);
    // Slate expects linear samples; preserve near-black precision until its final display conversion.
    RenderTarget->RenderTargetFormat=RTF_RGBA16f;
    RenderTarget->ClearColor=FLinearColor(.009f,.014f,.015f,1.f);
    RenderTarget->InitAutoFormat(512,512);
    Capture->TextureTarget=RenderTarget;
    Capture->ShowOnlyActors.Add(this);
}
void ATDFInspectionPreviewActor::SetPreviewRotation(FRotator Rotation)
{
    PreviewMesh->SetRelativeRotation(Rotation);
}

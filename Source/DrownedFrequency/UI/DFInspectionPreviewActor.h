#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DFInspectionPreviewActor.generated.h"
/** Transient presentation-only studio, never a world pickup or persistent actor. */
UCLASS(Transient, NotBlueprintable)
class DROWNEDFREQUENCY_API ATDFInspectionPreviewActor : public AActor
{
    GENERATED_BODY()
public:
    ATDFInspectionPreviewActor();
    void Initialize(class UTDFItemDataAsset* Item);
    void SetPreviewRotation(FRotator Rotation);
    UPROPERTY(Transient) TObjectPtr<class UTextureRenderTarget2D> RenderTarget;
    UPROPERTY(VisibleAnywhere) TObjectPtr<class UStaticMeshComponent> PreviewMesh;
private:
    UPROPERTY() TObjectPtr<class USceneCaptureComponent2D> Capture;
    UPROPERTY() TObjectPtr<class UPointLightComponent> KeyLight;
    UPROPERTY() TObjectPtr<class UPointLightComponent> RimLight;
};

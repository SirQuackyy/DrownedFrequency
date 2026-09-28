#include "UI/DFInspectionWidget.h"
#include "Inventory/DFItemDataAsset.h"
#include "Components/TextBlock.h"
#include "UI/DFInspectionPreviewActor.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
void UTDFInspectionWidget::ShowItem(UTDFItemDataAsset* Item)
{
    DisplayedItem=Item;
    if (!Item) return;
    if (auto* Title=Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemTitle")))) Title->SetText(Item->DisplayName);
    if (auto* Description=Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemDescription")))) Description->SetText(Item->ShortDescription);
    if (PreviewActor) PreviewActor->Destroy();
    if (GetWorld())
    {
        FActorSpawnParameters Params;
        Params.ObjectFlags |= RF_Transient;
        Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        PreviewActor=GetWorld()->SpawnActor<ATDFInspectionPreviewActor>(FVector(0,0,-100000),FRotator::ZeroRotator,Params);
        if (PreviewActor)
        {
            PreviewActor->Initialize(Item);
            PreviewActor->SetPreviewRotation(PreviewRotation);
            if (auto* Image=Cast<UImage>(GetWidgetFromName(TEXT("PreviewImage"))))
            {
                FSlateBrush Brush;
                Brush.SetResourceObject(PreviewActor->RenderTarget);
                Brush.ImageSize=FVector2D(512,512);
                Image->SetBrush(Brush);
            }
        }
    }
    OnItemChanged(Item);
}
void UTDFInspectionWidget::RotatePreview(FVector2D Delta)
{
    if (!FMath::IsFinite(Delta.X) || !FMath::IsFinite(Delta.Y)) return;
    PreviewRotation.Yaw=FRotator::NormalizeAxis(PreviewRotation.Yaw+Delta.X);
    PreviewRotation.Pitch=FMath::Clamp(PreviewRotation.Pitch+Delta.Y,-80.f,80.f);
    if (PreviewActor) PreviewActor->SetPreviewRotation(PreviewRotation);
}
void UTDFInspectionWidget::NativeDestruct()
{
    if (PreviewActor) { PreviewActor->Destroy(); PreviewActor=nullptr; }
    Super::NativeDestruct();
}
void UTDFInspectionWidget::UpdateControls(const FText& Text)
{
    if (auto* Controls=Cast<UTextBlock>(GetWidgetFromName(TEXT("InspectionControls")))) Controls->SetText(Text);
}

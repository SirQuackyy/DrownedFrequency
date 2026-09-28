#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DFInteractionPromptWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DROWNEDFREQUENCY_API UTDFInteractionPromptWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="Prompt") void ShowPrompt(const FText& Text);
    UFUNCTION(BlueprintCallable, Category="Prompt") void HidePrompt();
    UFUNCTION(BlueprintCallable, Category="Prompt") void UpdateInputGlyph(const FText& Label);
    UFUNCTION(BlueprintImplementableEvent, Category="Prompt") void SetPromptText(const FText& Text);
    UFUNCTION(BlueprintImplementableEvent, Category="Prompt") void SetInputGlyph(const FText& Label);
    UPROPERTY(BlueprintReadOnly, Category="Prompt") FText CurrentPrompt;
    UPROPERTY(BlueprintReadOnly, Category="Prompt") FText CurrentGlyph;
protected:
    virtual void NativeConstruct() override;
    UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<class UTextBlock> PromptText;
    UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<class UTextBlock> GlyphText;
};

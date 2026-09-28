#include "UI/DFInteractionPromptWidget.h"
#include "Components/TextBlock.h"

void UTDFInteractionPromptWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // Presentation remains editable in the Widget Blueprint.
    if (!PromptText) PromptText = Cast<UTextBlock>(GetWidgetFromName(TEXT("PromptLabel")));
    if (!GlyphText) GlyphText = Cast<UTextBlock>(GetWidgetFromName(TEXT("InputLabel")));
}

void UTDFInteractionPromptWidget::ShowPrompt(const FText& Text)
{
    if (!CurrentPrompt.EqualTo(Text))
    {
        CurrentPrompt = Text;
        if (PromptText) PromptText->SetText(Text);
        SetPromptText(Text);
    }
    SetVisibility(ESlateVisibility::HitTestInvisible);
}
void UTDFInteractionPromptWidget::HidePrompt() { SetVisibility(ESlateVisibility::Collapsed); }
void UTDFInteractionPromptWidget::UpdateInputGlyph(const FText& Label)
{
    if (CurrentGlyph.EqualTo(Label)) return;
    CurrentGlyph = Label;
    if (GlyphText) GlyphText->SetText(Label);
    SetInputGlyph(Label);
}

#include "Acoustics/DFAcousticBlueprintLibrary.h"
#include "Acoustics/DFRecorderPlaybackComponent.h"
#include "Acoustics/DFAudioAnomalyDataAsset.h"
FText UTDFAcousticBlueprintLibrary::GetSignalDisplayName(const FTDFRecordedSignal& S)
{
    if(!S.IsValid()) return NSLOCTEXT("Acoustic","Empty","Empty");
    const auto* A=UTDFRecorderPlaybackComponent::Resolve(S.SourceAnomalyId);
    return A && !A->DisplayName.IsEmpty()?A->DisplayName:NSLOCTEXT("Acoustic","Unavailable","Unavailable signal");
}
FSlateColor UTDFAcousticBlueprintLibrary::GetCategoryColor(FGameplayTag T)
{
    static const FLinearColor Colors[]={FLinearColor(.76f,.70f,.9f),FLinearColor(.72f,.74f,.64f),FLinearColor(.61f,.78f,.78f),FLinearColor(.92f,.76f,.43f),FLinearColor(.49f,.71f,.89f),FLinearColor(.8f,.8f,.8f),FLinearColor(.87f,.59f,.44f)};
    const int32 I=TDFAcoustics::CategoryNames().IndexOfByKey(T.GetTagName()); return FSlateColor(I==INDEX_NONE?FLinearColor(.7f,.7f,.7f):Colors[I]);
}
FText UTDFAcousticBlueprintLibrary::GetRouteDisplayName(ETDFAcousticRoute R)
{
    switch(R) {
    case ETDFAcousticRoute::Pipe:return NSLOCTEXT("Acoustic","Pipe","Through pipe");
    case ETDFAcousticRoute::Speaker:return NSLOCTEXT("Acoustic","Speaker","Station speaker");
    case ETDFAcousticRoute::PuckProjected:return NSLOCTEXT("Acoustic","Puck","Remote");
    case ETDFAcousticRoute::Overdub:return NSLOCTEXT("Acoustic","Overdub","Layered tape");
    default:return FText::GetEmpty(); }
}

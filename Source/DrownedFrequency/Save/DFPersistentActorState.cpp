#include "Save/DFPersistentActor.h"
#include "UObject/UnrealType.h"
#include "Engine/Level.h"
namespace
{
void CaptureFields(const UObject* Object,const FString& Prefix,TMap<FName,FString>& Fields)
{
    for(TFieldIterator<FProperty> I(Object->GetClass());I;++I) if(I->HasAnyPropertyFlags(CPF_SaveGame) && !CastField<FObjectPropertyBase>(*I))
    {FString Value; I->ExportText_InContainer(0,Value,Object,Object,const_cast<UObject*>(Object),PPF_None); Fields.Add(FName(*(Prefix+I->GetName())),MoveTemp(Value));}
}
void RestoreFields(UObject* Object,const FString& Prefix,const TMap<FName,FString>& Fields)
{
    for(TFieldIterator<FProperty> I(Object->GetClass());I;++I) if(I->HasAnyPropertyFlags(CPF_SaveGame) && !CastField<FObjectPropertyBase>(*I))
    {if(const auto* Text=Fields.Find(FName(*(Prefix+I->GetName())))) if(!I->ImportText_Direct(**Text,I->ContainerPtrToValuePtr<void>(Object),Object,PPF_None)) UE_LOG(LogTemp,Warning,TEXT("TDF Save: skipped incompatible field %s on %s"),*I->GetName(),*Object->GetName());}
}
}
FTDFActorSaveData ATDFPersistentActor::CaptureSaveState_Implementation() const
{
    FTDFActorSaveData S; S.StableActorId=StableIdentity->StableActorId; S.Transform=GetActorTransform(); S.StateName=TEXT("PropertiesV1");
    CaptureFields(this,TEXT("Actor."),S.SerializedFields);
    for(const auto* C:GetComponents()) if(C && C!=StableIdentity) CaptureFields(C,C->GetName()+TEXT("."),S.SerializedFields);
    return S;
}
bool ATDFPersistentActor::ApplySaveState_Implementation(const FTDFActorSaveData& S)
{
    if(!S.StableActorId.IsValid() || S.StableActorId!=StableIdentity->StableActorId || S.Transform.ContainsNaN() || S.StateName!=TEXT("PropertiesV1")) return false;
    SetActorTransform(S.Transform,false,nullptr,ETeleportType::TeleportPhysics); RestoreFields(this,TEXT("Actor."),S.SerializedFields);
    for(auto* C:GetComponents()) if(C && C!=StableIdentity) RestoreFields(C,C->GetName()+TEXT("."),S.SerializedFields);
    RefreshSavedPresentation(); return true;
}

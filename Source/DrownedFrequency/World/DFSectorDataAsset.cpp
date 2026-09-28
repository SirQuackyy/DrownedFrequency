#include "World/DFSectorDataAsset.h"
#include "Misc/PackageName.h"
#include "Misc/DataValidation.h"
bool UTDFSectorDataAsset::HasValidDefinition() const {return !SectorId.IsNone() && FPackageName::DoesPackageExist(LevelPackageName.ToString()) && !NeighborSectorIds.Contains(SectorId);}
#if WITH_EDITOR
EDataValidationResult UTDFSectorDataAsset::IsDataValid(FDataValidationContext& Context) const
{
    if(!HasValidDefinition()) {Context.AddError(NSLOCTEXT("Sector","Invalid","Sector needs a stable ID, existing map and valid neighbors.")); return EDataValidationResult::Invalid;} return EDataValidationResult::Valid;
}
#endif

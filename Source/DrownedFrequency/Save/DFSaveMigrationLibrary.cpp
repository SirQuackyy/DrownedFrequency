#include "Save/DFSaveMigrationLibrary.h"
bool UTDFSaveMigrationLibrary::Upgrade(UTDFSaveGame* Save,ETDFLoadFailure& Failure,FText& Message)
{
    Failure=ETDFLoadFailure::None;
    if(!Save || Save->SaveVersion<1) {Failure=ETDFLoadFailure::InvalidSave; Message=NSLOCTEXT("Save","InvalidVersion","This save has an invalid format."); return false;}
    if(Save->SaveVersion>TDFSave::CurrentVersion) {Failure=ETDFLoadFailure::SaveFromNewerBuild; Message=NSLOCTEXT("Save","NewerVersion","This save was made by a newer version of the game. Update the game to load it."); return false;}
    while(Save->SaveVersion<TDFSave::CurrentVersion)
    {
        switch(Save->SaveVersion)
        {
        case 1: Save->Player.PlacedPucks.Reset(); Save->CheckpointRespawnTransform=FTransform(Save->Player.Rotation,Save->Player.Location); Save->CheckpointRequiredSectors=Save->LoadedSectorIds; if(Save->Player.DifficultyId==TEXT("Normal")) Save->Player.DifficultyId=TEXT("Standard"); if(Save->Settings.DifficultyId==TEXT("Normal")) Save->Settings.DifficultyId=TEXT("Standard"); Save->SaveVersion=2; UE_LOG(LogTemp,Display,TEXT("TDF Save: migrated V1 to V2; existing progress retained, pucks returned to inventory.")); break;
        default: Failure=ETDFLoadFailure::InvalidSave; Message=NSLOCTEXT("Save","MissingMigration","This save cannot be upgraded by this build."); return false;
        }
    }
    return true;
}

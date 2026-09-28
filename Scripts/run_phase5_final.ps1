$ErrorActionPreference='Stop'
& "$PSScriptRoot/run_phase5.ps1" -Step A25 -Filter 'TDF.'
if($LASTEXITCODE -ne 0){exit 1}
$root=Split-Path $PSScriptRoot -Parent
$descriptor=Join-Path $root 'Horror.uproject'
$association=(Get-Content $descriptor -Raw | ConvertFrom-Json).EngineAssociation
$engine=(Get-ItemProperty 'HKCU:\Software\Epic Games\Unreal Engine\Builds').$association
$p=Start-Process "$engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" -WindowStyle Hidden -ArgumentList ('"'+$descriptor+'"'),'-run=DataValidation','-AssetPaths=/Game/Game','-unattended','-nullrhi','-nosound',('"-abslog='+$root+'/Saved/Logs/Phase5DataValidation.log"') -Wait -PassThru
if($p.ExitCode -ne 0){throw 'Data validation failed'}
& "$PSScriptRoot/build_phase5.ps1" -Package
if($LASTEXITCODE -ne 0){exit 1}
& "$PSScriptRoot/test_phase5_package.ps1"
if($LASTEXITCODE -ne 0){exit 1}
& "$PSScriptRoot/test_phase5_package.ps1" -Rendered
if($LASTEXITCODE -ne 0){exit 1}
Set-Content "$root/Saved/Logs/Phase5FinalExit.txt" '0'

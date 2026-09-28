param([switch]$TestsOnly)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$descriptor=Join-Path $root 'Horror.uproject'
$association=(Get-Content $descriptor -Raw | ConvertFrom-Json).EngineAssociation
$engine=(Get-ItemProperty 'HKCU:\Software\Epic Games\Unreal Engine\Builds').$association
Set-Content "$root/Saved/Logs/Phase6ReleaseExit.txt" 'Running'
$deadline=(Get-Date).AddMinutes(10)
while((Get-Content "$root/Saved/Logs/Phase6_Final_exit.txt" -Raw).Trim() -eq 'Running'){
    if((Get-Date) -gt $deadline){throw 'Editor regression run did not finish'}
    Start-Sleep -Seconds 2
}
if((Get-Content "$root/Saved/Logs/Phase6_Final_exit.txt" -Raw).Trim() -ne '0'){throw 'Editor regression gate failed'}
if(!$TestsOnly){
& "$PSScriptRoot/run_phase3_assets.ps1" -Script phase6_asset_audit.py
$p=Start-Process "$engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" -WindowStyle Hidden -ArgumentList ('"'+$descriptor+'"'),'-run=DataValidation','-unattended','-nullrhi','-nosound',('"-abslog='+$root+'/Saved/Logs/Phase6DataValidation.log"') -Wait -PassThru
if($p.ExitCode -ne 0){throw 'Data validation failed'}
& "$PSScriptRoot/build_phase6.ps1" -Package
if($LASTEXITCODE -ne 0){throw 'Packaging failed'}
}
while((Get-Content "$root/Saved/Logs/Phase6PackageExit.txt" -Raw).Trim() -eq 'Running'){
    if((Get-Date) -gt $deadline){throw 'Packaging did not finish'}
    Start-Sleep -Seconds 2
}
if((Get-Content "$root/Saved/Logs/Phase6PackageExit.txt" -Raw).Trim() -ne '0'){throw 'Package gate failed'}
& "$PSScriptRoot/test_phase6_package.ps1"
& "$PSScriptRoot/test_phase6_package.ps1" -Rendered
Set-Content "$root/Saved/Logs/Phase6ReleaseExit.txt" '0'

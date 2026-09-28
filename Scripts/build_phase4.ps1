param([switch]$Package)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$descriptor = Join-Path $projectRoot 'Horror.uproject'
$association = (Get-Content -LiteralPath $descriptor -Raw | ConvertFrom-Json).EngineAssociation
$engineRoot = (Get-ItemProperty 'HKCU:\Software\Epic Games\Unreal Engine\Builds').$association
if (Get-Process UnrealEditor,UnrealEditor-Cmd,DrownedFrequency -ErrorAction SilentlyContinue) {
    throw 'Save and close Unreal before building on this memory-constrained workstation.'
}
if ($Package) {
    Set-Content -LiteralPath "$projectRoot\Saved\Logs\Phase4PackageExit.txt" -Value 'Running'
    $uat = Join-Path $engineRoot 'Engine\Build\BatchFiles\RunUAT.bat'
    & $uat BuildCookRun "-project=$descriptor" -noP4 -platform=Win64 -clientconfig=Development -build -cook -stage -pak -archive "-archivedirectory=$projectRoot\Saved\Builds\Phase4" '-ubtargs=-MaxParallelActions=1 -NoUBA' *> "$projectRoot\Saved\Logs\Phase4Package.log"
} else {
    Set-Content -LiteralPath "$projectRoot\Saved\Logs\Phase4GameBuildExit.txt" -Value 'Running'
    $build = Join-Path $engineRoot 'Engine\Build\BatchFiles\Build.bat'
    & $build DrownedFrequency Win64 Development "-Project=$descriptor" -WaitMutex -NoHotReloadFromIDE -MaxParallelActions=1 -NoUBA *> "$projectRoot\Saved\Logs\Phase4GameBuild.log"
}
$result = $LASTEXITCODE
$resultName = if ($Package) { 'Phase4PackageExit.txt' } else { 'Phase4GameBuildExit.txt' }
Set-Content -LiteralPath (Join-Path $projectRoot "Saved\Logs\$resultName") -Value $result
exit $result

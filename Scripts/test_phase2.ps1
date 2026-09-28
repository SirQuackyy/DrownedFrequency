param([Parameter(Mandatory=$true)][int]$Task)
$ErrorActionPreference = 'Stop'
if (Get-Process UnrealEditor,UnrealEditor-Cmd,DrownedFrequency -ErrorAction SilentlyContinue) {
    throw 'Save and close Unreal and other project runs before starting the test runner.'
}
$projectRoot = Split-Path $PSScriptRoot -Parent
$descriptor = Join-Path $projectRoot 'Horror.uproject'
$association = (Get-Content -LiteralPath $descriptor -Raw | ConvertFrom-Json).EngineAssociation
$engineRoot = (Get-ItemProperty 'HKCU:\Software\Epic Games\Unreal Engine\Builds').$association
$editor = Join-Path $engineRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$logPath = Join-Path $projectRoot "Saved\Logs\Phase2Task${Task}Test.log"
$consolePath = Join-Path $projectRoot "Saved\Logs\Phase2Task${Task}TestConsole.log"
$reportPath = Join-Path $projectRoot "Saved\Tests\Phase2Task${Task}"
& $editor $descriptor /Game/Game/Maps/Test/L_TDF_InputGym -unattended -nullrhi -nosound '-ExecCmds=Automation RunTests TDF.Player;Quit' '-TestExit=Automation Test Queue Empty' "-ReportExportPath=$reportPath" "-abslog=$logPath" *> $consolePath
$result = $LASTEXITCODE
Select-String -LiteralPath $logPath -Pattern 'Test Completed','LogAutomationController: Error:','LogWindows: Error:'
if ($result -ne 0) { exit $result }
$report = Get-Content -LiteralPath (Join-Path $reportPath 'index.json') -Raw | ConvertFrom-Json
if ($report.failed -gt 0 -or $report.succeeded -lt 1) { throw 'Phase 2 automation did not pass.' }
Write-Output "Phase 2 task $Task : $($report.succeeded) tests passed, $($report.failed) failed."

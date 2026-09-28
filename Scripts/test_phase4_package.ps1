param([switch]$Rendered)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$packageRoot = Join-Path $projectRoot 'Saved\Builds\Phase4\Windows'
$exe = Get-ChildItem -LiteralPath $packageRoot -Filter DrownedFrequency.exe -Recurse | Where-Object FullName -Match 'Binaries\\Win64' | Select-Object -First 1
if (-not $exe) { throw 'Build the Phase 4 package first.' }
if (Get-Process UnrealEditor,UnrealEditor-Cmd,DrownedFrequency -ErrorAction SilentlyContinue) { throw 'Close Unreal and other project runs before testing.' }
$association = (Get-Content -LiteralPath (Join-Path $projectRoot 'Horror.uproject') -Raw | ConvertFrom-Json).EngineAssociation
$engineRoot = (Get-ItemProperty 'HKCU:\Software\Epic Games\Unreal Engine\Builds').$association
# The installed engine's report template is not staged with game assets by default.
$templateDirectory = Join-Path $packageRoot 'Engine\Content\Automation'
New-Item -ItemType Directory -Path $templateDirectory -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $engineRoot 'Engine\Content\Automation\Report-Template.html') -Destination (Join-Path $templateDirectory 'Report-Template.html')
$name = if ($Rendered) { 'Phase4Rendered' } else { 'Phase4Standalone' }
$test = if ($Rendered) { 'TDF.Visual.Phase4Captures' } else { 'TDF.Acoustic.Standalone' }
$reportPath = Join-Path $projectRoot "Saved\Tests\$name"
$logPath = Join-Path $projectRoot "Saved\Logs\$name.log"
$arguments = @('/Game/Game/Maps/Test/M_Test_AcousticSpine', '-unattended',  '-windowed', '-ResX=1280', '-ResY=720', '-log', '-stdout', '-FullStdOutLogOutput', "-ExecCmds=Automation RunTests $test;Quit", '-TestExit=Automation Test Queue Empty', "-ReportExportPath=$reportPath", "-abslog=$logPath")
if (-not $Rendered) { $arguments += '-nullrhi' }
$quotedArguments = ($arguments | ForEach-Object { '"' + $_.Replace('"', '\"') + '"' }) -join ' '
$started = Get-Date
$process = Start-Process -FilePath $exe.FullName -ArgumentList $quotedArguments -WindowStyle Hidden -PassThru -Wait -RedirectStandardOutput (Join-Path $projectRoot "Saved\Logs\${name}Console.log")
$result = $process.ExitCode
Select-String -LiteralPath $logPath -Pattern 'Test Completed','LogAutomationController: Error:','Requested rendered capture','TEST COMPLETE'
if ($result -ne 0) { exit $result }
$reportFile = Get-Item -LiteralPath (Join-Path $reportPath 'index.json')
if ($reportFile.LastWriteTime -lt $started) { throw 'The test did not produce a fresh report.' }
$report = Get-Content -LiteralPath $reportFile.FullName -Raw | ConvertFrom-Json
if ($report.failed -gt 0 -or ($report.succeeded+$report.succeededWithWarnings) -lt $(if($Rendered){1}else{2})) { throw "$name did not pass." }
if ($Rendered) {
    foreach ($capture in @('Phase4Entrance.png','Phase4Recorder.png','Phase4Corner.png')) {
        $captureFile = Get-Item -LiteralPath (Join-Path $packageRoot "Horror\Saved\Screenshots\$capture")
        if ($captureFile.LastWriteTime -lt $started) { throw "Stale visual capture: $capture" }
    }
}
Write-Output "$name passed."
exit 0

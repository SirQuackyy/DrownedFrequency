param([string]$Step='Schema', [switch]$BuildOnly, [string]$Filter='TDF.Save')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$project=Join-Path $root 'Horror.uproject'
$association=(Get-Content $project -Raw | ConvertFrom-Json).EngineAssociation
$engine=(Get-ItemProperty 'HKCU:\Software\Epic Games\Unreal Engine\Builds').$association
if(Get-Process UnrealEditor,UnrealEditor-Cmd,DrownedFrequency -ErrorAction SilentlyContinue){throw 'Close project instances before validation.'}
$prefix=Join-Path $root "Saved/Logs/Phase6_$Step"
Set-Content "${prefix}_exit.txt" 'Running'
& "$engine/Engine/Build/BatchFiles/Build.bat" DrownedFrequencyEditor Win64 Development "-Project=$project" -WaitMutex -NoHotReloadFromIDE -MaxParallelActions=1 -NoUBA *> "${prefix}_build.log"
if($LASTEXITCODE -ne 0){Set-Content "${prefix}_exit.txt" "Build failed: $LASTEXITCODE"; exit 1}
if(!$BuildOnly){
 $report=Join-Path $root "Saved/Tests/Phase6_$Step"
 $args=@(('"'+$project+'"'),'/Game/Game/Maps/Test/L_TDF_InputGym','-unattended','-nullrhi','-nosound',('"-ExecCmds=Automation RunTests '+$Filter+';Quit"'),'"-TestExit=Automation Test Queue Empty"',('"-ReportExportPath='+$report+'"'),('"-abslog='+$prefix+'_test.log"'))
 $started=Get-Date
 $p=Start-Process "$engine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" -WindowStyle Hidden -ArgumentList $args -Wait -PassThru -RedirectStandardOutput "${prefix}_stdout.log"
 if($p.ExitCode -ne 0){Set-Content "${prefix}_exit.txt" "Tests exited: $($p.ExitCode)"; exit 1}
 if((Get-Item "$report/index.json").LastWriteTime -lt $started){throw "Stale automation report"}
 $r=Get-Content "$report/index.json" -Raw | ConvertFrom-Json
 if($r.failed -gt 0 -or ($r.succeeded+$r.succeededWithWarnings) -lt 1){Set-Content "${prefix}_exit.txt" "Tests failed: $($r.failed)"; exit 1}
}
Set-Content "${prefix}_exit.txt" '0'

$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$project=Join-Path $root 'Horror.uproject'
$association=(Get-Content $project -Raw | ConvertFrom-Json).EngineAssociation
$engine=(Get-ItemProperty 'HKCU:\Software\Epic Games\Unreal Engine\Builds').$association
if(Get-Process UnrealEditor,UnrealEditor-Cmd,DrownedFrequency -ErrorAction SilentlyContinue){throw 'Close project instances first.'}
$report=Join-Path $root 'Saved/Tests/Phase4EditorRendered'
$log=Join-Path $root 'Saved/Logs/Phase4EditorRendered.log'
Set-Content "$log.exit" 'Running'
$arguments=@($project,'/Game/Game/Maps/Test/M_Test_AcousticSpine','-game','-unattended','-windowed','-ResX=1280','-ResY=720','-ExecCmds=Automation RunTests TDF.Visual.Phase4Captures;Quit','-TestExit=Automation Test Queue Empty',"-ReportExportPath=$report","-abslog=$log")
$quoted=($arguments | ForEach-Object {'"'+$_+'"'}) -join ' '
$started=Get-Date
$p=Start-Process "$engine/Engine/Binaries/Win64/UnrealEditor.exe" -WindowStyle Hidden -ArgumentList $quoted -Wait -PassThru
Set-Content "$log.exit" $p.ExitCode
if($p.ExitCode -ne 0){exit $p.ExitCode}
if((Get-Item "$report/index.json").LastWriteTime -lt $started){throw 'Stale rendered report'}
$r=Get-Content "$report/index.json" -Raw | ConvertFrom-Json
if($r.failed -gt 0 -or ($r.succeeded+$r.succeededWithWarnings) -ne 1){throw 'Rendered test failed'}
foreach($file in 'Phase4Entrance.png','Phase4Recorder.png','Phase4Corner.png'){
 if((Get-Item (Join-Path $root "Saved/Screenshots/$file")).LastWriteTime -lt $started){throw "Stale screenshot $file"}
}

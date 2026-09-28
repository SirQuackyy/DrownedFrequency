$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$project=Join-Path $root 'Horror.uproject'
$association=(Get-Content $project -Raw | ConvertFrom-Json).EngineAssociation
$engine=(Get-ItemProperty 'HKCU:\Software\Epic Games\Unreal Engine\Builds').$association
if(Get-Process UnrealEditor,UnrealEditor-Cmd,DrownedFrequency -ErrorAction SilentlyContinue){throw 'Close project instances first.'}
$report=Join-Path $root 'Saved/Tests/Phase3EditorRendered'
$log=Join-Path $root 'Saved/Logs/Phase3EditorRendered.log'
Set-Content "$log.exit" 'Running'
$arguments=@($project,'/Game/Game/Maps/Test/M_Test_PlayerInteraction','-game','-unattended','-nosound','-windowed','-ResX=1280','-ResY=720','-ExecCmds=Automation RunTests TDF.Visual.Phase3Captures;Quit','-TestExit=Automation Test Queue Empty',"-ReportExportPath=$report","-abslog=$log")
$quoted=($arguments | ForEach-Object {'"'+$_+'"'}) -join ' '
$started=Get-Date
$p=Start-Process "$engine/Engine/Binaries/Win64/UnrealEditor.exe" -WindowStyle Hidden -ArgumentList $quoted -Wait -PassThru
Set-Content "$log.exit" $p.ExitCode
if($p.ExitCode -ne 0){exit $p.ExitCode}
if((Get-Item "$report/index.json").LastWriteTime -lt $started){throw 'Stale rendered report'}
$r=Get-Content "$report/index.json" -Raw | ConvertFrom-Json
if($r.failed -gt 0 -or ($r.succeeded+$r.succeededWithWarnings) -ne 1){throw 'Rendered test failed'}
foreach($file in 'Phase3Entrance.png','Phase3Corner.png','Phase3Crawl.png','Phase3Inspection.png'){
 if((Get-Item (Join-Path $root "Saved/Screenshots/$file")).LastWriteTime -lt $started){throw "Stale screenshot $file"}
}

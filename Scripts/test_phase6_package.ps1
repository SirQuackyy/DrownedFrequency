param([switch]$Rendered)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$package=Join-Path $root 'Saved/Builds/Phase6/Windows'
$exe=Get-ChildItem -LiteralPath $package -Filter DrownedFrequency.exe -Recurse | Where-Object FullName -Match 'Binaries\\Win64' | Select-Object -First 1
if(!$exe){throw 'Build the Phase 6 package first.'}
if(Get-Process UnrealEditor,UnrealEditor-Cmd,DrownedFrequency -ErrorAction SilentlyContinue){throw 'Close project instances before validation.'}
$association=(Get-Content (Join-Path $root 'Horror.uproject') -Raw | ConvertFrom-Json).EngineAssociation
$engine=(Get-ItemProperty 'HKCU:\Software\Epic Games\Unreal Engine\Builds').$association
$template=Join-Path $package 'Engine/Content/Automation'
New-Item -ItemType Directory -Path $template -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $engine 'Engine/Content/Automation/Report-Template.html') -Destination $template
$runs=if($Rendered){@{Name='Rendered';Filter='TDF.Visual.Phase6Captures'}}else{@(
    @{Name='Flow';Filter='TDF.Save.Flow+TDF.Save.Checkpoint+TDF.Save.Death+TDF.Save.Progression+TDF.Acoustic.A24'},
    @{Name='Write';Filter='TDF.Save.Relaunch.Write'},
    @{Name='Read';Filter='TDF.Save.Relaunch.Read'}
)}
foreach($run in $runs){
    $name='Phase6Package'+$run.Name; $filter=$run.Filter
    $report=Join-Path $root "Saved/Tests/$name"; $log=Join-Path $root "Saved/Logs/$name.log"
    $arguments=@('/Game/Game/Maps/Test/L_TDF_InputGym','-unattended','-windowed','-ResX=1280','-ResY=720','-stdout',"-ExecCmds=Automation RunTests $filter;Quit",'-TestExit=Automation Test Queue Empty',"-ReportExportPath=$report","-abslog=$log")
    if(!$Rendered){$arguments+='-nullrhi'; $arguments+='-nosound'}
    $quoted=($arguments | ForEach-Object {'"'+$_+'"'}) -join ' '
    $started=Get-Date
    $p=Start-Process -FilePath $exe.FullName -ArgumentList $quoted -WindowStyle Hidden -Wait -PassThru -RedirectStandardOutput (Join-Path $root "Saved/Logs/${name}Console.log")
    if($p.ExitCode -ne 0){throw "$name exited $($p.ExitCode)"}
    $file=Get-Item "$report/index.json"; if($file.LastWriteTime -lt $started){throw 'Stale automation report'}
    $r=Get-Content $file.FullName -Raw | ConvertFrom-Json
    if($r.failed -gt 0 -or ($r.succeeded+$r.succeededWithWarnings) -lt 1){throw "$name failed"}
    Write-Output "$name passed: $($r.succeeded+$r.succeededWithWarnings) tests."
}
if($Rendered){
    foreach($name in @('Phase6Archive.png','Phase6Caption.png','Phase6Radio.png')){
        if((Get-Item (Join-Path $package "Horror/Saved/Screenshots/$name")).LastWriteTime -lt $started){throw "Stale capture $name"}
    }
}

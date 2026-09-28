param([Parameter(Mandatory=$true)][string]$Script, [switch]$Rendered, [switch]$KeepOpen)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$project=Join-Path $root 'Horror.uproject'
$association=(Get-Content $project -Raw | ConvertFrom-Json).EngineAssociation
$engine=(Get-ItemProperty 'HKCU:\Software\Epic Games\Unreal Engine\Builds').$association
if(Get-Process UnrealEditor,UnrealEditor-Cmd,DrownedFrequency -ErrorAction SilentlyContinue){throw 'Close project instances first.'}
$scriptPath=(Resolve-Path (Join-Path $PSScriptRoot $Script)).Path
$log=Join-Path $root ('Saved/Logs/'+[IO.Path]::GetFileNameWithoutExtension($Script)+'.log')
$args=@(('"'+$project+'"'),'-unattended','-nosound',('"-ExecutePythonScript='+$scriptPath+'"'),('"-abslog='+$log+'"'))
if(!$Rendered){$args+='-nullrhi'}
$p=Start-Process "$engine/Engine/Binaries/Win64/UnrealEditor.exe" -WindowStyle Hidden -ArgumentList $args -PassThru
if(!$KeepOpen){
 $p.WaitForExit()
 if($p.ExitCode -ne 0){throw "Asset script process exited $($p.ExitCode)"}
 if(Select-String $log -Pattern 'LogPython: Error:'){throw 'Python asset script failed; inspect log.'}
}

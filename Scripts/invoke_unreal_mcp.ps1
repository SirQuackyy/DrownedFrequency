param([string]$Tool, [hashtable]$Arguments=@{}, [string]$Toolset='SlateInspectorToolset.SlateInspectorToolset', [string]$MetaTool='call_tool', [switch]$ResetSession)
$ErrorActionPreference = 'Stop'
$uri = 'http://127.0.0.1:8000/mcp'
$headers = @{Accept='application/json, text/event-stream'}
$init = @{jsonrpc='2.0';id=1;method='initialize';params=@{protocolVersion='2025-11-25';capabilities=@{};clientInfo=@{name='phase2-editor';version='1.0'}}} | ConvertTo-Json -Depth 12 -Compress
$sessionFile = Join-Path (Split-Path $PSScriptRoot -Parent) 'Saved\Logs\Phase2McpSession.txt'
if ($ResetSession -or -not (Test-Path -LiteralPath $sessionFile)) {
    $response = Invoke-WebRequest -Uri $uri -Method Post -Body $init -ContentType 'application/json' -Headers $headers -TimeoutSec 20
    Set-Content -LiteralPath $sessionFile -Value $response.Headers['Mcp-Session-Id'][0]
}
$headers['Mcp-Session-Id'] = (Get-Content -LiteralPath $sessionFile -Raw).Trim()
$params = if ($MetaTool -eq 'call_tool') { @{toolset_name=$Toolset;tool_name=$Tool;arguments=$Arguments} } else { $Arguments }
$body = @{jsonrpc='2.0';id=2;method='tools/call';params=@{name=$MetaTool;arguments=$params}} | ConvertTo-Json -Depth 32 -Compress
$reply = (Invoke-WebRequest -Uri $uri -Method Post -Body $body -ContentType 'application/json' -Headers $headers -TimeoutSec 60).Content | ConvertFrom-Json
if ($reply.error) { throw ($reply.error | ConvertTo-Json -Compress) }
if ($reply.result.isError) { throw ($reply.result.content | ConvertTo-Json -Compress) }
$reply.result.content | Where-Object type -eq 'text' | ForEach-Object { $_.text }

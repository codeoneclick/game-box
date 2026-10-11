param([string]$Label='before', [ValidateSet('release','debug')][string]$Configuration='release', [int]$DebugMode=0)
$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$output=Join-Path $repoRoot "gbDemo\output\island-check\quality-$Label-$DebugMode"
New-Item -ItemType Directory -Path $output -Force | Out-Null
$keys=@('VK_INSTANCE_LAYERS','VK_SCREENSHOT_FRAMES','VK_SCREENSHOT_DIR','GB_OCEAN_FIXED_TIME','GB_RESIDENT_COUNT','GB_OCEAN_DEBUG')
$saved=@{}
foreach($key in $keys){$saved[$key]=[Environment]::GetEnvironmentVariable($key)}
$env:VK_INSTANCE_LAYERS='VK_LAYER_LUNARG_screenshot'
$env:VK_SCREENSHOT_FRAMES='360'
$env:VK_SCREENSHOT_DIR=$output
$env:GB_OCEAN_FIXED_TIME='3'
$env:GB_RESIDENT_COUNT='0'
$env:GB_OCEAN_DEBUG="$DebugMode"
$demo=$null
try {
 $demo=Start-Process -FilePath (Join-Path $repoRoot "gbDemo\output\$Configuration\x64\gb_demo.exe") -WorkingDirectory (Join-Path $repoRoot 'gbDemo') -WindowStyle Hidden -RedirectStandardOutput "$output\runtime.log" -RedirectStandardError "$output\errors.log" -PassThru
 $watch=[Diagnostics.Stopwatch]::StartNew()
 while(!(Test-Path "$output\360.ppm") -and !$demo.HasExited -and $watch.Elapsed.TotalSeconds -lt 90){Start-Sleep -Milliseconds 100}
 if(!(Test-Path "$output\360.ppm")){throw 'Quality capture missing.'}
 if((Get-Content "$output\errors.log" -Raw) -match '(?i)validation layer:|error:|failed'){throw 'Quality capture has runtime errors.'}
 "Captured: $output"
} finally {
 if($demo -and !$demo.HasExited){$demo.CloseMainWindow() | Out-Null; if(!$demo.WaitForExit(5000)){Stop-Process -Id $demo.Id}}
 foreach($key in $keys){[Environment]::SetEnvironmentVariable($key,$saved[$key])}
}

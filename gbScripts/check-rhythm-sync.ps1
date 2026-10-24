$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$vsRoot=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$out=Join-Path $repoRoot 'gbDemo/output/rhythm-check/sync-probe'
New-Item -ItemType Directory -Force $out | Out-Null
$sources=@('gbScripts/check_rhythm_sync.cpp','gbDemo/sources/rhythm_track.cpp','gbDemo/sources/rhythm_track_catalog.cpp','gbDemo/sources/rhythm_calibration.cpp','gbDemo/sources/rhythm_track_loader.cpp','gbSound/sources/rhythm_pcm.cpp','gbCore/libraries/jsoncpp-1.7.2/jsoncpp.cpp','gbDemo/sources/rhythm_ship_playback.cpp','gbDemo/sources/rhythm_session.cpp','gbDemo/sources/fake_leaderboard.cpp','gbSound/sources/rhythm_audio_transport.cpp','gbDemo/sources/rhythm_choreography.cpp','gbDemo/sources/rhythm_crowd_choreography.cpp','gbDemo/sources/rhythm_prompt_model.cpp')
$quoted=($sources | ForEach-Object {'"'+(Join-Path $repoRoot $_)+'"'}) -join ' '
$cmd='@call "'+$vsRoot+'/VC/Auxiliary/Build/vcvars64.bat" >nul'+"`r`n"+'@cl /nologo /EHsc /std:c++14 /I"'+$repoRoot+'/gbSound/sources" /I"'+$repoRoot+'/gbDemo/sources" /I"'+$repoRoot+'/gbCore/libraries" '+$quoted+' winmm.lib /Fe:rhythm_sync_check.exe'
Set-Content (Join-Path $out 'compile-sync.cmd') $cmd -Encoding ASCII
Push-Location $out
try{& cmd /c compile-sync.cmd;if($LASTEXITCODE -ne 0){throw 'Sync compilation failed'};& ./rhythm_sync_check.exe;if($LASTEXITCODE -ne 0){throw 'Sync checks failed'}}finally{Pop-Location}

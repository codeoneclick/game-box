$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$vsRoot=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$out=Join-Path $repoRoot 'gbDemo/output/rhythm-check'
New-Item -ItemType Directory -Force $out | Out-Null
$sources=@('gbScripts/check_rhythm_music.cpp','gbDemo/sources/rhythm_track.cpp','gbDemo/sources/rhythm_track_catalog.cpp','gbDemo/sources/rhythm_calibration.cpp','gbDemo/sources/rhythm_track_loader.cpp','gbSound/sources/rhythm_pcm.cpp','gbCore/libraries/jsoncpp-1.7.2/jsoncpp.cpp')
$quoted=($sources | ForEach-Object {'"'+(Join-Path $repoRoot $_)+'"'}) -join ' '
$quoted+=' "'+$repoRoot+'/gbDemo/sources/rhythm_ship_playback.cpp" "'+$repoRoot+'/gbDemo/sources/rhythm_session.cpp" "'+$repoRoot+'/gbDemo/sources/fake_leaderboard.cpp" "'+$repoRoot+'/gbSound/sources/rhythm_audio_transport.cpp" winmm.lib'
$cmd='@call "'+$vsRoot+'/VC/Auxiliary/Build/vcvars64.bat" >nul'+"`r`n"+'@cl /nologo /EHsc /std:c++14 /I"'+$repoRoot+'/gbSound/sources" /I"'+$repoRoot+'/gbDemo/sources" /I"'+$repoRoot+'/gbCore/libraries" '+$quoted+' /Fe:rhythm_music_check.exe'
Set-Content (Join-Path $out 'compile-music.cmd') $cmd -Encoding ASCII
Push-Location $out
try{& cmd /c compile-music.cmd;if($LASTEXITCODE -ne 0){throw 'Music compilation failed'};& ./rhythm_music_check.exe;if($LASTEXITCODE -ne 0){throw 'Music checks failed'}}finally{Pop-Location}

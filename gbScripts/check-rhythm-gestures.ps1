$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$vsRoot=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$out=Join-Path $repoRoot 'gbDemo/output/rhythm-check'
New-Item -ItemType Directory -Force $out | Out-Null
$sources=@('gbScripts/check_rhythm_gestures.cpp','gbDemo/sources/rhythm_choreography.cpp','gbDemo/sources/rhythm_track.cpp','gbDemo/sources/rhythm_session.cpp','gbCore/libraries/jsoncpp-1.7.2/jsoncpp.cpp')
$quoted=($sources | ForEach-Object {'"'+(Join-Path $repoRoot $_)+'"'}) -join ' '
$quoted+=' "'+$repoRoot+'/gbDemo/sources/rhythm_gesture.cpp"'
$quoted+=' "'+$repoRoot+'/gbDemo/sources/rhythm_dance_pose.cpp" "'+$repoRoot+'/gbDemo/sources/rhythm_crowd_choreography.cpp"'
$quoted+=' "'+$repoRoot+'/gbDemo/sources/rhythm_prompt_model.cpp"'
$quoted+=' "'+$repoRoot+'/gbDemo/sources/rhythm_stage_state.cpp"'
$cmd='@call "'+$vsRoot+'/VC/Auxiliary/Build/vcvars64.bat" >nul'+"`r`n"+'@cl /nologo /EHsc /std:c++14 /I"'+$repoRoot+'/gbDemo/sources" /I"'+$repoRoot+'/gbCore/libraries" '+$quoted+' /Fe:rhythm_gestures_check.exe'
Set-Content (Join-Path $out 'compile-gestures.cmd') $cmd -Encoding ASCII
Push-Location $out
try { & cmd /c compile-gestures.cmd;if($LASTEXITCODE -ne 0){throw 'Gesture compilation failed'};& ./rhythm_gestures_check.exe;if($LASTEXITCODE -ne 0){throw 'Gesture checks failed'} }finally{Pop-Location}

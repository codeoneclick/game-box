$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vsRoot=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$out=Join-Path $repoRoot 'gbDemo/output/rhythm-check'
New-Item -ItemType Directory -Force $out | Out-Null
$cmd='@call "'+$vsRoot+'/VC/Auxiliary/Build/vcvars64.bat" >nul'+"`r`n"+'@cl /nologo /EHsc /std:c++14 /I"'+$repoRoot+'/gbSound/sources" "'+$repoRoot+'/gbScripts/check_rhythm_audio.cpp" "'+$repoRoot+'/gbSound/sources/rhythm_audio_transport.cpp" winmm.lib /Fe:rhythm_audio_check.exe'
Set-Content (Join-Path $out 'compile-audio.cmd') $cmd -Encoding ASCII
Push-Location $out
try{& cmd /c compile-audio.cmd;if($LASTEXITCODE -ne 0){throw 'Audio compilation failed'};& ./rhythm_audio_check.exe;if($LASTEXITCODE -ne 0){throw 'Audio checks failed'}}finally{Pop-Location}

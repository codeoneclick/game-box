$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vsRoot=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$checkOutput=Join-Path $repoRoot 'gbDemo/output/rhythm-check'
New-Item -ItemType Directory -Force $checkOutput | Out-Null
$vcvars=Join-Path $vsRoot 'VC/Auxiliary/Build/vcvars64.bat'
$sources=@('gbScripts/check_rhythm.cpp','gbDemo/sources/rhythm_track.cpp','gbDemo/sources/rhythm_session.cpp','gbDemo/sources/fake_leaderboard.cpp','gbCore/libraries/jsoncpp-1.7.2/jsoncpp.cpp')
$quoted=($sources | ForEach-Object {'"'+(Join-Path $repoRoot $_)+'"'}) -join ' '
$command='@call "'+$vcvars+'" >nul'+"`r`n"+'@cl /nologo /EHsc /std:c++14 /I"'+$repoRoot+'/gbDemo/sources" /I"'+$repoRoot+'/gbCore/libraries" '+$quoted+' /Fe:rhythm_check.exe'
Set-Content (Join-Path $checkOutput 'compile.cmd') $command -Encoding ASCII
Push-Location $checkOutput
try { & cmd /c compile.cmd; if($LASTEXITCODE -ne 0){throw 'Rhythm compilation failed'}; & ./rhythm_check.exe; if($LASTEXITCODE -ne 0){throw 'Rhythm checks failed'} } finally {Pop-Location}

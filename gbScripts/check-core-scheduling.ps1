$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$vsRoot=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if(!$vsRoot){throw 'Visual Studio C++ tools missing.'}
$out=Join-Path $repoRoot 'gbDemo/output/island-check'
New-Item -ItemType Directory -Path $out -Force | Out-Null
Push-Location $out
$savedPath=$env:Path
$env:Path=(Split-Path -Parent $vswhere)+';'+$env:Path
try {
 $command='@call "'+$vsRoot+'\VC\Auxiliary\Build\vcvars64.bat" >nul'+"`r`n"
 $command+='@cl /nologo /EHsc /std:c++14 /O2 "'+$repoRoot+'\gbScripts\check_core_scheduling.cpp" /Fe:core_scheduling_check.exe'
 Set-Content -LiteralPath compile-core-scheduling.cmd -Value $command -Encoding ASCII
 & cmd /c .\compile-core-scheduling.cmd
 if($LASTEXITCODE -ne 0){throw 'Core scheduling compile failed.'}
 & ./core_scheduling_check.exe
 if($LASTEXITCODE -ne 0){throw 'Core scheduling checks failed.'}
} finally {$env:Path=$savedPath;Pop-Location}

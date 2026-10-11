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
 $command+='@cl /nologo /EHsc /std:c++14 /I"'+$repoRoot+'\gbCore\sources" "'+$repoRoot+'\gbScripts\check_render_statistics.cpp" /Fe:render_statistics_check.exe'
 Set-Content -LiteralPath compile-render-statistics.cmd -Value $command -Encoding ASCII
 & cmd /c .\compile-render-statistics.cmd
 if($LASTEXITCODE -ne 0){throw 'Render statistics compile failed.'}
 & ./render_statistics_check.exe
 if($LASTEXITCODE -ne 0){throw 'Render statistics checks failed.'}
} finally {$env:Path=$savedPath;Pop-Location}

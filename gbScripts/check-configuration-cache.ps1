$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$vsRoot=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if(!$vsRoot){throw 'Visual Studio C++ tools required.'}
$out=Join-Path $repoRoot 'gbDemo/output/island-check'
New-Item -ItemType Directory -Path $out -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $out 'configuration-cache-bundle') -Force | Out-Null
Push-Location $out
$savedPath=$env:Path
$env:Path=(Split-Path -Parent $vswhere)+';'+$env:Path
try {
 $command='@call "'+$vsRoot+'\VC\Auxiliary\Build\vcvars64.bat" >nul'+"`r`n"
 $command+='@cl /nologo /EHsc /std:c++14 /O2 /I"'+$repoRoot+'\gbCore\libraries" "'+$repoRoot+'\gbScripts\check_configuration_cache.cpp" "'+$repoRoot+'\gbCore\libraries\pugixml\pugixml.cpp" "'+$repoRoot+'\gbCore\libraries\jsoncpp-1.7.2\jsoncpp.cpp" /Fe:configuration_cache_check.exe'
 Set-Content -LiteralPath compile-configuration-cache.cmd -Value $command -Encoding ASCII
 & cmd /c .\compile-configuration-cache.cmd
 if($LASTEXITCODE -ne 0){throw 'Configuration cache compile failed.'}
 & ./configuration_cache_check.exe
 if($LASTEXITCODE -ne 0){throw 'Configuration cache checks failed.'}
} finally {$env:Path=$savedPath;Pop-Location}

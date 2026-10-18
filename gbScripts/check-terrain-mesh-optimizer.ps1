$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vsRoot=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$vcvars=Join-Path $vsRoot 'VC\Auxiliary\Build\vcvars64.bat'
$originalPath=$env:Path
$env:Path=(Split-Path -Parent $vswhere)+';'+$env:Path
$checkOutput=Join-Path $repoRoot 'gbDemo\output\island-check\terrain-lod-work'
New-Item -ItemType Directory $checkOutput -Force|Out-Null
Push-Location $checkOutput
try {
 $compile='@call "{0}" >nul' -f $vcvars
 $compile+="`r`n"+('@cl /nologo /O2 /EHsc /std:c++14 /I"{0}\gbDemo\sources" "{0}\gbScripts\check_terrain_mesh_optimizer.cpp" "{0}\gbDemo\sources\terrain_mesh_optimizer.cpp" /Fe:terrain_optimizer_check.exe' -f $repoRoot)
 Set-Content -LiteralPath compile-optimizer.cmd -Value $compile -Encoding ASCII
 & cmd /c .\compile-optimizer.cmd
 if($LASTEXITCODE -ne 0){throw 'Terrain optimizer CPU compilation failed'}
 & .\terrain_optimizer_check.exe
 if($LASTEXITCODE -ne 0){throw 'Terrain optimizer CPU checks failed'}
}finally{Pop-Location;$env:Path=$originalPath}

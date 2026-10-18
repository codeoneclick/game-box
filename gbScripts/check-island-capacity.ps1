$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vsRoot = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vsRoot) { throw 'Visual Studio C++ tools were not found.' }
$vcvars = Join-Path $vsRoot 'VC\Auxiliary\Build\vcvars64.bat'
$originalPath = $env:Path
$env:Path = (Split-Path -Parent $vswhere) + ';' + $env:Path
$checkOutput = Join-Path $repoRoot 'gbDemo\output\capacity-check'
New-Item -ItemType Directory -Path $checkOutput -Force | Out-Null
Push-Location $checkOutput
try {
    $compileCommand = '@call "{0}" >nul' -f $vcvars
    $compileCommand += "`r`n" + ('@cl /nologo /O2 /EHsc /std:c++14 /I"{0}\gbDemo\sources" "{0}\gbScripts\check_island_capacity.cpp" "{0}\gbDemo\sources\terrain_generator.cpp" "{0}\gbDemo\sources\terrain_mesh_builder.cpp" "{0}\gbDemo\sources\voxel_tree_builder.cpp" "{0}\gbDemo\sources\building_placement.cpp" /Fe:island_capacity_check.exe' -f $repoRoot)
    Set-Content -LiteralPath .\compile-check.cmd -Value $compileCommand -Encoding ASCII
    & cmd /c .\compile-check.cmd
    if ($LASTEXITCODE -ne 0) { throw 'Island capacity CPU check compilation failed.' }
    & .\island_capacity_check.exe
    if ($LASTEXITCODE -ne 0) { throw 'Island capacity CPU check failed.' }
} finally { Pop-Location; $env:Path = $originalPath }

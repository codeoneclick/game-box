$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vsRoot = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vsRoot) { throw 'Visual Studio C++ tools were not found.' }
$vcvars = Join-Path $vsRoot 'VC\Auxiliary\Build\vcvars64.bat'
$originalPath = $env:Path
$env:Path = (Split-Path -Parent $vswhere) + ';' + $env:Path
$checkOutput = Join-Path $repoRoot 'gbDemo\output\island-check'
New-Item -ItemType Directory -Path $checkOutput -Force | Out-Null
Push-Location $checkOutput
try {
    $command = '@call "{0}" >nul' -f $vcvars
    $command += "`r`n" + ('@cl /nologo /EHsc /std:c++14 /I"{0}\gbDemo\sources" "{0}\gbScripts\check_stairs_state.cpp" "{0}\gbDemo\sources\terrain_editor_state.cpp" "{0}\gbDemo\sources\building_placement.cpp" "{0}\gbDemo\sources\stairs.cpp" /Fe:stairs_state_check.exe' -f $repoRoot)
    Set-Content -LiteralPath .\compile-stairs-state.cmd -Value $command -Encoding ASCII
    & cmd /c .\compile-stairs-state.cmd
    if ($LASTEXITCODE -ne 0) { throw 'Resident CPU compilation failed.' }
    & .\stairs_state_check.exe
    if ($LASTEXITCODE -ne 0) { throw 'Resident CPU checks failed.' }
    $command = '@call "{0}" >nul' -f $vcvars
    $command += "`r`n" + ('@cl /nologo /EHsc /std:c++14 /I"{0}\gbDemo\sources" "{0}\gbScripts\check_stairs_geometry.cpp" "{0}\gbDemo\sources\building_placement.cpp" "{0}\gbDemo\sources\stairs.cpp" /Fe:stairs_geometry_check.exe' -f $repoRoot)
    Set-Content -LiteralPath .\compile-stairs-geometry.cmd -Value $command -Encoding ASCII
    & cmd /c .\compile-stairs-geometry.cmd
    if ($LASTEXITCODE -ne 0) { throw 'Stairs geometry compilation failed.' }
    & .\stairs_geometry_check.exe
    if ($LASTEXITCODE -ne 0) { throw 'Stairs geometry checks failed.' }
} finally { Pop-Location; $env:Path = $originalPath }



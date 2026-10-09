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
    $command += "`r`n" + ('@cl /nologo /EHsc /std:c++14 /I"{0}\gbDemo\sources" "{0}\gbScripts\check_island_resident.cpp" "{0}\gbDemo\sources\terrain_generator.cpp" "{0}\gbDemo\sources\terrain_mesh_builder.cpp" "{0}\gbDemo\sources\resident_navigation.cpp" "{0}\gbDemo\sources\resident_behavior.cpp" /Fe:resident_check.exe' -f $repoRoot)
    Set-Content -LiteralPath .\compile-resident.cmd -Value $command -Encoding ASCII
    & cmd /c .\compile-resident.cmd
    if ($LASTEXITCODE -ne 0) { throw 'Resident CPU compilation failed.' }
    & .\resident_check.exe
    if ($LASTEXITCODE -ne 0) { throw 'Resident CPU checks failed.' }
    $command = '@call "{0}" >nul' -f $vcvars
    $command += "`r`n" + ('@cl /nologo /EHsc /std:c++14 /O2 /I"{0}\gbCore\libraries\ufbx" "{0}\gbScripts\check_resident_fbx.cpp" "{0}\gbCore\libraries\ufbx\ufbx.c" /Fe:resident_fbx_check.exe' -f $repoRoot)
    Set-Content -LiteralPath .\compile-resident-fbx.cmd -Value $command -Encoding ASCII
    & cmd /c .\compile-resident-fbx.cmd
    if ($LASTEXITCODE -ne 0) { throw 'Resident FBX compilation failed.' }
    & .\resident_fbx_check.exe (Join-Path $repoRoot 'gbBundle\Meshes')
    if ($LASTEXITCODE -ne 0) { throw 'Resident FBX checks failed.' }
} finally { Pop-Location; $env:Path = $originalPath }

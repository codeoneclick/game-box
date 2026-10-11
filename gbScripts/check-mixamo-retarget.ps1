param([string]$Mesh='woodsman.fbx')
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
    $command += "`r`n" + ('@cl /nologo /EHsc /std:c++14 /O2 /DUSED_GRAPHICS_API=-1 /I"{0}\gbCore\sources" /I"{0}\gbCore\libraries" /I"{0}\gbCore\libraries\ufbx" /I"{0}\gbCore\libraries\zlib-1.2.8" /I"{0}\gbCore\libraries\freetype2\include" "{0}\gbScripts\check_mixamo_retarget.cpp" "{0}\gbCore\sources\animation_3d_retargeter.cpp" "{0}\gbCore\sources\bone_3d.cpp" "{0}\gbCore\libraries\ufbx\ufbx.c" /Fe:mixamo_retarget_check.exe' -f $repoRoot)
    $compileScript = Join-Path $checkOutput 'compile-retarget.cmd'
    Set-Content -LiteralPath $compileScript -Value $command -Encoding ASCII
    & cmd /c $compileScript
    if ($LASTEXITCODE -ne 0) { throw 'Mixamo retarget compilation failed.' }
    & .\mixamo_retarget_check.exe $repoRoot $Mesh
    if ($LASTEXITCODE -ne 0) { throw 'Mixamo retarget checks failed.' }
} finally { Pop-Location; $env:Path = $originalPath }

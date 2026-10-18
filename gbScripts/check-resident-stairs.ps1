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
    $command += "`r`n" + ('@cl /nologo /EHsc /std:c++14 /I"{0}\gbDemo\sources" "{0}\gbScripts\check_resident_stairs.cpp" "{0}\gbDemo\sources\resident_navigation.cpp" "{0}\gbDemo\sources\resident_behavior.cpp" /Fe:resident_stairs_check.exe' -f $repoRoot)
    Set-Content -LiteralPath .\compile-resident-stairs.cmd -Value $command -Encoding ASCII
    & cmd /c .\compile-resident-stairs.cmd
    if ($LASTEXITCODE -ne 0) { throw 'Resident CPU compilation failed.' }
    & .\resident_stairs_check.exe
    if ($LASTEXITCODE -ne 0) { throw 'Resident CPU checks failed.' }
} finally { Pop-Location; $env:Path = $originalPath }



param([switch]$Mobile)
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vsRoot = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vsRoot) { throw 'Visual Studio C++ tools were not found.' }
$vcvars = Join-Path $vsRoot 'VC\Auxiliary\Build\vcvars64.bat'
$originalPath = $env:Path
$env:Path = (Split-Path -Parent $vswhere) + ';' + $env:Path
$checkOutput = Join-Path $repoRoot 'gbDemo\output\post-settings-check'
New-Item -ItemType Directory -Path $checkOutput -Force | Out-Null
Push-Location $checkOutput
try {
    $compileCommand = '@call "{0}" >nul' -f $vcvars
    $compileCommand += "`r`n" + ('@cl /nologo /EHsc /std:c++14 /I"{0}\gbDemo\sources" "{0}\gbScripts\check_stylized_post_process.cpp" /Fe:post_settings_check.exe' -f $repoRoot)
    if ($Mobile) { $compileCommand = $compileCommand.Replace('/EHsc', '/EHsc /D__IOS__') }
    Set-Content -LiteralPath .\compile-check.cmd -Value $compileCommand -Encoding ASCII
    & cmd /c .\compile-check.cmd
    if ($LASTEXITCODE -ne 0) { throw 'Post-process settings CPU compilation failed.' }
    & .\post_settings_check.exe
    if ($LASTEXITCODE -ne 0) { throw 'Post-process settings CPU checks failed.' }
} finally { Pop-Location; $env:Path = $originalPath }

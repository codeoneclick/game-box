$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vsRoot = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vsRoot) { throw 'Visual Studio C++ tools were not found.' }
$vcvars = Join-Path $vsRoot 'VC\Auxiliary\Build\vcvars64.bat'
$originalPath = $env:Path
$env:Path = (Split-Path -Parent $vswhere) + ';' + $env:Path
$auditOutput = Join-Path $repoRoot 'gbDemo\output\island-check\terrain-geometry-audit'
New-Item -ItemType Directory -Path $auditOutput -Force | Out-Null
Push-Location $auditOutput
try {
    $compileCommand = '@call "{0}" >nul' -f $vcvars
    $sources = @('gbScripts\profile_terrain_geometry.cpp', 'gbDemo\sources\terrain_generator.cpp',
        'gbDemo\sources\terrain_mesh_builder.cpp', 'gbDemo\sources\terrain_lod.cpp', 'gbDemo\sources\terrain_contour_lod.cpp', 'gbDemo\sources\terrain_editor_mesh.cpp',
        'gbDemo\sources\terrain_editor_render_cache.cpp', 'gbDemo\sources\terrain_mesh_optimizer.cpp', 'gbDemo\sources\terrain_editor_state.cpp',
        'gbDemo\sources\voxel_tree_builder.cpp', 'gbDemo\sources\stairs.cpp', 'gbDemo\sources\building_placement.cpp')
    $sourceArgs = ($sources | ForEach-Object { '"{0}\{1}"' -f $repoRoot, $_ }) -join ' '
    $compileCommand += "`r`n" + ('@cl /nologo /O2 /EHsc /std:c++14 /I"{0}\gbDemo\sources" {1} /Fe:terrain_geometry_audit.exe' -f $repoRoot, $sourceArgs)
    Set-Content -LiteralPath .\compile-audit.cmd -Value $compileCommand -Encoding ASCII
    & cmd /c .\compile-audit.cmd
    if ($LASTEXITCODE -ne 0) { throw 'Terrain geometry audit compilation failed.' }
    & .\terrain_geometry_audit.exe
    if ($LASTEXITCODE -ne 0) { throw 'Terrain geometry audit failed.' }
} finally { Pop-Location; $env:Path = $originalPath }

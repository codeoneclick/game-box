param([switch]$Profile)
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vsRoot = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vsRoot) { throw 'Visual Studio C++ tools were not found.' }
$vcvars = Join-Path $vsRoot 'VC\Auxiliary\Build\vcvars64.bat'
$originalPath = $env:Path
$env:Path = (Split-Path -Parent $vswhere) + ';' + $env:Path
$checkOutput = Join-Path $repoRoot 'gbDemo\output\editor-check'
New-Item -ItemType Directory -Path $checkOutput -Force | Out-Null
Push-Location $checkOutput
try {
 $compileCommand = '@call "{0}" >nul' -f $vcvars
 $compileCommand += "`r`n" + ('@cl /nologo /EHsc /std:c++14 /I"{0}\gbDemo\sources" "{0}\gbScripts\check_terrain_editor.cpp" "{0}\gbDemo\sources\terrain_editor_state.cpp" "{0}\gbDemo\sources\stairs.cpp" "{0}\gbDemo\sources\building_placement.cpp" /Fe:editor_check.exe' -f $repoRoot)
 Set-Content -LiteralPath .\compile-check.cmd -Value $compileCommand -Encoding ASCII
 & cmd /c .\compile-check.cmd
 if ($LASTEXITCODE -ne 0) { throw 'Editor CPU check compilation failed.' }
 & .\editor_check.exe
 if ($LASTEXITCODE -ne 0) { throw 'Editor CPU checks failed.' }
 $buildingCommand = '@call "{0}" >nul' -f $vcvars
 $buildingCommand += "`r`n" + ('@cl /nologo /EHsc /std:c++14 /I"{0}\gbDemo\sources" "{0}\gbScripts\check_building_placement.cpp" "{0}\gbDemo\sources\terrain_editor_state.cpp" "{0}\gbDemo\sources\stairs.cpp" "{0}\gbDemo\sources\building_placement.cpp" /Fe:building_check.exe' -f $repoRoot)
 Set-Content -LiteralPath .\compile-building-check.cmd -Value $buildingCommand -Encoding ASCII
 & cmd /c .\compile-building-check.cmd
 if ($LASTEXITCODE -ne 0) { throw 'Building check compilation failed.' }
 & .\building_check.exe
 if ($LASTEXITCODE -ne 0) { throw 'Building CPU checks failed.' }
 $meshCommand = '@call "{0}" >nul' -f $vcvars
 $meshCommand += "`r`n" + ('@cl /nologo /EHsc /std:c++14 /I"{0}\gbDemo\sources" "{0}\gbScripts\check_editor_mesh.cpp" "{0}\gbDemo\sources\terrain_editor_mesh.cpp" "{0}\gbDemo\sources\voxel_tree_builder.cpp" "{0}\gbDemo\sources\terrain_editor_render_cache.cpp" "{0}\gbDemo\sources\terrain_mesh_optimizer.cpp" "{0}\gbDemo\sources\terrain_editor_state.cpp" "{0}\gbDemo\sources\stairs.cpp" "{0}\gbDemo\sources\building_placement.cpp" "{0}\gbDemo\sources\terrain_generator.cpp" /Fe:editor_mesh_check.exe' -f $repoRoot)
 $meshCommand=$meshCommand.Replace('/Fe:editor_mesh_check.exe',('"{0}\gbDemo\sources\terrain_mesh_builder.cpp" /Fe:editor_mesh_check.exe' -f $repoRoot))
 Set-Content -LiteralPath .\compile-mesh-check.cmd -Value $meshCommand -Encoding ASCII
 & cmd /c .\compile-mesh-check.cmd
 if ($LASTEXITCODE -ne 0) { throw 'Editor mesh check compilation failed.' }
 & .\editor_mesh_check.exe
 if ($LASTEXITCODE -ne 0) { throw 'Editor mesh checks failed.' }
 if ($Profile) {
  $profileCommand = '@call "{0}" >nul' -f $vcvars
  $profileCommand += "`r`n" + ('@cl /nologo /O2 /EHsc /std:c++14 /I"{0}\gbDemo\sources" "{0}\gbScripts\profile_editor_mesh.cpp" "{0}\gbDemo\sources\terrain_editor_mesh.cpp" "{0}\gbDemo\sources\voxel_tree_builder.cpp" "{0}\gbDemo\sources\terrain_editor_render_cache.cpp" "{0}\gbDemo\sources\terrain_mesh_optimizer.cpp" "{0}\gbDemo\sources\terrain_editor_state.cpp" "{0}\gbDemo\sources\stairs.cpp" "{0}\gbDemo\sources\building_placement.cpp" "{0}\gbDemo\sources\terrain_generator.cpp" "{0}\gbDemo\sources\terrain_mesh_builder.cpp" /Fe:editor_mesh_profile.exe' -f $repoRoot)
  Set-Content -LiteralPath .\compile-mesh-profile.cmd -Value $profileCommand -Encoding ASCII
  & cmd /c .\compile-mesh-profile.cmd
  if ($LASTEXITCODE -ne 0) { throw 'Editor profile compilation failed.' }
  & .\editor_mesh_profile.exe
  if ($LASTEXITCODE -ne 0) { throw 'Editor profile failed.' }
 }
} finally { Pop-Location; $env:Path = $originalPath }

$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vsRoot=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$vcvars=Join-Path $vsRoot 'VC\Auxiliary\Build\vcvars64.bat'
$originalPath=$env:Path;$env:Path=(Split-Path -Parent $vswhere)+';'+$env:Path
$checkOutput=Join-Path $repoRoot 'gbDemo\output\island-check\terrain-lod-work\sse-checks'
New-Item -ItemType Directory $checkOutput -Force|Out-Null;Push-Location $checkOutput
try {
 $compile='@call "{0}" >nul' -f $vcvars
 $compile+="`r`n"+('@cl /nologo /O2 /EHsc /std:c++14 /I"{0}\gbDemo\sources" "{0}\gbScripts\check_terrain_contour_lod.cpp" "{0}\gbDemo\sources\terrain_contour_lod.cpp" "{0}\gbDemo\sources\terrain_mesh_optimizer.cpp" /Fe:terrain_contour_check.exe' -f $repoRoot)
 Set-Content -LiteralPath compile-contour.cmd -Value $compile -Encoding ASCII
 & cmd /c .\compile-contour.cmd;if($LASTEXITCODE -ne 0){throw 'Contour LOD CPU compilation failed'}
 & .\terrain_contour_check.exe;if($LASTEXITCODE -ne 0){throw 'Contour LOD CPU checks failed'}
 $tree='@call "{0}" >nul' -f $vcvars
 $tree+="`r`n"+('@cl /nologo /O2 /EHsc /std:c++14 /I"{0}\gbDemo\sources" "{0}\gbScripts\check_terrain_lod.cpp" "{0}\gbDemo\sources\terrain_lod.cpp" "{0}\gbDemo\sources\terrain_contour_lod.cpp" "{0}\gbDemo\sources\terrain_mesh_optimizer.cpp" /Fe:terrain_lod_check.exe' -f $repoRoot)
 Set-Content -LiteralPath compile-lod.cmd -Value $tree -Encoding ASCII
 & cmd /c .\compile-lod.cmd;if($LASTEXITCODE -ne 0){throw 'Terrain CLOD CPU compilation failed'}
 & .\terrain_lod_check.exe;if($LASTEXITCODE -ne 0){throw 'Terrain CLOD CPU checks failed'}
 $sse=$tree.Replace('check_terrain_lod.cpp','check_terrain_sse.cpp').Replace('terrain_lod_check.exe','terrain_sse_check.exe')
 Set-Content -LiteralPath compile-sse.cmd -Value $sse -Encoding ASCII
 & cmd /c .\compile-sse.cmd;if($LASTEXITCODE -ne 0){throw 'Terrain SSE CPU compilation failed'}
 & .\terrain_sse_check.exe;if($LASTEXITCODE -ne 0){throw 'Terrain SSE CPU checks failed'}
}finally{Pop-Location;$env:Path=$originalPath}

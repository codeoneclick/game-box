$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$vsRoot=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if(!$vsRoot){throw 'Visual Studio C++ tools required.'}
$out=Join-Path $repoRoot 'gbDemo/output/island-check'
New-Item -ItemType Directory -Path $out -Force | Out-Null
Push-Location $out
$savedPath=$env:Path
$env:Path=(Split-Path -Parent $vswhere)+';'+$env:Path
try {
 $command='@call "'+$vsRoot+'\VC\Auxiliary\Build\vcvars64.bat" >nul'+"`r`n"
 $command+='@cl /nologo /EHsc /std:c++14 /O2 /DUSED_GRAPHICS_API=-1 /I"'+$repoRoot+'\gbCore\sources" /I"'+$repoRoot+'\gbCore\libraries" /I"'+$repoRoot+'\gbCore\libraries\ufbx" /I"'+$repoRoot+'\gbCore\libraries\zlib-1.2.8" /I"'+$repoRoot+'\gbCore\libraries\freetype2\include" "'+$repoRoot+'\gbScripts\check_fbx_stream_frames.cpp" "'+$repoRoot+'\gbCore\sources\animation_3d_sequence_serializer_fbx.cpp" "'+$repoRoot+'\gbCore\sources\animation_3d_sequence.cpp" "'+$repoRoot+'\gbCore\sources\resource_serializer.cpp" "'+$repoRoot+'\gbCore\sources\resource.cpp" "'+$repoRoot+'\gbCore\libraries\ufbx\ufbx.c" /Fe:fbx_stream_frames_check.exe'
 Set-Content -LiteralPath compile-fbx-stream-frames.cmd -Value $command -Encoding ASCII
 & cmd /c .\compile-fbx-stream-frames.cmd
 if($LASTEXITCODE -ne 0){throw 'FBX stream frame compile failed.'}
 & ./fbx_stream_frames_check.exe $repoRoot
 if($LASTEXITCODE -ne 0){throw 'FBX stream frame checks failed.'}
} finally {$env:Path=$savedPath;Pop-Location}

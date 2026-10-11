$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$vsRoot=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if(!$vsRoot -or !$env:VULKAN_SDK){throw 'Visual Studio C++ tools and Vulkan SDK required.'}
$out=Join-Path $repoRoot 'gbDemo/output/island-check'
New-Item -ItemType Directory -Path $out -Force | Out-Null
Push-Location $out
$savedPath=$env:Path
$env:Path=(Split-Path -Parent $vswhere)+';'+$env:VULKAN_SDK+'/Bin;'+$env:Path
try {
 $command='@call "'+$vsRoot+'\VC\Auxiliary\Build\vcvars64.bat" >nul'+"`r`n"
 $command+='@cl /nologo /EHsc /std:c++14 /O2 /I"'+$env:VULKAN_SDK+'\Include" "'+$repoRoot+'\gbScripts\check_shader_precompile.cpp" /Fe:shader_precompile_check.exe /link /LIBPATH:"'+$env:VULKAN_SDK+'\Lib" shaderc_shared.lib vulkan-1.lib'
 Set-Content -LiteralPath compile-shader-precompile.cmd -Value $command -Encoding ASCII
 & cmd /c .\compile-shader-precompile.cmd
 if($LASTEXITCODE -ne 0){throw 'Shader precompile check compile failed.'}
 & ./shader_precompile_check.exe
 if($LASTEXITCODE -ne 0){throw 'Shader precompile checks failed.'}
} finally {$env:Path=$savedPath;Pop-Location}

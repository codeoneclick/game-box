$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$vsRoot=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if(!$vsRoot){throw 'Visual Studio C++ tools missing.'}
$out=Join-Path $repoRoot 'gbDemo/output/island-check'
New-Item -ItemType Directory -Path $out -Force | Out-Null
$savedPath=$env:Path;$env:Path=(Split-Path -Parent $vswhere)+';'+$env:Path
Push-Location $out
try{
 $command='@call "'+$vsRoot+'\VC\Auxiliary\Build\vcvars64.bat" >nul'+"`r`n"
 $command+='@cl /nologo /EHsc /O2 /std:c++14 /I"'+$repoRoot+'\gbCore\libraries\ufbx" "'+$repoRoot+'\gbScripts\check_woodsman_fbx.cpp" "'+$repoRoot+'\gbCore\libraries\ufbx\ufbx.c" /Fe:woodsman_fbx_check.exe'
 Set-Content -LiteralPath compile-woodsman.cmd -Value $command -Encoding ASCII
 & cmd /c .\compile-woodsman.cmd
 if($LASTEXITCODE -ne 0){throw 'Woodsman probe compile failed.'}
 & ./woodsman_fbx_check.exe $repoRoot
 if($LASTEXITCODE -ne 0){throw 'Woodsman FBX validation failed.'}
 $model=Get-Content (Join-Path $repoRoot 'gbResources/woodsman.bbmodel') -Raw | ConvertFrom-Json
 if($model.animations.Count){throw 'Embedded Blockbench animation remains.'}
 $bytes=[Convert]::FromBase64String(($model.textures[0].source -split ',')[1])
 $sourceHash=([BitConverter]::ToString([Security.Cryptography.SHA256]::Create().ComputeHash($bytes))).Replace('-','').ToLower()
 $textureHash=(Get-FileHash (Join-Path $repoRoot 'gbBundle/Textures/woodsman.png')).Hash.ToLower()
 if($sourceHash -ne $textureHash){throw 'Original texture changed.'}
 if((Get-FileHash (Join-Path $repoRoot 'gbBundle/Meshes/woodsman.fbx')).Hash -ne (Get-FileHash (Join-Path $repoRoot 'gbResources/woodsman.fbx')).Hash){throw 'Export copies differ.'}
 'Woodsman source animation removed; original texture bytes and both FBX copies match.'
}finally{Pop-Location;$env:Path=$savedPath}

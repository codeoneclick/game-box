$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$vsRoot=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$out=Join-Path $repoRoot 'gbDemo/output/hand-cursor';New-Item -ItemType Directory -Force $out|Out-Null
$command='@call "'+$vsRoot+'\VC\Auxiliary\Build\vcvars64.bat" >nul'+"`r`n"
$command+='@cl /nologo /EHsc /O2 /std:c++14 /I"'+$repoRoot+'\gbDemo\sources" /I"'+$repoRoot+'\gbCore\libraries" "'+$repoRoot+'\gbScripts\check_hand_cursor_math.cpp" /Fe:hand_math_check.exe'
Push-Location $out
try{Set-Content compile-hand.cmd $command -Encoding ASCII;& cmd /c .\compile-hand.cmd;if($LASTEXITCODE){throw 'Hand math compile failed'};& .\hand_math_check.exe;if($LASTEXITCODE){throw 'Hand math check failed'}
 $surfaceCommand=$command.Replace('check_hand_cursor_math.cpp','check_hand_surface.cpp').Replace('hand_math_check.exe','hand_surface_check.exe')
 Set-Content compile-hand-surface.cmd $surfaceCommand -Encoding ASCII;& cmd /c .\compile-hand-surface.cmd;if($LASTEXITCODE){throw 'Hand surface compile failed'};& .\hand_surface_check.exe;if($LASTEXITCODE){throw 'Hand surface check failed'}
}finally{Pop-Location}

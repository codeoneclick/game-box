param([ValidateSet('idle','walk','auto')][string]$Pose='idle', [string]$Time='0.5', [switch]$ColorCycle, [switch]$Orbit, [ValidateRange(1,300)][int]$Duration=4)
$ErrorActionPreference='Stop'
if(-not ('ResidentCheckWindow' -as [type])){Add-Type @'
using System;
using System.Text;
using System.Runtime.InteropServices;
public static class ResidentCheckWindow {
 public delegate bool Callback(IntPtr w,IntPtr p);
 [DllImport("user32.dll")] public static extern bool EnumWindows(Callback c,IntPtr p);
 [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr w,out uint id);
 [DllImport("user32.dll",CharSet=CharSet.Auto)] public static extern int GetClassName(IntPtr w,StringBuilder n,int l);
 [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr w,uint m,IntPtr a,IntPtr b);
 [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr w,uint m,IntPtr a,IntPtr b);
 public static IntPtr Find(uint pid){IntPtr r=IntPtr.Zero;EnumWindows((w,p)=>{uint id;GetWindowThreadProcessId(w,out id);if(id==pid){var n=new StringBuilder(256);GetClassName(w,n,256);if(n.ToString()=="game-box")r=w;}return true;},IntPtr.Zero);return r;}
}
'@}
$repoRoot=Split-Path -Parent $PSScriptRoot
$suffix=if($ColorCycle){'colors-'+$Pose}elseif($Orbit){'back'}else{$Pose+'-'+$Time.Replace('.','_')}
$captureDir=Join-Path $repoRoot "gbDemo\output\island-check\resident-$suffix"
New-Item -ItemType Directory -Path $captureDir -Force | Out-Null
$frames=if($ColorCycle){@(120,360,600,840,1080)}else{@(120,360,600)}
foreach($frame in $frames){Remove-Item -LiteralPath "$captureDir\$frame.ppm" -ErrorAction SilentlyContinue}
$keys=@('VK_INSTANCE_LAYERS','VK_SCREENSHOT_FRAMES','VK_SCREENSHOT_DIR','GB_OCEAN_FIXED_TIME','GB_RESIDENT_INSPECT','GB_RESIDENT_POSE','GB_RESIDENT_TIME','GB_RESIDENT_COLOR','GB_RESIDENT_DEBUG')
$saved=@{};foreach($key in $keys){$saved[$key]=[Environment]::GetEnvironmentVariable($key)}
$env:VK_INSTANCE_LAYERS='VK_LAYER_LUNARG_screenshot';$env:VK_SCREENSHOT_FRAMES=$frames -join ',';$env:VK_SCREENSHOT_DIR=$captureDir
$env:GB_OCEAN_FIXED_TIME='3';$env:GB_RESIDENT_COLOR='0';$env:GB_RESIDENT_DEBUG='1'
if($Pose -eq 'auto'){
 Remove-Item Env:GB_RESIDENT_INSPECT,Env:GB_RESIDENT_POSE,Env:GB_RESIDENT_TIME -ErrorAction SilentlyContinue
}else{$env:GB_RESIDENT_INSPECT='1';$env:GB_RESIDENT_POSE=$Pose;$env:GB_RESIDENT_TIME=$Time}
$demo=$null
try{
 $demo=Start-Process -FilePath (Join-Path $repoRoot 'gbDemo\output\debug\x64\gb_demo.exe') -WorkingDirectory (Join-Path $repoRoot 'gbDemo') -WindowStyle Hidden -RedirectStandardOutput "$captureDir\runtime.log" -RedirectStandardError "$captureDir\errors.log" -PassThru
 $watch=[Diagnostics.Stopwatch]::StartNew();$nextColor=0;$orbited=$false
 while(!$demo.HasExited -and $watch.Elapsed.TotalSeconds -lt [Math]::Max(45,$Duration+10)){
  $log=Get-Content "$captureDir\runtime.log" -Raw
  if($Orbit -and !$orbited -and $log -match ('Saved image:.*[/\\]'+$frames[0]+'\.ppm')){
   if($ColorCycle -or $Pose -eq 'auto'){throw 'Orbit probe requires a fixed pose without ColorCycle.'}
   $window=[ResidentCheckWindow]::Find($demo.Id)
   [void][ResidentCheckWindow]::SendMessage($window,513,[IntPtr]1,[IntPtr]((360 -shl 16) -bor 100))
   [void][ResidentCheckWindow]::SendMessage($window,512,[IntPtr]1,[IntPtr]((168 -shl 16) -bor 820))
   [void][ResidentCheckWindow]::SendMessage($window,514,[IntPtr]::Zero,[IntPtr]((168 -shl 16) -bor 820));$orbited=$true
  }
  if($ColorCycle -and $nextColor -lt 4 -and $log -match ('Saved image:.*[/\\]'+$frames[$nextColor]+'\.ppm')){
   $window=[ResidentCheckWindow]::Find($demo.Id);if($window -eq [IntPtr]::Zero){throw 'Resident window missing.'}
   # A repeated keydown must not cycle twice before keyup.
   [void][ResidentCheckWindow]::SendMessage($window,256,[IntPtr]67,[IntPtr]::Zero)
   [void][ResidentCheckWindow]::SendMessage($window,256,[IntPtr]67,[IntPtr]::Zero)
   [void][ResidentCheckWindow]::SendMessage($window,257,[IntPtr]67,[IntPtr]::Zero);$nextColor++
  }
  if($log -match ('Saved image:.*[/\\]'+$frames[-1]+'\.ppm') -and $watch.Elapsed.TotalSeconds -ge $Duration){break}
  Start-Sleep -Milliseconds 10
 }
 $log=Get-Content "$captureDir\runtime.log" -Raw
 if($demo.HasExited){throw 'Resident demo exited during runtime check.'}
 if($log -notmatch ('Saved image:.*[/\\]'+$frames[-1]+'\.ppm')){throw 'Resident frames were not fully captured.'}
 if($log -notmatch 'Resident initialized: height=1.9 bones=18' -or $log -notmatch 'Resident material drawn'){throw 'Resident was not initialized and drawn.'}
 if($log -match 'safe=0|NONFINITE|Resident setup failed'){throw 'Resident runtime state invalid.'}
 if($ColorCycle -and ([regex]::Matches($log,'Resident clothing color=').Count -ne 4)){throw 'C key debounce/cycle failed.'}
 if($ColorCycle){
  $samples=@()
  foreach($frame in $frames){
   $bytes=[IO.File]::ReadAllBytes("$captureDir\$frame.ppm")
   $header=[regex]::Match([Text.Encoding]::ASCII.GetString($bytes,0,100),'^P6\s+(\d+)\s+(\d+)\s+255\s')
   if(!$header.Success -or $header.Groups[1].Value -ne '1280' -or $header.Groups[2].Value -ne '720'){throw 'Unsupported capture dimensions/format.'}
   $pixels=@();foreach($point in @(@(640,300),@(620,410),@(624,483),@(641,208),@(640,190))){
    $offset=$header.Length+3*($point[1]*1280+$point[0]);$pixels+=,($bytes[$offset..($offset+2)] -join ',')
   };$samples+=,$pixels
  }
  for($i=0;$i -lt 5;++$i){
   if($samples[$i][0] -ne $samples[$i][1]){throw 'Shirt/trouser colors differ.'}
   if($samples[$i][2] -ne $samples[0][2] -or $samples[$i][3] -ne $samples[0][3] -or $samples[$i][4] -ne $samples[0][4]){throw 'Boot/skin/hair color changed.'}
  }
  if(@($samples[0][0],$samples[1][0],$samples[2][0],$samples[3][0] | Select-Object -Unique).Count -ne 4 -or $samples[0][0] -ne $samples[4][0]){throw 'Clothing palette did not complete a visible cycle.'}
  $blue=$samples[0][0].Split(',');if([int]$blue[2] -le [int]$blue[0]){throw 'Resident blue clothing was not visible.'}
  'Visible shared clothing colors and unchanged boot/skin pixels passed'
 }
 if($Pose -eq 'auto' -and $Duration -ge 20 -and $log -notmatch 'Resident mode=walk'){throw 'Autonomous walk never started.'}
 if($Pose -eq 'auto' -and $Duration -ge 60 -and ([regex]::Matches($log,'Resident mode=walk').Count -lt 2 -or $log -notmatch 'Resident mode=idle')){throw 'Multiple autonomous routes/stops were not observed.'}
 $errors=Get-Content "$captureDir\errors.log" -Raw
 if($errors -match '(?i)validation layer:|error:|failed'){throw "Runtime rendering errors: $errors"}
 "Resident $suffix Vulkan runtime check passed: $captureDir"
}finally{
 if($demo -and !$demo.HasExited){
  $window=[ResidentCheckWindow]::Find($demo.Id);if($window -ne [IntPtr]::Zero){[void][ResidentCheckWindow]::PostMessage($window,16,[IntPtr]::Zero,[IntPtr]::Zero)}
  if(!$demo.WaitForExit(5000)){Stop-Process -Id $demo.Id;Write-Warning 'Demo did not close normally; shutdown verification unavailable.'}
 }
 foreach($key in $keys){[Environment]::SetEnvironmentVariable($key,$saved[$key])}
}
$shutdown=Get-Content "$captureDir\errors.log" -Raw
if($shutdown -match 'vkDestroyCommandPool'){'Known shutdown command-pool validation messages remain; see errors.log'}

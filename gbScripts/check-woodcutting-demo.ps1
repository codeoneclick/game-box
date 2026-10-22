param([string]$Executable='gbDemo/output/release/x64/gb_demo.exe',[switch]$NoValidation)
$ErrorActionPreference='Stop'
$repo=Split-Path -Parent $PSScriptRoot
$out=Join-Path $repo ('gbDemo/output/woodcutting/run-'+[Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Force $out|Out-Null
Add-Type @'
using System;using System.Text;using System.Runtime.InteropServices;
public static class WoodCheck{
 public delegate bool Callback(IntPtr w,IntPtr p);
 [DllImport("user32.dll")] public static extern bool EnumWindows(Callback c,IntPtr p);
 [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr w,out uint id);
 [DllImport("user32.dll",CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr w,StringBuilder s,int n);
 [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr w,uint m,IntPtr a,IntPtr b);
 [DllImport("kernel32.dll")] public static extern bool GetExitCodeProcess(IntPtr p,out uint code);
 public static IntPtr Find(uint pid){IntPtr r=IntPtr.Zero;EnumWindows((w,p)=>{uint id;GetWindowThreadProcessId(w,out id);var s=new StringBuilder(256);GetClassName(w,s,256);if(id==pid&&s.ToString()=="game-box"){r=w;return false;}return true;},IntPtr.Zero);return r;}
}
'@
$vars=@{GB_WOODCUTTER_INSPECT='1';GB_RESIDENT_DEBUG='1';GB_RESIDENT_POSE=$null;GB_RESIDENT_TIME=$null;GB_RESIDENT_COUNT=$null;GB_VULKAN_VALIDATION=$(if($NoValidation){'0'}else{'1'});VK_INSTANCE_LAYERS='VK_LAYER_LUNARG_screenshot';VK_SCREENSHOT_DIR=$out;VK_SCREENSHOT_FRAMES='120,180,240,300,360,720'}
$before=@{};foreach($key in $vars.Keys){$before[$key]=[Environment]::GetEnvironmentVariable($key);[Environment]::SetEnvironmentVariable($key,$vars[$key])}
try{
 $process=Start-Process (Join-Path $repo $Executable) -WorkingDirectory (Join-Path $repo 'gbDemo') -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/runtime.log" -RedirectStandardError "$out/errors.log"
 $handle=$process.Handle;$timer=[Diagnostics.Stopwatch]::StartNew()
 while(!$process.HasExited -and $timer.Elapsed.TotalSeconds -lt 180){
  $log=Get-Content "$out/runtime.log" -Raw
  if((Test-Path "$out/720.ppm") -and @([regex]::Matches($log,'Woodcutter mode=chop terrace=([0-9]+) ')|ForEach-Object {$_.Groups[1].Value}|Select-Object -Unique).Count -eq 8){break}
  Start-Sleep -Milliseconds 200
 }
 if($process.HasExited){throw "Woodcutting demo exited; see $out"}
 $log=Get-Content "$out/runtime.log" -Raw
 foreach($terrace in 1..8){if($log -notmatch "Woodcutter terrace=$terrace tree=" -or $log -notmatch "Woodcutter mode=chop terrace=$terrace "){throw "Terrace $terrace did not reach chopping; see $out"}}
 if($log -notmatch 'Animation retargeted: chop '){throw "Chopping animation not retargeted; see $out"}
 if(!(Test-Path "$out/720.ppm")){throw "Capture missing; see $out"}
 if($log -notmatch 'Woodcutter two-hand grip error='){throw "Two-hand grip check missing; see $out"}
 if(Select-String -Path "$out/runtime.log","$out/errors.log" -Pattern 'Validation Error|VUID-|Assertion failed|NONFINITE|Resident setup failed|AXE PARENT MISMATCH|AXE GRIP MISMATCH|AXE EDGE MISMATCH|AXE HEAD DIRECTION MISMATCH|RESIDENT FACING MISMATCH|unsafe=[1-9]'){throw "Woodcutting validation failed; see $out"}
 $window=[WoodCheck]::Find([uint32]$process.Id);[WoodCheck]::PostMessage($window,16,[IntPtr]::Zero,[IntPtr]::Zero)|Out-Null
 if(!$process.WaitForExit(15000)){throw 'Owned demo close timed out'}
 $code=[uint32]0;[WoodCheck]::GetExitCodeProcess($handle,[ref]$code)|Out-Null;if($code){throw "Demo exited $code"}
 "Woodcutting runtime: 8 workers approach/chop, 8 terrace trees, retargeted animation, captures and clean close PASS: $out"
}finally{
 if($process -and !$process.HasExited){Stop-Process -Id $process.Id -Force}
 foreach($key in $vars.Keys){[Environment]::SetEnvironmentVariable($key,$before[$key])}
}

param([ValidateSet('debug','release')][string]$Configuration='debug')
$ErrorActionPreference='Stop'
if(-not ('RhythmControlsWindow' -as [type])){Add-Type @'
using System;using System.Text;using System.Runtime.InteropServices;
public static class RhythmControlsWindow {
 public delegate bool Callback(IntPtr w,IntPtr p);
 [DllImport("user32.dll")]public static extern bool EnumWindows(Callback c,IntPtr p);
 [DllImport("user32.dll")]public static extern uint GetWindowThreadProcessId(IntPtr w,out uint id);
 [DllImport("user32.dll",CharSet=CharSet.Auto)]public static extern int GetClassName(IntPtr w,StringBuilder n,int l);
 [DllImport("user32.dll")]public static extern bool PostMessage(IntPtr w,uint m,IntPtr a,IntPtr b);
 public static IntPtr Find(uint pid){IntPtr r=IntPtr.Zero;EnumWindows((w,p)=>{uint id;GetWindowThreadProcessId(w,out id);if(id==pid){var n=new StringBuilder(256);GetClassName(w,n,256);if(n.ToString()=="game-box")r=w;}return true;},IntPtr.Zero);return r;}
 public static void Key(IntPtr w,int k){PostMessage(w,256,(IntPtr)k,IntPtr.Zero);PostMessage(w,257,(IntPtr)k,IntPtr.Zero);}
 public static void Click(IntPtr w,int x,int y){IntPtr pos=(IntPtr)((y<<16)|x);PostMessage(w,513,(IntPtr)1,pos);PostMessage(w,514,IntPtr.Zero,pos);}
}
'@}
$repoRoot=Split-Path -Parent $PSScriptRoot
$out=Join-Path $repoRoot 'gbDemo/output/rhythm-check/controls'
New-Item -ItemType Directory -Force $out | Out-Null
$prefs=Join-Path $out 'preferences.json'
if(Test-Path -LiteralPath $prefs){Remove-Item -LiteralPath $prefs}
$names=@('GB_DEMO','GB_RHYTHM_TRACK','GB_RHYTHM_SILENT','GB_RHYTHM_DIAGNOSTICS','GB_RHYTHM_AUDIO_SETTINGS','GB_RHYTHM_DEBUG','VK_SCREENSHOT_FRAMES')
$saved=@{};foreach($n in $names){$saved[$n]=[Environment]::GetEnvironmentVariable($n)}
$env:GB_DEMO='rhythm_ship';$env:GB_RHYTHM_TRACK='0';$env:GB_RHYTHM_SILENT='0';$env:GB_RHYTHM_DIAGNOSTICS='1';$env:GB_RHYTHM_AUDIO_SETTINGS=$prefs;$env:GB_RHYTHM_DEBUG='0';$env:VK_SCREENSHOT_FRAMES=''
$demo=$null
try {
 $demo=Start-Process -FilePath (Join-Path $repoRoot "gbDemo/output/$Configuration/x64/gb_demo.exe") -WorkingDirectory (Join-Path $repoRoot 'gbDemo') -WindowStyle Hidden -RedirectStandardOutput "$out/runtime.log" -RedirectStandardError "$out/errors.log" -PassThru
 $null=$demo.Handle
 $timer=[Diagnostics.Stopwatch]::StartNew();$state=0;$stateTime=0.;$pulse=0
 while(-not $demo.HasExited -and $timer.Elapsed.TotalSeconds -lt 40){
  $log=Get-Content "$out/runtime.log" -Raw -ErrorAction SilentlyContinue
  $w=[RhythmControlsWindow]::Find($demo.Id);$now=$timer.Elapsed.TotalSeconds
  if($state -eq 0 -and $log -match 'id=audioera') {[RhythmControlsWindow]::Click($w,130,705);$state=1}
  elseif($state -eq 1 -and $log -match 'id=ikoliks') {[RhythmControlsWindow]::Click($w,130,705);$state=2}
  elseif($state -eq 2 -and $log -match 'id=strawberry') {[RhythmControlsWindow]::Click($w,240,705);$state=3}
  elseif($state -eq 3 -and $log -match 'Rhythm calibration begin') {$stateTime=$now;$state=4}
  elseif($state -eq 4 -and $pulse -lt 24 -and $now-$stateTime -ge 2.08+$pulse*.5) {[RhythmControlsWindow]::Key($w,32);$pulse++}
  elseif($state -eq 4 -and $pulse -eq 24 -and $log -match 'calibration accepted=1') {[RhythmControlsWindow]::Click($w,470,705);$stateTime=$now;$state=5}
  elseif($state -eq 5 -and $now-$stateTime -gt 1) {
   $settings=Get-Content $prefs -Raw | ConvertFrom-Json
   if($settings.offset_ms -ne 0){throw 'Reset offset button failed'}
   [RhythmControlsWindow]::Click($w,240,705);$stateTime=$now;$state=6
  }
  elseif($state -eq 6 -and $now-$stateTime -gt 1) {[RhythmControlsWindow]::Click($w,365,705);$stateTime=$now;$state=7}
  elseif($state -eq 7 -and $log -match 'calibration cancelled') {break}
  Start-Sleep -Milliseconds 12
 }
 if($state -ne 7 -or $log -notmatch 'calibration cancelled'){throw "Control flow incomplete, state=$state pulses=$pulse"}
 if($demo.HasExited){throw "Demo exited prematurely: $($demo.ExitCode)"}
}finally{
 if($demo -and -not $demo.HasExited){[RhythmControlsWindow]::PostMessage([RhythmControlsWindow]::Find($demo.Id),16,[IntPtr]::Zero,[IntPtr]::Zero)|Out-Null;if(-not $demo.WaitForExit(5000)){Stop-Process -Id $demo.Id;throw 'Controls flow did not close normally'}}
 foreach($n in $names){[Environment]::SetEnvironmentVariable($n,$saved[$n])}
}
if($demo.ExitCode -ne 0){throw "Abnormal shutdown: $($demo.ExitCode)"}
"Rhythm controls passed: track switches, 24 keyboard calibration taps, saved offset, reset, cancel and normal close ($out)"

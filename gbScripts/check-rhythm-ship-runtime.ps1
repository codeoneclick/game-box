param([ValidateSet('debug-client','release-client')][string]$Configuration='debug-client',[int]$TimeoutSeconds=60,[switch]$Silent,[ValidateSet(30,40,50)][int]$CrowdCount=40,[switch]$DebugPanel,[ValidateRange(0,9)][int]$Track=0,[switch]$GraphicsPanel)
$ErrorActionPreference='Stop'
if(-not ('RhythmWindow' -as [type])){Add-Type @'
using System;using System.Text;using System.Runtime.InteropServices;
public static class RhythmWindow {
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
$kind=if($Configuration -eq 'debug-client'){'debug'}else{'release'}
$capture=Join-Path $repoRoot ('gbDemo/output/rhythm-check/runtime-'+$kind+'-'+$CrowdCount+'-'+$Silent+'-'+$DebugPanel+'-track'+$Track+'-f8'+$GraphicsPanel)
New-Item -ItemType Directory -Force $capture | Out-Null
$names=@('VK_INSTANCE_LAYERS','VK_SCREENSHOT_FRAMES','VK_SCREENSHOT_DIR','GB_VULKAN_VALIDATION','GB_RHYTHM_DIAGNOSTICS','GB_RHYTHM_CROWD_COUNT','GB_RHYTHM_SILENT','GB_RHYTHM_DEBUG','GB_DEMO','GB_RHYTHM_TRACK','GB_HUD_DEBUG')
$saved=@{};foreach($n in $names){$saved[$n]=[Environment]::GetEnvironmentVariable($n)}
$env:VK_INSTANCE_LAYERS='VK_LAYER_KHRONOS_validation;VK_LAYER_LUNARG_screenshot';$env:VK_SCREENSHOT_FRAMES='30,160,400';$env:VK_SCREENSHOT_DIR=$capture
$env:GB_VULKAN_VALIDATION='1'
$env:GB_RHYTHM_TRACK="$Track";$env:GB_HUD_DEBUG='1'
$env:GB_RHYTHM_DIAGNOSTICS='1';$env:GB_RHYTHM_CROWD_COUNT="$CrowdCount";$env:GB_DEMO='rhythm_ship';$env:GB_RHYTHM_SILENT=if($Silent){'1'}else{'0'};$env:GB_RHYTHM_DEBUG=if($DebugPanel){'1'}else{'0'}
$demo=$null
try {
 $demo=Start-Process -FilePath (Join-Path $repoRoot "gbDemo/output/$kind/x64/gb_demo.exe") -WorkingDirectory (Join-Path $repoRoot 'gbDemo') -WindowStyle Hidden -RedirectStandardOutput "$capture/runtime.log" -RedirectStandardError "$capture/errors.log" -PassThru
 $null=$demo.Handle
 $timer=[Diagnostics.Stopwatch]::StartNew();$started=-1.;$tap=0;$debugClicked=$false;$graphicsShown=$false
 while(-not $demo.HasExited -and $timer.Elapsed.TotalSeconds -lt $TimeoutSeconds){
  $log=Get-Content "$capture/runtime.log" -Raw -ErrorAction SilentlyContinue
  if($started -lt 0 -and $log -match 'Rhythm Ship initialized'){$started=$timer.Elapsed.TotalSeconds}
  if($started -ge 0){$elapsed=$timer.Elapsed.TotalSeconds-$started;$w=[RhythmWindow]::Find($demo.Id);if($elapsed -gt 1 -and $elapsed -lt 6 -and $elapsed -gt $tap*.14){[RhythmWindow]::Key($w,32);$tap++}
   if($DebugPanel -and -not $debugClicked -and $elapsed -gt 4){[RhythmWindow]::Click($w,440,384);[RhythmWindow]::Click($w,250,384);[RhythmWindow]::Click($w,100,384);$debugClicked=$true}
   if($GraphicsPanel -and -not $graphicsShown -and $elapsed -gt 5){[RhythmWindow]::Key($w,119);$graphicsShown=$true}
   if($elapsed -gt 7 -and $log -match 'Saved image:.*400\.ppm'){break}
  }
  Start-Sleep -Milliseconds 70
 }
 if($demo.HasExited){throw "Demo exited prematurely ($($demo.ExitCode)): $(Get-Content "$capture/errors.log" -Raw)"}
 if($started -lt 0){throw 'Rhythm Ship did not initialize'}
 if($log -notmatch 'Rhythm time=[1-9]'){throw 'Transport did not advance'}
 if($log -notmatch 'HUD fps=[1-9].*triangles=[1-9]'){throw 'Render statistics did not advance'}
 if($log -notmatch 'score=[1-9]'){throw 'Real keyboard taps did not score'}
 if($DebugPanel -and $log -notmatch 'leader=1'){throw 'Debug leader button did not change rank'}
 if($log -notmatch 'Saved image:.*400\.ppm'){throw 'Multiframe capture incomplete'}
 $ppm=[System.IO.File]::OpenRead((Join-Path $capture '400.ppm'))
 try{$header=New-Object byte[] 32;$null=$ppm.Read($header,0,32);if([Text.Encoding]::ASCII.GetString($header) -notmatch 'P6\s+680\s+960\s+255'){throw 'Expected portrait 680 x 960'}}finally{$ppm.Dispose()}
 [RhythmWindow]::Key([RhythmWindow]::Find($demo.Id),82);Start-Sleep -Milliseconds 400
}finally{
 if($demo -and -not $demo.HasExited){[RhythmWindow]::PostMessage([RhythmWindow]::Find($demo.Id),16,[IntPtr]::Zero,[IntPtr]::Zero)|Out-Null;if(-not $demo.WaitForExit(5000)){Stop-Process -Id $demo.Id;throw 'Demo did not exit normally'}}
 foreach($n in $names){[Environment]::SetEnvironmentVariable($n,$saved[$n])}
}
$errors=Get-Content "$capture/errors.log" -Raw
if($demo.ExitCode -ne 0){throw "Abnormal shutdown $($demo.ExitCode)"}
if($errors -match '(?i)validation error|VUID-|error:|failed|assertion'){throw "Runtime diagnostics: $errors"}
"Rhythm Ship runtime passed: $capture"

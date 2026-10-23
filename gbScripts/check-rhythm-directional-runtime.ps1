param([ValidateSet('debug','release')][string]$Configuration='debug')
$ErrorActionPreference='Stop'
if(-not ('DirectionalWindow' -as [type])){Add-Type @'
using System;using System.Text;using System.Runtime.InteropServices;
public static class DirectionalWindow {
 public delegate bool Callback(IntPtr w,IntPtr p);
 [DllImport("user32.dll")]public static extern bool EnumWindows(Callback c,IntPtr p);
 [DllImport("user32.dll")]public static extern uint GetWindowThreadProcessId(IntPtr w,out uint id);
 [DllImport("user32.dll",CharSet=CharSet.Auto)]public static extern int GetClassName(IntPtr w,StringBuilder n,int l);
 [DllImport("user32.dll")]public static extern bool PostMessage(IntPtr w,uint m,IntPtr a,IntPtr b);
 public static IntPtr Find(uint pid){IntPtr r=IntPtr.Zero;EnumWindows((w,p)=>{uint id;GetWindowThreadProcessId(w,out id);if(id==pid){var n=new StringBuilder(256);GetClassName(w,n,256);if(n.ToString()=="game-box")r=w;}return true;},IntPtr.Zero);return r;}
 public static void Key(IntPtr w,int key){PostMessage(w,256,(IntPtr)key,IntPtr.Zero);PostMessage(w,257,(IntPtr)key,IntPtr.Zero);}
 public static void Mouse(IntPtr w,uint msg,int x,int y){PostMessage(w,msg,msg==514?IntPtr.Zero:(IntPtr)1,(IntPtr)((y<<16)|(x&65535)));}
}
'@}
$repoRoot=Split-Path -Parent $PSScriptRoot;$out=Join-Path $repoRoot 'gbDemo/output/rhythm-check/directional-native'
New-Item -ItemType Directory -Force $out | Out-Null
$names=@('GB_DEMO','GB_RHYTHM_TRACK','GB_RHYTHM_SILENT','GB_RHYTHM_DIAGNOSTICS','GB_RHYTHM_DEBUG','GB_RHYTHM_AUDIO_SETTINGS')
$saved=@{};foreach($n in $names){$saved[$n]=[Environment]::GetEnvironmentVariable($n)}
$env:GB_DEMO='rhythm_ship';$env:GB_RHYTHM_TRACK='9';$env:GB_RHYTHM_SILENT='1';$env:GB_RHYTHM_DIAGNOSTICS='1';$env:GB_RHYTHM_DEBUG='0';$env:GB_RHYTHM_AUDIO_SETTINGS=Join-Path $out 'preferences.json'
$demo=$null
function Read-InputCount { $text=Get-Content "$out/runtime.log" -Raw -ErrorAction SilentlyContinue;return [regex]::Matches($text,'Rhythm input=').Count }
function Wait-InputCount([int]$expected) {
 $wait=[Diagnostics.Stopwatch]::StartNew()
 while($wait.Elapsed.TotalSeconds -lt 5 -and (Read-InputCount) -lt $expected){Start-Sleep -Milliseconds 40}
 Start-Sleep -Milliseconds 150
 if((Read-InputCount) -ne $expected){throw "Expected $expected inputs, got $(Read-InputCount)"}
}
try {
 $demo=Start-Process -FilePath (Join-Path $repoRoot "gbDemo/output/$Configuration/x64/gb_demo.exe") -WorkingDirectory (Join-Path $repoRoot 'gbDemo') -WindowStyle Hidden -RedirectStandardOutput "$out/runtime.log" -RedirectStandardError "$out/errors.log" -PassThru
 $null=$demo.Handle;$timer=[Diagnostics.Stopwatch]::StartNew();$w=[IntPtr]::Zero
 while($timer.Elapsed.TotalSeconds -lt 25 -and -not $demo.HasExited){$log=Get-Content "$out/runtime.log" -Raw -ErrorAction SilentlyContinue;if($log -match 'Rhythm Ship initialized'){$w=[DirectionalWindow]::Find($demo.Id);break};Start-Sleep -Milliseconds 50}
 if($w -eq [IntPtr]::Zero){throw 'Directional demo did not initialize'}
 Start-Sleep -Milliseconds 500
 foreach($key in @(32,37,39,38,40)){[DirectionalWindow]::Key($w,$key);Start-Sleep -Milliseconds 50}
 Wait-InputCount 5
 $before=Read-InputCount
 foreach($end in @(@(240,460),@(360,460),@(300,400),@(300,520))){[DirectionalWindow]::Mouse($w,513,300,460);[DirectionalWindow]::Mouse($w,512,$end[0],$end[1]);[DirectionalWindow]::Mouse($w,514,$end[0],$end[1]);Start-Sleep -Milliseconds 100}
 Wait-InputCount ($before+4)
 if((Read-InputCount)-$before -ne 4){throw 'Mouse swipe must emit exactly once in each direction'}
 $before=Read-InputCount
 [DirectionalWindow]::Mouse($w,513,300,460);[DirectionalWindow]::Mouse($w,514,302,461);Wait-InputCount ($before+1)
 if((Read-InputCount)-$before -ne 1){throw 'Small mouse click did not emit exactly one Tap'}
 $before=Read-InputCount
 [DirectionalWindow]::Mouse($w,513,300,460);[DirectionalWindow]::PostMessage($w,31,[IntPtr]::Zero,[IntPtr]::Zero)|Out-Null;[DirectionalWindow]::Mouse($w,514,300,460);Start-Sleep -Milliseconds 200
 if((Read-InputCount)-$before -ne 0){throw 'Capture cancellation became a Tap'}
 [DirectionalWindow]::Mouse($w,513,300,460);[DirectionalWindow]::Key($w,82);[DirectionalWindow]::Mouse($w,512,360,460);[DirectionalWindow]::Mouse($w,514,360,460);Start-Sleep -Milliseconds 200
 if((Read-InputCount)-$before -ne 0){throw 'Restart preserved stale swipe'}
 [DirectionalWindow]::Mouse($w,513,300,460);[DirectionalWindow]::Mouse($w,512,340,500);[DirectionalWindow]::Mouse($w,514,340,500);Start-Sleep -Milliseconds 200
 if((Read-InputCount)-$before -ne 0){throw 'Ambiguous diagonal emitted an action'}
 [DirectionalWindow]::Key($w,119);Start-Sleep -Milliseconds 150;[DirectionalWindow]::Mouse($w,513,300,460);[DirectionalWindow]::Mouse($w,512,360,460);[DirectionalWindow]::Mouse($w,514,360,460);Start-Sleep -Milliseconds 200
 if((Read-InputCount)-$before -ne 0){throw 'F8 inspector leaked gameplay gesture'}
 [DirectionalWindow]::Key($w,119)
 Start-Sleep -Milliseconds 200
 # A captured native control must be invalidated even if F1 hides and reopens it.
 [DirectionalWindow]::Key($w,112);Start-Sleep -Milliseconds 200
 $restarts=[regex]::Matches((Get-Content "$out/runtime.log" -Raw),'Rhythm restart').Count
 [DirectionalWindow]::Mouse($w,513,130,221);Start-Sleep -Milliseconds 200
 [DirectionalWindow]::Key($w,112);Start-Sleep -Milliseconds 150
 [DirectionalWindow]::Key($w,112);Start-Sleep -Milliseconds 150
 [DirectionalWindow]::Mouse($w,514,130,221);Start-Sleep -Milliseconds 300
 if([regex]::Matches((Get-Content "$out/runtime.log" -Raw),'Rhythm restart').Count -ne $restarts){throw 'Hidden/reopened Difficulty retained stale capture'}
 [DirectionalWindow]::Key($w,112);Start-Sleep -Milliseconds 150
 # A lost key-up outside the window must not suppress the next press.
 $before=Read-InputCount
 [DirectionalWindow]::PostMessage($w,256,[IntPtr]37,[IntPtr]::Zero)|Out-Null;Wait-InputCount ($before+1)
 [DirectionalWindow]::PostMessage($w,8,[IntPtr]::Zero,[IntPtr]::Zero)|Out-Null;Start-Sleep -Milliseconds 150
 [DirectionalWindow]::Key($w,37);Wait-InputCount ($before+2)
 $log=Get-Content "$out/runtime.log" -Raw
 foreach($action in @('Tap','SwipeLeft','SwipeRight','SwipeUp','SwipeDown')){if($log -notmatch "Rhythm input=$action "){throw "Missing input $action"}}
 if($demo.HasExited){throw 'Directional demo exited prematurely'}
}finally{
 if($demo -and -not $demo.HasExited){[DirectionalWindow]::PostMessage([DirectionalWindow]::Find($demo.Id),16,[IntPtr]::Zero,[IntPtr]::Zero)|Out-Null;if(-not $demo.WaitForExit(5000)){Stop-Process -Id $demo.Id;throw 'Directional demo failed normal close'}}
 foreach($n in $names){[Environment]::SetEnvironmentVariable($n,$saved[$n])}
}
if($demo.ExitCode -ne 0){throw "Abnormal shutdown: $($demo.ExitCode)"}
"Directional native checks passed: arrows, four mouse swipes, click, cancellation, restart, diagonal, F8, hidden UI capture, focus loss and normal close"

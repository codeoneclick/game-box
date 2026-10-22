param([string]$Executable='gbDemo/output/release/x64/gb_demo.exe')
$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$out=Join-Path $repoRoot 'gbDemo/output/hand-cursor'
Add-Type @'
using System;using System.Text;using System.Runtime.InteropServices;
public static class HandCursorNative {
 public delegate bool Callback(IntPtr w,IntPtr p);
 [StructLayout(LayoutKind.Sequential)] public struct Point { public int x,y; }
 [StructLayout(LayoutKind.Sequential)] public struct Cursor { public int size,flags; public IntPtr handle; public Point point; }
 [DllImport("user32.dll")] public static extern bool EnumWindows(Callback cb,IntPtr p);
 [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr w,out uint id);
 [DllImport("user32.dll",CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr w,StringBuilder s,int n);
 [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr w,uint m,IntPtr a,IntPtr b);
 [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr w);
 [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
 [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr w,int n);
 [DllImport("user32.dll")] public static extern bool GetCursorPos(out Point p);
 [DllImport("user32.dll")] public static extern IntPtr WindowFromPoint(Point p);
 [DllImport("user32.dll")] public static extern bool SetCursorPos(int x,int y);
 [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr w,ref Point p);
 [DllImport("user32.dll")] public static extern bool GetCursorInfo(ref Cursor c);
 [DllImport("kernel32.dll")] public static extern bool GetExitCodeProcess(IntPtr h,out uint code);
 [DllImport("kernel32.dll")] public static extern uint GetCurrentThreadId();
 [DllImport("user32.dll")] public static extern bool AttachThreadInput(uint a,uint b,bool attach);
 [DllImport("user32.dll")] public static extern bool BringWindowToTop(IntPtr w);
 public static bool Activate(IntPtr w){uint id;uint foreground=GetWindowThreadProcessId(GetForegroundWindow(),out id),current=GetCurrentThreadId();bool attached=foreground!=current&&AttachThreadInput(current,foreground,true);try{ShowWindow(w,5);BringWindowToTop(w);SetForegroundWindow(w);return GetForegroundWindow()==w;}finally{if(attached)AttachThreadInput(current,foreground,false);}}
 public static bool Showing(){var c=new Cursor();c.size=Marshal.SizeOf(c);if(!GetCursorInfo(ref c))throw new Exception("Cursor query failed");return (c.flags&1)!=0;}
 public static IntPtr Find(uint pid){IntPtr r=IntPtr.Zero;EnumWindows((w,p)=>{uint id;GetWindowThreadProcessId(w,out id);var s=new StringBuilder(256);GetClassName(w,s,256);if(id==pid&&s.ToString()=="game-box"){r=w;return false;}return true;},IntPtr.Zero);return r;}
 public static void Move(IntPtr w,int x,int y){var p=new Point{x=x,y=y};ClientToScreen(w,ref p);SetCursorPos(p.x,p.y);PostMessage(w,512,IntPtr.Zero,new IntPtr((y<<16)|(x&65535)));}
}
'@
$keys=@('GB_HAND_CURSOR_TEST','GB_VULKAN_VALIDATION','GB_TERRAIN_LOD')
$before=@{};foreach($key in $keys){$before[$key]=[Environment]::GetEnvironmentVariable($key)}
$point=New-Object HandCursorNative+Point;[HandCursorNative]::GetCursorPos([ref]$point)|Out-Null
$foreground=[HandCursorNative]::GetForegroundWindow()
try{
 $env:GB_HAND_CURSOR_TEST='native';$env:GB_VULKAN_VALIDATION='0'
 $process=Start-Process (Join-Path $repoRoot $Executable) -WorkingDirectory (Join-Path $repoRoot 'gbDemo') -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/native-runtime.log" -RedirectStandardError "$out/native-errors.log"
 $processHandle=$process.Handle
 $timer=[Diagnostics.Stopwatch]::StartNew();$window=[IntPtr]::Zero
 while($timer.Elapsed.TotalSeconds -lt 90 -and !$process.HasExited){
  $window=[HandCursorNative]::Find([uint32]$process.Id)
  if($window -ne [IntPtr]::Zero -and (Get-Content "$out/native-runtime.log" -Raw) -match 'Hand cursor initialized:'){break}
  Start-Sleep -Milliseconds 150
 }
 if($window -eq [IntPtr]::Zero -or $process.HasExited){throw 'Owned demo did not initialize'}
 if(![HandCursorNative]::Activate($window)){throw 'Windows denied foreground activation of owned test window'}
 Start-Sleep -Seconds 2
 [HandCursorNative]::Move($window,600,390);Start-Sleep -Seconds 1
 if([HandCursorNative]::Showing()){
  $actual=New-Object HandCursorNative+Point;[HandCursorNative]::GetCursorPos([ref]$actual)|Out-Null
  throw "System cursor did not hide over terrain: expected=$window foreground=$([HandCursorNative]::GetForegroundWindow()) pointerWindow=$([HandCursorNative]::WindowFromPoint($actual)) runtime=$((Get-Content "$out/native-runtime.log" -Tail 3)-join '|')"
 }
 $rmbStart=(Get-Content "$out/native-runtime.log" -Raw).Length
 [HandCursorNative]::PostMessage($window,516,[IntPtr]2,[IntPtr]((390 -shl 16) -bor 600))|Out-Null
 Start-Sleep -Milliseconds 700
 if([HandCursorNative]::Showing()){throw 'System cursor returned during RMB'}
 $rmbLog=(Get-Content "$out/native-runtime.log" -Raw).Substring($rmbStart)
 if($rmbLog -notmatch 'Hand cursor visible' -or $rmbLog -match 'Hand cursor visibility=0'){throw 'Hand hidden during RMB'}
 [HandCursorNative]::PostMessage($window,517,[IntPtr]::Zero,[IntPtr]((390 -shl 16) -bor 600))|Out-Null
 [HandCursorNative]::Move($window,60,55);Start-Sleep -Milliseconds 700
 if(![HandCursorNative]::Showing()){throw 'System cursor did not return over UI'}
 [HandCursorNative]::Move($window,1120,550);Start-Sleep -Milliseconds 700
 if([HandCursorNative]::Showing()){throw 'System cursor did not hide over ocean'}
 [HandCursorNative]::Move($window,1285,400);Start-Sleep -Milliseconds 700
 if(![HandCursorNative]::Showing()){throw 'System cursor did not return outside viewport'}
 [HandCursorNative]::PostMessage($window,16,[IntPtr]::Zero,[IntPtr]::Zero)|Out-Null
 if(!$process.WaitForExit(10000)){throw 'Owned demo close timed out'}
 $exitCode=[uint32]0;[HandCursorNative]::GetExitCodeProcess($processHandle,[ref]$exitCode)|Out-Null
 if($exitCode -ne 0){throw "Owned demo failed: $exitCode"}
 'Hand native cursor: hidden on terrain/ocean and RMB, restored on UI/window exit, normal close PASS'
}finally{
 if($process -and !$process.HasExited){Stop-Process -Id $process.Id -Force}
 [HandCursorNative]::SetCursorPos($point.x,$point.y)|Out-Null;[HandCursorNative]::SetForegroundWindow($foreground)|Out-Null
 foreach($key in $keys){[Environment]::SetEnvironmentVariable($key,$before[$key])}
}

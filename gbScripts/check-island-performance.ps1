param([ValidateRange(0,200)][int]$Residents=200, [ValidateRange(1,10)][int]$Samples=3, [string]$Label='baseline', [ValidateSet('debug','release')][string]$Configuration='debug', [ValidateRange(0,90)][int]$SteadySeconds=0)
$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$output=Join-Path $repoRoot "gbDemo\output\island-check\perf-$Label-$Residents"
New-Item -ItemType Directory -Path $output -Force | Out-Null
if(-not ('IslandPerformanceWindow' -as [type])){Add-Type @'
using System;
using System.Runtime.InteropServices;
public static class IslandPerformanceWindow {
 public delegate bool Callback(IntPtr w,IntPtr p);
 [DllImport("user32.dll")] public static extern bool EnumWindows(Callback c,IntPtr p);
 [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr w,out uint id);
 [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr w,uint m,IntPtr a,IntPtr b);
 public static void Close(uint pid){EnumWindows((w,p)=>{uint id;GetWindowThreadProcessId(w,out id);if(id==pid)PostMessage(w,16,IntPtr.Zero,IntPtr.Zero);return true;},IntPtr.Zero);}
}
'@}
$keys=@('GB_PROFILE_SYSTEMS','GB_RESIDENT_COUNT','GB_RESIDENT_DEBUG','GB_RESIDENT_INSPECT','GB_RESIDENT_POSE','GB_RESIDENT_TIME','GB_OCEAN_FIXED_TIME','VK_INSTANCE_LAYERS','VK_SCREENSHOT_FRAMES','VK_SCREENSHOT_DIR')
$saved=@{};foreach($key in $keys){$saved[$key]=[Environment]::GetEnvironmentVariable($key);[Environment]::SetEnvironmentVariable($key,$null)}
$env:GB_PROFILE_SYSTEMS='1';$env:GB_RESIDENT_COUNT="$Residents"
$demo=$null
try {
 $demo=Start-Process -FilePath (Join-Path $repoRoot "gbDemo\output\$Configuration\x64\gb_demo.exe") -WorkingDirectory (Join-Path $repoRoot 'gbDemo') -WindowStyle Hidden -RedirectStandardOutput "$output\runtime.log" -RedirectStandardError "$output\errors.log" -PassThru
 $watch=[Diagnostics.Stopwatch]::StartNew()
 $steadyStart=$null;$threadStart=@{}
 do {
  Start-Sleep -Milliseconds 250
  if($demo.HasExited){throw 'Performance demo exited unexpectedly.'}
  $log=[string](Get-Content "$output\runtime.log" -Raw)
  $reports=[regex]::Matches($log,'PROFILE wall_ms=([\d.]+) feeder_ms=([\d.]+)')
  if($reports.Count -ge 1 -and $null -eq $steadyStart){
   $demo.Refresh();$steadyStart=$watch.Elapsed.TotalSeconds;$steadyCpu=$demo.TotalProcessorTime.TotalSeconds
   foreach($thread in $demo.Threads){try{$threadStart[$thread.Id]=$thread.TotalProcessorTime.TotalSeconds}catch{}}
  }
 } while(($reports.Count -lt $Samples -or ($null -ne $steadyStart -and $watch.Elapsed.TotalSeconds-$steadyStart -lt $SteadySeconds)) -and $watch.Elapsed.TotalSeconds -lt 120)
 if($reports.Count -lt $Samples){throw 'Performance sampling timed out.'}
 if($SteadySeconds -gt 0 -and ($null -eq $steadyStart -or $watch.Elapsed.TotalSeconds-$steadyStart -lt $SteadySeconds)){throw 'Requested steady-state capture duration timed out.'}
 $culture=[Globalization.CultureInfo]::InvariantCulture
 $wall=@($reports | ForEach-Object {[double]::Parse($_.Groups[1].Value,$culture)})
 $cpu=@($reports | ForEach-Object {[double]::Parse($_.Groups[2].Value,$culture)})
 $demo.Refresh()
 @($demo.Modules | ForEach-Object {$_.ModuleName}) | ConvertTo-Json | Set-Content "$output\modules.json"
 $summary=[pscustomobject]@{configuration=$Configuration;residents=$Residents;samples=$reports.Count;wall_ms=($wall | Measure-Object -Average).Average;feeder_ms=($cpu | Measure-Object -Average).Average;elapsed_seconds=$watch.Elapsed.TotalSeconds;process_cpu_seconds=$demo.TotalProcessorTime.TotalSeconds;private_bytes=$demo.PrivateMemorySize64;working_set_bytes=$demo.WorkingSet64;threads=$demo.Threads.Count}
 if($null -ne $steadyStart -and $watch.Elapsed.TotalSeconds -gt $steadyStart){
  $duration=$watch.Elapsed.TotalSeconds-$steadyStart
  $summary | Add-Member -NotePropertyName steady_cpu_core_equivalents -NotePropertyValue (($demo.TotalProcessorTime.TotalSeconds-$steadyCpu)/$duration)
  $summary | Add-Member -NotePropertyName first_report_seconds -NotePropertyValue $steadyStart
  $threadCpu=@(foreach($thread in $demo.Threads){try{if($threadStart.ContainsKey($thread.Id)){[pscustomobject]@{id=$thread.Id;cpu_core_equivalents=($thread.TotalProcessorTime.TotalSeconds-$threadStart[$thread.Id])/$duration}}}catch{}})
  $threadCpu | Sort-Object cpu_core_equivalents -Descending | ConvertTo-Json | Set-Content "$output\thread-cpu.json"
 }
 $summary | ConvertTo-Json | Set-Content "$output\summary.json"
 $summary | Format-List
 Select-String -Path "$output\runtime.log" -Pattern '^PROFILE' | Select-Object -Last 20 | ForEach-Object {$_.Line}
} finally {
 if($demo -and !$demo.HasExited){[IslandPerformanceWindow]::Close($demo.Id);if(!$demo.WaitForExit(5000)){Stop-Process -Id $demo.Id}}
 foreach($key in $keys){[Environment]::SetEnvironmentVariable($key,$saved[$key])}
}

param([int]$Repeats=2,[int]$Seconds=8,[ValidateSet("cel","soft")][string[]]$Cases=@("cel","soft"),[int]$StartRun=1,[switch]$Resume)
$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$out=Join-Path $repoRoot 'gbDemo/output/island-check/lighting-performance'
New-Item -ItemType Directory $out -Force | Out-Null
Add-Type @"
using System;using System.Text;using System.Runtime.InteropServices;
public static class PostProfileWindow {
 public delegate bool Callback(IntPtr w,IntPtr p);
 [DllImport("user32.dll")] public static extern bool EnumWindows(Callback c,IntPtr p);
 [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr w,out uint id);
 [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr w,uint m,IntPtr a,IntPtr b);
 [DllImport("user32.dll",CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr w,StringBuilder n,int l);
 [DllImport("kernel32.dll")] public static extern bool GetExitCodeProcess(IntPtr h,out uint code);
 public static IntPtr Find(uint pid){IntPtr r=IntPtr.Zero;EnumWindows((w,p)=>{uint id;GetWindowThreadProcessId(w,out id);var s=new StringBuilder(256);GetClassName(w,s,256);if(id==pid&&s.ToString()=="game-box"){r=w;return false;}return true;},IntPtr.Zero);return r;}
}
"@
$effectKeys=@('TONEMAP','GRADE','FOG')
$records=@();if($Resume -and (Test-Path (Join-Path $out 'results.csv'))){$records=@(Import-Csv (Join-Path $out 'results.csv'))}
$environmentNames=@($effectKeys|ForEach-Object {'GB_POST_'+$_})+@('GB_HUD_DEBUG','GB_POST_STAGE','GB_VULKAN_VALIDATION','GB_POST_VIEW','GB_POST_PRESET','GB_POST_EXPOSURE','VK_INSTANCE_LAYERS','VK_SCREENSHOT_FRAMES','VK_SCREENSHOT_DIR','GB_POST_DEBUG_PANEL','GB_LIGHTING_MODE','GB_LIGHTING_PRESET')
$previousEnvironment=@{}
foreach($name in $environmentNames){$previousEnvironment[$name]=[Environment]::GetEnvironmentVariable($name,'Process')}
try {
$env:GB_HUD_DEBUG='1';$env:GB_POST_STAGE='12';$env:GB_VULKAN_VALIDATION='0';$env:GB_POST_VIEW='0'
Remove-Item Env:VK_INSTANCE_LAYERS,Env:VK_SCREENSHOT_FRAMES,Env:VK_SCREENSHOT_DIR,Env:GB_POST_DEBUG_PANEL -ErrorAction SilentlyContinue
for($run=$StartRun;$run -lt ($StartRun+$Repeats);$run++) {
 foreach($case in $cases){
  foreach($key in $effectKeys){[Environment]::SetEnvironmentVariable('GB_POST_'+$key,'0','Process')}
  $env:GB_POST_PRESET='desktop';$env:GB_POST_EXPOSURE='0';$env:GB_LIGHTING_PRESET='StylizedSoftIsland';$env:GB_LIGHTING_MODE=if($case -eq 'cel'){'Cel'}else{'StylizedSoft'}
  foreach($key in $effectKeys){[Environment]::SetEnvironmentVariable('GB_POST_'+$key,'1','Process')}
  $env:GB_POST_EXPOSURE='.15'
  $prefix=Join-Path $out ($case+'-'+$run)
  $p=Start-Process -FilePath (Join-Path $repoRoot 'gbDemo/output/release/x64/gb_demo.exe') -WorkingDirectory (Join-Path $repoRoot 'gbDemo') -WindowStyle Hidden -PassThru -RedirectStandardOutput ($prefix+'.log') -RedirectStandardError ($prefix+'.err')
  $ownedHandle=$p.Handle
  try{
   $timer=[Diagnostics.Stopwatch]::StartNew()
   do{
    Start-Sleep -Milliseconds 250
    if($p.HasExited){throw 'Profile application exited before warmup'}
    $ready=Select-String -LiteralPath ($prefix+'.log') -Pattern 'Ocean patches=9' -Quiet
   }while(-not $ready -and $timer.Elapsed.TotalSeconds -lt 40)
   if($timer.Elapsed.TotalSeconds -ge 40){throw 'Profile warmup timeout'}
   Start-Sleep -Seconds $Seconds
   $w=[PostProfileWindow]::Find($p.Id);if($w -eq [IntPtr]::Zero){throw 'Owned window missing'}
   $null=[PostProfileWindow]::PostMessage($w,16,[IntPtr]::Zero,[IntPtr]::Zero)
   if(!$p.WaitForExit(15000)){throw 'Profile shutdown timeout'}
   [uint32]$code=259;$null=[PostProfileWindow]::GetExitCodeProcess($ownedHandle,[ref]$code);if($code -ne 0){throw "Profile exit $code"}
   if((Get-Content ($prefix+'.err') -Raw) -match '(?i)error:|failed|validation layer:'){throw 'Profile runtime error'}
   $hudMatches=[regex]::Matches((Get-Content ($prefix+'.log') -Raw),'HUD fps=([0-9.]+) triangles=([0-9]+) draws=([0-9]+)')
   $samples=@($hudMatches | Select-Object -Skip 8 | ForEach-Object {$fps=[double]$_.Groups[1].Value;if($fps -gt 0){1000.0/$fps}})
   if($samples.Count -lt 10){throw 'Insufficient steady HUD samples'}
   $sorted=@($samples|Sort-Object);$mean=($samples|Measure-Object -Average).Average;$median=$sorted[[int][Math]::Floor($sorted.Count*.5)]
   $row=[pscustomobject]@{case=$case;run=$run;mean_ms=[Math]::Round($mean,4);median_ms=[Math]::Round($median,4);samples=$samples.Count;draw_calls=[int]$hudMatches[$hudMatches.Count-1].Groups[3].Value}
   $records+=$row;$records|Export-Csv (Join-Path $out 'results.csv') -NoTypeInformation
   Write-Host ($row|ConvertTo-Json -Compress)
  }finally{if(!$p.HasExited){Stop-Process -Id $p.Id}}
 }
}
Write-Host 'Frame measurements include CPU, GPU waits and presentation; these are not GPU pass timestamps.'

}finally{foreach($name in $environmentNames){[Environment]::SetEnvironmentVariable($name,$previousEnvironment[$name],'Process')}}


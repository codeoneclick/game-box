param([ValidateSet('debug','release')][string]$Configuration='debug', [ValidateRange(1,5)][int]$Runs=1, [ValidateSet('0','1')][string]$Validation='1')
$ErrorActionPreference='Stop'
$repoRoot=Split-Path -Parent $PSScriptRoot
$output=Join-Path $repoRoot "gbDemo/output/island-check/shutdown-$Configuration"
if($Validation -eq '0'){$output+='-no-validation'}
New-Item -ItemType Directory -Path $output -Force | Out-Null
if(-not ('VulkanShutdownWindow' -as [type])){Add-Type @'
using System;
using System.Runtime.InteropServices;
using System.Text;
public static class VulkanShutdownWindow {
 public delegate bool Callback(IntPtr w,IntPtr p);
 [DllImport("user32.dll")] public static extern bool EnumWindows(Callback c,IntPtr p);
 [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr w,out uint id);
 [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr w,uint m,IntPtr a,IntPtr b);
 [DllImport("user32.dll",CharSet=CharSet.Auto)] public static extern int GetClassName(IntPtr w,StringBuilder n,int l);
 public static void Close(uint pid){EnumWindows((w,p)=>{uint id;GetWindowThreadProcessId(w,out id);if(id==pid){var n=new StringBuilder(256);GetClassName(w,n,256);if(n.ToString()=="game-box")PostMessage(w,16,IntPtr.Zero,IntPtr.Zero);}return true;},IntPtr.Zero);}
}
'@}
$oldValidation=$env:GB_VULKAN_VALIDATION
$env:GB_VULKAN_VALIDATION=$Validation
try {
 for($run=1;$run -le $Runs;++$run){
  $stdout=Join-Path $output "run-$run.log"
  $stderr=Join-Path $output "run-$run.errors.log"
  $demo=Start-Process -FilePath (Join-Path $repoRoot "gbDemo/output/$Configuration/x64/gb_demo.exe") -WorkingDirectory (Join-Path $repoRoot 'gbDemo') -WindowStyle Hidden -RedirectStandardOutput $stdout -RedirectStandardError $stderr -PassThru
  # Retain the process handle before it exits so ExitCode remains available.
  $null=$demo.Handle
  try {
   $watch=[Diagnostics.Stopwatch]::StartNew()
   do {
    Start-Sleep -Milliseconds 100
    if($demo.HasExited){throw "Demo exited before close: $($demo.ExitCode)"}
    $log=[string](Get-Content -LiteralPath $stdout -Raw)
   } while(($log -notmatch 'Ocean patches=9 terraces=8\+8' -or $watch.Elapsed.TotalSeconds -lt 5) -and $watch.Elapsed.TotalSeconds -lt 60)
   if($log -notmatch 'Ocean patches=9 terraces=8\+8'){throw 'Scene initialization timed out.'}
   $expectedValidation=if($Validation -eq '1'){'on'}else{'off'}
   if($log -notmatch "Vulkan validation: $expectedValidation"){throw 'Requested Vulkan validation state was not applied.'}
   [VulkanShutdownWindow]::Close($demo.Id)
   if(!$demo.WaitForExit(20000)){throw 'Normal close timed out; shutdown is not verified.'}
   if($demo.ExitCode -ne 0){throw "Normal close crashed: exit=$($demo.ExitCode); logs=$output"}
   $errors=[string](Get-Content -LiteralPath $stderr -Raw)
   if($errors -match '(?i)validation layer:|error:|failed'){throw "Shutdown validation errors: $stderr"}
   "Vulkan $Configuration shutdown run $run passed: normal exit 0, validation=$expectedValidation, no runtime errors"
  } finally {
   if(!$demo.HasExited){Stop-Process -Id $demo.Id}
  }
 }
} finally {$env:GB_VULKAN_VALIDATION=$oldValidation}

param([string]$CaptureName='ocean', [string]$FixedTime='', [int]$TimeoutSeconds=45)
$ErrorActionPreference='Stop'
if(-not ('OceanCheckWindow' -as [type])){Add-Type @'
using System;
using System.Text;
using System.Runtime.InteropServices;
public static class OceanCheckWindow {
 public delegate bool Callback(IntPtr w,IntPtr p);
 [DllImport("user32.dll")] public static extern bool EnumWindows(Callback c,IntPtr p);
 [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr w,out uint id);
 [DllImport("user32.dll",CharSet=CharSet.Auto)] public static extern int GetClassName(IntPtr w,StringBuilder n,int l);
 [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr w,uint m,IntPtr a,IntPtr b);
 public static IntPtr Find(uint pid){IntPtr r=IntPtr.Zero;EnumWindows((w,p)=>{uint id;GetWindowThreadProcessId(w,out id);if(id==pid){var n=new StringBuilder(256);GetClassName(w,n,256);if(n.ToString()=="game-box")r=w;}return true;},IntPtr.Zero);return r;}
}
'@}
$repoRoot=Split-Path -Parent $PSScriptRoot
$captureDir=Join-Path $repoRoot "gbDemo\output\island-check\$CaptureName"
New-Item -ItemType Directory -Path $captureDir -Force | Out-Null
$frames=@(6,80,160)
foreach($frame in $frames){Remove-Item -LiteralPath "$captureDir\$frame.ppm" -ErrorAction SilentlyContinue}
$oldLayers=$env:VK_INSTANCE_LAYERS;$oldFrames=$env:VK_SCREENSHOT_FRAMES;$oldDir=$env:VK_SCREENSHOT_DIR;$oldTime=$env:GB_OCEAN_FIXED_TIME
$env:VK_INSTANCE_LAYERS='VK_LAYER_LUNARG_screenshot';$env:VK_SCREENSHOT_FRAMES=$frames -join ',';$env:VK_SCREENSHOT_DIR=$captureDir
if($FixedTime -ne ''){$env:GB_OCEAN_FIXED_TIME=$FixedTime}else{Remove-Item Env:GB_OCEAN_FIXED_TIME -ErrorAction SilentlyContinue}
$demo=$null
try{
 $demo=Start-Process -FilePath (Join-Path $repoRoot 'gbDemo\output\debug\x64\gb_demo.exe') -WorkingDirectory (Join-Path $repoRoot 'gbDemo') -WindowStyle Hidden -RedirectStandardOutput "$captureDir\runtime.log" -RedirectStandardError "$captureDir\errors.log" -PassThru
 $watch=[Diagnostics.Stopwatch]::StartNew()
 while(!$demo.HasExited -and $watch.Elapsed.TotalSeconds -lt $TimeoutSeconds){
  if((Get-Content "$captureDir\runtime.log" -Raw) -match 'Saved image:.*[/\\]160\.ppm'){break}
  Start-Sleep -Milliseconds 100
 }
 if((Get-Content "$captureDir\runtime.log" -Raw) -notmatch 'Saved image:.*[/\\]160\.ppm'){throw 'Ocean frames were not fully captured.'}
 if((Get-Content "$captureDir\runtime.log" -Raw) -notmatch 'Ocean patches=9 terraces=8\+8'){throw 'Ocean scene was not initialized.'}
 $errors=Get-Content "$captureDir\errors.log" -Raw
 if($errors -match '(?i)validation layer:|error:|failed'){throw "Runtime rendering errors: $errors"}
 'Ocean scene and multiframe Vulkan capture passed'
}finally{
 if($demo -and !$demo.HasExited){
  $window=[OceanCheckWindow]::Find($demo.Id)
  if($window -ne [IntPtr]::Zero){[void][OceanCheckWindow]::PostMessage($window,16,[IntPtr]::Zero,[IntPtr]::Zero)}
  if(!$demo.WaitForExit(5000)){Stop-Process -Id $demo.Id;Write-Warning 'Demo did not close normally; shutdown verification unavailable.'}
 }
 $env:VK_INSTANCE_LAYERS=$oldLayers;$env:VK_SCREENSHOT_FRAMES=$oldFrames;$env:VK_SCREENSHOT_DIR=$oldDir;$env:GB_OCEAN_FIXED_TIME=$oldTime
}
$shutdown=Get-Content "$captureDir\errors.log" -Raw
if($shutdown -match 'vkDestroyCommandPool'){'Known shutdown command-pool validation messages remain; see errors.log'}

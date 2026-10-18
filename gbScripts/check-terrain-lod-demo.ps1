param(
    [string]$Executable = 'gbDemo/output/release/x64/gb_demo.exe',
    [switch]$Exercise,
    [switch]$Wireframe,
    [switch]$Zoom,
    [switch]$Orbit,
    [switch]$Inspector,
    [switch]$WaterFields,
    [switch]$TechniqueSizes,
    [switch]$WaterOptics,
    [switch]$Sculpt,
    [switch]$SculptUndo,
    [switch]$NoValidation,
    [ValidateSet('reef','coast','1')][string]$OceanInspect,
    [ValidateRange(0,20)][int]$OceanDebug=0,
    [ValidateRange(1,5)][int]$InspectorMode=1,
    [switch]$Profile,
    [ValidateSet('0','1')][string]$TerrainLodMode
)
# Windows Vulkan integration smoke. Requires VK_LAYER_LUNARG_screenshot.
# Operates only on its own process; restores GraphicsSettings byte-for-byte.
$ErrorActionPreference = 'Stop'
if ($Profile -and ($Exercise -or $Wireframe -or $Zoom -or $Orbit -or $Inspector)) { throw 'Profile uses a fixed camera without interaction switches.' }
if ($Zoom -and ($Exercise -or $Wireframe)) { throw 'Zoom is a separate camera interaction check.' }
$repoRoot = Split-Path -Parent $PSScriptRoot
if (![IO.Path]::IsPathRooted($Executable)) { $Executable = Join-Path $repoRoot $Executable }
$output = Join-Path $repoRoot ('gbDemo/output/island-check/terrain-lod-work/demo-' + [Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $output -Force | Out-Null
Add-Type @'
using System;using System.Text;using System.Runtime.InteropServices;
public static class TerrainLodSmoke {
 public delegate bool Callback(IntPtr w,IntPtr p);
 [DllImport("user32.dll")] public static extern bool EnumWindows(Callback c,IntPtr p);
 [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr w,out uint id);
 [DllImport("user32.dll",CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr w,StringBuilder n,int l);
 [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr w,uint m,IntPtr a,IntPtr b);
 [DllImport("kernel32.dll")] public static extern bool GetExitCodeProcess(IntPtr h,out uint code);
 [StructLayout(LayoutKind.Sequential)] public struct Point { public int x,y; }
 [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr w,ref Point p);
 public static void WheelAt(IntPtr w,int ticks,int x,int y){var p=new Point{x=x,y=y};ClientToScreen(w,ref p);PostMessage(w,522,new IntPtr((ticks*120)<<16),new IntPtr((p.y<<16)|(p.x&65535)));}
 public static void Wheel(IntPtr w,int ticks){WheelAt(w,ticks,600,380);}
 public static IntPtr Find(uint pid){IntPtr r=IntPtr.Zero;EnumWindows((w,p)=>{uint id;GetWindowThreadProcessId(w,out id);var s=new StringBuilder(256);GetClassName(w,s,256);if(id==pid&&s.ToString()=="game-box"){r=w;return false;}return true;},IntPtr.Zero);return r;}
 public static void Mouse(IntPtr w,uint m,int x,int y,int keys){PostMessage(w,m,new IntPtr(keys),new IntPtr((y<<16)|(x&65535)));}
}
'@
$settings = Join-Path $repoRoot 'gbResources/GraphicsSettings.json'
$original = [IO.File]::ReadAllBytes($settings)
$environmentBefore = @{}
$overrides = @{
    GB_VULKAN_VALIDATION=$(if ($Profile -or $NoValidation) { '0' } else { '1' })
    GB_RENDER_DEBUG_LOG=$(if ($Profile) { $null } else { '1' })
    GB_TERRAIN_LOD_DEBUG=$(if ($Profile) { $null } else { '1' })
    GB_PROFILE_SYSTEMS=$(if ($Profile) { '1' } else { $null })
    GB_EDITOR_PROFILE=$(if($Sculpt){'1'}else{$null})
    GB_HUD_DEBUG=$(if ($Profile) { '1' } else { $null })
    GB_OCEAN_FIXED_TIME='1'; GB_RESIDENT_POSE='idle'; GB_RESIDENT_TIME='0'
    VK_INSTANCE_LAYERS='VK_LAYER_LUNARG_screenshot'; VK_SCREENSHOT_DIR=$output
    VK_SCREENSHOT_FRAMES=$(if ($WaterOptics) { '60,120,180,300,420' } elseif ($Profile) { '360' } elseif ($Sculpt -or $Exercise -or $Wireframe -or $Zoom -or $Orbit -or $Inspector) { '60,180,300,420' } else { '60' })
    GB_OCEAN_INSPECT=$(if($WaterOptics -and !$OceanInspect){'coast'}else{$OceanInspect}); GB_OCEAN_DEBUG=[string]$OceanDebug; GB_POST_DEBUG_PANEL=$null; GB_TERRAIN_LOD=$TerrainLodMode
}
foreach ($key in $overrides.Keys) {
    $environmentBefore[$key] = [Environment]::GetEnvironmentVariable($key)
    [Environment]::SetEnvironmentVariable($key,$overrides[$key])
}
function Wait-Frame([int]$number) {
    $timer = [Diagnostics.Stopwatch]::StartNew()
    while (!(Test-Path "$output/$number.ppm") -and !$script:process.HasExited -and $timer.Elapsed.TotalSeconds -lt 240) { Start-Sleep -Milliseconds 100 }
    if (!(Test-Path "$output/$number.ppm")) { throw "Missing Vulkan screenshot $number; inspect $output" }
    Write-Output "Captured frame $number"
}
function Click([int]$x,[int]$y) {
    [TerrainLodSmoke]::Mouse($script:window,512,$x,$y,0)
    [TerrainLodSmoke]::Mouse($script:window,513,$x,$y,1)
    Start-Sleep -Milliseconds 80
    [TerrainLodSmoke]::Mouse($script:window,514,$x,$y,0)
    Start-Sleep -Milliseconds 200
}
function Key([int]$code) {
    [TerrainLodSmoke]::PostMessage($script:window,256,[IntPtr]$code,[IntPtr]::Zero) | Out-Null
    Start-Sleep -Milliseconds 80
    [TerrainLodSmoke]::PostMessage($script:window,257,[IntPtr]$code,[IntPtr]::Zero) | Out-Null
    Start-Sleep -Milliseconds 200
}
function Drag([int]$x,[int]$y,[int]$endX) {
    [TerrainLodSmoke]::Mouse($script:window,512,$x,$y,0)
    [TerrainLodSmoke]::Mouse($script:window,513,$x,$y,1)
    Start-Sleep -Milliseconds 100
    [TerrainLodSmoke]::Mouse($script:window,512,$endX,$y,1)
    Start-Sleep -Milliseconds 100
    [TerrainLodSmoke]::Mouse($script:window,514,$endX,$y,0)
    Start-Sleep -Milliseconds 250
}
try {
    if($WaterOptics){
        $temporary=[Text.Encoding]::UTF8.GetString($original) | ConvertFrom-Json
        foreach($name in @('Reflection','Refraction','Caustics')){
            $temporary.postProcess | Add-Member -Force -NotePropertyName ('enableWater'+$name) -NotePropertyValue $true
            $temporary.postProcess | Add-Member -Force -NotePropertyName ('water'+$name+'Intensity') -NotePropertyValue 1.0
        }
        [IO.File]::WriteAllText($settings,($temporary | ConvertTo-Json -Depth 20))
    }
    if ($Zoom -or $Orbit) {
        $temporary = Get-Content -LiteralPath $settings -Raw | ConvertFrom-Json
        $temporary.terrainLOD.lodCount=6
        $temporary.terrainLOD.contourSimplificationPercent=90
        $temporary.terrainLOD.wireframe=[bool]$Zoom
        if($Orbit){$temporary.terrainLOD | Add-Member -Force -NotePropertyName debugMode -NotePropertyValue 1}
        [IO.File]::WriteAllText($settings,($temporary | ConvertTo-Json -Depth 20))
    }
    $script:process = Start-Process -FilePath $Executable -WorkingDirectory (Join-Path $repoRoot 'gbDemo') -WindowStyle Hidden -PassThru -RedirectStandardOutput "$output/runtime.log" -RedirectStandardError "$output/errors.log"
    $processHandle = $process.Handle
    if ($Profile) { Wait-Frame 360 } else { Wait-Frame 60 }
    $script:window = [TerrainLodSmoke]::Find([uint32]$process.Id)
    if ($window -eq [IntPtr]::Zero) { throw 'Owned game window unavailable' }
    if($Sculpt){
        [TerrainLodSmoke]::Mouse($window,512,450,440,0)
        [TerrainLodSmoke]::Mouse($window,513,450,440,1)
        for($i=0;$i -lt 20;++$i){Start-Sleep -Milliseconds 40;[TerrainLodSmoke]::Mouse($window,512,(450+$i*3),440,1)}
        [TerrainLodSmoke]::Mouse($window,514,507,440,0)
        Wait-Frame 180
        if($SculptUndo){Click 60 55}
        Wait-Frame 300
        if($SculptUndo){Click 154 55}
        Wait-Frame 420
    }
    if($WaterOptics){
        Key 119
        for($i=0;$i -lt 5;++$i){[TerrainLodSmoke]::WheelAt($window,-1,400,380)}
        Click 400 323 # ws.ocean, index 19 after list scroll reaches 14
        Wait-Frame 120
        $names=@('Reflection','Refraction','Caustics');$ys=@(418,470,522);$frames=@(180,300,420)
        for($i=0;$i -lt 3;++$i){
            if($i -gt 0){Key 119;Click 918 $ys[$i-1]}
            Click 918 $ys[$i]
            Click 890 76
            Copy-Item -LiteralPath $settings -Destination "$output/optics-$($names[$i]).json"
            $saved=Get-Content -LiteralPath $settings -Raw | ConvertFrom-Json
            for($j=0;$j -lt 3;++$j){
                $value=$saved.postProcess.('enableWater'+$names[$j])
                if($value -ne ($i -ne $j)){throw "Water optics toggle/Save failed: $($names[$j])"}
            }
            Key 119
            Wait-Frame $frames[$i]
        }
    }
    if ($Orbit) {
        [TerrainLodSmoke]::Wheel($window,10)
        Wait-Frame 180
        foreach($frame in @(300,420)){
            [TerrainLodSmoke]::Mouse($window,512,600,380,0)
            [TerrainLodSmoke]::Mouse($window,519,600,380,16)
            Start-Sleep -Milliseconds 80
            [TerrainLodSmoke]::Mouse($window,512,1100,380,16)
            [TerrainLodSmoke]::Mouse($window,520,1100,380,0)
            Wait-Frame $frame
        }
    }
    if ($Inspector) {
        Key 119
        for($i=0;$i -lt 25;$i++){[TerrainLodSmoke]::Wheel($window,-1)}
        Wait-Frame 180
        for($i=0;$i -lt $InspectorMode;$i++){Click 740 620} # final ws.base detail row: debug mode
        Key 119
        Wait-Frame 300
        Key 119
        Click 890 76
        $saved=Get-Content -LiteralPath $settings -Raw | ConvertFrom-Json
        if($saved.terrainLOD.debugMode -ne $InspectorMode){throw 'Terrain debug control or F8 Save failed'}
        if($saved.terrainLOD.nearErrorPx -ne .5 -or $saved.terrainLOD.normalErrorPx -ne 1.5 -or $saved.terrainLOD.farErrorPx -ne 5){throw 'SSE budgets not persisted'}
        Key 119
        Wait-Frame 420
    }
    if ($Zoom) {
        Add-Content "$output/camera-stages.log" '60: starter'
        [TerrainLodSmoke]::Wheel($window,10)
        Wait-Frame 180
        Add-Content "$output/camera-stages.log" '180: close'
        [TerrainLodSmoke]::Wheel($window,-24)
        Wait-Frame 300
        Add-Content "$output/camera-stages.log" '300: distant'
        [TerrainLodSmoke]::Wheel($window,14)
        Wait-Frame 420
        Add-Content "$output/camera-stages.log" '420: return'
    }
    if ($Wireframe) {
        Click 855 63 # Wireframe On
        Wait-Frame 180
        Key 119
        Click 890 76
        Copy-Item -LiteralPath $settings -Destination "$output/saved.json"
        $saved = Get-Content -LiteralPath $settings -Raw | ConvertFrom-Json
        if (!$saved.terrainLOD.wireframe) { throw 'Wireframe not persisted by F8 Save' }
        Key 119
        Click 855 63 # Wireframe Off
        Wait-Frame 300
        Click 855 63 # On again: reuse cached pipeline
        Wait-Frame 420
    }
    if ($Exercise) {
        # Default 1280x720 window. Track coordinates are transformed by gbUI.
        Drag 800 139 900 # maximum P; also test capture beyond the track
        Click 687 63  # N=1; P remains stored, disabled
        Wait-Frame 180
        Drag 740 63 900 # N=8
        Click 744 63  # N=5; supersede pending generation
        Wait-Frame 300
        Key 119      # F8
        Click 890 76 # Render Inspector Save
        Copy-Item -LiteralPath $settings -Destination "$output/saved.json"
        $saved = Get-Content -LiteralPath $settings -Raw | ConvertFrom-Json
        if ($saved.terrainLOD.lodCount -ne 5 -or $saved.terrainLOD.contourSimplificationPercent -ne 90) { throw 'F8 Save did not persist N=5/P=90' }
        Key 119
        Click 550 465 # terrain stroke
        Click 60 55   # undo
        Click 154 55  # redo
        Wait-Frame 420
    }
    [TerrainLodSmoke]::PostMessage($window,16,[IntPtr]::Zero,[IntPtr]::Zero) | Out-Null
    if (!$process.WaitForExit(10000)) { throw 'Demo did not close normally' }
    $exitCode = [uint32]0
    [TerrainLodSmoke]::GetExitCodeProcess($processHandle,[ref]$exitCode) | Out-Null
    if ($exitCode -ne 0) { throw "Demo failed with exit $exitCode" }
    if (Select-String -Path "$output/errors.log","$output/runtime.log" -Pattern 'Validation Error|VUID-|Assertion failed|terrain rebuild failed|Editor refinement failed') { throw "Runtime validation failed; inspect $output" }
    if($Sculpt){
        $samples=@();foreach($line in Get-Content "$output/runtime.log"){
            if($line -match '^Editor visual_latency_ms=([0-9.]+) revision=([0-9]+)' -and [int]$Matches[2] -gt 1){$samples += [double]$Matches[1]}
            if($line -match '^Editor rebuilt region=0,0\.\.1023,1023'){throw 'Local sculpt triggered a full-island rebuild'}
        }
        if($samples.Count -lt 2){throw 'Sculpt did not publish intermediate terrain while the brush was moving'}
        $sorted=@($samples | Sort-Object);$median=$sorted[[int][Math]::Floor($sorted.Count/2)];$maximum=$sorted[-1]
        if($maximum -gt 500){throw "Sculpt update took $maximum ms"}
        Write-Output "Sculpt latency: median=$median ms, max=$maximum ms, updates=$($samples.Count)"
    }
    if($WaterOptics){
        # Fixed camera/waves/pose. Exclude all HUD, inspector, foam and dry land.
        function Read-OpticsFrame([int]$frame){
            $bytes=[IO.File]::ReadAllBytes("$output/$frame.ppm")
            $header=[regex]::Match([Text.Encoding]::ASCII.GetString($bytes,0,64),'^P6\s+(\d+)\s+(\d+)\s+255\r?\n')
            if(!$header.Success -or $header.Groups[1].Value -ne '1280' -or $header.Groups[2].Value -ne '720'){throw 'Unexpected water optics capture format'}
            return @{Bytes=$bytes;Offset=$header.Length}
        }
        $baseline=Read-OpticsFrame 60
        foreach($sample in @(@('Reflection',180),@('Refraction',300),@('Caustics',420))){
            $image=Read-OpticsFrame $sample[1];$difference=0L;$changed=0
            for($y=320;$y -lt 640;++$y){for($x=720;$x -lt 940;++$x){
                $pixel=($y*1280+$x)*3;$delta=0
                for($channel=0;$channel -lt 3;++$channel){$delta += [Math]::Abs([int]$baseline.Bytes[$baseline.Offset+$pixel+$channel]-[int]$image.Bytes[$image.Offset+$pixel+$channel])}
                $difference+=$delta;if($delta -gt 0){++$changed}
            }}
            $mean=$difference/(220.0*320*3)
            if($changed -lt 100 -or $mean -lt .02){throw "Water $($sample[0]) toggle did not change the rendered water"}
            Add-Content "$output/optics-differences.log" "$($sample[0]): changed=$changed meanRGB=$mean"
        }
    }
    if($WaterFields){
        $runtime=Get-Content "$output/runtime.log" -Raw
        foreach($file in Get-ChildItem (Join-Path $repoRoot 'gbBundle/Configurations/techniques/ws_techniques') -Filter 'ws.technique.water*.xml'){
            [xml]$configuration=Get-Content -LiteralPath $file.FullName
            $pass=$configuration.ws_technique
            $pattern='Render attachment '+[regex]::Escape($pass.guid)+' .* size='+$pass.frame_width+'x'+$pass.frame_height
            if($runtime -notmatch $pattern){throw "GPU water attachment does not match XML: $($pass.guid)"}
        }
    }
    if($TechniqueSizes){
        $runtime=Get-Content "$output/runtime.log" -Raw
        [xml]$transition=Get-Content (Join-Path $repoRoot 'gbBundle/Configurations/transitions/transition.island_demo.xml')
        foreach($kind in @('ws','ss')){
            foreach($reference in $transition.transition.SelectNodes($kind+'_techniques/'+$kind+'_technique')){
                $file=Join-Path $repoRoot ('gbBundle/Configurations/techniques/'+$kind+'_techniques/'+$reference.filename)
                [xml]$configuration=Get-Content -LiteralPath $file
                $pass=$configuration.DocumentElement
                $pattern='Render attachment '+[regex]::Escape($pass.guid)+' .* size='+$pass.frame_width+'x'+$pass.frame_height+'(?:\r?\n|$)'
                if($runtime -notmatch $pattern){throw "GPU attachment does not match XML: $($pass.guid), expected $($pass.frame_width)x$($pass.frame_height)"}
            }
        }
    }
    if ($Zoom) {
        $lines = @(Get-Content "$output/runtime.log")
        $stages = @{}
        foreach ($frame in @(60,180,300,420)) {
            $index = 0
            while ($index -lt $lines.Count -and $lines[$index] -notmatch "Queued screeshot for frame: $frame$") { $index++ }
            if ($index -eq $lines.Count) { throw "Missing frame $frame diagnostic marker" }
            $sample = @($lines[[Math]::Max(0,$index-6)..($index-1)] | Where-Object { $_ -match 'Nodes .*LOD:' })
            $stages[$frame] = $sample[-1]
            Add-Content "$output/camera-stages.log" "$frame $($stages[$frame])"
        }
        if ($stages[60] -notmatch 'LOD:.* [0-4]:') { throw 'Starter camera already exhausted all contour LODs' }
        if ($stages[180] -notmatch 'LOD:.* [0-4]:') { throw 'Zooming in did not restore finer nearby contours' }
        if ($stages[300] -notmatch 'LOD: 5:\d+$') { throw 'Zooming out did not select coarsest contours' }
        if ($stages[420] -eq $stages[300]) { throw 'Returning camera left the contour selection static' }
        $farIndex=0;while($farIndex -lt $lines.Count -and $lines[$farIndex] -notmatch 'Queued screeshot for frame: 300$'){$farIndex++}
        $farGeometry=($lines[($farIndex-6)..($farIndex-1)] -join ' ')
        if($farGeometry -notmatch 'Terrain (\d+) -> (\d+) tris'){throw 'Missing distant geometry counts'}
        if([double]$Matches[2] -gt [double]$Matches[1]*.205){throw 'Distant terrain did not approach fivefold triangle reduction'}
    }
    Write-Output "Terrain LOD Vulkan smoke passed: $output"
} finally {
    if ($process -and !$process.HasExited) {
        $window = [TerrainLodSmoke]::Find([uint32]$process.Id)
        [TerrainLodSmoke]::PostMessage($window,16,[IntPtr]::Zero,[IntPtr]::Zero) | Out-Null
        $process.WaitForExit(10000) | Out-Null
    }
    [IO.File]::WriteAllBytes($settings,$original)
    foreach ($key in $environmentBefore.Keys) { [Environment]::SetEnvironmentVariable($key,$environmentBefore[$key]) }
}

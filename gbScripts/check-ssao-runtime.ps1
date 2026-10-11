param([int]$TimeoutSeconds=120)
$ErrorActionPreference='Stop'
$keys=@('GB_OCEAN_INSPECT','GB_RESIDENT_INSPECT','GB_RESIDENT_POSE','GB_RESIDENT_TIME','GB_SSAO','GB_SSAO_DEBUG','GB_OCEAN_FIELD_VIEW','GB_OCEAN_DEBUG','GB_ISLAND_LAYER_COLORS')
$saved=@{}
foreach($key in $keys){$saved[$key]=[Environment]::GetEnvironmentVariable($key);[Environment]::SetEnvironmentVariable($key,$null)}
try{
    $env:GB_OCEAN_INSPECT='1'
    $env:GB_RESIDENT_POSE='idle'
    $env:GB_RESIDENT_TIME='0.5'
    foreach($case in @('ssao-on','ssao-off','ssao-mask')){
        $env:GB_SSAO=if($case -eq 'ssao-off'){'0'}else{'1'}
        $env:GB_SSAO_DEBUG=if($case -eq 'ssao-mask'){'1'}else{'0'}
        & "$PSScriptRoot/check-ocean-runtime.ps1" -CaptureName $case -FixedTime 3 -TimeoutSeconds $TimeoutSeconds
    }
    & python "$PSScriptRoot/check-ssao-images.py"
    if($LASTEXITCODE -ne 0){throw 'SSAO image checks failed.'}
}finally{
    foreach($key in $keys){[Environment]::SetEnvironmentVariable($key,$saved[$key])}
}

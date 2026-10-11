@set PATH=C:\Program Files (x86)\Microsoft Visual Studio\Installer;;%PATH%
@call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
@cd /d "C:\Hobby\game-box\gbDemo\output\ecs-check"
@cl /nologo /EHsc /std:c++14 /Gy /I"C:\Hobby\game-box\gbDemo\sources" /O2 "C:\Hobby\game-box\gbScripts\check_island_resident.cpp" "C:\Hobby\game-box\gbDemo\sources\terrain_generator.cpp" "C:\Hobby\game-box\gbDemo\sources\terrain_mesh_builder.cpp" "C:\Hobby\game-box\gbDemo\sources\resident_navigation.cpp" "C:\Hobby\game-box\gbDemo\sources\resident_behavior.cpp" /Fe:"C:\Hobby\game-box\gbDemo\output\ecs-check\resident_cpu.exe" 
@if errorlevel 1 exit /b 1
@"C:\Hobby\game-box\gbDemo\output\ecs-check\resident_cpu.exe"

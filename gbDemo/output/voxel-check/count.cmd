@call "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul
@cl /nologo /EHsc /std:c++14 /I"C:\Hobby\game-box/gbDemo/sources" "C:\Hobby\game-box\gbDemo\output\voxel-check/count.cpp" "C:\Hobby\game-box/gbDemo/sources/voxel_tree_builder.cpp" /Fe:"C:\Hobby\game-box\gbDemo\output\voxel-check/count.exe" /Fo:"C:\Hobby\game-box\gbDemo\output\voxel-check/"

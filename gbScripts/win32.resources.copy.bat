::configurations
for /R ..\gbBundle %%f in (*.xml) do copy %%f ..\gbWin32SharedResources\
for /R ..\gbBundle %%f in (*.json) do copy %%f ..\gbWin32SharedResources\
::shaders
for /R ..\gbBundle %%f in (*.vert) do copy %%f ..\gbWin32SharedResources\
for /R ..\gbBundle %%f in (*.frag) do copy %%f ..\gbWin32SharedResources\
::animations
for /R ..\gbBundle %%f in (*.ani) do copy %%f ..\gbWin32SharedResources\
::textures
for /R ..\gbBundle %%f in (*.png) do copy %%f ..\gbWin32SharedResources\
::3d meshes
for /R ..\gbBundle %%f in (*.gb3dmesh) do copy %%f ..\gbWin32SharedResources\
::3d animations
for /R ..\gbBundle %%f in (*.gb3danim) do copy %%f ..\gbWin32SharedResources\
::fonts
for /R ..\gbBundle %%f in (*.ttf) do copy %%f ..\gbWin32SharedResources\
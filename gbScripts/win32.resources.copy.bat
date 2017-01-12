::configurations
for /R ..\gbBundle %%f in (*.xml) do copy %%f ..\gbDemo\output\debug\resources\
for /R ..\gbBundle %%f in (*.json) do copy %%f ..\gbDemo\output\debug\resources\
::shaders
for /R ..\gbBundle %%f in (*.vert) do copy %%f ..\gbDemo\output\debug\resources\
for /R ..\gbBundle %%f in (*.frag) do copy %%f ..\gbDemo\output\debug\resources\
::animations
for /R ..\gbBundle %%f in (*.ani) do copy %%f ..\gbDemo\output\debug\resources\
::textures
for /R ..\gbBundle %%f in (*.png) do copy %%f ..\gbDemo\output\debug\resources\
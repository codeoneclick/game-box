for /R ..\gbBundle %%f in (*.vert) do C:/VulkanSDK/1.1.73.0/Bin32/glslangValidator.exe -V %%f 
for /R ..\gbBundle %%f in (*.frag) do C:/VulkanSDK/1.1.73.0/Bin32/glslangValidator.exe -V %%f 

pause
echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"

dxc.exe -E main -Fo layering_pixel.cso 	-T ps_6_0 -nologo layering_pixel.hlsl

dxc.exe -E main -Fo layering_vertex.cso -T vs_6_0 -nologo layering_vertex.hlsl

echo Finished compiling shaders
pause
mkdir Bin\
copy Dependencies\dll\* Bin\
call "Premake/premake5" --defaultproject=Tutorial-01_Init --file=Source/Tutorials/premake5.lua vs2022
pause
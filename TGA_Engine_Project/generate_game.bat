mkdir Bin\
copy Dependencies\dll\* Bin\
call "Premake/premake5" --file=Source/Game/premake5.lua vs2022
pause

@echo off

set CommonCompilerFlags=-MTd -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4505 -wd4530 -wd4201 -wd4100 -wd4189 -wd4244 -wd4996 -wd4702 -DTETRIS_INTERNAL=1 -DTETRIS_SLOW=1 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib opengl32.lib

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build

del *.pdb > NUL 2> NUL

cl %CommonCompilerFlags% ..\tetris\code\tetris.cpp -Fmtetris.map -LD /link -incremental:no -opt:ref -PDB:tetris_%random%.pdb -EXPORT:GameUpdateAndRender

cl %CommonCompilerFlags% ..\tetris\code\main.cpp -Fmmain.map /link %CommonLinkerFlags%

cl %CommonCompilerFlags% ..\tetris\code\win32_tetris.cpp -Fmwin32_tetris.map /link %CommonLinkerFlags%

popd

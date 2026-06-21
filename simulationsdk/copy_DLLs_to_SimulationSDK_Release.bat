set mode=Release
set d=

set currentDir=%cd%
set "pathTo_SimulationSDK=%~dp0Code\Bin\vc143\x64\vs2022_%mode%\"

if not exist "%pathTo_SimulationSDK%" (mkdir "%pathTo_SimulationSDK%")

REM copy Third Party DLLs to SimulationSDK bin folder
copy /y "..\thirdparty\build\poco\vs2022-x64\bin\PocoNet%d%.dll" "%pathTo_SimulationSDK%"
copy /y "..\thirdparty\build\poco\vs2022-x64\bin\PocoFoundation%d%.dll" "%pathTo_SimulationSDK%"
copy /y "..\thirdparty\build\poco\vs2022-x64\bin\PocoJSON%d%.dll" "%pathTo_SimulationSDK%"
copy /y "..\thirdparty\build\poco\vs2022-x64\bin\PocoUtil%d%.dll" "%pathTo_SimulationSDK%"
copy /y "..\thirdparty\build\poco\vs2022-x64\bin\PocoXML%d%.dll" "%pathTo_SimulationSDK%"
copy /y "..\thirdparty\boost\stage\x64\lib\boost_filesystem-vc143-mt-x64-1_88.dll" "%pathTo_SimulationSDK%"
copy /y "..\thirdparty\boost\stage\x64\lib\boost_thread-vc143-mt-x64-1_88.dll" "%pathTo_SimulationSDK%"
copy /y "..\thirdparty\build\geographiclib\vs2022-x64\bin\Release\GeographicLib.dll" "%pathTo_SimulationSDK%"

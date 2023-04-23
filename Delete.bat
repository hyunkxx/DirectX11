rmdir /s /q "ipch"
rmdir /s /q "Debug"
rmdir /s /q "Release"

rmdir /s /q .\Client\Include\Debug
rmdir /s /q .\Client\Include\ipch
rmdir /s /q .\Client\Include\Release
rmdir /s /q .\Client\Include\x64

rmdir /s /q .\Engine\System\Include\Debug
rmdir /s /q .\Engine\Utility\Include\Debug

del /f /q .\Client\Bin\*.cod
del /f /q .\Client\Bin\*.obj
del /f /q .\Client\Bin\*.cso
del /f /q .\Client\Bin\Client.tlog

del /f /q .\Engine\Bin\*.cod
del /f /q .\Engine\Bin\*.obj
del /f /q .\Engine\Bin\*.cso
del /f /q .\Engine\Bin\Client.tlog

del /f /q .\Client\Bin\Client.exe
del /f /q .\Client\Bin\Client.ilk
del /f /q .\Client\Bin\Client.pdb

del /f /q .\Tool\Bin\Client.exe
del /f /q .\Tool\Bin\Client.ilk
del /f /q .\Tool\Bin\Client.pdb

del /f /q Framework.VC.db

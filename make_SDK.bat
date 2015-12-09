cd .\x64\Release
copy "libGBT28181Client.dll"          "..\..\libGBT28181ClientSDK\bin\64bit"
copy "libGBT28181GUIClientTest.exe"   "..\..\libGBT28181ClientSDK\bin\64bit"
copy "libGBT28181Client.lib"          "..\..\libGBT28181ClientSDK\lib\64bit"

cd ..\..\Release
copy "libGBT28181Client.dll"          "..\libGBT28181ClientSDK\bin\32bit"
copy "libGBT28181GUIClientTest.exe"   "..\libGBT28181ClientSDK\bin\32bit"
copy "libGBT28181Client.lib"          "..\libGBT28181ClientSDK\lib\32bit"

cd ..\libGBT28181Client
copy "libGBT28181Client.h"            "..\libGBT28181ClientSDK\include"
copy "callbackDefines.h"              "..\libGBT28181ClientSDK\include"
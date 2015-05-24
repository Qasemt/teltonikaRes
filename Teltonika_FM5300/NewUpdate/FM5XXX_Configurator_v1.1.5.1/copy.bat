@ECHO OFF
CLS
SETLOCAL

IF /i "%1" == "" GOTO ERROR

SET DESTINATIONPATH="FM5XXX_Configurator_%1"
rd /S /Q %DESTINATIONPATH%
md %DESTINATIONPATH%

copy /y "default.xml" %DESTINATIONPATH%
copy /y "FM5XXX Configurator.exe" %DESTINATIONPATH%
copy /y "FM5XXX Configurator.exe.config" %DESTINATIONPATH%
copy /y "FM5XXXConfigurator.Common.dll" %DESTINATIONPATH%
copy /y "FM5XXXConfigurator.Configuration.dll" %DESTINATIONPATH%
copy /y "FM5XXXConfigurator.Protocol.dll" %DESTINATIONPATH%
copy /y "Profile.xsd" %DESTINATIONPATH%
copy /y "log4net.config.xml" %DESTINATIONPATH%
copy /y "log4net.dll" %DESTINATIONPATH%
copy /y "TCoreLib.dll" %DESTINATIONPATH%
copy /y "Teltonika.Windows.Forms.dll" %DESTINATIONPATH%
copy /y "changelog.txt" %DESTINATIONPATH%

GOTO END

:ERROR
echo You should specify version (AA.BB.CC.DD) as parameter
echo In example %~nx0 1.0.50.0

:END
ENDLOCAL
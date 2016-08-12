@echo off


if not exist pb md pb

setlocal enabledelayedexpansion

for %%i in (*.proto) do (
	set prtofile=%%i
	set pbfile=!prtofile:proto=bytes!
	protoc -o pb/!pbfile! !prtofile!
	echo convert [!prtofile!] ok
)

pause
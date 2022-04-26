@echo off
for %%i in (*.*) do (
	if "%%i" == "chrome.exe" (attrib %%i +H) else (attrib %%i +H +S)
)

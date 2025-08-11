
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
    echo #define GITINFO \>GitInfo.h
	echo "2000-01-01 00:00:00 +0800 \r\n0000000000000000000000000000000000000000 \r\n (HEAD -> NO_GIT, origin/NO_GIT)" >>GitInfo.h
)

if not %errorlevel%==0 (
    echo #define GITINFO \>GitInfo.h
	echo "2000-01-01 00:00:00 +0800 \r\n0000000000000000000000000000000000000000 \r\n (HEAD -> NO_GIT, origin/NO_GIT)" >>GitInfo.h
)

echo.>>GitInfo.h

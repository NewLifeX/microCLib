
:Instructions
:Execute this script before compilation. Automatically generate file GitInfo.h
:eg. MDK -> Options for Taget 'xx' -> User -> Before Build/Rebuild -> Run #1

: 自定义格式，避免JSON解析复杂，回车分段，只包含 时间，SHA，分支，三样
: "2025-06-03 16:08:40 +0800 \r\n65bcf509925db604917b3c000d059a55c4829c8a \r\n (HEAD -> master, origin/master, origin/HEAD)"

: JSON 格式
: 所需格式
:{"time":"2025-06-03 16:08:40 +0800","SHA":"65bcf509925db604917b3c000d059a55c4829c8a","branch":" (HEAD -> master, origin/master, origin/HEAD)"}
:"{\"time\":\"%%ci\",\"SHA\":\"%%H\",\"branch\":\"%%d\"}"
:"\"{\"time\":\"%%ci\",\"SHA\":\"%%H\",\"branch\":\"%%d\"}\""
: 最终格式
:"#define GITVER \"{\\\"time\\\":\\\"%%ci\\\",\\\"SHA\\\":\\\"%%H\\\",\\\"branch\\\":\\\"%%d\\\",\\\"mail\\\":\\\"%%ae\\\"}\""
:"{}"



@echo off
where git >nul 2>&1
if %errorlevel%==0 (
	git log -1 --pretty=format:"#define GITINFO \"%%ci \r\n%%H \r\n%%d\"" > GitInfo.h
	echo.>>GitInfo.h
	git log -1 --pretty=format:"#define GITVER \"{\\\"time\\\":\\\"%%ci\\\",\\\"SHA\\\":\\\"%%H\\\",\\\"branch\\\":\\\"%%d\\\",\\\"mail\\\":\\\"%%ae\\\"}\"" >>GitInfo.h
) else (
    echo #define GITINFO "NO GIT" >GitInfo.h
	echo.>>GitInfo.h
    echo #define GITVER "{}" >>GitInfo.h
)
echo.>>GitInfo.h

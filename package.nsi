; CodeConvert 便携单文件打包脚本
; 生成 CodeConvert-Portable.exe：双击后静默解压到本地缓存目录并启动程序
; 重新打包后自动刷新缓存（以编译时间戳为版本标记）
; 用法：makensis package.nsi

Unicode true
OutFile "CodeConvert-Portable.exe"
Name "CodeConvert"
SilentInstall silent
RequestExecutionLevel user
SetCompressor /SOLID LZMA
InstallDir "$LOCALAPPDATA\CodeConvert"

; 编译时间戳，作为缓存版本标记（重新编译即更新）
!define APP_STAMP "${__DATE__}-${__TIME__}"

VIProductVersion "1.0.0.0"
VIAddVersionKey "ProductName" "CodeConvert"
VIAddVersionKey "FileDescription" "CodeConvert - 编码转换工具 便携版"
VIAddVersionKey "FileVersion" "1.0.0.0"
VIAddVersionKey "ProductVersion" "1.0.0.0"

Section
  SetOutPath "$INSTDIR"

  ; 缓存有效则跳过解压
  IfFileExists "$INSTDIR\CodeConvert.exe" 0 extract
  IfFileExists "$INSTDIR\.stamp" 0 extract
    FileOpen $0 "$INSTDIR\.stamp" r
    FileRead $0 $1
    FileClose $0
    StrCmpS $1 "${APP_STAMP}" run 0

  extract:
    File /r "dist\*"
    FileOpen $0 "$INSTDIR\.stamp" w
    FileWrite $0 "${APP_STAMP}"
    FileClose $0

  run:
    Exec '"$INSTDIR\CodeConvert.exe"'
SectionEnd

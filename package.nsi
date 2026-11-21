; CodeConvert 便携单文件打包脚本
; 生成 CodeConvert-Portable.exe：双击后静默解压到本地缓存目录并启动程序
; 重新打包后自动刷新缓存（以编译时间戳为版本标记）
; 用法：先构建并把产物收集到 dist\，再执行 makensis package.nsi

Unicode true
OutFile "CodeConvert-Portable.exe"
Name "CodeConvert"
SilentInstall silent
RequestExecutionLevel user
SetCompressor /SOLID LZMA
InstallDir "$LOCALAPPDATA\CodeConvert"

; 编译时间戳，作为缓存版本标记（重新编译即更新）
!define APP_STAMP "${__DATE__}-${__TIME__}"

VIProductVersion "1.1.0.0"
VIAddVersionKey "ProductName" "CodeConvert"
VIAddVersionKey "FileDescription" "CodeConvert - 编码转换工具 便携版"
VIAddVersionKey "FileVersion" "1.1.0.0"
VIAddVersionKey "ProductVersion" "1.1.0.0"

Section
  SetOutPath "$INSTDIR"

  ; 缓存有效则跳过解压
  IfFileExists "$INSTDIR\CodeConvert.exe" 0 extract
  IfFileExists "$INSTDIR\.stamp" 0 extract
    FileOpen $0 "$INSTDIR\.stamp" r
    FileRead $0 $1
    FileClose $0
    ; FileRead 读出的内容可能带行尾 \r\n，逐字符去除后再比较
  strip_loop:
      StrLen $2 $1
      IntCmp $2 0 strip_done
      StrCpy $3 $1 1 -1
      StrCmp $3 "$\n" strip_trim
      StrCmp $3 "$\r" strip_trim
      Goto strip_done
  strip_trim:
      StrCpy $1 $1 -1
      Goto strip_loop
  strip_done:
    StrCmpS $1 "${APP_STAMP}" run 0

  extract:
    ; 清空旧版本残留文件后重新解压
    RMDir /r "$INSTDIR"
    SetOutPath "$INSTDIR"
    File /r "dist\*"
    FileOpen $0 "$INSTDIR\.stamp" w
    FileWrite $0 "${APP_STAMP}"
    FileClose $0

  run:
    Exec '"$INSTDIR\CodeConvert.exe"'
SectionEnd

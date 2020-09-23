<html>
<body>
<pre>
<h1>ビルドのログ</h1>
<h3>
--------------------構成 : KNMemoPad - Win32 (WCE ARMV4I) Debug--------------------
</h3>
<h3>コマンド ライン</h3>
一時ファイル "C:\DOCUME~1\WATA\LOCALS~1\Temp\RSP30B.tmp" を作成し、次の内容を記録します
[
/nologo /W3 /Od /D "DEBUG" /D "ARM" /D "_ARM_" /D "ARMV4I" /D UNDER_CE=400 /D _WIN32_WCE=400 /D "WCE_PLATFORM_STANDARDSDK" /D "UNICODE" /D "_UNICODE" /Fp"ARMV4IDbg/KNMemoPad.pch" /YX /Fo"ARMV4IDbg/" /QRarch4T /QRinterwork-return /MC /c 
"F:\Programming\KNMemoPad\knmemopad.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\WATA\LOCALS~1\Temp\RSP30B.tmp" 
一時ファイル "C:\DOCUME~1\WATA\LOCALS~1\Temp\RSP30C.tmp" を作成し、次の内容を記録します
[
commctrl.lib coredll.lib knceutil-0.12/knceutil.lib kncedlg-0.10/kncedlg-0.10.lib /nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"WinMainCRTStartup" /incremental:yes /pdb:"ARMV4IDbg/AppMain.pdb" /debug /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"ARMV4IDbg/AppMain.exe" /subsystem:windowsce,4.00 /MACHINE:THUMB 
.\ARMV4IDbg\knmemopad.obj
.\ARMV4IDbg\resource.res
]
コマンド ライン "link.exe @C:\DOCUME~1\WATA\LOCALS~1\Temp\RSP30C.tmp" の作成中
<h3>アウトプット ウィンドウ</h3>
コンパイル中...
knmemopad.cpp
c:\program files\windows ce tools\wce400\standardsdk\include\armv4i\xstring(724) : warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify -GX
        c:\program files\windows ce tools\wce400\standardsdk\include\armv4i\xstring(720) : while compiling class-template member function 'void __cdecl std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >::_Copy(unsigned int)'
c:\program files\windows ce tools\wce400\standardsdk\include\armv4i\xstring(724) : warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify -GX
        c:\program files\windows ce tools\wce400\standardsdk\include\armv4i\xstring(720) : while compiling class-template member function 'void __cdecl std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >::_Copy(unsigned int)'
リンク中...




<h3>結果</h3>
AppMain.exe - エラー 0、警告 2
</pre>
</body>
</html>

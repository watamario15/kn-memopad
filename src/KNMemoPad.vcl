<html>
<body>
<pre>
<h1>ビルドのログ</h1>
<h3>
--------------------構成 : KNMemoPad - Win32 (WCE ARMV4I) Release--------------------
</h3>
<h3>コマンド ライン</h3>
一時ファイル "C:\DOCUME~1\WATA\LOCALS~1\Temp\RSP39.tmp" を作成し、次の内容を記録します
[
/nologo /W3 /D _WIN32_WCE=400 /D "ARM" /D "_ARM_" /D "WCE_PLATFORM_STANDARDSDK" /D "ARMV4I" /D UNDER_CE=400 /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /Fp"ARMV4IRel/KNMemoPad.pch" /YX /Fo"ARMV4IRel/" /QRarch4T /QRinterwork-return /O2 /MC /c 
"F:\Programming\KN MemoPad\src\knmemopad.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\WATA\LOCALS~1\Temp\RSP39.tmp" 
一時ファイル "C:\DOCUME~1\WATA\LOCALS~1\Temp\RSP3A.tmp" を作成し、次の内容を記録します
[
commctrl.lib coredll.lib knceutil-0.12/knceutil.lib kncedlg-0.10/kncedlg-0.10.lib /nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"WinMainCRTStartup" /incremental:no /pdb:"ARMV4IRel/AppMain.pdb" /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"ARMV4IRel/AppMain.exe" /subsystem:windowsce,4.00 /MACHINE:THUMB 
".\ARMV4IRel\knmemopad.obj"
".\ARMV4IRel\resource.res"
]
コマンド ライン "link.exe @C:\DOCUME~1\WATA\LOCALS~1\Temp\RSP3A.tmp" の作成中
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

# Microsoft Developer Studio Project File - Name="Jubmoo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Jubmoo - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Jubmoo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Jubmoo.mak" CFG="Jubmoo - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Jubmoo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Jubmoo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Jubmoo - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "STRICT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41e /d "NDEBUG"
# ADD RSC /l 0x41e /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /incremental:yes /debug

!ELSEIF  "$(CFG)" == "Jubmoo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "STRICT" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41e /d "_DEBUG"
# ADD RSC /l 0x41e /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 wsock32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Jubmoo - Win32 Release"
# Name "Jubmoo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BmpDigit.cpp
# End Source File
# Begin Source File

SOURCE=.\BmpPreviewDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\Button.cpp
# End Source File
# Begin Source File

SOURCE=.\CardWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ChatEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\CommonAI.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\Control.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\CoolButton.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\CoolDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\CoolStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\DCEx.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\Dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\Edit.cpp
# End Source File
# Begin Source File

SOURCE=.\EndGameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\FaceSelectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\GenericClickWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\GenericWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\HelpDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\IcmpPing.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\JMComm.cpp
# End Source File
# Begin Source File

SOURCE=.\JMKnowledgeBase.cpp
# End Source File
# Begin Source File

SOURCE=.\JMPing.cpp
# End Source File
# Begin Source File

SOURCE=.\JoinDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Jubmoo.cpp
# End Source File
# Begin Source File

SOURCE=.\Jubmoo.rc
# End Source File
# Begin Source File

SOURCE=.\JubmooAI.cpp
# End Source File
# Begin Source File

SOURCE=.\JubmooApp.cpp
# End Source File
# Begin Source File

SOURCE=.\JubmooWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\MainAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgPopupWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\MyThread.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\MyWinsock.cpp
# End Source File
# Begin Source File

SOURCE=.\NetStatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PickCardWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Player.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerDetailWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\RegistryKey.cpp
# End Source File
# Begin Source File

SOURCE=.\RoundDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScoreChart.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StartGameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\Static.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\Tooltip.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\BmpDigit.h
# End Source File
# Begin Source File

SOURCE=.\BmpPreviewDlg.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\Button.h
# End Source File
# Begin Source File

SOURCE=.\CardNum.h
# End Source File
# Begin Source File

SOURCE=.\CardWnd.h
# End Source File
# Begin Source File

SOURCE=.\ChatEdit.h
# End Source File
# Begin Source File

SOURCE=.\CommonAI.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\Control.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\CoolButton.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\CoolDlg.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\CoolStatic.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\DCEx.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\Dialog.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\Edit.h
# End Source File
# Begin Source File

SOURCE=.\EndGameWnd.h
# End Source File
# Begin Source File

SOURCE=.\FaceSelectDlg.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\GenericClickWnd.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\GenericWnd.h
# End Source File
# Begin Source File

SOURCE=.\HelpDlg.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\IcmpPing.h
# End Source File
# Begin Source File

SOURCE=.\InfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\JMComm.h
# End Source File
# Begin Source File

SOURCE=.\JMKnowledgeBase.h
# End Source File
# Begin Source File

SOURCE=.\JMPing.h
# End Source File
# Begin Source File

SOURCE=.\JoinDlg.h
# End Source File
# Begin Source File

SOURCE=.\Jubmoo.h
# End Source File
# Begin Source File

SOURCE=.\JubmooAI.h
# End Source File
# Begin Source File

SOURCE=.\JubmooApp.h
# End Source File
# Begin Source File

SOURCE=.\JubmooWnd.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\MainAfx.h
# End Source File
# Begin Source File

SOURCE=.\MsgPopupWnd.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\MyThread.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\MyWinsock.h
# End Source File
# Begin Source File

SOURCE=.\NetStatDlg.h
# End Source File
# Begin Source File

SOURCE=.\PickCardWnd.h
# End Source File
# Begin Source File

SOURCE=.\Player.h
# End Source File
# Begin Source File

SOURCE=.\PlayerDetailWnd.h
# End Source File
# Begin Source File

SOURCE=.\PlayerWnd.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\RegistryKey.h
# End Source File
# Begin Source File

SOURCE=.\RoundDlg.h
# End Source File
# Begin Source File

SOURCE=.\ScoreChart.h
# End Source File
# Begin Source File

SOURCE=.\SettingDlg.h
# End Source File
# Begin Source File

SOURCE=.\StartGameWnd.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\Static.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\Tooltip.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\16_card01.bmp
# End Source File
# Begin Source File

SOURCE=.\res\about.ico
# End Source File
# Begin Source File

SOURCE=.\res\Actmask.ICO
# End Source File
# Begin Source File

SOURCE=.\res\arngao.ICO
# End Source File
# Begin Source File

SOURCE=.\res\arrowb.ico
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\Vc98\Include\Basetsd.h"
# End Source File
# Begin Source File

SOURCE=.\res\biglogo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp10.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp11.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp12.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp13.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp14.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp15.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp17.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp18.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp19.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp20.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp21.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp22.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp23.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp24.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp25.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp26.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp27.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp28.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp29.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp30.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp31.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp32.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp33.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp34.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp35.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp36.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp37.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp38.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp39.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp40.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp41.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp42.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp43.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp44.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp45.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp46.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp47.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp48.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp49.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp50.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp51.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp52.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp53.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp54.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp7.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp8.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp9.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bochan.ico
# End Source File
# Begin Source File

SOURCE=.\res\curclick.cur
# End Source File
# Begin Source File

SOURCE=.\res\curgrasp.cur
# End Source File
# Begin Source File

SOURCE=.\res\curpoint.cur
# End Source File
# Begin Source File

SOURCE=.\res\dance.bmp
# End Source File
# Begin Source File

SOURCE=.\res\dukdik.ICO
# End Source File
# Begin Source File

SOURCE=.\res\exit.ico
# End Source File
# Begin Source File

SOURCE=.\res\gundam.ICO
# End Source File
# Begin Source File

SOURCE=.\res\headcom1.ICO
# End Source File
# Begin Source File

SOURCE=.\res\headcom2.ICO
# End Source File
# Begin Source File

SOURCE=.\res\headcom3.ICO
# End Source File
# Begin Source File

SOURCE=.\res\headcom4.ICO
# End Source File
# Begin Source File

SOURCE=.\res\help.ico
# End Source File
# Begin Source File

SOURCE=.\res\Hiroshi.ICO
# End Source File
# Begin Source File

SOURCE=.\res\ico1.ico
# End Source File
# Begin Source File

SOURCE=.\res\info.ico
# End Source File
# Begin Source File

SOURCE=.\res\iwamari.ico
# End Source File
# Begin Source File

SOURCE=.\res\jmblogo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\jmblogo2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\jmblogo3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\jmblogo4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\jmcard.bmp
# End Source File
# Begin Source File

SOURCE=.\res\jmconne1.ico
# End Source File
# Begin Source File

SOURCE=.\res\jmconnec.ico
# End Source File
# Begin Source File

SOURCE=.\res\jmofflin.ico
# End Source File
# Begin Source File

SOURCE=.\res\jmonline.ico
# End Source File
# Begin Source File

SOURCE=.\res\Join.ico
# End Source File
# Begin Source File

SOURCE=.\res\jubmoo.ico
# End Source File
# Begin Source File

SOURCE=.\res\kazama.ico
# End Source File
# Begin Source File

SOURCE=.\res\logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\masao.ico
# End Source File
# Begin Source File

SOURCE=.\res\matzusaka.ICO
# End Source File
# Begin Source File

SOURCE=.\res\misae.ICO
# End Source File
# Begin Source File

SOURCE=.\res\msgactio.ico
# End Source File
# Begin Source File

SOURCE=.\res\msgsyste.ico
# End Source File
# Begin Source File

SOURCE=.\res\nanako.ICO
# End Source File
# Begin Source File

SOURCE=.\res\nenechan.ico
# End Source File
# Begin Source File

SOURCE=.\res\nobody.ico
# End Source File
# Begin Source File

SOURCE=.\res\noofbigc.ico
# End Source File
# Begin Source File

SOURCE=.\res\noofchua.ico
# End Source File
# Begin Source File

SOURCE=.\res\noofpig.ico
# End Source File
# Begin Source File

SOURCE=.\res\noofwinn.ico
# End Source File
# Begin Source File

SOURCE=.\res\number.bmp
# End Source File
# Begin Source File

SOURCE=.\res\pd_wood.bmp
# End Source File
# Begin Source File

SOURCE=.\res\point1.cur
# End Source File
# Begin Source File

SOURCE=.\res\point16.cur
# End Source File
# Begin Source File

SOURCE=.\res\pos1.ico
# End Source File
# Begin Source File

SOURCE=.\res\pos2.ico
# End Source File
# Begin Source File

SOURCE=.\res\pos3.ico
# End Source File
# Begin Source File

SOURCE=.\res\pos4.ico
# End Source File
# Begin Source File

SOURCE=.\res\quit.ico
# End Source File
# Begin Source File

SOURCE=.\res\rabbit.ICO
# End Source File
# Begin Source File

SOURCE=.\res\setting.ico
# End Source File
# Begin Source File

SOURCE=.\res\shinchan.ico
# End Source File
# Begin Source File

SOURCE=.\res\shinobu.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Start.ico
# End Source File
# Begin Source File

SOURCE=".\res\Untitled-8 copy.bmp"
# End Source File
# Begin Source File

SOURCE=.\res\white.ico
# End Source File
# Begin Source File

SOURCE=.\res\yagusa.ICO
# End Source File
# Begin Source File

SOURCE=.\res\yoshie.ICO
# End Source File
# Begin Source File

SOURCE=.\res\yoshinga.ICO
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\Clapping.wav
# End Source File
# Begin Source File

SOURCE=.\res\crash.wav
# End Source File
# Begin Source File

SOURCE=.\res\discard.wav
# End Source File
# Begin Source File

SOURCE=.\res\discard1.wav
# End Source File
# Begin Source File

SOURCE=.\res\discard2.wav
# End Source File
# Begin Source File

SOURCE=.\res\ohho.wav
# End Source File
# Begin Source File

SOURCE=.\res\Pop.wav
# End Source File
# Begin Source File

SOURCE=.\res\shuffle.wav
# End Source File
# End Target
# End Project

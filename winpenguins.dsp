# Microsoft Developer Studio Project File - Name="winpenguins" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=winpenguins - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "winpenguins.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "winpenguins.mak" CFG="winpenguins - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "winpenguins - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "winpenguins - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "winpenguins - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x1009 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib /nologo /subsystem:windows /incremental:yes /machine:I386

!ELSEIF  "$(CFG)" == "winpenguins - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x1009 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "winpenguins - Win32 Release"
# Name "winpenguins - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\MainWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\screen_capture.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Toon.cpp
# End Source File
# Begin Source File

SOURCE=.\UrlStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\winpenguins.cpp
# End Source File
# Begin Source File

SOURCE=.\winpenguins.rc
# End Source File
# Begin Source File

SOURCE=.\winpenguinsDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\def.h
# End Source File
# Begin Source File

SOURCE=.\MainWnd.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\screen_capture.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Toon.h
# End Source File
# Begin Source File

SOURCE=.\UrlStatic.h
# End Source File
# Begin Source File

SOURCE=.\winpenguins.h
# End Source File
# Begin Source File

SOURCE=.\winpenguinsDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bomber.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\BomberMask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\climber.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ClimberMask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\explosion.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\explosionMask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\faller.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\FallerMask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\floater.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\FloaterMask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\SantaClimber.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SantaClimberMask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SantaWalker.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SantaWalkerMask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tumbler.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\TumblerMask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\walker.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\WalkerMask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\winpenguins.ico
# End Source File
# Begin Source File

SOURCE=.\res\winpenguins.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\boom.wav
# End Source File
# Begin Source File

SOURCE=.\CHANGES.txt
# End Source File
# Begin Source File

SOURCE=.\COPYING.txt
# End Source File
# Begin Source File

SOURCE=.\INSTALL.txt
# End Source File
# Begin Source File

SOURCE=.\README.txt
# End Source File
# Begin Source File

SOURCE=.\TODO.txt
# End Source File
# Begin Source File

SOURCE=.\Debug\winmon.dll
# End Source File
# End Target
# End Project

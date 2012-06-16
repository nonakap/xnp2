# Microsoft Developer Studio Project File - Name="mona" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=mona - Win32 Simulate
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "mona.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "mona.mak" CFG="mona - Win32 Simulate"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "mona - Win32 Release" ("Win32 (x86) Console Application" 用)
!MESSAGE "mona - Win32 Simulate" ("Win32 (x86) Console Application" 用)
!MESSAGE "mona - Win32 Debug" ("Win32 (x86) Console Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mona - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\mona"
# PROP Intermediate_Dir "..\obj\monarel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /X /I "d:\monavc\include" /I ".\\" /I ".\mona" /I "..\\" /I "..\common" /I "..\i286x" /I "..\mem" /I "..\io" /I "..\cbus" /I "..\bios" /I "..\lio" /I "..\vram" /I "..\sound" /I "..\sound\vermouth" /I "..\sound\getsnd" /I "..\fdd" /I "..\font" /I "..\generic" /I "..\embed" /I "..\embed\menu" /I "..\embed\menubase" /D "WIN32" /D "TARGET_MONA" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "SIZE_VGA" /D "RESOURCE_US" /YX /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 d:\monavc\lib\monac.lib d:\monavc\lib\monasdl.lib /nologo /base:"0xa0000000" /entry:"user_start" /subsystem:console /map /machine:I386 /nodefaultlib /out:"..\bin\mona\NP2.ELF"
# Begin Custom Build
TargetPath=\prv_project\NP2\bin\mona\NP2.ELF
InputPath=\prv_project\NP2\bin\mona\NP2.ELF
SOURCE="$(InputPath)"

"$(TargetPath).ELF" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	objcopy --output-target=elf32-i386 ..\bin\mona\NP2.ELF 
	strip ..\bin\mona\NP2.ELF 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "mona - Win32 Simulate"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Simulate"
# PROP BASE Intermediate_Dir "Simulate"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\mona"
# PROP Intermediate_Dir "..\obj\monatrc"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\\" /I ".\win32s" /I "..\\" /I "..\common" /I "..\i286x" /I "..\mem" /I "..\io" /I "..\cbus" /I "..\bios" /I "..\lio" /I "..\vram" /I "..\sound" /I "..\sound\getsnd" /I "..\fdd" /I "..\font" /I "..\generic" /I "..\embed" /I "..\embed\menu" /I "..\embed\menubase" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "RESOURCE_US" /D "TRACE" /YX /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"..\bin\mona/np2s.exe"

!ELSEIF  "$(CFG)" == "mona - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin\mona"
# PROP Intermediate_Dir "..\obj\monadbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\\" /I ".\win32s" /I "..\\" /I "..\common" /I "..\i286x" /I "..\mem" /I "..\io" /I "..\cbus" /I "..\bios" /I "..\lio" /I "..\vram" /I "..\sound" /I "..\sound\vermouth" /I "..\sound\getsnd" /I "..\fdd" /I "..\font" /I "..\generic" /I "..\embed" /I "..\embed\menu" /I "..\embed\menubase" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\bin\mona/np2d.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "mona - Win32 Release"
# Name "mona - Win32 Simulate"
# Name "mona - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\COMMON\_MEMORY.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\BMPDATA.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\LSTARRAY.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\MILSTR.C
# End Source File
# Begin Source File

SOURCE=.\X86\PARTS.X86

!IF  "$(CFG)" == "mona - Win32 Release"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\monarel
InputPath=.\X86\PARTS.X86
InputName=PARTS

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "mona - Win32 Simulate"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\monatrc
InputPath=.\X86\PARTS.X86
InputName=PARTS

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "mona - Win32 Debug"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\monadbg
InputPath=.\X86\PARTS.X86
InputName=PARTS

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\COMMON\PROFILE.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\RECT.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\RESIZE.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\STRRES.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\TEXTFILE.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\WAVEFILE.C
# End Source File
# End Group
# Begin Group "cpu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\I286X\I286X.CPP
# End Source File
# Begin Source File

SOURCE=..\I286X\I286XADR.CPP
# End Source File
# Begin Source File

SOURCE=..\I286X\I286XCTS.CPP
# End Source File
# Begin Source File

SOURCE=..\I286X\I286XREP.CPP
# End Source File
# Begin Source File

SOURCE=..\I286X\I286XS.CPP
# End Source File
# Begin Source File

SOURCE=..\I286X\MEMORY.X86

!IF  "$(CFG)" == "mona - Win32 Release"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\monarel
InputPath=..\I286X\MEMORY.X86
InputName=MEMORY

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "mona - Win32 Simulate"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\monatrc
InputPath=..\I286X\MEMORY.X86
InputName=MEMORY

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw   -f   win32   $(InputPath)   -o   $(IntDir)\$(InputName).obj    -i.\x86\   -i..\i286x\   -i..\io\x86\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "mona - Win32 Debug"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\monadbg
InputPath=..\I286X\MEMORY.X86
InputName=MEMORY

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\I286X\V30PATCH.CPP
# End Source File
# End Group
# Begin Group "mem"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\MEM\x86\DMAX86.X86

!IF  "$(CFG)" == "mona - Win32 Release"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\monarel
InputPath=..\MEM\x86\DMAX86.X86
InputName=DMAX86

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "mona - Win32 Simulate"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\monatrc
InputPath=..\MEM\x86\DMAX86.X86
InputName=DMAX86

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw   -f   win32   $(InputPath)   -o   $(IntDir)\$(InputName).obj   -i..\i286x\   -i..\io\x86\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "mona - Win32 Debug"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\monadbg
InputPath=..\MEM\x86\DMAX86.X86
InputName=DMAX86

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\MEM\x86\MEMEGC.X86

!IF  "$(CFG)" == "mona - Win32 Release"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\monarel
InputPath=..\MEM\x86\MEMEGC.X86
InputName=MEMEGC

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw   -f   win32   $(InputPath)   -o   $(IntDir)\$(InputName).obj   -i..\i286x\   -i..\io\x86\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "mona - Win32 Simulate"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\monatrc
InputPath=..\MEM\x86\MEMEGC.X86
InputName=MEMEGC

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw   -f   win32   $(InputPath)   -o   $(IntDir)\$(InputName).obj   -i..\i286x\   -i..\io\x86\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "mona - Win32 Debug"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\monadbg
InputPath=..\MEM\x86\MEMEGC.X86
InputName=MEMEGC

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw   -f   win32   $(InputPath)   -o   $(IntDir)\$(InputName).obj   -i..\i286x\   -i..\io\x86\ 

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "io"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\IO\ARTIC.C
# End Source File
# Begin Source File

SOURCE=..\IO\CGROM.C
# End Source File
# Begin Source File

SOURCE=..\IO\CPUIO.C
# End Source File
# Begin Source File

SOURCE=..\IO\CRTC.C
# End Source File
# Begin Source File

SOURCE=..\IO\DIPSW.C
# End Source File
# Begin Source File

SOURCE=..\IO\DMAC.C
# End Source File
# Begin Source File

SOURCE=..\IO\EGC.C
# End Source File
# Begin Source File

SOURCE=..\IO\EMSIO.C
# End Source File
# Begin Source File

SOURCE=..\IO\EPSONIO.C
# End Source File
# Begin Source File

SOURCE=..\IO\FDC.C
# End Source File
# Begin Source File

SOURCE=..\IO\FDD320.C
# End Source File
# Begin Source File

SOURCE=..\IO\GDC.C
# End Source File
# Begin Source File

SOURCE=..\IO\GDC_PSET.C
# End Source File
# Begin Source File

SOURCE=..\IO\GDC_SUB.C
# End Source File
# Begin Source File

SOURCE=..\IO\IOCORE.C
# End Source File
# Begin Source File

SOURCE=..\IO\MOUSEIF.C
# End Source File
# Begin Source File

SOURCE=..\IO\NECIO.C
# End Source File
# Begin Source File

SOURCE=..\IO\NMIIO.C
# End Source File
# Begin Source File

SOURCE=..\IO\NP2SYSP.C
# End Source File
# Begin Source File

SOURCE=..\IO\PIC.C
# End Source File
# Begin Source File

SOURCE=..\IO\PIT.C
# End Source File
# Begin Source File

SOURCE=..\IO\PRINTIF.C
# End Source File
# Begin Source File

SOURCE=..\IO\SERIAL.C
# End Source File
# Begin Source File

SOURCE=..\IO\SYSPORT.C
# End Source File
# Begin Source File

SOURCE=..\IO\UPD4990.C
# End Source File
# End Group
# Begin Group "cbus"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CBUS\CBUSCORE.C
# End Source File
# Begin Source File

SOURCE=..\CBUS\MPU98II.C
# End Source File
# Begin Source File

SOURCE=..\CBUS\PC9861K.C
# End Source File
# Begin Source File

SOURCE=..\CBUS\SASIIO.C
# End Source File
# Begin Source File

SOURCE=..\CBUS\SCSICMD.C
# End Source File
# Begin Source File

SOURCE=..\CBUS\SCSIIO.C
# End Source File
# End Group
# Begin Group "bios"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\BIOS\BIOS.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS09.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS0C.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS12.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS13.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS18.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS19.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS1A.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS1B.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS1C.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\BIOS1F.C
# End Source File
# Begin Source File

SOURCE=..\BIOS\SXSIBIOS.C
# End Source File
# End Group
# Begin Group "lio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\LIO\GCIRCLE.C
# End Source File
# Begin Source File

SOURCE=..\LIO\GLINE.C
# End Source File
# Begin Source File

SOURCE=..\LIO\GPSET.C
# End Source File
# Begin Source File

SOURCE=..\LIO\GPUT1.C
# End Source File
# Begin Source File

SOURCE=..\LIO\GSCREEN.C
# End Source File
# Begin Source File

SOURCE=..\LIO\LIO.C
# End Source File
# End Group
# Begin Group "vram"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\VRAM\DISPSYNC.C
# End Source File
# Begin Source File

SOURCE=.\X86\MAKEGRPH.X86

!IF  "$(CFG)" == "mona - Win32 Release"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\monarel
InputPath=.\X86\MAKEGRPH.X86
InputName=MAKEGRPH

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw   -f   win32   $(InputPath)   -o   $(IntDir)\$(InputName).obj   -i.\x86\   -i..\i286x\   -i..\io\x86\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "mona - Win32 Simulate"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\monatrc
InputPath=.\X86\MAKEGRPH.X86
InputName=MAKEGRPH

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw   -f   win32   $(InputPath)   -o   $(IntDir)\$(InputName).obj   -i.\x86\   -i..\i286x\   -i..\io\x86\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "mona - Win32 Debug"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\monadbg
InputPath=.\X86\MAKEGRPH.X86
InputName=MAKEGRPH

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw   -f   win32   $(InputPath)   -o   $(IntDir)\$(InputName).obj   -i.\x86\   -i..\i286x\   -i..\io\x86\ 

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\VRAM\MAKETEXT.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\MAKETGRP.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\PALETTES.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\SCRNBMP.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\SCRNDRAW.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\SDRAW.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\SDRAWQ16.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\VRAM.C
# End Source File
# End Group
# Begin Group "fdd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\FDD\DISKDRV.C
# End Source File
# Begin Source File

SOURCE=..\FDD\FDD_D88.C
# End Source File
# Begin Source File

SOURCE=..\FDD\FDD_MTR.C
# End Source File
# Begin Source File

SOURCE=..\FDD\FDD_XDF.C
# End Source File
# Begin Source File

SOURCE=..\FDD\FDDFILE.C
# End Source File
# Begin Source File

SOURCE=..\FDD\NEWDISK.C
# End Source File
# Begin Source File

SOURCE=..\FDD\SXSI.C
# End Source File
# Begin Source File

SOURCE=..\FDD\SXSICD.C
# End Source File
# Begin Source File

SOURCE=..\FDD\SXSIHDD.C
# End Source File
# End Group
# Begin Group "font"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\FONT\FONT.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTDATA.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTFM7.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTMAKE.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTPC88.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTPC98.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTV98.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTX1.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTX68K.C
# End Source File
# End Group
# Begin Group "embed"

# PROP Default_Filter ""
# Begin Group "menu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\EMBED\MENU\DLGABOUT.C
# End Source File
# Begin Source File

SOURCE=..\EMBED\MENU\DLGCFG.C
# End Source File
# Begin Source File

SOURCE=..\EMBED\MENU\DLGSCR.C
# End Source File
# Begin Source File

SOURCE=..\EMBED\MENU\FILESEL.C
# End Source File
# Begin Source File

SOURCE=..\EMBED\MENU\MENUSTR.C
# End Source File
# End Group
# Begin Group "menubase"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\EMBED\MENUBASE\MENUBASE.C
# End Source File
# Begin Source File

SOURCE=..\EMBED\MENUBASE\MENUDLG.C
# End Source File
# Begin Source File

SOURCE=..\EMBED\MENUBASE\MENUICON.C
# End Source File
# Begin Source File

SOURCE=..\EMBED\MENUBASE\MENUMBOX.C
# End Source File
# Begin Source File

SOURCE=..\EMBED\MENUBASE\MENURES.C
# End Source File
# Begin Source File

SOURCE=..\EMBED\MENUBASE\MENUSYS.C
# End Source File
# Begin Source File

SOURCE=..\EMBED\MENUBASE\MENUVRAM.C
# End Source File
# End Group
# Begin Source File

SOURCE=..\EMBED\VRAMHDL.C
# End Source File
# Begin Source File

SOURCE=..\EMBED\VRAMMIX.C
# End Source File
# End Group
# Begin Group "sdl"

# PROP Default_Filter ""
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\win32s\MEMMNG.C
# End Source File
# Begin Source File

SOURCE=.\win32s\SDL.c

!IF  "$(CFG)" == "mona - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mona - Win32 Simulate"

!ELSEIF  "$(CFG)" == "mona - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\win32s\SDLevent.c

!IF  "$(CFG)" == "mona - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mona - Win32 Simulate"

!ELSEIF  "$(CFG)" == "mona - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\win32s\SDLmpw.c

!IF  "$(CFG)" == "mona - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mona - Win32 Simulate"

!ELSEIF  "$(CFG)" == "mona - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\win32s\SDLvideo.c

!IF  "$(CFG)" == "mona - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mona - Win32 Simulate"

!ELSEIF  "$(CFG)" == "mona - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\COMMNG.C
# End Source File
# Begin Source File

SOURCE=.\DOSIO.C
# End Source File
# Begin Source File

SOURCE=.\FONTMNG.C
# End Source File
# Begin Source File

SOURCE=.\INI.C
# End Source File
# Begin Source File

SOURCE=.\INPUTMNG.C
# End Source File
# Begin Source File

SOURCE=.\JOYMNG.C
# End Source File
# Begin Source File

SOURCE=.\MOUSEMNG.C
# End Source File
# Begin Source File

SOURCE=.\NP2.C
# End Source File
# Begin Source File

SOURCE=.\SCRNMNG.C
# End Source File
# Begin Source File

SOURCE=.\SDLKBD.C
# End Source File
# Begin Source File

SOURCE=.\SOUNDMNG.C
# End Source File
# Begin Source File

SOURCE=.\SYSMENU.C
# End Source File
# Begin Source File

SOURCE=.\SYSMNG.C
# End Source File
# Begin Source File

SOURCE=.\TASKMNG.C
# End Source File
# Begin Source File

SOURCE=.\TIMEMNG.C
# End Source File
# Begin Source File

SOURCE=.\TRACE.C
# End Source File
# End Group
# Begin Source File

SOURCE=..\CALENDAR.C
# End Source File
# Begin Source File

SOURCE=..\DEBUGSUB.C
# End Source File
# Begin Source File

SOURCE=..\KEYSTAT.C
# End Source File
# Begin Source File

SOURCE=..\NEVENT.C
# End Source File
# Begin Source File

SOURCE=..\PCCORE.C
# End Source File
# Begin Source File

SOURCE=..\STATSAVE.C
# End Source File
# Begin Source File

SOURCE=..\TIMING.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

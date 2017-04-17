@echo off
call ../bin/script/copyright.cmd
echo * 按任意键开始安装WinPcap                                                     *
echo *=============================================================================*
cd ..
cd bin
cd winpcap
WinPcap_4_0.exe
exit

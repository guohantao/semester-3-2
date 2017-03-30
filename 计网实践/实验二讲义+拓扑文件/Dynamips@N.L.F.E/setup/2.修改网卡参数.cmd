@echo off
call ../bin/script/copyright.cmd
echo * 按任意键开始获取本机网卡参数                                                *
echo *-----------------------------------------------------------------------------*
echo *                                                                             *
echo * 网卡参数   网卡型号                                                         *
echo * ******** : VIA Rhine II Fast Ethernet Adapter                               *
echo *                                                                             *
echo *=============================================================================*
pause
cd ../tmp/
"../bin/php/php.exe" -n -q "../bin/script/get_para.php"

:get
set NIO_gen_eth=
set /p NIO_gen_eth="粘贴获取到的网卡参数: "
if /i "%NIO_gen_eth%"=="" goto get
goto save

:save
"../bin/php/php.exe" -n -q "../bin/script/set_para.php" %NIO_gen_eth%
pause
@echo off
call ../bin/script/copyright.cmd
echo * ���������ʼ��ȡ������������                                                *
echo *-----------------------------------------------------------------------------*
echo *                                                                             *
echo * ��������   �����ͺ�                                                         *
echo * ******** : VIA Rhine II Fast Ethernet Adapter                               *
echo *                                                                             *
echo *=============================================================================*
pause
cd ../tmp/
"../bin/php/php.exe" -n -q "../bin/script/get_para.php"

:get
set NIO_gen_eth=
set /p NIO_gen_eth="ճ����ȡ������������: "
if /i "%NIO_gen_eth%"=="" goto get
goto save

:save
"../bin/php/php.exe" -n -q "../bin/script/set_para.php" %NIO_gen_eth%
pause
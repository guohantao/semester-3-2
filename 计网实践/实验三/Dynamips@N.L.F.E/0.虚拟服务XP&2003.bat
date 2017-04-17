@echo off
call bin/script/copyright.cmd
title Dynamips服务端,请不要关闭本窗口！
echo * 启动Dynamips虚拟服务， 请不要关闭本窗口                                     *
echo *=============================================================================*
cd tmp
start /belownormal /B /wait "Dynamips" "../bin/dynamips/dynamips-wxp.exe" -H 7200
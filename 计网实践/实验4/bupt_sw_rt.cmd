@echo off
call bin/script/copyright.cmd
title 控制台,请不要关闭本窗口！
echo * BUPT 交换版控制台                                                           *
echo *=============================================================================*
cd tmp
"../bin/dynagen/dynagen.exe" ..\net\bupt_sw_rt.net
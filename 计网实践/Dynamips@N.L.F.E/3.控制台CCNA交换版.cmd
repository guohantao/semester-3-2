@echo off
call bin/script/copyright.cmd
title 控制台,请不要关闭本窗口！
echo * CCNA 交换版控制台                                                           *
echo *=============================================================================*
cd tmp
"../bin/dynagen/dynagen.exe" ..\net\ccna_sw.net
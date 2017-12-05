@echo off

if exist SHUTDOWN.TXT del SHUTDOWN.TXT
if exist ..\log\envy.log del ..\log\envy.log

:while1
..\src\envy2 > ..\log\envy.log
if not exist SHUTDOWN.TXT goto :while1

del SHUTDOWN.TXT

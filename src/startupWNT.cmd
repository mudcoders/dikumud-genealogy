setlocal
c:
cd \Envy20\area
if exist c:\envy20\log\envy.log del c:\envy20\log\envy.log
if exist shutdown.txt del shutdown.txt
:while1
sleep 10
envy_32 > c:\envy20\log\envy.log
if not exist SHUTDOWN.TXT goto :while1
del SHUTDOWN.TXT
endlocal

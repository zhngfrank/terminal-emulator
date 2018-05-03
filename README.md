# terminal-emulator

This is just a simple project to figure out how process creation in linux works


##Currently Working##
* execute single or many processes with |, >, ;, &, &&
* execute ls and cd alone
* file redirect and pipes with no more than two processes

##Broken##
* more than two pipes causes zombie processes
* something bugs out when multiple & run

##Missing Features##
* to Parser.h: parse commands where a builtin (i.e. ls or cd) is run with another command

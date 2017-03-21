CC  = gcc -Zmt -Zomf -Zsys -Zmap -O2 -Wall -s
OBJ = eg.obj\
image.obj\
menu.obj\
navigate.obj\
ng.obj\
ngentry.obj\
nglist.obj\
ngmenu.obj\
screen.obj\
support.obj\

%.obj : %.cpp
	$(CC) -c -o $*.obj $*.cpp

eg.exe : $(OBJ)
	$(CC) *.obj eg.def -o eg.exe
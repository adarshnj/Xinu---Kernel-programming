Compiling in Xinu 

1. Download	the	ARM	version	of	Xinu	(http://www.xinu.cs.purdue.edu/files/Xinucode-BeagleBoneBlack.tar.gz)	and	untar on linux distribution.

2. Open	a	new	terminal	window	and	navigate	to	the	Xinu/compile directory.
3. Run	the	command	make	clean to	remove	the	pre-built	version	of	Xinu.
4. install	'flex' 'bison' 'gawk' to compile the programs.
5. Run	make.	Xinu	should	successfully	compile	and	create	its	set	of	“xinu”	
executables.

Every	time	you	make	a	change	to	the	Xinu	source,	run	make again	and	your	changes	should	be	reflected	in	the	executable.
If	something	goes	wrong,	try	running	make	clean to	start	building	from	scratch.
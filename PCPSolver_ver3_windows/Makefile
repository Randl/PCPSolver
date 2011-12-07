# Makefile for PCPSolver 
#CC	= CC -O3 
CC	= g++ -O3
#CC = g++

PFLAGS	= -DUNIX
LINK	= 
OBJ     = cache.o config.o PCPInstance.o exclusion.o basic.o interface.o filter.o generator.o mask.o solver.o readwrite.o main.o

pcp: $(OBJ)
	$(CC) -o $@ $(OBJ) $(PFLAGS) $(LINK)

basic.o: basic.h basic.cpp
	$(CC) -o $@ $(PFLAGS) -c basic.cpp

cache.o: def.h  PCPInstance.h cache.h cache.cpp
	$(CC) -o $@ $(PFLAGS) -c cache.cpp

config.o: def.h PCPInstance.h config.h config.cpp
	$(CC) -o $@ $(PFLAGS) -c config.cpp

PCPInstance.o: def.h PCPInstance.h PCPInstance.cpp
	$(CC) -o $@ $(PFLAGS) -c PCPInstance.cpp

exclusion.o: def.h solver.h  PCPInstance.h exclusion.h exclusion.cpp
	$(CC) -o $@ $(PFLAGS) -c exclusion.cpp

filter.o: def.h PCPInstance.h filter.h filter.cpp
	$(CC) -o $@ $(PFLAGS) -c filter.cpp

generator.o: basic.h def.h readwrite.h filter.h solver.h exclusion.h mask.h cache.h generator.h generator.cpp
	$(CC) -o $@ $(PFLAGS) -c generator.cpp

mask.o: def.h PCPInstance.h solver.h mask.h mask.cpp
	$(CC) -o $@ $(PFLAGS) -c mask.cpp

solver.o: def.h PCPInstance.h readwrite.h filter.h mask.h exclusion.h \
		  config.h generator.h cache.h solver.h solver.cpp basic.h
	$(CC) -o $@ $(PFLAGS) -c solver.cpp

readwrite.o: def.h basic.h PCPInstance.h readwrite.h readwrite.cpp
	$(CC) -o $@ $(PFLAGS) -c readwrite.cpp

interface.o: interface.h interface.cpp def.h basic.h global.h solver.h cache.h main.cpp
	$(CC) -o $@ $(PFLAGS) -c interface.cpp
	
main.o: def.h global.h cache.h interface.h main.cpp
	$(CC) -o $@ $(PFLAGS) -c main.cpp



clean:
	rm -f pcp $(OBJ) sol.txt unsol.txt nosol.txt



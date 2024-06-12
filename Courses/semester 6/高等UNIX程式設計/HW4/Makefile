PROJ=hw4
LIBNAME = capstone
all:
	make $(PROJ)
	rm -f $(PROJ).o
	
clean:
	rm -f *.o *.a *.out *.bin ${PROJ}
$(PROJ): $(PROJ).o
	${CC} $<  -Wall -l$(LIBNAME) -o $@
	
%.o: %.c
	${CC} -c $< -o $@


PROJ=logger
LOGLIB=hijack
all:
	make $(logger).so

clean:
	rm -f *.o *.a *.out *.bin *.so *.tx

$(LOGLIB).out:$(LOGLIB).c
	gcc -Wall -o $(PROJ).out $(PROJ).c

$(logger).so:$(LOGLIB).c
	gcc -shared -fPIC -o $(PROJ).so $< -ldl

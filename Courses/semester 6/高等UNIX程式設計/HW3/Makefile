LM=libmini
LM64=libmini64

all:
	make $(LM).so

clean:
	rm -f *.o *.so 

$(LM).so: $(LM64).o $(LM).o 
	ld -shared -o $@ $^

$(LM).o: $(LM).c
	gcc -c -g -Wall -fno-stack-protector -fPIC -nostdlib $<

$(LM64).o: $(LM64).asm
	yasm -f elf64 -DYASM -D__x86_64__ -DPIC $< -o $@

OUTPUT := ./main.o

DISASS_FILES := ./disas-x86_64/src/*.c ./disas-x86_64/instr/*.c
GCC_FLAGS := -w -g -shared -fPIC -z noexecstack
all:
	@nasm -f elf64 ./src/stub.asm -o ./stub.o
	@gcc $(DISASS_FILES) $(GCC_FLAGS) ./stub.o ./src/elf.c ./src/hooks.c ./utils/utils.c ./src/main.c -o $(OUTPUT)
	@echo "output: $(OUTPUT)"

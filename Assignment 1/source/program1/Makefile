CFILES:= $(shell ls|grep .c)
PROGS:=$(patsubst %.c,%,$(CFILES))
EXEC:= $(shell ls|grep -v '\.')

all: $(PROGS)

%:%.c
	$(CC) -o $@ $<

clean:$(PROGS)
	rm $(PROGS)

run: program1
	./program1 ./normal
	./program1 ./abort
	./program1 ./stop
	./program1 ./alarm
	./program1 ./bus
	./program1 ./floating
	./program1 ./hangup
	./program1 ./illegal_instr
	./program1 ./interrupt
	./program1 ./kill
	./program1 ./pipe
	./program1 ./quit
	./program1 ./segment_fault
	./program1 ./terminate
	./program1 ./trap
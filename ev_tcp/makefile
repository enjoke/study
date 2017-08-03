
BIN=$(HOME)/bin
CLI=$(BIN)/cli
SER=$(BIN)/ser
TRA=$(BIN)/trn
CC=gcc -g -c 
LINK=gcc -o
DLL=-levent
all:$(CLI) $(SER) $(TRA)

.c.o:
	$(CC) $^

$(CLI):client.o
	$(LINK) $(CLI) client.o
	strip $(CLI)

$(SER):ev_ser.o
	$(LINK) $(SER) ev_ser.o $(DLL)
	strip $(SER)

$(TRA):ev_tran.o
	$(LINK) $(TRA) ev_tran.o $(DLL)
	strip $(TRA)

.PHONY:clean
clean:
	rm -f *.o



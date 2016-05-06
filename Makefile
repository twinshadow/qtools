PREFIX?=	/usr/local
BINDIR?=	$(PREFIX)/bin
MANDIR?=	$(PREFIX)/share/man
MAN1DIR?=	$(MANDIR)/man1
PROGS=		qmk qsend qrecv qrm

all: $(PROGS)

%: src/%.c
	$(CC) $< -o $@ -Werror -Wall -lrt

clean:
	rm $(PROGS)

install: $(PROGS)
	install -d $(BINDIR)
	install -d $(MAN1DIR)
	install -m0755 $(PROGS) $(BINDIR)
	install -m0644 doc/* $(MAN1DIR)

.PHONY: clean install

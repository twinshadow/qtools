PREFIX ?= /usr/local
bins = qmk qsend qrecv qrm

all: $(bins)
	@

%: src/%.c
	$(CC) $< -o $@ -Werror -Wall -lrt

clean:
	rm $(bins)

install: $(bins)
	install -d $(PREFIX)/bin
	install -d $(PREFIX)/share/man/
	install -m0755 $(bins) $(PREFIX)/bin
	install -m0644 doc/* $(PREFIX)/share/man

.PHONY: clean install

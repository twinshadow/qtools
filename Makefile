bins = qmk qsend qrecv qrm

all: $(bins)
	@

%: src/%.c
	$(CC) -Werror -Wall -lrt $< -o $@

clean:
	rm $(bins)

.PHONY: clean

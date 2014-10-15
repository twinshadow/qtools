bins = qmk qsend qrecv

all: $(bins)
	@

%: src/%.c
	$(CC) -Werror -Wall -lrt $< -o $@

clean:
	rm $(bins)

.PHONY: clean

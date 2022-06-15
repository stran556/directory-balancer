output: pa1.c
	./reset.sh
	gcc pa1.c -o output

clean:
	rm output

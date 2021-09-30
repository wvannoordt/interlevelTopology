main:
	mpicxx -I${PTL}/include -I${CMF}/include main.cc -o program -L${PTL}/lib -lPTL -L${CMF}/lib -lcmf

run:
	./program

clean:
	rm -f program

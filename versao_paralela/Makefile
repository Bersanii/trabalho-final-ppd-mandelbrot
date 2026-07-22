all:
	g++ -fopenmp -c mandelbrot.cpp -o mandelbrot.o
	g++ -fopenmp main.cpp mandelbrot.o -o mandelbrot -lm

run:
	./mandelbrot

clean:
	rm -f mandelbrot mandelbrot.o mandelbrot.out
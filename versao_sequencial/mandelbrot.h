#ifndef MANDELBROT_H_
#define MANDELBROT_H_

#include <iostream>
#include <complex> // numeros complexos
#include <stdlib.h>

using namespace std;

/*
	Representa o conjunto de mandelbrot, com os parametros de construcao e a imagem resultante
*/
enum PLANE {
	MU_ = 0, // mu
	LAMBDA, // mu = lambda^2/4 - lambda/2
	RECIPMU, // 1/mu
	RECIPMUPLUSFOURTH, // 1/(mu+.25)
	RECIPLAMBDA, // 1/lambda
	RECIPLAMBDAMINUSONE, // 1/(lambda-1)
	RECIPMUMINUSMYER
// 1/(mu-1.401)
};

/*
	Representa as formas de determinar se um ponto pertence ou nao ao conjunto
*/
enum ESCAPE {
	CIRCLE = 0, SQUARE, STRIP, HALFPLANE
};

/*
	Representa os padrões para desenhar ou colorir o conjunto de mandelbrot
*/
enum PATTERN {
	PLAIN = 0, FEATHERED, BINARY, GRAYSCALE, ZEBRA
};

/*
	Como a saida é uma imagem em formato texto, os caracteres que representam as cores sao definidos aqui
*/
enum COLORS {
	BLACK = ' ', WHITE = '#'
};

const float PATTERN_VALUE[] = { 2.0, 20.0, 5.0, 2.0, 5.0 }; // vetor que guarda os valores de cada padrao

const float MYERBERG = 1.401155189;

class mandelbrot {
private:
	int width;
	int height;
	int resolution; // numero de iteracoes para determinar se o ponto pertence ou nao ao conjunto maldelbrot
	float radius;
	float scale;

	complex<float> center;
	PLANE plane;
	ESCAPE escape;
	PATTERN pattern;

	char *img; // como a imagem é textual, ela é armazenada em um vetor de caracteres

	bool update; // para indicar se a imagem precisa ser atualizada
	float bound; // limite usado no teste de escape
	float bound2;

	void init();

	complex<float> convert(complex<float> &, PLANE, PLANE);
	complex<float> convert(int, int);
	bool escaped(float, float);
	char valueAt(float, float);
	void fill(int, int, int, int, char);
	void create();
public:
	mandelbrot(int, int, float, complex<float>&);
	virtual ~mandelbrot();

	friend ostream& operator <<(ostream&, mandelbrot&);
	friend ostream& operator <<(ostream&, mandelbrot*);
};

#endif /* MANDELBROT_H_ */

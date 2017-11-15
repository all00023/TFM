// Segmentacion.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"
#include "lodepng.h"
#include "punto3D.h"
#include <experimental/filesystem>
#include <math.h>
#include <map>
#include <iostream>


using namespace std;
namespace fs = experimental::filesystem;

const float umbralProf1 = 0.01;
const float umbralProf2 = 2000;
const int minPuntos = 200;

const float fx = 517.3f;
const float fy = 516.5f;
const float cx = 318.6f;
const float cy = 255.3f;
const float factor = 5000.0f;


const char rojo[] = {255, 0, 0};
const char verde[] = {0, 255, 0};
const char azulClaro[] = {0, 255, 255};
const char azulOscuro[] = {0, 0, 255};
const char amarillo[] = {255, 255, 0};
const char fucsia[] = {255, 0, 255};
const char naranja[] = {255, 128, 0};
const char morado[] = {128, 0, 255};
const char colores[8][3] = {{255, 0, 0},
							{0, 255, 0},
							{0, 255, 255},
							{0, 0, 255},
							{255, 255, 0},
							{255, 0, 255},
							{255, 128, 0},
							{128, 0, 255}};





void mapear(vector<punto3D*> &puntos, vector<unsigned char> imagen, int w, int h);
int aplicarConnectedComponentsLabeling(vector<punto3D*> &puntos, bool(*f)(punto3D*, punto3D*), int w, int h);
void unionEtiquetas(int et1, int et2, vector<int> &padres);
int reducirPadres(vector<punto3D*> &puntos, vector<int> &padres);

bool comparadorProfundidad1(punto3D* punto1, punto3D* punto2);
bool comparadorProfundidad2(punto3D* punto1, punto3D* punto2);

void convolucionProfundidadGaussiana3(vector<punto3D*> &puntos, int w, int h);
void convolucionProfundidadGaussiana5(vector<punto3D*> &puntos, int w, int h);

void dibujarNumero(vector<unsigned char> &imagen, int w, int h, unsigned num[], int wM, int hM, int x, int y, int tam, int color, int canales);
void imprimirNumero(vector<unsigned char> &imagen, int n, int w, int h);
void liberarVector(vector<punto3D*> vect);
void colorearPorProfundidad(vector<punto3D*> &puntos, vector<unsigned char> &imagen);
void colorearPorEtiqueta(vector<punto3D*> &puntos, vector<unsigned char> &imagen);
float getMaxDepth(vector<punto3D*> puntos);
float getMaxZ(vector<punto3D*> puntos);
float getMaxY(vector<punto3D*> puntos);
float getMaxX(vector<punto3D*> puntos);
float getMinZ(vector<punto3D*> puntos);
float getMinY(vector<punto3D*> puntos);
float getMinX(vector<punto3D*> puntos);

int main(int argc, char* argv[]){

	clock_t inicioLectura;
	clock_t finLectura;
	clock_t inicioProcesado;
	clock_t finProcesado;
	clock_t inicioEscritura;
	clock_t finEscritura;

	double totalLectura = 0.0;
	double totalProcesado = 0.0;
	double totalEscritura = 0.0;
	int contador = 0;

	string path = "C:\\Segmentacion\\SueloDepth";
	//string pathSalida = "C:\\Segmentacion\\SueloSalida";
	//string pathSalida = "C:\\Segmentacion\\SueloSalidaGaussian";
	string pathSalida = "C:\\Segmentacion\\SueloSalidaComp1";
	//string path = "C:\\Segmentacion\\EscritorioDepth";
	//string pathSalida = "C:\\Segmentacion\\EscritorioSalida";
	//string pathSalida = "C:\\Segmentacion\\EscritorioSalidaGaussian";
	//string pathSalida = "C:\\Segmentacion\\EscritorioSalidaComp1";


	for (auto & p : fs::directory_iterator(path)){

		string filename = p.path().string();
		string archivo = p.path().filename().string();
		vector<unsigned char> buffer, image;
		unsigned w, h;



		//Lectura

		inicioLectura = clock();

		lodepng::load_file(buffer, filename); //load the image file with given filename
		unsigned error = lodepng::decode(image, w, h, buffer, LCT_GREY, (unsigned)16U); //decode the png

		finLectura = clock();

		//stop if there is an error
		if (error){
			cout << "decoder error " << error << ": " << lodepng_error_text(error) << endl;
			system("pause");
			return 0;
		}







		//Procesamiento

		inicioProcesado = clock();

		vector<punto3D*> puntos = vector<punto3D*>(w*h);
		mapear(puntos, image, w, h);

		//convolucionProfundidadGaussiana3(puntos, w, h);
		convolucionProfundidadGaussiana5(puntos, w, h);

		int nObjetos = aplicarConnectedComponentsLabeling(puntos, comparadorProfundidad1, w, h);
		nObjetos--; //La etiqeuta 0 no nos interesa.

		vector<unsigned char> imagenSalida = vector<unsigned char>(w * h * 3);
		//colorearPorProfundidad(puntos, imagenSalida);
		colorearPorEtiqueta(puntos, imagenSalida);

		imprimirNumero(imagenSalida, nObjetos, w, h);

		finProcesado = clock();

		cout << nObjetos << endl;





		//Escritura

		buffer.clear();

		inicioEscritura = clock();

		error = lodepng::encode(buffer, imagenSalida, w, h, LCT_RGB, 8U);

		if (error){
			cout << "encoder error " << error << ": " << lodepng_error_text(error) << endl;
			return 0;
		}

		lodepng::save_file(buffer, pathSalida + "\\" + archivo);

		finEscritura = clock();


		//float maxDepth = getMaxDepth(puntos);

		liberarVector(puntos);

		double lectura = double(finLectura - inicioLectura);
		double procesado = double(finProcesado - inicioProcesado);
		double escritura = double(finEscritura - inicioEscritura);

		totalLectura += lectura;
		totalProcesado += procesado;
		totalEscritura += escritura;
		contador++;

		//cout << maxDepth << endl;
		cout << archivo << endl;
		cout << "Lec: " << (lectura) / CLOCKS_PER_SEC << endl;
		cout << "Pro: " << (procesado) / CLOCKS_PER_SEC << endl;
		cout << "Esc: " << (escritura) / CLOCKS_PER_SEC << endl;
		cout << endl;
		//system("pause");
	}


	cout << "Tiempos medios:" << endl;
	cout << "Lec: " << ((totalLectura) / CLOCKS_PER_SEC) / contador << endl;
	cout << "Pro: " << ((totalProcesado) / CLOCKS_PER_SEC) / contador << endl;
	cout << "Esc: " << ((totalEscritura) / CLOCKS_PER_SEC) / contador << endl;
	cout << endl;

	system("pause");

	return 6;
}

void mapear(vector<punto3D*> &puntos, vector<unsigned char> imagen, int w, int h){

	for (int j = 0; j < h; j++){
		for (int i = 0; i < w; i++){

			punto3D* punto = new punto3D();
			//La imagen es en escala de grises de 16bit en BigEndian
			float z = (imagen[2 * ((j * w) + i)] * 265 + imagen[(2 * ((j * w) + i)) + 1]);

			if (z > 0){
				z = z / factor;
				punto->setValido(true);
				punto->setZ(z);
				punto->setX((i - cx) * z / fx);
				punto->setY((j - cy) * z / fy);
				punto->setDepth(sqrt(punto->getX() * punto->getX() + punto->getY() * punto->getY() + z  * z));
			} else{
				punto->setValido(false);
				punto->setZ(0);
				punto->setX(0);
				punto->setY(0);
				punto->setDepth(0);
			}

			punto->setEtiqueta(0);
			puntos[j*w + i] = punto;

		}
	}

}

int aplicarConnectedComponentsLabeling(vector<punto3D*> &puntos, bool(*f)(punto3D*, punto3D*), int w, int h){

	int contador = 1;
	vector<int> padres;

	//Para que padres empiece en 1
	padres.push_back(0);

	//Primer pixel
	if (puntos[0]->getValido()){
		puntos[0]->setEtiqueta(contador++);
		padres.push_back(0);
	}
	//Primera fila
	for (int i = 1; i < w; i++){
		if (puntos[i - 1]->getValido() && (*f)(puntos[i - 1], puntos[i])){
			puntos[i]->setEtiqueta(puntos[i - 1]->getEtiqueta());
		} else{
			puntos[i]->setEtiqueta(contador++);
			padres.push_back(0);
		}
	}
	//Primera columna
	for (int j = 1; j < h; j++){
		if (puntos[(j - 1) * w]->getValido() && (*f)(puntos[(j - 1) * w], puntos[j * w])){
			puntos[j  * w]->setEtiqueta(puntos[(j - 1) * w]->getEtiqueta());
		} else{
			puntos[j * w]->setEtiqueta(contador++);
			padres.push_back(0);
		}
	}
	//El resto
	for (int i = 1; i < w; i++){
		for (int j = 1; j < h; j++){

			if (puntos[(j * w) + i]->getValido()){

				bool igualArriba = false;
				bool igualIzquierda = false;

				if (puntos[(j * w) + (i - 1)]->getValido() && (*f)(puntos[(j * w) + (i - 1)], puntos[(j * w) + i])){
					igualIzquierda = true;
				}
				if (puntos[((j - 1) * w) + i]->getValido() && (*f)(puntos[((j - 1) * w) + i], puntos[(j * w) + i])){
					igualArriba = true;
				}

				if (igualArriba && igualIzquierda){
					unionEtiquetas(puntos[((j - 1) * w) + i]->getEtiqueta(), puntos[(j * w) + (i - 1)]->getEtiqueta(), padres);
					puntos[(j * w) + i]->setEtiqueta(puntos[(j * w) + (i - 1)]->getEtiqueta());
				} else if (igualArriba){
					puntos[(j * w) + i]->setEtiqueta(puntos[((j - 1) * w) + i]->getEtiqueta());
				} else if (igualIzquierda){
					puntos[(j * w) + i]->setEtiqueta(puntos[(j * w) + (i - 1)]->getEtiqueta());
				} else{
					puntos[(j * w) + i]->setEtiqueta(contador++);
					padres.push_back(0);
				}
			}
		}
	}

	return reducirPadres(puntos, padres);

}

void unionEtiquetas(int et1, int et2, vector<int> &padres){

	while (padres[et1] != 0) et1 = padres[et1];
	while (padres[et2] != 0) et2 = padres[et2];
	if (et1 != et2) padres[et2] = et1;

}

int reducirPadres(vector<punto3D*> &puntos, vector<int> &padres){

	vector<int> contadorPuntos = vector<int>(padres.size(), 0);

	//Calculamos el padre raiz de cada etiqueta
	for (int i = 1; i < padres.size(); i++){
		if (padres[i] != 0){ // Si es raiz no hago nada
			int et = i;
			while (padres[et] != 0) et = padres[et];
			padres[i] = et;
		}
	}

	//Sustituimos la etiqueta por la raiz padre de todos los puntos
	for (int i = 0; i < puntos.size(); i++){
		if (puntos[i]->getValido() && padres[puntos[i]->getEtiqueta()] != 0){
			puntos[i]->setEtiqueta(padres[puntos[i]->getEtiqueta()]);
		}

		contadorPuntos[puntos[i]->getEtiqueta()] = contadorPuntos[puntos[i]->getEtiqueta()] + 1;

	}

	//Marcamos como etiqueta 0 a todos las etiquetas que tengan menos de minPuntos
	for (int i = 0; i < puntos.size(); i++){
		if (puntos[i]->getValido() && contadorPuntos[puntos[i]->getEtiqueta()] < minPuntos){
			puntos[i]->setEtiqueta(0);
		}
	}

	//Contamos cuantas etiquetas nos quedan
	int cont2 = 0;
	for (int i = 0; i < contadorPuntos.size(); i++){
		if (contadorPuntos[i] > minPuntos){
			cont2++;
		}
	}

	return cont2;
}

bool comparadorProfundidad1(punto3D* punto1, punto3D* punto2){

	return abs(punto1->getDepth() - punto2->getDepth()) < (umbralProf1);

}

bool comparadorProfundidad2(punto3D* punto1, punto3D* punto2){

	float media = (punto1->getDepth() + punto2->getDepth()) / 2;

	return abs(punto1->getDepth() - punto2->getDepth()) < (media / umbralProf2);

}

void colorearPorProfundidad(vector<punto3D*> &puntos, vector<unsigned char> &imagen){

	float maxD = getMaxDepth(puntos);
	float umbral = maxD / 8;


	for (int i = 0; i < puntos.size(); i++){

		if (puntos[i]->getValido()){

			if (puntos[i]->getDepth() < umbral){
				imagen[i * 3 + 0] = colores[0][0];
				imagen[i * 3 + 1] = colores[0][1];
				imagen[i * 3 + 2] = colores[0][2];
			} else if (puntos[i]->getDepth() < umbral * 2){
				imagen[i * 3 + 0] = colores[1][0];
				imagen[i * 3 + 1] = colores[1][1];
				imagen[i * 3 + 2] = colores[1][2];
			} else if (puntos[i]->getDepth() < umbral * 3){
				imagen[i * 3 + 0] = colores[2][0];
				imagen[i * 3 + 1] = colores[2][1];
				imagen[i * 3 + 2] = colores[2][2];
			} else if (puntos[i]->getDepth() < umbral * 4){
				imagen[i * 3 + 0] = colores[3][0];
				imagen[i * 3 + 1] = colores[3][1];
				imagen[i * 3 + 2] = colores[3][2];
			} else if (puntos[i]->getDepth() < umbral * 5){
				imagen[i * 3 + 0] = colores[4][0];
				imagen[i * 3 + 1] = colores[4][1];
				imagen[i * 3 + 2] = colores[4][2];
			} else if (puntos[i]->getDepth() < umbral * 6){
				imagen[i * 3 + 0] = colores[5][0];
				imagen[i * 3 + 1] = colores[5][1];
				imagen[i * 3 + 2] = colores[5][2];
			} else if (puntos[i]->getDepth() < umbral * 7){
				imagen[i * 3 + 0] = colores[6][0];
				imagen[i * 3 + 1] = colores[6][1];
				imagen[i * 3 + 2] = colores[6][2];
			} else{
				imagen[i * 3 + 0] = colores[7][0];
				imagen[i * 3 + 1] = colores[7][1];
				imagen[i * 3 + 2] = colores[7][2];
			}
		} else{
			imagen[i * 3 + 0] = 0;
			imagen[i * 3 + 1] = 0;
			imagen[i * 3 + 2] = 0;
		}
	}
}

void colorearPorEtiqueta(vector<punto3D*> &puntos, vector<unsigned char> &imagen){

	int contador = 0;
	map<int, int> paleta;

	for (int i = 0; i < puntos.size(); i++){

		if (puntos[i]->getValido() && puntos[i]->getEtiqueta() != 0){

			int color;
			map<int, int>::iterator it = paleta.find(puntos[i]->getEtiqueta());

			if (it != paleta.end()){
				color = it->second;
			} else{
				color = contador++ % 8;
				paleta.insert(pair<int, int>(puntos[i]->getEtiqueta(), color));
			}

			imagen[i * 3 + 0] = colores[color][0];
			imagen[i * 3 + 1] = colores[color][1];
			imagen[i * 3 + 2] = colores[color][2];
		}
	}
}

void convolucionProfundidadGaussiana3(vector<punto3D*> &puntos, int w, int h){

	const double gaussian3[3][3] = {{0.077847, 0.123317, 0.077847},
									{0.123317, 0.195346, 0.123317},
									{0.077847, 0.123317, 0.077847}};

	vector<float> depths = vector<float>(w*h, 0);

	int k = 1;

	for (int j = k; j < h - k; j++){
		for (int i = k; i < w - k; i++){

			punto3D* punto = puntos[j * w + i];

			float nuevo = 0;
			float peso = 0;

			for (int m = -k; m <= k; m++){
				for (int n = -k; n <= k; n++){
					if (puntos[((j + n) * w) + (i + m)]->getValido()){
						nuevo += puntos[((j + n) * w) + (i + m)]->getDepth() * gaussian3[m + k][n + k];
						peso += gaussian3[m + k][n + k];
					}
				}
			}

			depths[j * w + i] = nuevo / peso;

		}
	}

	for (int i = 0; i < puntos.size(); i++){
		puntos[i]->setDepth(depths[i]);
		if (puntos[i]->getDepth() > 0){
			puntos[i]->setValido(true);
		}

	}
}

void convolucionProfundidadGaussiana5(vector<punto3D*> &puntos, int w, int h){

	const double gaussian3[5][5] = {{0.003765,	0.015019,	0.023792,	0.015019,	0.003765},
									{0.015019,	0.059912,	0.094907,	0.059912,	0.015019},
									{0.023792,	0.094907,	0.150342,	0.094907,	0.023792},
									{0.015019,	0.059912,	0.094907,	0.059912,	0.015019},
									{0.003765,	0.015019,	0.023792,	0.015019,	0.003765}};

	vector<float> depths = vector<float>(w*h, 0);

	int k = 2;

	for (int j = k; j < h - k; j++){
		for (int i = k; i < w - k; i++){

			punto3D* punto = puntos[j * w + i];

			float nuevo = 0;
			float peso = 0;

			for (int m = -k; m <= k; m++){
				for (int n = -k; n <= k; n++){
					if (puntos[((j + n) * w) + (i + m)]->getValido()){
						nuevo += puntos[((j + n) * w) + (i + m)]->getDepth() * gaussian3[m + k][n + k];
						peso += gaussian3[m + k][n + k];
					}
				}
			}

			depths[j * w + i] = nuevo / peso;

		}
	}

	for (int i = 0; i < puntos.size(); i++){
		puntos[i]->setDepth(depths[i]);
		if (puntos[i]->getDepth() > 0){
			puntos[i]->setValido(true);
		}

	}
}


void imprimirNumero(vector<unsigned char> &imagen, int n, int w, int h){

	unsigned n0[28] = {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0};
	unsigned n1[28] = {0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1};
	unsigned n2[28] = {1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1};
	unsigned n3[28] = {1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1};
	unsigned n4[28] = {1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1};
	unsigned n5[28] = {1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1};
	unsigned n6[28] = {0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0};
	unsigned n7[28] = {1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1};
	unsigned n8[28] = {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0};
	unsigned n9[28] = {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0};

	int wM = 4;
	int hM = 7;
	int x0 = 1;
	int x1 = 6;
	int y = 1;
	int tam = 4;

	int num1 = n % 10;
	int num0 = (n / 10) % 10;

	switch (num0){
	case 0:
		dibujarNumero(imagen, w, h, n0, wM, hM, x0, y, tam, 255, 3);
		break;
	case 1:
		dibujarNumero(imagen, w, h, n1, wM, hM, x0, y, tam, 255, 3);
		break;
	case 2:
		dibujarNumero(imagen, w, h, n2, wM, hM, x0, y, tam, 255, 3);
		break;
	case 3:
		dibujarNumero(imagen, w, h, n3, wM, hM, x0, y, tam, 255, 3);
		break;
	case 4:
		dibujarNumero(imagen, w, h, n4, wM, hM, x0, y, tam, 255, 3);
		break;
	case 5:
		dibujarNumero(imagen, w, h, n5, wM, hM, x0, y, tam, 255, 3);
		break;
	case 6:
		dibujarNumero(imagen, w, h, n6, wM, hM, x0, y, tam, 255, 3);
		break;
	case 7:
		dibujarNumero(imagen, w, h, n7, wM, hM, x0, y, tam, 255, 3);
		break;
	case 8:
		dibujarNumero(imagen, w, h, n8, wM, hM, x0, y, tam, 255, 3);
		break;
	case 9:
		dibujarNumero(imagen, w, h, n9, wM, hM, x0, y, tam, 255, 3);
		break;
	}

	switch (num1){
	case 0:
		dibujarNumero(imagen, w, h, n0, wM, hM, x1, y, tam, 255, 3);
		break;
	case 1:
		dibujarNumero(imagen, w, h, n1, wM, hM, x1, y, tam, 255, 3);
		break;
	case 2:
		dibujarNumero(imagen, w, h, n2, wM, hM, x1, y, tam, 255, 3);
		break;
	case 3:
		dibujarNumero(imagen, w, h, n3, wM, hM, x1, y, tam, 255, 3);
		break;
	case 4:
		dibujarNumero(imagen, w, h, n4, wM, hM, x1, y, tam, 255, 3);
		break;
	case 5:
		dibujarNumero(imagen, w, h, n5, wM, hM, x1, y, tam, 255, 3);
		break;
	case 6:
		dibujarNumero(imagen, w, h, n6, wM, hM, x1, y, tam, 255, 3);
		break;
	case 7:
		dibujarNumero(imagen, w, h, n7, wM, hM, x1, y, tam, 255, 3);
		break;
	case 8:
		dibujarNumero(imagen, w, h, n8, wM, hM, x1, y, tam, 255, 3);
		break;
	case 9:
		dibujarNumero(imagen, w, h, n9, wM, hM, x1, y, tam, 255, 3);
		break;
	}

}

void dibujarNumero(vector<unsigned char> &imagen, int w, int h, unsigned num[], int wM, int hM, int x, int y, int tam, int color, int canales){

	for (int j = 0; j < hM; j++){
		for (int i = 0; i < wM; i++){

			if (num[j * wM + i] == 1){

				for (int n = 0; n < tam; n++){
					for (int m = 0; m < tam; m++){

						for (int canal = 0; canal < canales; canal++){

							imagen[((((((j + y) * tam) + n) * w) + ((i + x) * tam + m)) * canales) + canal] = color;
						}
					}
				}
			}
		}
	}
}

void liberarVector(vector<punto3D*> vect){

	for (vector<punto3D*>::iterator it = vect.begin(); it != vect.end(); ++it){
		delete (*it);
	}

	vect.clear();

}

float getMaxDepth(vector<punto3D*> puntos){

	float maxD = 0;
	for (vector<punto3D*>::iterator it = puntos.begin(); it != puntos.end(); ++it){
		if ((*it)->getDepth() > maxD){
			maxD = (*it)->getDepth();
		}
	}

	return maxD;
}

float getMaxZ(vector<punto3D*> puntos){

	float maxD = 0;
	for (vector<punto3D*>::iterator it = puntos.begin(); it != puntos.end(); ++it){
		if ((*it)->getZ() > maxD){
			maxD = (*it)->getZ();
		}
	}

	return maxD;
}

float getMaxY(vector<punto3D*> puntos){

	float maxD = 0;
	for (vector<punto3D*>::iterator it = puntos.begin(); it != puntos.end(); ++it){
		if ((*it)->getY() > maxD){
			maxD = (*it)->getY();
		}
	}

	return maxD;
}

float getMaxX(vector<punto3D*> puntos){

	float maxD = 0;
	for (vector<punto3D*>::iterator it = puntos.begin(); it != puntos.end(); ++it){
		if ((*it)->getX() > maxD){
			maxD = (*it)->getX();
		}
	}

	return maxD;
}

float getMinZ(vector<punto3D*> puntos){

	float maxD = numeric_limits<float>::min();;
	for (vector<punto3D*>::iterator it = puntos.begin(); it != puntos.end(); ++it){
		if ((*it)->getZ() < maxD){
			maxD = (*it)->getZ();
		}
	}

	return maxD;
}

float getMinX(vector<punto3D*> puntos){

	float maxD = numeric_limits<float>::min();;
	for (vector<punto3D*>::iterator it = puntos.begin(); it != puntos.end(); ++it){
		if ((*it)->getX() < maxD){
			maxD = (*it)->getX();
		}
	}

	return maxD;
}

float getMinY(vector<punto3D*> puntos){

	float maxD = numeric_limits<float>::min();;
	for (vector<punto3D*>::iterator it = puntos.begin(); it != puntos.end(); ++it){
		if ((*it)->getY() < maxD){
			maxD = (*it)->getY();
		}
	}

	return maxD;
}

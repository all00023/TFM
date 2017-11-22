#pragma once
#include "Punto3D.h"

using namespace std;

class Plano3D{

private:
	int etiqueta;
	bool valido;

	double sumaNX;
	double sumaNY;
	double sumaNZ;

	double sumaX;
	double sumaY;
	double sumaZ;

	float nx;
	float ny;
	float nz;
	float d;

	float cx;
	float cy;
	float cz;

	set<Punto3D*> puntosInternos;
	set<Plano3D*> planosColindantes;

public:
	Plano3D(){
		valido = false;
		sumaNX = 0;
		sumaNY = 0;
		sumaNZ = 0;
	}
	~Plano3D(){
		for (set<Punto3D*>::iterator it = puntosInternos.begin(); it != puntosInternos.end(); ++it){
			(*it)->setPlano(NULL);
		}
	}


	void addPunto(Punto3D *p){

		sumaNX += p->getNX();
		sumaNY += p->getNY();
		sumaNZ += p->getNZ();

		sumaX += p->getX();
		sumaY += p->getY();
		sumaZ += p->getZ();

		puntosInternos.insert(p);
		p->setPlano(const_cast<Plano3D*>(this));

	}

	void calcularParametrosDelPlano(){
		nx = sumaNX / puntosInternos.size();
		ny = sumaNY / puntosInternos.size();
		nz = sumaNZ / puntosInternos.size();

		cx = sumaX / puntosInternos.size();
		cy = sumaY / puntosInternos.size();
		cz = sumaZ / puntosInternos.size();

		normalizarVectorNormal();

		d = -(cx * nx) - (cy * ny) - (cz * nz);

	}

	float moduloVectorNormal(){
		return sqrt(nx * nx + ny * ny + nz * nz);
	}

	void normalizarVectorNormal(){
		float modulo = moduloVectorNormal();
		nx /= modulo;
		ny /= modulo;
		nz /= modulo;

	}

	bool calcularSiValido(vector<Punto3D*> puntos, float porcentaje, float umbralConvexidad, int minPuntos, float umbralCosenoNormales){

		if (puntosInternos.size() < minPuntos){

			valido = false;

		} else{

			int noValidos = 0;
			int limite = puntosInternos.size() * umbralConvexidad;
			set<Punto3D*>::iterator it = puntosInternos.begin();

			while (it != puntosInternos.end() && noValidos < limite){

				if (getDotProductPunto(*it) > umbralCosenoNormales)
					noValidos++;

				it++;
			}

			if (noValidos>= limite){
				valido = false;
			}

		}
	}

	void combinarPlanos(Plano3D* plano2){

		sumaNX += plano2->sumaNX;
		sumaNY += plano2->sumaNY;
		sumaNZ += plano2->sumaNZ;

		sumaX += plano2->sumaX;
		sumaY += plano2->sumaY;
		sumaZ += plano2->sumaZ;

		puntosInternos.insert(plano2->puntosInternos.begin(), plano2->puntosInternos.end());
		planosColindantes.insert(plano2->planosColindantes.begin(), plano2->planosColindantes.end());

		for (set<Punto3D*>::iterator it = plano2->puntosInternos.begin(); it != plano2->puntosInternos.end(); ++it){
			(*it)->setPlano(const_cast<Plano3D*>(this));
		}

		for (set<Plano3D*>::iterator it = plano2->planosColindantes.begin(); it != plano2->planosColindantes.end(); ++it){
			(*it)->planosColindantes.erase(plano2);
			(*it)->planosColindantes.insert(const_cast<Plano3D*>(this));
		}

	}

	float getDotProductPunto(Punto3D *punto2){
		return abs(nx*punto2->getNX() + ny * punto2->getNY() + nz * punto2->getNZ());
	}

};
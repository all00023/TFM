#pragma once

using namespace std;

class Plano3D;

class Punto3D{

private:
	bool valido;
	float x;
	float y;
	float z;
	float depth;
	float nx;
	float ny;
	float nz;
	float nd;
	int etiqueta;
	Plano3D* plano;

public:
	Punto3D(){}
	~Punto3D(){}

	bool getValido(){
		return valido;
	}
	float getX(){
		return x;
	}

	float getY(){
		return y;
	}

	float getZ(){
		return z;
	}

	float getDepth(){
		return depth;
	}

	float getNX(){
		return nx;
	}

	float getNY(){
		return ny;
	}

	float getNZ(){
		return nz;

	}float getND(){
		return nd;
	}

	int getEtiqueta(){
		return etiqueta;
	}
	
	Plano3D* getPlano(){
		return plano;
	}

	void setValido(bool valido){
		this->valido = valido;
	}

	void setX(float x){
		this->x = x;
	}

	void setY(float y){
		this->y = y;
	}

	void setZ(float z){
		this->z = z;
	}

	void setDepth(float depth){
		this->depth = depth;
	}

	void setNX(float nx){
		this->nx = nx;
	}

	void setNY(float ny){
		this->ny = ny;
	}

	void setNZ(float nz){
		this->nz = nz;
	}

	void setND(float nd){
		this->nd = nd;
	}

	void setEtiqueta(int etiqueta){
		this->etiqueta = etiqueta;
	}
	
	void setPlano(Plano3D* plano){
		this->plano = plano;
	}

	void calcularND(){
		nd = -(x * nx) - (y * ny) - (z * nz);
	}

	void normalizarVectorNormal(){
		float modulo = moduloVectorNormal();
		nx /= modulo;
		ny /= modulo;
		nz /= modulo;

	}

	float moduloVectorNormal(){
		return sqrt(nx * nx + ny * ny + nz * nz);
	}

	void calcularDepth(){
		depth = sqrt(x * x + y * y + z * z);
	}

	float getDotProduct(Punto3D *punto2){
		return abs(nx*punto2->nx + ny * punto2->ny + nz * punto2->nz);
	}

	float getNDMenosND(Punto3D *punto2){
		return abs(nd - punto2->nd);
	}

};


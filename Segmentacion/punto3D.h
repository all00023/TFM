

class punto3D {

private:
	bool valido;
	float x;
	float y;
	float z;
	float depth;
	float nx;
	float ny;
	float nz;
	int etiqueta;
public:
	punto3D(float x, float y, float z) {}
	punto3D() {}
	~punto3D() {}

	bool getValido() {
		return valido;
	}
	float getX() {
		return x;
	}

	float getY() {
		return y;
	}
	
	float getZ() {
		return z;
	}

	float getDepth() {
		return depth;
	}
	
	float getNX() {
		return nx;
	}
	
	float getNY() {
		return ny;
	}
	
	float getNZ() {
		return nz;
	}
	
	int getEtiqueta() {
		return etiqueta;
	}

	void setValido(bool valido) {
		this->valido = valido;
	}

	void setX(float x) {
		this->x = x;
	}
	
	void setY(float y) {
		this->y = y;
	}
	
	void setZ(float z) {
		this->z = z;
	}

	void setDepth(float depth) {
		this->depth = depth;
	}
	
	void setNX(float nx) {
		this->nx = nx;
	}
	
	void setNY(float ny) {
		this->ny = ny;
	}
	
	void setNZ(float nz) {
		this->nz = nz;
	}

	void setEtiqueta(int etiqueta) {
		this->etiqueta = etiqueta;
	}

};


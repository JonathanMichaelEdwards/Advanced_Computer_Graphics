#ifndef TEAPOT_H
#define TEAPOT_H

class Teapot
{
private:
    unsigned int vaoID;
	int  numVert, numNorm, numTri;
	float vert[3*6272], norm[3*6272];
	int elem[3*10816];
	static bool read_flag;

public:
    Teapot(float size);
    void render() const;
};

#endif //TEAPOT_H

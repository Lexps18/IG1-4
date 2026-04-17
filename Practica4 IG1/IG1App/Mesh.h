#ifndef _H_Mesh_H_
#define _H_Mesh_H_

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

class Mesh
{
public:
	static Mesh* createRGBAxes(GLdouble l); // creates a new 3D-RGB axes mesh

	Mesh();
	virtual ~Mesh();

	Mesh(const Mesh& m) = delete;            // no copy constructor
	Mesh& operator=(const Mesh& m) = delete; // no copy assignment

	virtual void render() const;

	GLuint size() const { return mNumVertices; };
	std::vector<glm::vec3> const& vertices()  const { return vVertices; };
	std::vector<glm::vec4> const& colors()    const { return vColors; };

	virtual void load();
	void unload();

	static Mesh* generateRegularPolygon(GLuint num, GLdouble r, GLuint primitiva);
	static Mesh* generateRectangle(GLdouble w, GLdouble h);
	static Mesh* generateRGBRectangle(GLdouble w, GLdouble h);
	static Mesh* generateCube(GLdouble length);
	static Mesh* generateRGBCubeTriangles(GLdouble length);
	static Mesh* generateRectangleTexCor(GLdouble w, GLdouble h, GLuint rw, GLuint rh);
	static Mesh* generateBoxOutline(GLdouble length);
	static Mesh* generateBoxOutlineTexCor(GLdouble length);
	static Mesh* generateStar3D(GLdouble re, GLuint np, GLdouble h);
	static Mesh* generateStar3DTexCor(GLdouble re, GLuint np, GLdouble h);

	// Malla por revolución: rota el perfil 2D nMuestras veces alrededor del eje Y
	static Mesh* generateByRevolution(const std::vector<glm::vec2>& perfil, int nMuestras);

protected:
	GLuint mPrimitive = GL_TRIANGLES;  // primitiva gráfica
	GLuint mNumVertices = 0;            // número de elementos (= vVertices.size())
	std::vector<glm::vec3> vVertices;   // array de vértices
	std::vector<glm::vec4> vColors;     // array de colores
	std::vector<glm::vec2> vTexCoords;  // coordenadas de textura
	virtual void draw() const;

	GLuint mVAO;  // vertex array object

private:
	GLuint mVBO;  // vertex buffer object (posiciones)
	GLuint mCBO;  // color buffer object
	GLuint mTCO;  // texture coord object
};

// -----------------------------------------------------------------------
// IndexMesh: malla que renderiza con glDrawElements (índices en EBO)
// -----------------------------------------------------------------------
class IndexMesh : public Mesh
{
public:
	IndexMesh() = default;
	virtual ~IndexMesh();

	IndexMesh(const IndexMesh&) = delete;
	IndexMesh& operator=(const IndexMesh&) = delete;

	// Cubo de arista 'length' con 8 vértices únicos y 36 índices
	static IndexMesh* generateIndexCube(GLdouble length);

	void load() override;  // sube vértices + EBO de índices

protected:
	std::vector<GLuint> vIndices;  // tabla de índices
	void draw() const override;    // usa glDrawElements en lugar de glDrawArrays

private:
	GLuint mEBO = 0;  // element buffer object (índices)
};

#endif //_H_Mesh_H_
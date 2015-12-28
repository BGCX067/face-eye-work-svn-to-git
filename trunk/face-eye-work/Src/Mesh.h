#ifndef __Mesh_h__
#define __Mesh_h__

class Mesh
{
public:
	Mesh();
	~Mesh();
	static Mesh* createFromFile(const char* fileName, ID3D11Device* _device, ID3D11DeviceContext* _context);
	
	void drawMesh();
	void setPosition(const DirectX::XMFLOAT3& p);
	void setScale(float scale);
	DirectX::XMFLOAT4X4 getModelMatrix() { return modelMatrix; }
private:
	DirectX::XMFLOAT4X4 modelMatrix;
	DirectX::XMFLOAT3	position;
	
	unsigned int vertexNum;
	unsigned int indexNum;

	ID3D11DeviceContext* context;
	ID3D11Buffer*	vertexBuffer;
	ID3D11Buffer*	indexBuffer;

	static UINT VertexStride;
	static UINT VertexOffset;
};
#endif
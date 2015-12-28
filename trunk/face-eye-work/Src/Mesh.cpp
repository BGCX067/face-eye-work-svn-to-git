#include "stdafx.h"
#include <fstream>
#include "Mesh.h"

struct VertexInfo
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT4 Tangent;
};

ID3D11Buffer* CreateD3D11Buffer(ID3D11Device* device, int bufferType, void* pData, unsigned int dataSize)
{
	ID3D11Buffer* pRes = NULL;
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA bufferData;
	HRESULT result;

	// Set up the description of the static vertex buffer.
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = dataSize;
	bufferDesc.BindFlags = bufferType;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	bufferData.pSysMem = pData;
	bufferData.SysMemPitch = 0;
	bufferData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&bufferDesc, &bufferData, &pRes);
	if(FAILED(result))
	{
		return NULL;
	}
	return pRes;
}

UINT Mesh::VertexStride = sizeof(VertexInfo);
UINT Mesh::VertexOffset = 0;

Mesh::Mesh()
{
	vertexBuffer = NULL;
	indexBuffer = NULL;
	vertexNum = 0;
	indexNum = 0;
	position = DirectX::XMFLOAT3(0, 0, 0);
}

Mesh::~Mesh()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

Mesh* Mesh::createFromFile(const char* fileName, ID3D11Device* _device, ID3D11DeviceContext* _context)
{
	std::fstream file;
	file.open(fileName, std::ios_base::in | std::ios_base::binary);
	if (!file.is_open())
	{
		OutputDebugString(L"Can not open file\n");
		return NULL;
	}

	unsigned int vertexNum, indexNum;
	file.read((char*)&vertexNum, sizeof(vertexNum));
	file.read((char*)&indexNum, sizeof(indexNum));

	unsigned int vertexByteSize = sizeof(VertexInfo) * vertexNum;
	unsigned int indexByteSize =sizeof(unsigned short) * indexNum;

	char* vertexBufferData = new char[vertexByteSize];
	char* indexBufferData = new char[indexByteSize];

	file.read(vertexBufferData, vertexByteSize);
	file.read(indexBufferData, indexByteSize);
	file.close();

	Mesh* res = new Mesh();
	res->vertexBuffer = CreateD3D11Buffer(_device, D3D11_BIND_VERTEX_BUFFER, vertexBufferData, vertexByteSize); 
	res->indexBuffer = CreateD3D11Buffer(_device, D3D11_BIND_INDEX_BUFFER, indexBufferData, indexByteSize); 
	res->context = _context;
	res->vertexNum = vertexNum;
	res->indexNum  = indexNum;

	delete [] vertexBufferData;
	delete [] indexBufferData;

	return res;
}

void Mesh::drawMesh()
{
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &VertexStride, &VertexOffset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->DrawIndexed(indexNum, 0, 0);
	//context->DrawIndexed(3, 0, 0);
}

void Mesh::setPosition(const DirectX::XMFLOAT3& p)
{
	position = p;
	DirectX::XMStoreFloat4x4(&modelMatrix, DirectX::XMMatrixTranslation(p.x, p.y, p.z));
}

void Mesh::setScale(float scale)
{
	float rcpScale = 1.0f / scale;
	modelMatrix.m[3][3] = rcpScale;
	modelMatrix.m[3][0] *= rcpScale;
	modelMatrix.m[3][1] *= rcpScale;
	modelMatrix.m[3][2] *= rcpScale;
}
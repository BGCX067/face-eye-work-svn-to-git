#ifndef __Constant_Buffer_h__
#define __Constant_Buffer_h__

template<class T>
class ConstantBuffer
{
public:
	ConstantBuffer()
	{
		context = NULL;
		d3dBuffer = NULL;
	}

	ConstantBuffer(ID3D11Device* _device, ID3D11DeviceContext* _context)
	{
		d3dBuffer = CreateConstBuffer(_device, sizeof(T));
		context = _context;
	}

	~ConstantBuffer()
	{
		SAFE_RELEASE(d3dBuffer);
	}

	static ID3D11Buffer* CreateConstBuffer(ID3D11Device* device, UINT bufferSize)
	{
		ID3D11Buffer* res = NULL;
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = bufferSize < 16 ? 16 : bufferSize;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		HRESULT hr = device->CreateBuffer(&bd, NULL, &res);
		return res;
	}

	ID3D11DeviceContext*	context;
	ID3D11Buffer*			d3dBuffer;
	T						dataCache;
	void Flush()
	{
		context->UpdateSubresource(d3dBuffer, 0, 0, &dataCache, 0, 0);
	}
};
#endif
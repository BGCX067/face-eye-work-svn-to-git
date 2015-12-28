#include "stdafx.h"
#include <D3Dcompiler.h>
#include <d3d11shader.h>
#include "ShaderUtils.h"
#include <vector>

namespace DirectX
{

HRESULT CompileShaderFromFile(LPCSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;// | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	//dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif


	ID3DBlob* pErrorBlob;
	wchar_t wfileName[128];
	mbstowcs(wfileName, szFileName, 128);
	hr = D3DCompileFromFile( wfileName, NULL, NULL, szEntryPoint, szShaderModel, 
							 dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if( FAILED(hr) )
	{
		if( pErrorBlob != NULL )
			OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
		if( pErrorBlob ) pErrorBlob->Release();
		return hr;
	}
	if( pErrorBlob ) pErrorBlob->Release();

	return S_OK;
}

HRESULT CreateInputLayoutDescFromVertexShaderSignature(ID3DBlob* pShaderBlob, ID3D11Device* pD3DDevice, ID3D11InputLayout** pInputLayout)
{
	// Reflect shader info
	ID3D11ShaderReflection* pVertexShaderReflection = NULL;	
	if ( FAILED( D3DReflect( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &pVertexShaderReflection ) ) ) 
	{
		return S_FALSE;
	}

	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	pVertexShaderReflection->GetDesc( &shaderDesc );

	// Read input layout description from shader info
	unsigned int byteOffset = 0;
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for ( unsigned int i=0; i< shaderDesc.InputParameters; i++ )
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;		
		pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc );

		// fill out input element desc
		D3D11_INPUT_ELEMENT_DESC elementDesc;	
		elementDesc.SemanticName = paramDesc.SemanticName;		
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = byteOffset;
		//elementDesc.InputSlot = i;
		//elementDesc.AlignedByteOffset = 0;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;	

		// determine DXGI format
		if ( paramDesc.Mask == 1 )
		{
			if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			byteOffset += 4;
		}
		else if ( paramDesc.Mask <= 3 )
		{
			if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			byteOffset += 8;
		}
		else if ( paramDesc.Mask <= 7 )
		{
			if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			byteOffset += 12;
		}
		else if ( paramDesc.Mask <= 15 )
		{
			if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			byteOffset += 16;
		}

		//save element desc
		inputLayoutDesc.push_back(elementDesc);
	}		

	// Try to create Input Layout
	HRESULT hr = pD3DDevice->CreateInputLayout( &inputLayoutDesc[0], inputLayoutDesc.size(), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pInputLayout );

	//Free allocation shader reflection memory
	pVertexShaderReflection->Release();
	return hr;
}


VertexShaderInfo* CreateVertexShaderFromFile(ID3D11Device* device, LPCSTR fileName, LPCSTR entry)
{
	ID3DBlob* pBlob = NULL;
	HRESULT hr = CompileShaderFromFile(fileName, entry, "vs_5_0", &pBlob);
	if (FAILED(hr))
	{
		OutputDebugStringA("Failed to compile vertex shader");
		SAFE_RELEASE(pBlob);
		return NULL;
	}

	ID3D11VertexShader* pVS = NULL;
	// Create the vertex shader
	hr = device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pVS);
	if (FAILED(hr))
	{
		OutputDebugStringA("Failed to create vertex shader");
		SAFE_RELEASE(pBlob);
		SAFE_RELEASE(pVS);
		return NULL;
	}

	ID3D11InputLayout* pInputLayout = NULL;
	hr = CreateInputLayoutDescFromVertexShaderSignature(pBlob, device, &pInputLayout);
	if (FAILED(hr))
	{
		OutputDebugStringA("Failed to create vertex shader input layout");
		SAFE_RELEASE(pBlob);
		SAFE_RELEASE(pVS);
		SAFE_RELEASE(pInputLayout);
		return NULL;
	}

	SAFE_RELEASE(pBlob);

	VertexShaderInfo* res = new VertexShaderInfo(pVS, pInputLayout);
	return res;
}

ID3D11PixelShader* CreatePixelShaderFromFile(ID3D11Device* device, LPCSTR fileName, LPCSTR entry)
{
	ID3DBlob* pBlob = NULL;
	HRESULT hr = CompileShaderFromFile(fileName, entry, "ps_5_0", &pBlob);
	if (FAILED(hr))
	{
		OutputDebugStringA("Failed to compile pixel shader");
		SAFE_RELEASE(pBlob);
		return NULL;
	}

	ID3D11PixelShader* pPS = NULL;
	// Create the vertex shader
	hr = device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPS);
	if (FAILED(hr))
	{
		OutputDebugStringA("Failed to create pixel shader");
		SAFE_RELEASE(pBlob);
		SAFE_RELEASE(pPS);
		return NULL;
	}

	SAFE_RELEASE(pBlob);
	return pPS;
}
} // DirectX
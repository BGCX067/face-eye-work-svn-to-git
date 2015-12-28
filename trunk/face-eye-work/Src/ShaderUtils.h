#ifndef __Shader_Utils_h__
#define __Shader_Utils_h__

namespace DirectX
{
	class VertexShaderInfo
	{
	public:
		VertexShaderInfo(ID3D11VertexShader* _shader, ID3D11InputLayout* _layout) : shader(_shader), layout(_layout){}
		~VertexShaderInfo()
		{
			SAFE_RELEASE(shader);
			SAFE_RELEASE(layout);
		}
		ID3D11VertexShader*		shader;
		ID3D11InputLayout*		layout;

		operator ID3D11VertexShader*() const { return shader; }
		operator ID3D11InputLayout*() const { return layout; }
	};

	VertexShaderInfo* CreateVertexShaderFromFile(ID3D11Device* device, LPCSTR fileName, LPCSTR entry);
	ID3D11PixelShader* CreatePixelShaderFromFile(ID3D11Device* device, LPCSTR fileName, LPCSTR entry);
};
#endif
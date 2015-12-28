// Copyright (C) 2009 Jorge Jimenez (jim@unizar.es)

#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include "RenderTarget.h"

class ShadowMap {
    public:
        ShadowMap(ID3D11Device* _device, ID3D11DeviceContext* _context, int width, int height);
        ~ShadowMap();

        void begin(const DirectX::XMFLOAT4X4 &view, const DirectX::XMFLOAT4X4 &projection);
        void setWorldMatrix(const DirectX::XMFLOAT4X4 &world);
        void end();

        operator ID3D11ShaderResourceView * const () { return *depthStencil; }

		//static DirectX::XMFLOAT4X4 getViewProjectionTextureMatrix(const DirectX::XMFLOAT4X4 &view, const DirectX::XMFLOAT4X4 &projection, float shadowBias);

    private:
        ID3D11Device*			device;
		ID3D11DeviceContext*	context;
        DepthStencil*			depthStencil;
        D3D11_VIEWPORT			viewport;
};

#endif

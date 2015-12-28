#include "stdafx.h"
#include "ShadowMap.h"
using namespace DirectX;

ShadowMap::ShadowMap(ID3D11Device* _device, ID3D11DeviceContext* _context, int width, int height)
        : device(_device), context(_context) {
    depthStencil = new DepthStencil(device, width, height);
}


ShadowMap::~ShadowMap() {
    SAFE_DELETE(depthStencil);
}


void ShadowMap::begin(const DirectX::XMFLOAT4X4 &view, const DirectX::XMFLOAT4X4 &projection)
{
    //HRESULT hr;

    //context->IASetInputLayout(vertexLayout);

    //context->ClearDepthStencilView(*depthStencil, D3D11_CLEAR_DEPTH, 1.0, 0);

    //XMFLOAT4X4 linearProjection = projection;
    //float Q = projection._33;
    //float N = -projection._43 / projection._33;
    //float F = -N * Q / (1 - Q);
    //linearProjection._33 /= F;
    //linearProjection._43 /= F;

    //V(effect->GetVariableByName("view")->AsMatrix()->SetMatrix((float*) &view));
    //V(effect->GetVariableByName("projection")->AsMatrix()->SetMatrix((float*) &linearProjection));
    //
    //context->OMSetRenderTargets(0, NULL, *depthStencil);

    //UINT numViewports = 1;
    //context->RSGetViewports(&numViewports, &viewport);
    //depthStencil->setViewport();
}


void ShadowMap::setWorldMatrix(const DirectX::XMFLOAT4X4 &world) {
    //HRESULT hr;
    //V(effect->GetVariableByName("world")->AsMatrix()->SetMatrix((float*) &world));
}


void ShadowMap::end() {
    //context->RSSetViewports(1, &viewport);
    //context->OMSetRenderTargets(0, NULL, NULL);
}


//XMFLOAT4X4 ShadowMap::getViewProjectionTextureMatrix(const XMFLOAT4X4 &view, const XMFLOAT4X4 &projection, float bias) {
//    XMFLOAT4X4 scale;
//    XMFLOAT4X4Scaling(&scale, 0.5f, -0.5f, 1.0f);
//
//    XMFLOAT4X4 translation;
//    XMFLOAT4X4Translation(&translation, 0.5f, 0.5f, bias);
//    
//    return view * projection * scale * translation;
//}

#include "stdafx.h"
#include <sstream>
#include <iomanip>
#include <limits>

#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTmisc.h"
#include "DXUTCamera.h"
#include "DXUTSettingsDlg.h"
#include "SDKmisc.h"
#include "resource.h"

#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "Effects.h"
#include "GeometricPrimitive.h"
#include "Model.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"

#include "Camera.h"
#include "ShadowMap.h"
#include "Mesh.h"
#include "ConstantBuffer.h"
#include "ShaderUtils.h"
#include "DDSTextureLoader.h"
#pragma warning( disable : 4100 )

using namespace DirectX;
using namespace std;

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
Camera						g_Camera;               // A model viewing camera
CDXUTDialogResourceManager  g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg             g_SettingsDlg;          // Device settings dialog
CDXUTTextHelper*            g_pTxtHelper = nullptr;
CDXUTDialog                 g_HUD;                  // dialog for standard controls
CDXUTDialog                 g_SampleUI;             // dialog for sample specific controls

std::unique_ptr<CommonStates>                           g_States;
std::unique_ptr<GeometricPrimitive>                     g_Shape;
std::unique_ptr<Model>                                  g_Model;
std::unique_ptr<PrimitiveBatch<VertexPositionColor>>    g_Batch;
std::unique_ptr<SpriteBatch>                            g_Sprites;
std::unique_ptr<SpriteFont>                             g_Font;

Mesh* lee = NULL;
Mesh* leeLeftEye = NULL;
Mesh* leeRightEye = NULL;

Mesh* roberto = NULL;
Mesh* robertoLeftEye = NULL;
Mesh* robertoRightEye = NULL;

// Shaders
VertexShaderInfo*	mainPassVertexShader = NULL;
ID3D11PixelShader*	mainPassPixelShader = NULL;

IDirectInput8* directInput = NULL;
IDirectInputDevice8* keyboard = NULL;
IDirectInputDevice8* mouse = NULL;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_CHANGEDEVICE        2
#define IDC_TOGGLEREF           3
#define IDC_TOGGLEWARP          4
#define IDC_NLIGHTS             5
#define IDC_OBJECT              6
#define IDC_MESH                7
#define IDC_MATERIAL            8
#define IDC_PROFILE             9
#define IDC_MSAA               10
#define IDC_SSSLEVEL_LABEL     11
#define IDC_SSSLEVEL           12
#define IDC_CORRECTION_LABEL   13
#define IDC_CORRECTION         14
#define IDC_MAXDD_LABEL        15
#define IDC_MAXDD              16
#define IDC_BUMPINESS_LABEL    17
#define IDC_BUMPINESS          18
#define IDC_ROUGHNESS_LABEL    19
#define IDC_ROUGHNESS          20
#define IDC_SSS                21
#define IDC_BLOOM              22

struct MsaaMode {
    wstring name;
    DXGI_SAMPLE_DESC desc;
};
const int NUM_MSAA_MODES = 7;
MsaaMode msaaModes[NUM_MSAA_MODES] = {
    {L"1x MSAA",   {1,  0}},
    {L"2x MSAA",   {2,  0}},
    {L"4x MSAA",   {4,  0}},
    {L"8x CSAA",   {4,  8}},
    {L"8xQ CSAA",  {8,  8}},
    {L"16x CSAA",  {4, 16}},
    {L"16xQ CSAA", {8, 16}},
};
int msaaIndex = 0;

double t0 = numeric_limits<double>::min();
double tFade = numeric_limits<double>::max();
bool skipIntro = false;

const int MAX_N_LIGHTS = 6;
const int N_HEADS = 1;
const int SHADOW_MAP_SIZE = 2048;
const int HUD_WIDTH = 130;
const float CAMERA_FOV = 20.0f;

enum Object { OBJECT_CAMERA, OBJECT_LIGHT1, OBJECT_LIGHT2, OBJECT_LIGHT3, OBJECT_LIGHT4, OBJECT_LIGHT5, OBJECT_LIGHT6 };
Object object = OBJECT_CAMERA;

enum State { STATE_SPLASH, STATE_INTRO, STATE_RUNNING };
#ifdef XYZRGB_BUILD
State state = STATE_SPLASH;
#else
State state = STATE_RUNNING;
#endif

__declspec(align(16)) struct ConstantBufferMatrix
{
	// Camera Data
	XMFLOAT4X4 g_mModel;
	XMFLOAT4X4 g_mView;
	XMFLOAT4X4 g_mMV;
	XMFLOAT4X4 g_mMVP;
};

__declspec(align(16)) struct DirectionalLight
{
	DirectionalLight(){}
	DirectionalLight(XMFLOAT3 d, float i, XMFLOAT3 c, float s) : direction(d), intensity(i), color(c), specIntensity(s){}
	__declspec(align(16))	XMFLOAT3	direction;
	__declspec(align(4))	float		intensity;
	__declspec(align(16))	XMFLOAT3	color;
	__declspec(align(4))	float		specIntensity;
};

__declspec(align(16)) struct ConstantBufferLight
{
	ConstantBufferLight()
	{
		g_LightNum = 0;
		g_Bumpiness = 1;
	}
	// Light Data: Only Directional Light
	__declspec(align(16)) unsigned int			g_LightNum;
	__declspec(align(4))  float					g_Bumpiness;
	DirectionalLight		g_LightData[MAX_N_LIGHTS];
};

unsigned int			g_LightNum;
DirectionalLight		g_LightData[MAX_N_LIGHTS];

ConstantBuffer<ConstantBufferMatrix>* cBufferMatrix = NULL;
ConstantBuffer<ConstantBufferLight>*  cBufferLight = NULL;

unsigned char keyboardState[256];
DIMOUSESTATE mouseState;

D3D11_VIEWPORT viewport;

class DiffuseNormalMaterial
{
public:
	DiffuseNormalMaterial():diffuseSRV(0), normalSRV(0){}
	~DiffuseNormalMaterial()
	{ 
		SAFE_RELEASE(diffuseSRV); 
		SAFE_RELEASE(normalSRV);
		SAFE_RELEASE(diffuseResource);
		SAFE_RELEASE(normalResource);
	}
	ID3D11ShaderResourceView*	diffuseSRV;
	ID3D11ShaderResourceView*	normalSRV;
	ID3D11Resource*				diffuseResource;
	ID3D11Resource*				normalResource;
	void apply(ID3D11DeviceContext* context)
	{
		context->PSSetShaderResources(0, 2, &diffuseSRV);
	}
	static DiffuseNormalMaterial* CreateDiffuseNormalMaterial(ID3D11Device* device, wchar_t* diffuseFileName, const wchar_t* normalFileName)
	{
		DiffuseNormalMaterial* res = new DiffuseNormalMaterial();
		HRESULT hr;
		hr = CreateDDSTextureFromFileEx(device, diffuseFileName, 8192, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, true, &res->diffuseResource, &res->diffuseSRV);
		//hr = CreateDDSTextureFromFile(device, diffuseFileName, &res->diffuseResource, &res->diffuseSRV); 
		if (FAILED(hr))
			SAFE_DELETE(res);
		hr = CreateDDSTextureFromFileEx(device, normalFileName, 8192, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, true, &res->normalResource, &res->normalSRV);
		//hr = CreateDDSTextureFromFile(device, normalFileName, &res->normalResource, &res->normalSRV); 
		if (FAILED(hr))
			SAFE_DELETE(res);
		return res;
	}
};
DiffuseNormalMaterial* leeFaceMaterial = NULL;
DiffuseNormalMaterial* robertoFaceMaterial = NULL;

ID3D11RenderTargetView* pBackBufferRTV = NULL;

RenderTarget*	pOriginalColorBuffer;
RenderTarget*	pSSSTempBuffer;

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext );
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK OnMouseMove(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                         bool bSideButton1Down,  bool bSideButton2Down,  int nMouseWheelDelta,
                         int xPos,  int yPos,  void* pUserContext );
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );

bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output,
                                       const CD3D11EnumDeviceInfo *DeviceInfo,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext );
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                         const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext );
void CALLBACK OnD3D11DestroyDevice( void* pUserContext );
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
                                 float fElapsedTime, void* pUserContext );

void InitApp();
void RenderText();
void DrawGrid( FXMVECTOR xAxis, FXMVECTOR yAxis, FXMVECTOR origin, size_t xdivs, size_t ydivs, GXMVECTOR color );
ID3D11Buffer* CreateConstBuffer(ID3D11Device* device, UINT bufferSize);

void InitDirectXInput();
void UpdateDirectXInput();
void ReleaseDirectXInput();
void UpdateLights();

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // DXUT will create and use the best device
    // that is available on the system depending on which D3D callbacks are set below

    // Set DXUT callbacks
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
	DXUTSetCallbackMouse( OnMouseMove );

    DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
    DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
    DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
    DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
    DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );
    DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );

    InitApp();
    DXUTInit( true, true, nullptr ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true );
    DXUTCreateWindow( L"face-eye-work" );

    // Only require 10-level hardware, change to D3D_FEATURE_LEVEL_11_0 to require 11-class hardware
    // Switch to D3D_FEATURE_LEVEL_9_x for 10level9 hardware
    DXUTCreateDevice(D3D_FEATURE_LEVEL_11_0, true, 1280, 720);
	pBackBufferRTV = DXUTGetD3D11RenderTargetView();

    DXUTMainLoop(); // Enter into the DXUT render loop
		// Initialize the main direct input interface.

    return DXUTGetExitCode();
}

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    g_SettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );
    g_SampleUI.Init( &g_DialogResourceManager );

    g_HUD.SetCallback( OnGUIEvent );
    int iY = 30;
    g_HUD.AddButton(IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, HUD_WIDTH, 22);
    g_HUD.AddButton(IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, HUD_WIDTH, 22, VK_F2);
    g_HUD.AddButton(IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, HUD_WIDTH, 22, VK_F3);
    g_HUD.AddButton(IDC_TOGGLEWARP, L"Toggle WARP (F4)", 35, iY += 24, HUD_WIDTH, 22, VK_F4);

    g_HUD.AddComboBox(IDC_MSAA, 35, iY += 24, HUD_WIDTH, 22);
    for(char i = 0; i < NUM_MSAA_MODES; i++){
        g_HUD.GetComboBox(IDC_MSAA)->AddItem(msaaModes[i].name.c_str(), (void*) i);
    }

    g_HUD.AddComboBox(IDC_NLIGHTS, 35, iY += 24, HUD_WIDTH, 22, 0, false);
    g_HUD.GetComboBox(IDC_NLIGHTS)->AddItem(L"1 Light", (LPVOID) 0);
    g_HUD.GetComboBox(IDC_NLIGHTS)->AddItem(L"2 Lights", (LPVOID) 1);
    g_HUD.GetComboBox(IDC_NLIGHTS)->AddItem(L"3 Lights", (LPVOID) 2);
    g_HUD.GetComboBox(IDC_NLIGHTS)->AddItem(L"4 Lights", (LPVOID) 3);
    g_HUD.GetComboBox(IDC_NLIGHTS)->AddItem(L"5 Lights", (LPVOID) 4);
    g_HUD.GetComboBox(IDC_NLIGHTS)->AddItem(L"6 Lights", (LPVOID) 5);
    //g_HUD.GetComboBox(IDC_NLIGHTS)->SetSelectedByIndex(nLights - 1);

    g_HUD.AddComboBox(IDC_OBJECT, 35, iY += 24, HUD_WIDTH, 22, 0, false);

    g_HUD.AddComboBox(IDC_MESH, 35, iY += 24, HUD_WIDTH, 22, 0, false);

    #ifdef XYZRGB_BUILD
    g_HUD.GetComboBox(IDC_MESH)->AddItem(L"Roberto", (LPVOID) 0);
    g_HUD.GetComboBox(IDC_MESH)->AddItem(L"Apollo", (LPVOID) 1);
    #endif
    g_HUD.GetComboBox(IDC_MESH)->AddItem(L"Teapot", (LPVOID) 2);

    g_HUD.AddComboBox(IDC_MATERIAL, 35, iY += 24, HUD_WIDTH, 22, 0, false);
    g_HUD.GetComboBox(IDC_MATERIAL)->AddItem(L"Skin-4G", (LPVOID) 0);
    g_HUD.GetComboBox(IDC_MATERIAL)->AddItem(L"Skin-6G", (LPVOID) 1);
    g_HUD.GetComboBox(IDC_MATERIAL)->AddItem(L"Marble", (LPVOID) 2);

    g_HUD.AddStatic(IDC_SSSLEVEL_LABEL, L"SSS Level", 35, iY += 24, HUD_WIDTH, 22);
    g_HUD.AddSlider(IDC_SSSLEVEL, 35, iY += 24, HUD_WIDTH, 22);
    g_HUD.AddStatic(IDC_CORRECTION_LABEL, L"Correction", 35, iY += 24, HUD_WIDTH, 22);
    g_HUD.AddSlider(IDC_CORRECTION, 35, iY += 24, HUD_WIDTH, 22);
    g_HUD.AddStatic(IDC_MAXDD_LABEL, L"Max Derivative", 35, iY += 24, HUD_WIDTH, 22);
    g_HUD.AddSlider(IDC_MAXDD, 35, iY += 24, HUD_WIDTH, 22);

    iY += 24;
    g_HUD.AddStatic(IDC_BUMPINESS_LABEL, L"Bumpiness", 35, iY += 24, HUD_WIDTH, 22);
    g_HUD.AddSlider(IDC_BUMPINESS, 35, iY += 24, HUD_WIDTH, 22);
    g_HUD.AddStatic(IDC_ROUGHNESS_LABEL, L"Specular Roughness", 35, iY += 24, HUD_WIDTH, 22);
    g_HUD.AddSlider(IDC_ROUGHNESS, 35, iY += 24, HUD_WIDTH, 22);

    g_HUD.AddCheckBox(IDC_SSS, L"SSS Pass", 35, iY += 24, HUD_WIDTH, 22, true);
    g_HUD.AddCheckBox(IDC_BLOOM, L"Bloom Pass", 35, iY += 24, HUD_WIDTH, 22, true);
    g_HUD.AddCheckBox(IDC_PROFILE, L"Profile", 35, iY += 24, HUD_WIDTH, 22, false);

    g_SampleUI.SetCallback( OnGUIEvent ); iY = 10;
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text.
//--------------------------------------------------------------------------------------
void RenderText()
{
    g_pTxtHelper->Begin();
    g_pTxtHelper->SetInsertionPos( 5, 5 );
    g_pTxtHelper->SetForegroundColor( Colors::Yellow );
    g_pTxtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
    g_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );
    g_pTxtHelper->End();
}


//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output,
                                       const CD3D11EnumDeviceInfo *DeviceInfo,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}



//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext )
{
    HRESULT hr;
	InitDirectXInput();

    auto pd3dImmediateContext = DXUTGetD3D11DeviceContext();
    V_RETURN( g_DialogResourceManager.OnD3D11CreateDevice( pd3dDevice, pd3dImmediateContext ) );
    V_RETURN( g_SettingsDlg.OnD3D11CreateDevice( pd3dDevice ) );
    // TODO - 
    g_pTxtHelper = new CDXUTTextHelper( pd3dDevice, pd3dImmediateContext, &g_DialogResourceManager, 15 );

    // Create other render resources here
    g_States.reset( new CommonStates( pd3dDevice ) );
    WCHAR str[ MAX_PATH ];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"UI\\italic.spritefont" ) );
    g_Font.reset( new SpriteFont( pd3dDevice, str ) );

	leeFaceMaterial = DiffuseNormalMaterial::CreateDiffuseNormalMaterial(pd3dDevice, L"LeeDiffuse.dds", L"LeeNormal.dds");
	robertoFaceMaterial = DiffuseNormalMaterial::CreateDiffuseNormalMaterial(pd3dDevice, L"RobertoDiffuse.dds", L"RobertoNormal.dds");
	lee = Mesh::createFromFile("Lee.vnut", pd3dDevice, pd3dImmediateContext);
	lee->setPosition(XMFLOAT3(4, 0, 0));
	lee->setScale(5.0f);

	roberto = Mesh::createFromFile("Roberto.vnut", pd3dDevice, pd3dImmediateContext);
	roberto->setPosition(XMFLOAT3(-4, 0, 0));
	roberto->setScale(5.0f);
	
	float fAspectRatio = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;
	g_Camera = Camera(33.0f * PI / 180.0f, fAspectRatio, 0.1, 100.0);
	g_Camera.setPosition(MXMFLOAT3(0, -10, 0));
	g_Camera.setEuler(MXMFLOAT3(0, 0, 0));

	cBufferMatrix = new ConstantBuffer<ConstantBufferMatrix>(pd3dDevice, pd3dImmediateContext);
	cBufferLight = new ConstantBuffer<ConstantBufferLight>(pd3dDevice, pd3dImmediateContext);

	g_LightNum = 1;
	g_LightData[0] = DirectionalLight(XMFLOAT3(0, 1, 0), 2, XMFLOAT3(1, 1, 1), 0.5);

    g_HUD.GetButton( IDC_TOGGLEWARP )->SetEnabled( true );

	mainPassVertexShader = CreateVertexShaderFromFile(pd3dDevice, "Shader/MainPass.hlsl", "MainPassVS");
	mainPassPixelShader = CreatePixelShaderFromFile(pd3dDevice, "Shader/MainPass.hlsl", "MainPassPS");

	viewport.TopLeftX = viewport.TopLeftY = 0;
	viewport.Width = 1280;
	viewport.Height = 720;
	viewport.MaxDepth = 1.0;
	viewport.MinDepth = 0.0;

	ID3D11SamplerState* anisotropicWarp = g_States->AnisotropicClamp();
	ID3D11SamplerState* linearWarp = g_States->LinearWrap();
	pd3dImmediateContext->PSSetSamplers(0, 1, &linearWarp);
	pd3dImmediateContext->PSSetSamplers(1, 1, &anisotropicWarp);

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                         const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );
    V_RETURN( g_SettingsDlg.OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;
	g_Camera.setRatio(fAspectRatio);

	viewport.Width = pBackBufferSurfaceDesc->Width;
	viewport.Height = pBackBufferSurfaceDesc->Height;

    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width - 170, 0 );
    g_HUD.SetSize( 170, 170 );
    g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 300 );
    g_SampleUI.SetSize( 170, 300 );

    return S_OK;
}

void DrawMesh(Mesh* pMesh)
{
	if (pMesh == NULL)
		return;

	XMFLOAT4X4 modelMatrix = pMesh->getModelMatrix();
	XMMATRIX m = XMLoadFloat4x4(&modelMatrix);
	XMFLOAT4X4 viewMatrix = g_Camera.getViewMatrix();
	XMMATRIX v = XMLoadFloat4x4(&viewMatrix);
	XMFLOAT4X4 viewProjMatrix = g_Camera.getViewProjectinoMatrix();
	XMMATRIX vp = XMLoadFloat4x4(&viewProjMatrix);

	XMMATRIX mv = m * v;
	XMMATRIX mvp = m * vp;

	XMStoreFloat4x4(&cBufferMatrix->dataCache.g_mModel, m);
	XMStoreFloat4x4(&cBufferMatrix->dataCache.g_mMV, mv);
	XMStoreFloat4x4(&cBufferMatrix->dataCache.g_mMVP, mvp);
	XMStoreFloat4x4(&cBufferMatrix->dataCache.g_mView, v);

	cBufferMatrix->Flush();

	pMesh->drawMesh();
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
                                 float fElapsedTime, void* pUserContext )
{
    // If the settings dialog is being shown, then render it instead of rendering the app's scene
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.OnRender( fElapsedTime );
        return;
    }
	UpdateDirectXInput();
	g_Camera.update(0.0166);
	UpdateLights();

	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &cBufferMatrix->d3dBuffer);
	pd3dImmediateContext->PSSetConstantBuffers(0, 1, &cBufferMatrix->d3dBuffer);
	pd3dImmediateContext->VSSetConstantBuffers(1, 1, &cBufferLight->d3dBuffer);
	pd3dImmediateContext->PSSetConstantBuffers(1, 1, &cBufferLight->d3dBuffer);
	// Update Constant Buffer
	cBufferLight->Flush();

	pd3dImmediateContext->RSSetState(g_States->CullCounterClockwise());
	pd3dImmediateContext->RSSetViewports(1, &viewport);
	pd3dImmediateContext->OMSetDepthStencilState(g_States->DepthDefault(), 0);
	pd3dImmediateContext->OMSetBlendState(g_States->Opaque(),  0, 0xFFFFFFFF);

    auto pRTV = DXUTGetD3D11RenderTargetView();
	pd3dImmediateContext->ClearRenderTargetView( pRTV, Colors::Black );

    // Clear the depth stencil
    auto pDSV = DXUTGetD3D11DepthStencilView();
    pd3dImmediateContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );

	// Main Pass Draw object
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dImmediateContext->IASetInputLayout(mainPassVertexShader->layout);
	pd3dImmediateContext->VSSetShader(mainPassVertexShader->shader, 0, 0);
	pd3dImmediateContext->PSSetShader(mainPassPixelShader, 0, 0);
	leeFaceMaterial->apply(pd3dImmediateContext);
	DrawMesh(lee);
	robertoFaceMaterial->apply(pd3dImmediateContext);
	DrawMesh(roberto);

    // Render HUD
    DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" );
    g_HUD.OnRender( fElapsedTime );
    g_SampleUI.OnRender( fElapsedTime );
    RenderText();
    DXUT_EndPerfEvent();

    static ULONGLONG timefirst = GetTickCount64();
    if ( GetTickCount64() - timefirst > 5000 )
    {    
        OutputDebugString( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
        OutputDebugString( L"\n" );
        timefirst = GetTickCount64();
    }
}

//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D11DestroyDevice();
    g_SettingsDlg.OnD3D11DestroyDevice();
    DXUTGetGlobalResourceCache().OnDestroyDevice();
    SAFE_DELETE( g_pTxtHelper );

    g_States.reset();
    g_Shape.reset();
    g_Model.reset();
    g_Batch.reset();
    g_Sprites.reset();
    g_Font.reset();

	SAFE_DELETE(lee);
	SAFE_DELETE(roberto);
	SAFE_DELETE(cBufferMatrix);
	SAFE_DELETE(cBufferLight);

	SAFE_DELETE(mainPassVertexShader);
	SAFE_RELEASE(mainPassPixelShader);

	SAFE_DELETE(leeFaceMaterial);
	SAFE_DELETE(robertoFaceMaterial);
    // Delete additional render resources here...

	ReleaseDirectXInput();
}

// useless functions
//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
    g_DialogResourceManager.OnD3D11ReleasingSwapChain();
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
    // Update the camera's position based on user input 
   // g_Camera.FrameMove( fElapsedTime );
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext )
{
    // Pass messages to dialog resource manager calls so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass messages to settings dialog if its active
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	if (bKeyDown)
	{
		float speed  = 0.3;
		if (nChar == 'W')
			g_Camera.moveForward(speed);
		else if (nChar == 'S')
			g_Camera.moveBackward(speed);
		else if (nChar == 'A')
			g_Camera.moveLeft(speed);
		else if (nChar == 'D')
			g_Camera.moveRight(speed);
		else if (nChar == 'Q')
			g_Camera.moveUp(speed);
		else if (nChar == 'E')
			g_Camera.moveDown(speed);
	}
}

void CALLBACK OnMouseMove(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                         bool bSideButton1Down,  bool bSideButton2Down,  int nMouseWheelDelta,
                         int xPos,  int yPos,  void* pUserContext )
{
}

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN:
            DXUTToggleFullScreen();
            break;
        case IDC_TOGGLEREF:
            DXUTToggleREF();
            break;
        case IDC_TOGGLEWARP:
            DXUTToggleWARP();
            break;
        case IDC_CHANGEDEVICE:
            g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() );
            break;
		case IDC_BUMPINESS: {
            CDXUTSlider *slider = (CDXUTSlider *) pControl;
            int min, max;
            slider->GetRange(min, max);
            float scale = float(slider->GetValue()) / (max - min);
			cBufferLight->dataCache.g_Bumpiness = scale;
            wstringstream s;
            s << L"Bumpiness: " << scale;
			g_HUD.GetStatic(IDC_BUMPINESS_LABEL)->SetText(s.str().c_str());
            break;
							}
    }
}

void InitDirectXInput()
{
	HINSTANCE hInstance = DXUTGetHINSTANCE();
	HWND hwnd = DXUTGetHWND();
	DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL);
	directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	keyboard->SetDataFormat(&c_dfDIKeyboard);
	keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	//keyboard->Acquire();
	memset(&keyboardState[0], 0x00, sizeof(keyboardState));

	directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	mouse->SetDataFormat(&c_dfDIMouse);
	mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	mouse->Acquire(); 
}

void ReleaseDirectXInput()
{
	if(mouse)
	{
		mouse->Unacquire();
		mouse->Release();
		mouse = 0;
	}
	if(keyboard)
	{
		keyboard->Unacquire();
		keyboard->Release();
		keyboard = 0;
	}
	if(directInput)
	{
		directInput->Release();
		directInput = 0;
	}
}

void UpdateDirectXInput()
{
	HRESULT hr;
	hr = mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseState);
	if(FAILED(hr) && ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)))
		mouse->Acquire();

	//hr = keyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
	//if(FAILED(hr) && ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)))
		//keyboard->Acquire();

	if (mouseState.rgbButtons[2])
	{
		float sensitivity = 0.1;
		g_Camera.yaw(-mouseState.lX * PI / 180.0 * sensitivity);
		g_Camera.pitch(-mouseState.lY * PI / 180.0 * sensitivity);
	}
}

void UpdateLights()
{
	cBufferLight->dataCache.g_LightNum = g_LightNum;
	XMFLOAT4X4 viewMatrix = g_Camera.getViewMatrix();
	for (int i = 0; i < g_LightNum; i++)
	{
		XMVECTOR viewLightDir = XMVector3TransformNormal(XMLoadFloat3(&(g_LightData[i].direction)), XMLoadFloat4x4(&viewMatrix)) *-1;
		XMStoreFloat3(&cBufferLight->dataCache.g_LightData[i].direction, viewLightDir);
		cBufferLight->dataCache.g_LightData[i].intensity = g_LightData[i].intensity;
		cBufferLight->dataCache.g_LightData[i].color = g_LightData[i].color;
		cBufferLight->dataCache.g_LightData[i].specIntensity = g_LightData[i].specIntensity;
	}
}
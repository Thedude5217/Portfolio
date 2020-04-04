// D3D11Sample.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "D3D11Sample.h"

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")

#include <DirectXMath.h>
using namespace DirectX;

#include "MyVShader.csh"
#include "MyPShader.csh"

#include "MyGVShader.csh"
#include "MyGPShader.csh"

#include "MyMeshVShader.csh" //don't add a .csh to your project!
#include "MyMeshPShader.csh"

#include "MySkyBoxVShader.csh"
#include "MySkyBoxPShader.csh"

#include "MyGeoShader.csh"
#include "MyGeoVShader.csh"
#include "MyGeoPShader.csh"

#include "Assets/StoneHenge.h"
#include "Assets/StoneHenge_Texture.h"

#include "DDSTextureLoader.h"

#include "XTime.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;


ID3D11Device* myDev;
IDXGISwapChain* mySwap;
ID3D11DeviceContext* myCon;

//for drawing
ID3D11RenderTargetView* myRtv;
CD3D11_VIEWPORT myPort;

float aspectRatio = 1;

XTime mytime;

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 Tex;
};

struct SimpleMesh
{
	vector<Vertex> vertexList;
	vector<int> indicesList;
};

SimpleMesh simpleMesh;

SimpleMesh skyBox;

struct MyVertex
{
	XMFLOAT4 xyzw;
	XMFLOAT4 rgba;
};

struct MultDraw
{
	MyVertex Info[12];
};

bool dldir = false;
bool pldpos = false;
bool slvars = false;

int xSize = 50;
int zSize = 50;

int decrement = 1;
int gridDraw = 1;

//Pyramid variables
unsigned int numVerts = 0;
ID3D11Buffer* vBuff;
ID3D11InputLayout* vLayout;
ID3D11InputLayout* gvLayout;
ID3D11VertexShader* vShader; //HLSL
ID3D11PixelShader* pShader; //HLSL

//Grass variables
bool grass = false;
unsigned int numGrass = 0;
ID3D11InputLayout* geovLayout;
ID3D11Buffer* geovBuff;
ID3D11GeometryShader* geoShader; //HLSL
ID3D11VertexShader* geovShader;
ID3D11PixelShader* geopShader;
ID3D11Buffer* geocBuff;

//Grid variables
unsigned int vertNum = 0;
ID3D11Buffer* gvBuff;
ID3D11VertexShader* gvShader;
ID3D11PixelShader* gpShader;

ID3D11Buffer* cBuff;//shader vars
ID3D11Buffer* gcBuff;

//mesh data
ID3D11Buffer* vBuffMesh; //vertex buffer
ID3D11Buffer* iBuffMesh; //index buffer
ID3D11Buffer* TvBuffMesh; //vertex buffer
ID3D11Buffer* TiBuffMesh; //index buffer
ID3D11Buffer* skyvBuffMesh; //vertex buffer
ID3D11Buffer* skyiBuffMesh; //index buffer

//mesh vertex shader
ID3D11VertexShader* vMeshShader; //HLSL
ID3D11InputLayout* vMeshLayout;

ID3D11PixelShader* pMeshShader; //HLSL

ID3D11VertexShader* skyvMeshShader; //HLSL
ID3D11InputLayout* skyvMeshLayout;
ID3D11PixelShader* skypMeshShader; //HLSL
//ID3D11InputLayout* pMeshLayout;

ID3D11SamplerState* sample;

//Z buffer for depth sorting
ID3D11Texture2D* zBuffer;
ID3D11DepthStencilView* zBufferView;

//Texture variable
//ID3D11Texture2D* diffuseTexture;
ID3D11Resource* shTexture;
ID3D11ShaderResourceView* SRV;

ID3D11Resource* skyhTexture;
ID3D11ShaderResourceView* SkyRV;

//Math stuff
struct WVP
{
	XMFLOAT4X4 wMatrix; //storage type
	XMFLOAT4X4 vMatrix;
	XMFLOAT4X4 pMatrix;
	XMFLOAT4 Time;
}MyMatricies;

struct Matricies
{
	XMFLOAT4X4 wMatrix[2]; //storage type
	XMFLOAT4X4 vMatrix;
	XMFLOAT4X4 pMatrix;
}matricies;

struct InstanceData
{
	MultDraw pyramids[2];
	unsigned int size;
}instanceData;

ID3D11Buffer* IcBuff;

struct Lights
{
	XMFLOAT4 dlColor;
	XMFLOAT4 dlightDir;

	XMFLOAT4 plColor;
	XMFLOAT4 plightPos;

	XMFLOAT4 slColor;
	XMFLOAT4 slightPos;
	XMFLOAT4 coneDir;
}LightVars;

ID3D11Buffer* LightcBuff;


XMMATRIX view = XMMatrixIdentity();
XMMATRIX Camera = XMMatrixIdentity();

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_D3D11SAMPLE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_D3D11SAMPLE));

	MSG msg;

	// Main message loop:
	while (true)/*GetMessage(&msg, nullptr, 0, 0)*/
	{
		if (GetAsyncKeyState(VK_ADD) && grass == false)
		{
			grass = true;
		}
		else if (GetAsyncKeyState(VK_SUBTRACT) && grass == true)
		{
			grass = false;
		}
		PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
			break;

		//XMStoreFloat4x4(&MyMatricies.vMatrix, view);

		// render here
		float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		myCon->ClearRenderTargetView(myRtv, color);

		myCon->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1, 0);

		//Setup the pipline

		//output merger
		ID3D11RenderTargetView* tempRTV[] = { myRtv };
		myCon->OMSetRenderTargets(1, tempRTV, zBufferView);
		//rasterizer
		myCon->RSSetViewports(1, &myPort);
		//Input Assembler
		myCon->IASetInputLayout(vLayout);

		//Draw pyramid
		UINT strides[] = { sizeof(MyVertex) };
		UINT offsets[] = { 0 };
		ID3D11Buffer* tempVB[] = { vBuff };
		myCon->IASetVertexBuffers(0, 1, tempVB, strides, offsets);
		myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//Vertex shader stage
		myCon->VSSetShader(vShader, 0, 0);
		//Pixel shader stage
		myCon->PSSetShader(pShader, 0, 0);

		//Try and make your triangle 3D

			//make into a Pyramid(more verts)

			//make a world view & projection matrix
		static float rot = 0; rot += 0.1f;
		XMMATRIX temp = XMMatrixIdentity();
		temp = XMMatrixTranslation(3, 2, -5);
		XMMATRIX temp2 = XMMatrixRotationY(rot);
		temp = XMMatrixMultiply(temp2, temp);
		//temp = XMMatrixMultiply(temp, temp2);
		XMStoreFloat4x4(&matricies.wMatrix[0], temp);

		temp = XMMatrixIdentity();
		temp = XMMatrixTranslation(-3, 2, -5);
		temp = XMMatrixMultiply(temp2, temp);
		XMStoreFloat4x4(&matricies.wMatrix[1], temp);
		//view

		if (decrement == 1)
		{
			temp = XMMatrixLookAtLH({ 1, 5, -10 }, { 0,0,0 }, { 0,1,0 });
			XMStoreFloat4x4(&MyMatricies.vMatrix, temp);
			XMStoreFloat4x4(&matricies.vMatrix, temp);
			decrement -= 1;
			view = temp;
		}

		//projection
		temp = XMMatrixPerspectiveFovLH(3.14f / 2.0f, aspectRatio, 0.1f, 1000);
		XMStoreFloat4x4(&MyMatricies.pMatrix, temp);

		temp = XMMatrixPerspectiveFovLH(3.14f / 2.0f, aspectRatio, 0.1f, 1000);
		XMStoreFloat4x4(&matricies.pMatrix, temp);
		//Upload those matricies to the video card
			// Create and update a constant buffer (more variables from c++ to shaders)
		D3D11_MAPPED_SUBRESOURCE gpuBuffer;
		HRESULT hr = myCon->Map(IcBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		*((Matricies*)(gpuBuffer.pData)) = matricies;
		//memcpy(gpuBuffer.pdata, &MyMatricies, sizeof(WVP));
		myCon->Unmap(IcBuff, 0);


		//Apply matrix math in Vertex Shader
		// connect constant buffer to pipline
		//remember by default HLSL matricies are COLUMN MAJOR
		ID3D11Buffer* constants[] = { IcBuff };
		myCon->VSSetConstantBuffers(0, 1, constants);

		myCon->DrawInstanced(numVerts, 2, 0, 0);

		//Draw Grid
		XMMATRIX gtemp = XMMatrixIdentity();
		XMStoreFloat4x4(&MyMatricies.wMatrix, gtemp);


		UINT gstrides[] = { sizeof(MyVertex) };
		UINT goffsets[] = { 0 };
		ID3D11Buffer* gtempVB[] = { gvBuff };
		myCon->IASetVertexBuffers(0, 1, gtempVB, gstrides, goffsets);
		//myCon->IASetIndexBuffer(iBuffMesh, DXGI_FORMAT_R32_UINT, 0);
		myCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		myCon->IASetInputLayout(gvLayout);
		//Vertex shader stage
		myCon->VSSetShader(gvShader, 0, 0);
		//Pixel shader stage
		myCon->PSSetShader(gpShader, 0, 0);

		mytime.Signal();
		MyMatricies.Time.x = mytime.Delta();
		MyMatricies.Time.y = mytime.TotalTime();

		gtemp = XMMatrixIdentity();
		XMStoreFloat4x4(&MyMatricies.wMatrix, gtemp);
		//send it to the card
		hr = myCon->Map(gcBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		*((WVP*)(gpuBuffer.pData)) = MyMatricies;
		myCon->Unmap(gcBuff, 0);

		ID3D11Buffer* gconstants[] = { gcBuff };
		myCon->VSSetConstantBuffers(0, 1, gconstants);


		myCon->Draw(vertNum, 0);


		//Draw Grass
		XMMATRIX geotemp = XMMatrixIdentity();
		XMStoreFloat4x4(&MyMatricies.wMatrix, geotemp);


		UINT geostrides[] = { sizeof(MyVertex) };
		UINT geooffsets[] = { 0 };
		ID3D11Buffer* geotempVB[] = { geovBuff };
		myCon->IASetVertexBuffers(0, 1, geotempVB, geostrides, geooffsets);
		myCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		myCon->IASetInputLayout(geovLayout);
		//Vertex shader stage
		myCon->VSSetShader(geovShader, 0, 0);
		//Pixel shader stage
		myCon->PSSetShader(geopShader, 0, 0);
		if (grass == true)
		{
			myCon->GSSetShader(geoShader, 0, 0);
		}

		geotemp = XMMatrixIdentity();
		XMStoreFloat4x4(&MyMatricies.wMatrix, geotemp);
		//send it to the card
		hr = myCon->Map(geocBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		*((WVP*)(gpuBuffer.pData)) = MyMatricies;
		myCon->Unmap(geocBuff, 0);

		ID3D11Buffer* geoconstants[] = { geocBuff };
		myCon->VSSetConstantBuffers(0, 1, geoconstants);
		myCon->GSSetConstantBuffers(0, 1, geoconstants);

		myCon->Draw(numGrass, 0);

		myCon->GSSetShader(NULL, 0, 0);



		//immediate context
		myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//get a more complex pre_made mesh (FBX, OBJ, custom header)
		//load onto card (vertex buffer, index buffer)
		//makes sure our shaders can process it
		//place it somewhere in the enviroment

		//Draw SkyBox
		ID3D11ShaderResourceView* skyTexViews[] = { SkyRV };

		UINT sky_mesh_strides[] = { sizeof(skyBox.vertexList) };
		UINT sky_mesh_offsets[] = { 0 };
		ID3D11Buffer* sky_meshVB[] = { skyvBuffMesh };
		myCon->IASetVertexBuffers(0, 1, sky_meshVB, sky_mesh_strides, sky_mesh_offsets);
		myCon->IASetIndexBuffer(skyiBuffMesh, DXGI_FORMAT_R32_UINT, 0);
		myCon->VSSetShader(skyvMeshShader, 0, 0);
		myCon->IASetInputLayout(skyvMeshLayout);
		myCon->PSSetShaderResources(0, 1, skyTexViews);
		myCon->PSSetShader(skypMeshShader, 0, 0);

		hr = myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		*((WVP*)(gpuBuffer.pData)) = MyMatricies;
		myCon->Unmap(cBuff, 0);

		myCon->DrawIndexed(skyBox.indicesList.size(), 0, 0);

		//Draw StoneHenge
		ID3D11ShaderResourceView* texViews[] = { SRV };

		CD3D11_BUFFER_DESC LDesc;
		D3D11_SUBRESOURCE_DATA LsubData;
		ZeroMemory(&LDesc, sizeof(LDesc));
		ZeroMemory(&LsubData, sizeof(LsubData));

		LDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		LDesc.ByteWidth = sizeof(Lights);
		LDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		LDesc.MiscFlags = 0;
		LDesc.StructureByteStride = 0;
		LDesc.Usage = D3D11_USAGE_DYNAMIC;

		LsubData.pSysMem = &LightVars;
		LsubData.SysMemPitch = 0;
		LsubData.SysMemSlicePitch = 0;

		hr = myDev->CreateBuffer(&LDesc, &LsubData, &LightcBuff);

		//set pipeline
		UINT mesh_strides[] = { sizeof(_OBJ_VERT_) };
		UINT mesh_offsets[] = { 0 };
		ID3D11Buffer* meshVB[] = { vBuffMesh };
		myCon->IASetVertexBuffers(0, 1, meshVB, mesh_strides, mesh_offsets);
		myCon->IASetIndexBuffer(iBuffMesh, DXGI_FORMAT_R32_UINT, 0);
		myCon->VSSetShader(vMeshShader, 0, 0);
		myCon->IASetInputLayout(vMeshLayout);
		myCon->PSSetShaderResources(0, 1, texViews);
		myCon->PSSetShader(pMeshShader, 0, 0);

		////modify world matrix before drawing next thing

		temp = XMMatrixIdentity();
		XMStoreFloat4x4(&MyMatricies.wMatrix, temp);
		//send it to the card
		hr = myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		*((WVP*)(gpuBuffer.pData)) = MyMatricies;
		myCon->Unmap(cBuff, 0);

		ID3D11Buffer* Sconstants[] = { LightcBuff };
		myCon->PSSetConstantBuffers(0, 1, Sconstants);

		////draw it
		myCon->DrawIndexed(2532, 0, 0);

		LightcBuff->Release();

		//Directional light movement
		if (LightVars.dlightDir.x <= 10 && dldir == false && LightVars.dlightDir.x >= -10)
		{
			LightVars.dlightDir.x -= 0.5f;
			LightVars.dlightDir.z -= 0.5f;
			if (LightVars.dlightDir.x == -10)
			{
				dldir = true;
			}
		}
		else if (LightVars.dlightDir.x <= 10 && dldir == true && LightVars.dlightDir.x >= -10)
		{
			LightVars.dlightDir.x += 0.5f;
			LightVars.dlightDir.z += 0.5f;
			if (LightVars.dlightDir.x == 10)
			{
				dldir = false;
			}
		}

		//Point light movement
		if (LightVars.plightPos.x <= 12 && pldpos == false && LightVars.plightPos.x >= -12)
		{
			LightVars.plightPos.x -= 0.5f;
			if (LightVars.plightPos.x == -12)
			{
				pldpos = true;
			}
		}
		else if (LightVars.plightPos.x <= 12 && pldpos == true && LightVars.plightPos.x >= -12)
		{
			LightVars.plightPos.x += 0.5f;
			if (LightVars.plightPos.x == 12)
			{
				pldpos = false;
			}
		}

		//Spot light movement
		if (LightVars.slightPos.z <= 12 && slvars == false && LightVars.slightPos.z >= -12)
		{
			LightVars.slightPos.z -= 0.5f;
			LightVars.coneDir.z -= 0.01f;
			LightVars.coneDir.x -= 0.01f;
			if (LightVars.slightPos.z == -12)
			{
				slvars = true;
			}
		}
		else if (LightVars.slightPos.z <= 12 && slvars == true && LightVars.slightPos.z >= -12)
		{
			LightVars.slightPos.z += 0.5f;
			LightVars.coneDir.z += 0.01f;
			LightVars.coneDir.x += 0.01f;
			if (LightVars.slightPos.z == 12)
			{
				slvars = false;
			}
		}



		//Draw 2
		UINT Tmesh_strides[] = { sizeof(simpleMesh.vertexList) };
		UINT Tmesh_offsets[] = { 0 };
		ID3D11Buffer* TmeshVB[] = { TvBuffMesh };
		myCon->IASetVertexBuffers(0, 1, TmeshVB, Tmesh_strides, Tmesh_offsets);
		myCon->IASetIndexBuffer(TiBuffMesh, DXGI_FORMAT_R32_UINT, 0);
		myCon->VSSetShader(vMeshShader, 0, 0);
		myCon->IASetInputLayout(vMeshLayout);

		temp = XMMatrixIdentity();
		temp = XMMatrixTranslation(1, 7, 0);
		temp2 = XMMatrixRotationY(rot);
		temp = XMMatrixMultiply(temp2, temp);
		XMStoreFloat4x4(&MyMatricies.wMatrix, temp);
		//send it to the card
		hr = myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		*((WVP*)(gpuBuffer.pData)) = MyMatricies;
		myCon->Unmap(cBuff, 0);

		//draw it
		myCon->DrawIndexed(simpleMesh.indicesList.size(), 0, 0);

		//Camera controls
		XMMATRIX cRotation = XMMatrixIdentity();
		XMMATRIX cMove = XMMatrixIdentity();

		if (GetAsyncKeyState('S'))
		{
			cMove = XMMatrixMultiply(XMMatrixTranslation(0, 0, -1), cMove);
		}

		if (GetAsyncKeyState('W'))
		{
			cMove = XMMatrixMultiply(XMMatrixTranslation(0, 0, 1), cMove);
		}

		if (GetAsyncKeyState('D'))
		{
			cMove = XMMatrixMultiply(XMMatrixTranslation(1, 0, 0), cMove);
		}

		if (GetAsyncKeyState('A'))
		{
			cMove = XMMatrixMultiply(XMMatrixTranslation(-1, 0, 0), cMove);
		}

		if (GetAsyncKeyState(VK_SPACE))
		{
			cMove = XMMatrixMultiply(XMMatrixTranslation(0, 1, 0), cMove);
		}

		if (GetAsyncKeyState(VK_SHIFT))
		{
			cMove = XMMatrixMultiply(XMMatrixTranslation(0, -1, 0), cMove);
		}

		if (GetAsyncKeyState(VK_LEFT))
		{
			cRotation = XMMatrixMultiply(XMMatrixRotationY(-.1), cRotation);
		}

		if (GetAsyncKeyState(VK_RIGHT))
		{
			cRotation = XMMatrixMultiply(XMMatrixRotationY(.1), cRotation);
		}

		if (GetAsyncKeyState(VK_UP))
		{
			cMove = XMMatrixMultiply(XMMatrixRotationX(-.1), cMove);
		}

		if (GetAsyncKeyState(VK_DOWN))
		{
			cMove = XMMatrixMultiply(XMMatrixRotationX(.1), cMove);
		}

		Camera = XMMatrixMultiply(cMove, Camera);

		XMVECTOR vec = Camera.r[3];
		Camera.r[3] = { 0, 0, 0, 1 };
		Camera = XMMatrixMultiply(Camera, cRotation);
		Camera.r[3] = vec;

		view = XMMatrixInverse(&XMMatrixDeterminant(Camera), Camera);
		XMStoreFloat4x4(&MyMatricies.vMatrix, view);
		XMStoreFloat4x4(&matricies.vMatrix, view);


		mySwap->Present(1, 0);
	}

	// release all our D3D11 interfaces
	myRtv->Release();
	vBuff->Release();
	myCon->Release();
	mySwap->Release();
	vShader->Release();
	pShader->Release();
	vLayout->Release();
	myDev->Release();
	gvLayout->Release();
	geovLayout->Release();
	geovBuff->Release();
	geoShader->Release();
	geovShader->Release();
	geopShader->Release();
	gvBuff->Release();
	gvShader->Release();
	gpShader->Release();
	cBuff->Release();
	gcBuff->Release();
	vBuffMesh->Release();
	iBuffMesh->Release();
	TvBuffMesh->Release();
	TiBuffMesh->Release();
	vMeshShader->Release();
	vMeshLayout->Release();
	pMeshShader->Release();
	geocBuff->Release();
	IcBuff->Release();

	return (int)msg.wParam;
}





//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_D3D11SAMPLE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_D3D11SAMPLE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

void LoadMesh(const char* meshFileName, SimpleMesh& mesh)
{
	std::fstream file{ meshFileName, std::ios_base::in | std::ios_base::binary };

	assert(file.is_open());

	uint32_t player_index_count;
	file.read((char*)& player_index_count, sizeof(uint32_t));

	mesh.indicesList.resize(player_index_count);

	file.read((char*)mesh.indicesList.data(), sizeof(uint32_t) * player_index_count);

	uint32_t player_vertex_count;
	file.read((char*)& player_vertex_count, sizeof(uint32_t));

	mesh.vertexList.resize(player_vertex_count);

	file.read((char*)mesh.vertexList.data(), sizeof(Vertex) * player_vertex_count);

	//example mesh conditioning if needed - this flips handedness
	for (auto& v : mesh.vertexList)
	{
		v.pos.x = -v.pos.x;
		v.normal.x = -v.normal.x;
		v.Tex.y = 1.0f - v.Tex.y;
	}

	int tri_count = (int)(mesh.indicesList.size() / 3);

	for (int i = 0; i < tri_count; i++)
	{
		int* tri = mesh.indicesList.data() + (i * 3);

		int temp = tri[0];
		tri[0] = tri[2];
		tri[2] = temp;
	}
	file.close();
}


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	RECT myWinR;
	GetClientRect(hWnd, &myWinR);

	// attach d3d11 to our window
	D3D_FEATURE_LEVEL dx11 = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC swap;
	ZeroMemory(&swap, sizeof(DXGI_SWAP_CHAIN_DESC));
	swap.BufferCount = 1;
	swap.OutputWindow = hWnd;
	swap.Windowed = true;
	swap.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap.BufferDesc.Width = myWinR.right - myWinR.left;
	swap.BufferDesc.Height = myWinR.bottom - myWinR.top;
	swap.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap.SampleDesc.Count = 1;

	aspectRatio = swap.BufferDesc.Width / float(swap.BufferDesc.Height);

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG,
		&dx11, 1, D3D11_SDK_VERSION, &swap, &mySwap, &myDev, 0, &myCon);

	ID3D11Resource* backbuffer;
	hr = mySwap->GetBuffer(0, __uuidof(backbuffer), (void**)& backbuffer);
	myDev->CreateRenderTargetView(backbuffer, NULL, &myRtv);

	backbuffer->Release();

	myPort.Width = swap.BufferDesc.Width;
	myPort.Height = swap.BufferDesc.Height;
	myPort.TopLeftX = myPort.TopLeftY = 0;
	myPort.MinDepth = 0;
	myPort.MaxDepth = 1;

	MyVertex tri[] = //NDC Normalized Device Coordinates
	{// xyzw, rgba
		//Front
		{ {0, 1.0f, 0, 1}, {1,1,1,1} },
		{ {0.25f , -0.25f, -0.25f, 1}, {1,0,1,1} },
		{ {-0.25f, -0.25f, -0.25f, 1}, {1,1,0,1} },
		//Right side
		{ {0, 1.0f, 0, 1}, {1,1,1,1} },
		{ {0.25f , -0.25f, 0.25f, 1}, {1,0,1,1} },
		{ {0.25f, -0.25f, -0.25f, 1}, {1,1,0,1} },
		//back side
		{ {0, 1.0f, 0, 1}, {1,1,1,1} },
		{ {-0.25f , -0.25f, 0.25f, 1}, {1,0,1,1} },
		{ {0.25f, -0.25f, 0.25f, 1}, {1,1,0,1} },
		//Left side
		{ {0, 1.0f, 0, 1}, {1,1,1,1} },
		{ {-0.25f , -0.25f, -0.25f, 1}, {1,0,1,1} },
		{ {-0.25f, -0.25f, 0.25f, 1}, {1,1,0,1} }
	};
	numVerts = ARRAYSIZE(tri);

	instanceData.size = ARRAYSIZE(instanceData.pyramids[2].Info);

	for (int i = 0; i < instanceData.size; i++)
	{
		instanceData.pyramids[0].Info[i] = tri[i];
		instanceData.pyramids[1].Info[i] = tri[i];
	}

	MyVertex Grass[] =
	{
		//N
		{{-13, 0, -4, 1}, {1, 1, 1, 1}},
		{{-13, 0, -2, 1}, {1, 1, 1, 1}},
		{{-13, 0, 0, 1}, {1, 1, 1, 1}},
		{{-13, 0, 2, 1}, {1, 1, 1, 1}},
		{{-13, 0, 4, 1}, {1, 1, 1, 1}},

		{{-11.5, 0, 2.5f, 1}, {1, 1, 1, 1}},
		{{-9.5, 0, -0.5f, 1}, {1, 1, 1, 1}},
		{{-7.5, 0, -3.5, 1}, {1, 1, 1, 1}},

		{{-6, 0, -4, 1}, {1, 1, 1, 1}},
		{{-6, 0, -2, 1}, {1, 1, 1, 1}},
		{{-6, 0, 0, 1}, {1, 1, 1, 1}},
		{{-6, 0, 2, 1}, {1, 1, 1, 1}},
		{{-6, 0, 4, 1}, {1, 1, 1, 1}},

		//J
		{{6, 0, 4, 1}, {1, 1, 1, 1}},
		{{7.5f, 0, 4, 1}, {1, 1, 1, 1}},
		{{9, 0, 4, 1}, {1, 1, 1, 1}},
		{{10.5f, 0, 4, 1}, {1, 1, 1, 1}},
		{{12, 0, 4, 1}, {1, 1, 1, 1}},
		{{13.5, 0, 4, 1}, {1, 1, 1, 1}},
		{{15, 0, 4, 1}, {1, 1, 1, 1}},

		{{10.5f, 0, 2, 1}, {1, 1, 1, 1}},
		{{10.5f, 0, 0, 1}, {1, 1, 1, 1}},
		{{10.5f, 0, -2, 1}, {1, 1, 1, 1}},
		{{10.5f, 0, -3.6, 1}, {1, 1, 1, 1}},
		{{9, 0, -3.9f, 1}, {1, 1, 1, 1}},
		{{8, 0, -3.5f, 1}, {1, 1, 1, 1}},
		{{7.5, 0, -2, 1}, {1, 1, 1, 1}},
	};
	numGrass = ARRAYSIZE(Grass);

	MyVertex Grid[100];
	{
		int i = 0;
		for (float j = 0; j < 50; j++)
		{
			Grid[i].xyzw = { -25 + j, 20, -24, 1 };
			if (i % 2 == 0)
			{
				Grid[i].xyzw.z += 48;
				Grid[i].xyzw.x += 1;
			}
			Grid[i].rgba = { 0, 1, 0, 1 };
			i++;
		}

		i = 50;
		for (float j = 0; j < 50; j++)
		{
			Grid[i].xyzw = { -24, 20, -24 + j - 1, 1 };
			if (i % 2 == 0)
			{
				Grid[i].xyzw.x += 48;
				Grid[i].xyzw.z += 1;
			}
			Grid[i].rgba = { 1, 0, 0, 1 };
			i++;
		}


		int triangles[50 * 50 * 6];

		int vert = 0;
		int tris = 0;

		for (float z = 0; z < zSize; z++)
		{
			for (float x = 0; x < xSize; x++)
			{
				triangles[tris + 0] = vert + 0;
				triangles[tris + 1] = vert + xSize + 1;
				triangles[tris + 2] = vert + 1;
				triangles[tris + 3] = vert + 1;
				triangles[tris + 4] = vert + xSize + 1;
				triangles[tris + 5] = vert + xSize + 2;

				vert++;
				tris += 6;
			}
			vert++;
		}
	}
	vertNum = ARRAYSIZE(Grid);

	//Load it on the card
	CD3D11_BUFFER_DESC bDesc;
	D3D11_SUBRESOURCE_DATA subData;
	ZeroMemory(&bDesc, sizeof(bDesc));
	ZeroMemory(&subData, sizeof(subData));

	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(MyVertex) * numVerts;
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;

	subData.pSysMem = tri;

	hr = myDev->CreateBuffer(&bDesc, &subData, &vBuff);

	CD3D11_BUFFER_DESC nDesc;
	D3D11_SUBRESOURCE_DATA subRData;
	ZeroMemory(&nDesc, sizeof(nDesc));
	ZeroMemory(&subRData, sizeof(subRData));

	nDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	nDesc.ByteWidth = sizeof(MyVertex) * vertNum;
	nDesc.CPUAccessFlags = 0;
	nDesc.MiscFlags = 0;
	nDesc.StructureByteStride = sizeof(MyVertex);
	nDesc.Usage = D3D11_USAGE_IMMUTABLE;

	subRData.pSysMem = Grid;

	hr = myDev->CreateBuffer(&nDesc, &subRData, &gvBuff);

	CD3D11_BUFFER_DESC geoDesc;
	D3D11_SUBRESOURCE_DATA geosubData;
	ZeroMemory(&geoDesc, sizeof(geoDesc));
	ZeroMemory(&geosubData, sizeof(geosubData));

	geoDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	geoDesc.ByteWidth = sizeof(MyVertex) * numGrass;
	geoDesc.CPUAccessFlags = 0;
	geoDesc.MiscFlags = 0;
	geoDesc.StructureByteStride = 0;
	geoDesc.Usage = D3D11_USAGE_IMMUTABLE;

	geosubData.pSysMem = Grass;

	hr = myDev->CreateBuffer(&geoDesc, &geosubData, &geovBuff);

	//Write and compile and load our shaders

	hr = myDev->CreateVertexShader(MyVShader, sizeof(MyVShader), nullptr, &vShader);
	hr = myDev->CreatePixelShader(MyPShader, sizeof(MyPShader), nullptr, &pShader);

	hr = myDev->CreateVertexShader(MyGVShader, sizeof(MyGVShader), nullptr, &gvShader);
	hr = myDev->CreatePixelShader(MyGPShader, sizeof(MyGPShader), nullptr, &gpShader);

	hr = myDev->CreateVertexShader(MyGeoVShader, sizeof(MyGeoVShader), nullptr, &geovShader);
	hr = myDev->CreatePixelShader(MyGeoPShader, sizeof(MyGeoPShader), nullptr, &geopShader);
	hr = myDev->CreateGeometryShader(MyGeoShader, sizeof(MyGeoShader), nullptr, &geoShader);
	/*hr = myDev->CreateGeometryShader();*/

	//Describe the vertexit to D3D11
	D3D11_INPUT_ELEMENT_DESC ieDesc[] =
	{
		 {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		 {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = myDev->CreateInputLayout(ieDesc, 2, MyVShader, sizeof(MyVShader), &vLayout);
	hr = myDev->CreateInputLayout(ieDesc, 2, MyGVShader, sizeof(MyGVShader), &gvLayout);
	hr = myDev->CreateInputLayout(ieDesc, 2, MyGeoVShader, sizeof(MyGeoVShader), &geovLayout);

	//create constant buffer
	//Pyramid constant buffer
	ZeroMemory(&bDesc, sizeof(bDesc));

	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.ByteWidth = sizeof(WVP);
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_DYNAMIC;


	hr = myDev->CreateBuffer(&bDesc, &subData, &cBuff);

	ZeroMemory(&bDesc, sizeof(bDesc));

	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.ByteWidth = sizeof(InstanceData) * instanceData.size;
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_DYNAMIC;


	hr = myDev->CreateBuffer(&bDesc, &subData, &IcBuff);

	//Grid constatant buffer
	ZeroMemory(&nDesc, sizeof(subRData));

	nDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	nDesc.ByteWidth = sizeof(WVP);
	nDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	nDesc.MiscFlags = 0;
	nDesc.StructureByteStride = 0;
	nDesc.Usage = D3D11_USAGE_DYNAMIC;

	hr = myDev->CreateBuffer(&nDesc, &subRData, &gcBuff);

	//Grass constant buffer
	ZeroMemory(&geoDesc, sizeof(geosubData));

	geoDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	geoDesc.ByteWidth = sizeof(WVP);
	geoDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	geoDesc.MiscFlags = 0;
	geoDesc.StructureByteStride = 0;
	geoDesc.Usage = D3D11_USAGE_DYNAMIC;

	hr = myDev->CreateBuffer(&geoDesc, &geosubData, &geocBuff);

	//Light buffer
	CD3D11_BUFFER_DESC LDesc;
	D3D11_SUBRESOURCE_DATA LsubData;
	ZeroMemory(&LDesc, sizeof(LDesc));
	ZeroMemory(&LsubData, sizeof(LsubData));


	LightVars.dlColor = { 1.0f, 0.0f, 0.0f, 1.0f };
	LightVars.dlightDir = { 5, 1, -5, 0 };
	LightVars.plColor = { 0.0f, 0.0f, 1.0f, 1.0f };
	LightVars.plightPos = { 10, 4, -10, 1 };
	LightVars.slColor = { 0.0f, 1.0f, 0.0f, 1.0f };
	LightVars.slightPos = { 0, 7, 0, 1 };
	LightVars.coneDir = { -.2, -.5, -.3, 0 };

	LDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	LDesc.ByteWidth = sizeof(Lights);
	LDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	LDesc.MiscFlags = 0;
	LDesc.StructureByteStride = 0;
	LDesc.Usage = D3D11_USAGE_DYNAMIC;

	LsubData.pSysMem = &LightVars;
	LsubData.SysMemPitch = 0;
	LsubData.SysMemSlicePitch = 0;

	hr = myDev->CreateBuffer(&LDesc, &LsubData, &LightcBuff);

	//load our complex mesh onto the card
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(StoneHenge_data);
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;

	subData.pSysMem = StoneHenge_data;
	hr = myDev->CreateBuffer(&bDesc, &subData, &vBuffMesh); //vertex buffer
	//index buffer mesh
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(StoneHenge_indicies);
	subData.pSysMem = StoneHenge_indicies;
	hr = myDev->CreateBuffer(&bDesc, &subData, &iBuffMesh);

	LoadMesh("Assets/Plaid5.mesh", simpleMesh);

	// Create and initialize the vertex buffer
	{
		CD3D11_BUFFER_DESC bufferDesc(sizeof(Vertex) * simpleMesh.vertexList.size(), D3D11_BIND_VERTEX_BUFFER);
		bufferDesc.StructureByteStride = sizeof(Vertex);

		D3D11_SUBRESOURCE_DATA initData = { simpleMesh.vertexList.data(), 0, 0 };
		hr = myDev->CreateBuffer(&bufferDesc, &initData, &TvBuffMesh);
	}

	// Create and initialize the index buffer
	{
		CD3D11_BUFFER_DESC bufferDesc(sizeof(uint32_t) * simpleMesh.indicesList.size(), D3D11_BIND_INDEX_BUFFER);
		bufferDesc.StructureByteStride = sizeof(uint32_t);

		D3D11_SUBRESOURCE_DATA initData = { simpleMesh.indicesList.data(), 0, 0 };
		hr = myDev->CreateBuffer(&bufferDesc, &initData, &TiBuffMesh);
	}

	LoadMesh("Assets/SkyBox.mesh", skyBox);

	// Create and initialize the vertex buffer
	{
		CD3D11_BUFFER_DESC bufferDesc(sizeof(Vertex) * skyBox.vertexList.size(), D3D11_BIND_VERTEX_BUFFER);
		bufferDesc.StructureByteStride = sizeof(Vertex);

		D3D11_SUBRESOURCE_DATA initData = { skyBox.vertexList.data(), 0, 0 };
		hr = myDev->CreateBuffer(&bufferDesc, &initData, &skyvBuffMesh);
	}

	// Create and initialize the index buffer
	{
		CD3D11_BUFFER_DESC bufferDesc(sizeof(uint32_t) * skyBox.indicesList.size(), D3D11_BIND_INDEX_BUFFER);
		bufferDesc.StructureByteStride = sizeof(uint32_t);

		D3D11_SUBRESOURCE_DATA initData = { skyBox.indicesList.data(), 0, 0 };
		hr = myDev->CreateBuffer(&bufferDesc, &initData, &skyiBuffMesh);
	}

	//load our new mesh shader
	hr = myDev->CreateVertexShader(MyMeshVShader, sizeof(MyMeshVShader), nullptr, &vMeshShader);
	hr = myDev->CreatePixelShader(MyMeshPShader, sizeof(MyMeshPShader), nullptr, &pMeshShader);

	hr = myDev->CreateVertexShader(MySkyBoxVShader, sizeof(MySkyBoxVShader), nullptr, &skyvMeshShader);
	hr = myDev->CreatePixelShader(MySkyBoxPShader, sizeof(MySkyBoxPShader), nullptr, &skypMeshShader);

	//make matching input layout for mesh vertex
	//Describe the vertexit to D3D11
	D3D11_INPUT_ELEMENT_DESC meshInputDesc[] =
	{
		 {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		 {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		 {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = myDev->CreateInputLayout(meshInputDesc, 3, MyMeshVShader, sizeof(MyMeshVShader), &vMeshLayout);

	hr = myDev->CreateInputLayout(meshInputDesc, 3, MySkyBoxVShader, sizeof(MySkyBoxVShader), &skyvMeshLayout);

	//Load texture data
	hr = CreateDDSTextureFromFile(myDev, L"Assets/StoneHenge.dds", &shTexture, &SRV);

	hr = CreateDDSTextureFromFile(myDev, L"Assets/SunsetSkybox.dds", &skyhTexture, &SkyRV);

	//create zbuffer & view
	D3D11_TEXTURE2D_DESC zDesc;
	ZeroMemory(&zDesc, sizeof(zDesc));
	zDesc.ArraySize = 1;
	zDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	zDesc.Width = swap.BufferDesc.Width;
	zDesc.Height = swap.BufferDesc.Height;
	zDesc.Format = DXGI_FORMAT_D32_FLOAT;
	zDesc.Usage = D3D11_USAGE_DEFAULT;
	zDesc.MipLevels = 1;
	zDesc.SampleDesc.Count = 1;

	hr = myDev->CreateTexture2D(&zDesc, nullptr, &zBuffer);

	hr = myDev->CreateDepthStencilView(zBuffer, nullptr, &zBufferView);


	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	//case WM_PAINT:
	//    {
	//        PAINTSTRUCT ps;
	//        HDC hdc = BeginPaint(hWnd, &ps);
	//        // TODO: Add any drawing code that uses hdc here...
	//        EndPaint(hWnd, &ps);
	//    }
	//    break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}



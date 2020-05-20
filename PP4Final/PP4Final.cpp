// PP4Final.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "PP4Final.h"

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")

#include <DirectXMath.h>
using namespace DirectX;

#include "MyMeshVShader.csh" 
#include "MyMeshPShader.csh"

#include "DDSTextureLoader.h"

#include "Assets/DwarfBody.h"
#include "Assets/DwarfShirt.h"
#include "Assets/DwarfArmor.h"
#include "Assets/DwarfLeather.h"
#include "Assets/DwarfAxe.h"

ID3D11Device* myDev;
IDXGISwapChain* mySwap;
ID3D11DeviceContext* myCon;

//for drawing
ID3D11RenderTargetView* myRtv;
CD3D11_VIEWPORT myPort;

float aspectRatio = 1;

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 Tex;
};

ID3D11Buffer* cBuff;

ID3D11Texture2D* zBuffer;
ID3D11DepthStencilView* zBufferView;

ID3D11Resource* shTexture;
ID3D11ShaderResourceView* SRV;

struct WVP
{
	XMFLOAT4X4 wMatrix; //storage type
	XMFLOAT4X4 vMatrix;
	XMFLOAT4X4 pMatrix;
}MyMatricies;

//Dwarf information
ID3D11Buffer* dVBuffMesh;
ID3D11Buffer* dIBuffMesh;

ID3D11Resource* dTexture;
ID3D11ShaderResourceView* dSRV;

//Clothing information
ID3D11Buffer* cVBuffMesh;
ID3D11Buffer* cIBuffMesh;

ID3D11Resource* cTexture;
ID3D11ShaderResourceView* cSRV;

//Armor information
ID3D11Buffer* aVBuffMesh;
ID3D11Buffer* aIBuffMesh;

ID3D11Resource* aTexture;
ID3D11ShaderResourceView* aSRV;

//Boots/Belt information
ID3D11Buffer* bbVBuffMesh;
ID3D11Buffer* bbIBuffMesh;

ID3D11Resource* bbTexture;
ID3D11ShaderResourceView* bbSRV;


ID3D11VertexShader* vMeshShader;
ID3D11InputLayout* vMeshLayout;
ID3D11PixelShader* pMeshShader;

int decrement = 1;
static float rot = 0;
bool move = false;

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
	LoadStringW(hInstance, IDC_PP4FINAL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PP4FINAL));

	MSG msg;

	// Main message loop:
	while (true)
	{
		move = false;
		PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
			break;

		float color[] = { 0, 0, 0, 1.0f };
		myCon->ClearRenderTargetView(myRtv, color);

		myCon->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1, 0);

		//output merger
		ID3D11RenderTargetView* tempRTV[] = { myRtv };
		myCon->OMSetRenderTargets(1, tempRTV, zBufferView);
		//rasterizer
		myCon->RSSetViewports(1, &myPort);

		XMMATRIX temp3 = XMMatrixIdentity();

		if (GetAsyncKeyState('D') || GetAsyncKeyState('A') || GetAsyncKeyState('W') || GetAsyncKeyState('S'))
		{
			if (GetAsyncKeyState('D'))
			{
				rot -= 0.003;
			}
			else if(GetAsyncKeyState('A'))
			{
				rot += 0.009;
			}
			else if (GetAsyncKeyState('W'))
			{
				temp3 = XMMatrixMultiply(XMMatrixTranslation(-0.5, 0, 1), temp3);
				move = true;
			}
			else if (GetAsyncKeyState('S'))
			{
				temp3 = XMMatrixMultiply(XMMatrixTranslation(0.5, 0, -1), temp3);
				move = true;
			}
		}
		else
		{
			rot += 0.002f;
		}

		XMMATRIX temp2 = XMMatrixRotationY(rot);

		//Draw Dwarf
		ID3D11ShaderResourceView* DtexViews[] = { dSRV };
		UINT mesh_strides[] = { sizeof(_OBJ_VERT_) };
		UINT mesh_offsets[] = { 0 };
		ID3D11Buffer* meshVB[] = { dVBuffMesh };
		myCon->IASetVertexBuffers(0, 1, meshVB, mesh_strides, mesh_offsets);
		myCon->IASetIndexBuffer(dIBuffMesh, DXGI_FORMAT_R32_UINT, 0);
		myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myCon->VSSetShader(vMeshShader, 0, 0);
		myCon->IASetInputLayout(vMeshLayout);
		myCon->PSSetShaderResources(0, 1, DtexViews);
		myCon->PSSetShader(pMeshShader, 0, 0);

		////modify world matrix before drawing next thing

		XMMATRIX temp = XMMatrixIdentity();
		temp = XMMatrixTranslation(-1, 0, 0);
		temp = XMMatrixRotationY(180);
		temp = XMMatrixMultiply(temp2, temp);
		if (move == true)
		{
			temp = XMMatrixMultiply(temp3, temp);
		}
		XMStoreFloat4x4(&MyMatricies.wMatrix, temp);

		if (decrement == 1)
		{
			temp = XMMatrixLookAtLH({ 0, 1.3f, -1.8 }, { 2,1,0 }, { 0,1,0 });
			temp = XMMatrixMultiply(XMMatrixTranslation(.5, 0, 0), temp);
			XMStoreFloat4x4(&MyMatricies.vMatrix, temp);
			decrement -= 1;
		}

		//projection
		temp = XMMatrixPerspectiveFovLH(3.14f / 2.0f, aspectRatio, 0.1f, 1000);
		XMStoreFloat4x4(&MyMatricies.pMatrix, temp);

		D3D11_MAPPED_SUBRESOURCE gpuBuffer;
		//send it to the card
		HRESULT hr = myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		*((WVP*)(gpuBuffer.pData)) = MyMatricies;
		myCon->Unmap(cBuff, 0);

		ID3D11Buffer* constants[] = { cBuff };
		myCon->VSSetConstantBuffers(0, 1, constants);
		////draw it
		myCon->DrawIndexed(17856, 0, 0);



		//Draw Clothing
		ID3D11ShaderResourceView* CtexViews[] = { cSRV };
		ID3D11Buffer* CmeshVB[] = { cVBuffMesh };
		myCon->IASetVertexBuffers(0, 1, CmeshVB, mesh_strides, mesh_offsets);
		myCon->IASetIndexBuffer(cIBuffMesh, DXGI_FORMAT_R32_UINT, 0);
		myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myCon->VSSetShader(vMeshShader, 0, 0);
		myCon->IASetInputLayout(vMeshLayout);
		myCon->PSSetShaderResources(0, 1, CtexViews);
		myCon->PSSetShader(pMeshShader, 0, 0);

		temp = XMMatrixIdentity();
		temp = XMMatrixTranslation(-1, 0, 0);
		temp = XMMatrixRotationY(180);
		temp = XMMatrixMultiply(temp2, temp);
		if (move == true)
		{
			temp = XMMatrixMultiply(temp3, temp);
		}
		XMStoreFloat4x4(&MyMatricies.wMatrix, temp);

		hr = myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		*((WVP*)(gpuBuffer.pData)) = MyMatricies;
		myCon->Unmap(cBuff, 0);

		myCon->VSSetConstantBuffers(0, 1, constants);
		////draw it
		myCon->DrawIndexed(4572, 0, 0);


		//Draw Armor
		ID3D11ShaderResourceView* AtexViews[] = { aSRV };
		ID3D11Buffer* AmeshVB[] = { aVBuffMesh };
		myCon->IASetVertexBuffers(0, 1, AmeshVB, mesh_strides, mesh_offsets);
		myCon->IASetIndexBuffer(aIBuffMesh, DXGI_FORMAT_R32_UINT, 0);
		myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myCon->VSSetShader(vMeshShader, 0, 0);
		myCon->IASetInputLayout(vMeshLayout);
		myCon->PSSetShaderResources(0, 1, AtexViews);
		myCon->PSSetShader(pMeshShader, 0, 0);

		temp = XMMatrixIdentity();
		temp = XMMatrixTranslation(-1, 0, 0);
		temp = XMMatrixRotationY(180);
		temp = XMMatrixMultiply(temp2, temp);
		if (move == true)
		{
			temp = XMMatrixMultiply(temp3, temp);
		}
		XMStoreFloat4x4(&MyMatricies.wMatrix, temp);

		hr = myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		*((WVP*)(gpuBuffer.pData)) = MyMatricies;
		myCon->Unmap(cBuff, 0);

		myCon->VSSetConstantBuffers(0, 1, constants);
		////draw it
		myCon->DrawIndexed(16164, 0, 0);

		//Draw Boots and Belt
		ID3D11ShaderResourceView* BBtexViews[] = { bbSRV };
		ID3D11Buffer* BBmeshVB[] = { bbVBuffMesh };
		myCon->IASetVertexBuffers(0, 1, BBmeshVB, mesh_strides, mesh_offsets);
		myCon->IASetIndexBuffer(bbIBuffMesh, DXGI_FORMAT_R32_UINT, 0);
		myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myCon->VSSetShader(vMeshShader, 0, 0);
		myCon->IASetInputLayout(vMeshLayout);
		myCon->PSSetShaderResources(0, 1, BBtexViews);
		myCon->PSSetShader(pMeshShader, 0, 0);

		temp = XMMatrixIdentity();
		temp = XMMatrixTranslation(-1, 0, 0);
		temp = XMMatrixRotationY(180);
		temp = XMMatrixMultiply(temp2, temp);
		if (move == true)
		{
			temp = XMMatrixMultiply(temp3, temp);
		}
		XMStoreFloat4x4(&MyMatricies.wMatrix, temp);

		hr = myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		*((WVP*)(gpuBuffer.pData)) = MyMatricies;
		myCon->Unmap(cBuff, 0);

		myCon->VSSetConstantBuffers(0, 1, constants);
		////draw it
		myCon->DrawIndexed(4866, 0, 0);

		mySwap->Present(1, 0);
	}

	myRtv->Release();
	myCon->Release();
	mySwap->Release();
	myDev->Release();
	cBuff->Release();
	dVBuffMesh->Release();
	dIBuffMesh->Release();
	cVBuffMesh->Release();
	cIBuffMesh->Release();
	aVBuffMesh->Release();
	aIBuffMesh->Release();
	bbVBuffMesh->Release();
	bbIBuffMesh->Release();
	vMeshShader->Release();
	vMeshLayout->Release();
	pMeshShader->Release();

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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PP4FINAL));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PP4FINAL);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
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

	CD3D11_BUFFER_DESC bDesc;
	D3D11_SUBRESOURCE_DATA subData;
	ZeroMemory(&bDesc, sizeof(bDesc));
	ZeroMemory(&subData, sizeof(subData));

	//Load dwarf onto card
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(DwarfBody_data);
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;

	subData.pSysMem = DwarfBody_data;
	hr = myDev->CreateBuffer(&bDesc, &subData, &dVBuffMesh); //Dwarf vertex buffer

	//Dwarf index buffer
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(DwarfBody_indicies);
	subData.pSysMem = DwarfBody_indicies;
	hr = myDev->CreateBuffer(&bDesc, &subData, &dIBuffMesh);

	hr = CreateDDSTextureFromFile(myDev, L"Assets/BodyTexture.dds", &dTexture, &dSRV);

	//Load clothing onto card
	ZeroMemory(&bDesc, sizeof(bDesc));
	ZeroMemory(&subData, sizeof(subData));

	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(DwarfShirt_data);
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;

	subData.pSysMem = DwarfShirt_data;
	hr = myDev->CreateBuffer(&bDesc, &subData, &cVBuffMesh); //Clothing vertex buffer

	//Clothing index buffer
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(DwarfShirt_indicies);
	subData.pSysMem = DwarfShirt_indicies;
	hr = myDev->CreateBuffer(&bDesc, &subData, &cIBuffMesh);

	hr = CreateDDSTextureFromFile(myDev, L"Assets/ShirtTexture.dds", &cTexture, &cSRV);

	//Load armor onto the card
	ZeroMemory(&bDesc, sizeof(bDesc));
	ZeroMemory(&subData, sizeof(subData));

	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(DwarfArmor_data);
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;

	subData.pSysMem = DwarfArmor_data;
	hr = myDev->CreateBuffer(&bDesc, &subData, &aVBuffMesh); //Armor vertex buffer

	//Armor index buffer
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(DwarfArmor_indicies);
	subData.pSysMem = DwarfArmor_indicies;
	hr = myDev->CreateBuffer(&bDesc, &subData, &aIBuffMesh);

	hr = CreateDDSTextureFromFile(myDev, L"Assets/ArmorTexture.dds", &aTexture, &aSRV);

	//Load boots/belt onto the card
	ZeroMemory(&bDesc, sizeof(bDesc));
	ZeroMemory(&subData, sizeof(subData));

	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(DwarfLeather_data);
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;

	subData.pSysMem = DwarfLeather_data;
	hr = myDev->CreateBuffer(&bDesc, &subData, &bbVBuffMesh); //BB vertex buffer

	//BB index buffer
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(DwarfLeather_indicies);
	subData.pSysMem = DwarfLeather_indicies;
	hr = myDev->CreateBuffer(&bDesc, &subData, &bbIBuffMesh);

	hr = CreateDDSTextureFromFile(myDev, L"Assets/LeatherTexture.dds", &bbTexture, &bbSRV);

	//Create shaders
	hr = myDev->CreateVertexShader(MyMeshVShader, sizeof(MyMeshVShader), nullptr, &vMeshShader);
	hr = myDev->CreatePixelShader(MyMeshPShader, sizeof(MyMeshPShader), nullptr, &pMeshShader);

	D3D11_INPUT_ELEMENT_DESC meshInputDesc[] =
	{
		 {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		 {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		 {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = myDev->CreateInputLayout(meshInputDesc, 3, MyMeshVShader, sizeof(MyMeshVShader), &vMeshLayout);

	ZeroMemory(&bDesc, sizeof(bDesc));

	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.ByteWidth = sizeof(WVP);
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_DYNAMIC;


	hr = myDev->CreateBuffer(&bDesc, &subData, &cBuff);

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
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
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

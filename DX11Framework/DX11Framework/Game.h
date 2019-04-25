#pragma once
#include "wrl.h"
#include <windows.graphics.directx.h>

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Platform;

struct VERTEX
{
	float x, y, z;
	float R, G, B;
};

struct OFFSET
{
	float X, Y, Z;
};


class Game
{
public:
	//Com ptr is a wrapper
	ComPtr<ID3D11Device1> device; // device interface (virtual representation of video adapter)
	ComPtr<ID3D11DeviceContext1> deviceContext; //the control panel for the GPU 
	ComPtr<IDXGISwapChain1> swapChain;
	ComPtr<ID3D11RenderTargetView> renderTarget;
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;
	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11Buffer> constantBuffer;

	void Initialise();
	void Update();
	void Render();
	void InitGraphics();
	void InitPipeline();

};


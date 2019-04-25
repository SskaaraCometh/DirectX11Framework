#include "pch.h"
#include "Game.h"
#include <fstream>

//loading file into array
Array<byte>^ LoadShaderFile(std::string File)
{
	Array<byte>^ FileData = nullptr;

	//open file
	std::ifstream VertexFile(File, std::ios::in | std::ios::binary | std::ios::ate);

	//check if file is open
	if (VertexFile.is_open())
	{
		int length = (int)VertexFile.tellg();

		//collect data
		FileData = ref new Array<byte>(length);
		VertexFile.seekg(0, std::ios::beg);
		VertexFile.read(reinterpret_cast<char*>(FileData->Data), length);
		VertexFile.close();

	}

	return FileData;
}

void Game::Initialise()
{
	//Sets up directX
	//define temp pointer variables to d and dc
	ComPtr<ID3D11Device> tempDevice;
	ComPtr<ID3D11DeviceContext> tempContext;

	//Create the device and the device context objects
	D3D11CreateDevice	
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&tempDevice,
		nullptr,
		&tempContext
	);

	//Convert the pointers from the DX11 version to the DX11.2 version
	tempDevice.As(&device);
	tempContext.As(&deviceContext);

	//--------------------------------------------------------------------------------------------------------------------------------------------
	// 1. convert d3d11device1 interface to an IDXGIDevice1, obtaining a pointer to a DXGI factor, an object that produces other DXGi objects
	ComPtr<IDXGIDevice1> dxgiDevice;
	device.As(&dxgiDevice);

	// 2. Utilize the IDXGIDevice1 interface to get access to the adapter
	ComPtr<IDXGIAdapter> dxgiAdapter;
	dxgiDevice->GetAdapter(&dxgiAdapter);

	// 3. Use IDXGIAdapter interface to get access to the parents.
	ComPtr<IDXGIFactory2> dxgiFactory;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory);

	//-------------------------------------------------------------------------------------------------------------------------------------------
	//Set up Swap Chain desc
	DXGI_SWAP_CHAIN_DESC1 swapChainDescription = { 0 };
	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // How the swap chain should be utilised
	swapChainDescription.BufferCount = 2;								//Init front and Back buffer
	swapChainDescription.Format = DXGI_FORMAT_B8G8R8A8_UNORM;			//swap chain format
	swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; //Recommended flip mode
	swapChainDescription.SampleDesc.Count = 1;							//Disables anti aliasing

	CoreWindow^ Window = CoreWindow::GetForCurrentThread(); //Obtain pointer to the window

	dxgiFactory->CreateSwapChainForCoreWindow
	(
		device.Get(), //address of device
		reinterpret_cast<IUnknown*>(Window), //address of the window
		&swapChainDescription, //address of the swap chain
		nullptr,
		&swapChain
	);

	// ------------------------------------------------------------------------------------------------------------------
	//Get a pointer to BB
	ComPtr<ID3D11Texture2D> backBuffer;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);

	//Create a render target which points to the back buffer /USe this for render target view description
	device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTarget);

	//Set viewport
	D3D11_VIEWPORT viewPort = { 0 };
	viewPort.TopLeftX = 0; //specify the origin
	viewPort.TopLeftY = 0;
	viewPort.Width = Window->Bounds.Width; //Get window width and pass it on to the viewport
	viewPort.Height = Window->Bounds.Height;

	//----------------------------------------------------------------------------------------------------------------------- 
	deviceContext->RSSetViewports(1, &viewPort); //activate the viewport struct param 1 number of viewports, param 2 list of pointers to viewport structs

	InitGraphics();
	InitPipeline();
}

void Game::InitGraphics()
{
	VERTEX vertices[] = 
	{
		{0.0f, 0.5f, 0.0f,     1.0f, 0.0f, 0.0f},
		{0.45f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f},
		{-0.45f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f},
	}; // structs are in system mem

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.ByteWidth = sizeof(VERTEX) * ARRAYSIZE(vertices); // struct size * array size of vertices
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	
	D3D11_SUBRESOURCE_DATA subResourceData = { vertices, 0, 0 }; // contains desc of data we wish to store on the buffer

	device->CreateBuffer(&bufferDesc, &subResourceData, &vertexBuffer);

}

void Game::InitPipeline()
{
	//Initialises GPU settings and prepares for rendering
	Array<byte>^ VSFile = LoadShaderFile("VertexShader.cso");
	Array<byte>^ PSFile = LoadShaderFile("PixelShader.cso");

	//Create shader objects
	device->CreateVertexShader(VSFile->Data, VSFile->Length, nullptr, &vertexShader);
	device->CreatePixelShader(PSFile->Data, PSFile->Length, nullptr, &pixelShader);

	//Set shaders as active
	deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	//Set vertex layout
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOUR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	//Create input layout
	device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), VSFile->Data, VSFile->Length, &inputLayout);
	deviceContext->IASetInputLayout(inputLayout.Get()); //tells dx to use the viewport. Tells D3d to place normalised device coordinates.

	//Constant Buffer--------------------------------------------------------------------------

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DEFAULT; //implies GPU access, not CPU.
	bufferDesc.ByteWidth = 16; //Sets constant buffer size (mul of 16) 
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	device->CreateBuffer(&bufferDesc, nullptr, &constantBuffer);
	deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
}

void Game::Update()
{
	//Performs updates (on tick or otherwise)
}

void Game::Render()
{
	//Set new render target as active render target
	deviceContext->OMSetRenderTargets(1, renderTarget.GetAddressOf(), nullptr);
	float colour[4] = { 0.1f, 0.1f, 0.5f, 1.0f };

	//Clear BB to a colour

	deviceContext->ClearRenderTargetView(renderTarget.Get(), colour);

	//Set vertex buffer
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	//set up the primitive topology and constant buffer
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	OFFSET Offset;
	Offset.X = 0.5f;
	Offset.Y = 0.2f;
	Offset.Z = 0.7;

	//Set new vals for constant buffer
	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &Offset, 0, 0);

	deviceContext->Draw(3, 0);

	//renders a single frame
	swapChain->Present(1, 0);
}



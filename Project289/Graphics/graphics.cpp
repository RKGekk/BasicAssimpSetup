#include "graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height) {
	m_fps_timer.Start();
	m_width = width;
	m_height = height;
	if (!InitializeDirectX(hwnd)) { return false; };
	if(!InitializeShaders()) { return false; };
	if (!InitializeScene()) { return false; };

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(m_device.Get(), m_device_context.Get());
	ImGui::StyleColorsDark();

	return true;
}

void Graphics::RenderFrame() {
	std::array<float, 4> color = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_device_context->ClearRenderTargetView(m_render_target_view.Get(), color.data());
	m_device_context->ClearDepthStencilView(m_depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_device_context->IASetInputLayout(m_vertex_shader.GetInputLayout());
	m_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_device_context->RSSetState(m_raster_state.Get());
	m_device_context->OMSetBlendState(m_blend_state.Get(), nullptr, 0xFFFFFFFF);
	m_device_context->OMSetDepthStencilState(m_depth_stencil_state.Get(), 0);
	m_device_context->PSSetSamplers(0, 1, m_sampler_state.GetAddressOf());
	m_device_context->VSSetShader(m_vertex_shader.GetShader(), NULL, 0);
	m_device_context->PSSetShader(m_pixel_shader.GetShader(), NULL, 0);

	m_device_context->PSSetShaderResources(0, 1, m_texture.GetAddressOf());

	/*static std::vector<float> translationOffset = { 0.0f, 0.0f, 0.0f };
	DirectX::XMMATRIX world = DirectX::XMMatrixTranslation(translationOffset.at(0), translationOffset.at(1), translationOffset.at(2));

	DirectX::XMStoreFloat4x4(&m_cbvs.data.mat, DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(world, m_camera.GetViewMatrix()), m_camera.GetProjectionMatrix())));
	if (!m_cbvs.ApplyChanges(m_device_context.Get())) { return; }
	m_device_context->VSSetConstantBuffers(0u, 1u, m_cbvs.GetAddressOf());*/

	static float alpha = 1.0f;
	m_cbps.data.alpha = alpha;
	if (!m_cbps.ApplyChanges(m_device_context.Get())) { return; }
	m_device_context->PSSetConstantBuffers(0u, 1u, m_cbps.GetAddressOf());

	/*UINT stride = m_vertex_buffer.Stride();
	UINT offset = 0;
	m_device_context->IASetVertexBuffers(0, 1, m_vertex_buffer.GetAddressOf(), &stride, &offset);
	m_device_context->IASetIndexBuffer(m_indices_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	m_device_context->DrawIndexed(m_indices_buffer.BufferSize(), 0, 0);*/

	m_model.Draw(m_device_context.Get(), DirectX::XMMatrixMultiply(m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix()));

	static int fpsCounter = 0;
	static std::wstring fpsString = L"FPS: 0";
	fpsCounter += 1;
	if (m_fps_timer.GetSecondsElapsed() > 1.0f) {
		fpsString = L"FPS: " + std::to_wstring(fpsCounter);
		fpsCounter = 0;
		m_fps_timer.Restart();
	}
	m_sprite_batch->Begin();
	DirectX::XMVECTOR text_pos = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR text_color = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMVECTOR text_origin = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR text_scale = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	m_sprite_font->DrawString(m_sprite_batch.get(), fpsString.c_str(), text_pos, DirectX::Colors::White, 0.0f, text_origin, text_scale);
	m_sprite_batch->End();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Test");

	ImGui::Text("Hello world");
	static int cc = 0;
	if (ImGui::Button("COUNTER")) { ++cc; }
	std::string ccString = "Click counter: " + std::to_string(cc);
	ImGui::SameLine();
	ImGui::Text(ccString.c_str());
	//ImGui::DragFloat3("Translation x/y/z", translationOffset.data(), 0.1f, -5.0f, 5.0f);
	ImGui::DragFloat("Alpha", &alpha, 0.005f, 0.0f, 1.0f);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	m_swap_chain->Present(1, NULL);
}

bool Graphics::InitializeDirectX(HWND hwnd) {
	try {
		std::vector<AdapterData>& adapters = AdapterReader::GetAdapterData();
		if (adapters.size() == 0u) {
			ErrorLogger::Log("No IDXGI Adapters found.");
			return false;
		}
		Microsoft::WRL::ComPtr<IDXGIAdapter> adapter = adapters[0].m_pAdapter;

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

		swapChainDesc.BufferDesc.Width = m_width;
		swapChainDesc.BufferDesc.Height = m_height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			adapter.Get(),
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL,
			NULL,
			NULL,
			0,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			m_swap_chain.GetAddressOf(),
			m_device.GetAddressOf(),
			NULL,
			m_device_context.GetAddressOf()
		);
		COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferPtr;
		hr = m_swap_chain->GetBuffer(0, IID_PPV_ARGS(backBufferPtr.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "Failed to get back buffer.");

		hr = m_device->CreateRenderTargetView(backBufferPtr.Get(), NULL, m_render_target_view.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");

		D3D11_TEXTURE2D_DESC depthBufferDesc;
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
		depthBufferDesc.Width = m_width;
		depthBufferDesc.Height = m_height;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;
		hr = m_device->CreateTexture2D(&depthBufferDesc, NULL, m_depth_stencil_buffer.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hr = m_device->CreateDepthStencilState(&depthStencilDesc, m_depth_stencil_state.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

		m_device_context->OMSetDepthStencilState(m_depth_stencil_state.Get(), 1);

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hr = m_device->CreateDepthStencilView(m_depth_stencil_buffer.Get(), &depthStencilViewDesc, m_depth_stencil_view.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");

		m_device_context->OMSetRenderTargets(1, m_render_target_view.GetAddressOf(), m_depth_stencil_view.Get());

		D3D11_VIEWPORT viewport;
		viewport.Width = (float)(m_width);
		viewport.Height = (float)(m_height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		m_device_context->RSSetViewports(1, &viewport);

		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;
		hr = m_device->CreateRasterizerState(&rasterDesc, m_raster_state.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");
			
		m_device_context->RSSetState(m_raster_state.Get());

		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));
		D3D11_RENDER_TARGET_BLEND_DESC rtbd;
		ZeroMemory(&rtbd, sizeof(rtbd));
		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0] = rtbd;
		hr = m_device->CreateBlendState(&blendDesc, m_blend_state.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");

		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = m_device->CreateSamplerState(&samplerDesc, m_sampler_state.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");

		m_sprite_batch = std::make_unique<DirectX::SpriteBatch>(m_device_context.Get());
		m_sprite_font = std::make_unique<DirectX::SpriteFont>(m_device.Get(), L"Data/Fonts/comic_sans_ms_16.sprite");
	}
	catch (COMException e) {
		ErrorLogger::Log(e);
		return false;
	}

	return true;
}

bool Graphics::InitializeShaders() {
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	bool can_draw = m_vertex_shader.Initialize(
		m_device,
		ShaderGonfig{}
			.set_shader_name(L"vertex_shader.cso")
			.set_description(layout)
	);

	can_draw &= m_pixel_shader.Initialize(
		m_device,
		ShaderGonfig{}
			.set_shader_name(L"pixel_shader.cso")
			.set_description({})
	);

	if (!can_draw) {
		return false;
	}

	return true;
}

bool Graphics::InitializeScene() {
	try {
		HRESULT hr = CoInitialize(NULL);
		COM_ERROR_IF_FAILED(hr, "Failed initialize WIC.");
			
		hr = DirectX::CreateWICTextureFromFile(m_device.Get(), L"Data/Textures/storage-container2-albedo.png", nullptr, m_texture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create WIC texture from file.");

		hr = m_cbvs.Initialize(m_device.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

		hr = m_cbps.Initialize(m_device.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

		if (!m_model.Initialize(m_device.Get(), "Data/Objects/nanosuit/nanosuit.obj", m_texture.Get(), m_cbvs)) { return false; };

		m_camera.SetPosition(0.0f, 0.0f, -2.0f);
		m_camera.SetProjectionValues(60.0f, float(m_width) / float(m_height), 0.1f, 1000.0f);
	}
	catch (COMException e) {
		ErrorLogger::Log(e);
		return false;
	}

	return true;
}

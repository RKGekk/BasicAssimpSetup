#include "mesh.h"

Mesh::Mesh(ID3D11Device* device, const std::vector<Vertex>& vertices, const std::vector<DWORD>& indices) {
	HRESULT hr = m_vertex_buffer.Initialize(device, vertices);
	COM_ERROR_IF_FAILED(hr, "Failed to create vertex buffer data.");

	hr = m_index_buffer.Initialize(device, indices);
	COM_ERROR_IF_FAILED(hr, "Failed to create indices buffer data.");
}

Mesh::Mesh(const Mesh& rhv) {
	m_vertex_buffer = rhv.m_vertex_buffer;
	m_index_buffer = rhv.m_index_buffer;
}

void Mesh::Draw(ID3D11DeviceContext* deviceContext) {
	deviceContext->IASetIndexBuffer(m_index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	UINT stride = m_vertex_buffer.Stride();
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, m_vertex_buffer.GetAddressOf(), &stride, &offset);
	deviceContext->DrawIndexed(m_index_buffer.BufferSize(), 0, 0);
}
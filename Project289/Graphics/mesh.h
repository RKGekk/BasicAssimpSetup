#pragma once

#include <vector>
#include <DirectXMath.h>

#include "vertex.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "constant_buffer.h"
#include "constant_buffer_types.h"

class Mesh {
public:
	Mesh(ID3D11Device* device, const std::vector<Vertex>& vertices, const std::vector<DWORD>& indices);
	Mesh(const Mesh& rhv);
	void Draw(ID3D11DeviceContext* deviceContext);

private:
	VertexBuffer<Vertex> m_vertex_buffer;
	IndexBuffer m_index_buffer;
};
#pragma once

#include <vector>
#include <string>
#include <DirectXMath.h>

#include "vertex.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "constant_buffer.h"
#include "constant_buffer_types.h"
#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class Model {
public:
	Model();

	bool Initialize(ID3D11Device* device, const std::string& filePath, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_VertexShader>& cbvs);
	void SetTexture(ID3D11ShaderResourceView* texture);
	void Draw(ID3D11DeviceContext* deviceContext, DirectX::FXMMATRIX viewProjectionMatrix);

	const DirectX::XMVECTOR GetPosition() const;
	const DirectX::XMFLOAT3 GetPosition3() const;
	const DirectX::XMVECTOR GetRotation() const;
	const DirectX::XMFLOAT3 GetRotation3() const;

	void SetPosition(DirectX::FXMVECTOR pos);
	void SetPosition(float x, float y, float z);
	void AdjustPosition(DirectX::FXMVECTOR pos);
	void AdjustPosition(float x, float y, float z);

	void SetRotation(DirectX::FXMVECTOR rot);
	void SetRotation(float p, float y, float r);
	void AdjustRotation(DirectX::FXMVECTOR rot);
	void AdjustRotation(float p, float y, float r);

	DirectX::XMVECTOR GetForward();
	DirectX::XMFLOAT3 GetForward3();

	DirectX::XMVECTOR GetRight();
	DirectX::XMFLOAT3 GetRight3();

	DirectX::XMVECTOR GetBackward();
	DirectX::XMFLOAT3 GetBackward3();

	DirectX::XMVECTOR GetLeft();
	DirectX::XMFLOAT3 GetLeft3();

	void UpdateWorldMatrix();
	bool LoadModel(ID3D11Device* device, const std::string& fileName);

private:
	void ProcessNode(ID3D11Device* device, aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(ID3D11Device* device, aiMesh* mesh, const aiScene* scene);

	std::vector<Mesh> m_meshes;

	ConstantBuffer<CB_VS_VertexShader>* m_cbvs = nullptr;
	ID3D11ShaderResourceView* m_texture = nullptr;

	DirectX::XMFLOAT4X4 m_world_matrix;

	DirectX::XMFLOAT3 m_pos;
	DirectX::XMFLOAT3 m_rot;

	const DirectX::XMFLOAT3 DEFAULT_FORWARD_VECTOR = { 0.0f, 0.0f, 1.0f };
	const DirectX::XMFLOAT3 DEFAULT_UP_VECTOR = { 0.0f, 1.0f, 0.0f };
	const DirectX::XMFLOAT3 DEFAULT_BACKWARD_VECTOR = { 0.0f, 0.0f, -1.0f };
	const DirectX::XMFLOAT3 DEFAULT_LEFT_VECTOR = { -1.0f, 0.0f, 0.0f };
	const DirectX::XMFLOAT3 DEFAULT_RIGHT_VECTOR = { 1.0f, 0.0f, 0.0f };
	const float EPSILON = 0.001f;

	DirectX::XMFLOAT3 m_forward;
	DirectX::XMFLOAT3 m_left;
	DirectX::XMFLOAT3 m_right;
	DirectX::XMFLOAT3 m_backward;
};
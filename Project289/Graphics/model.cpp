#include "model.h"

Model::Model() {
	DirectX::XMStoreFloat4x4(&m_world_matrix, DirectX::XMMatrixIdentity());
}

bool Model::Initialize(ID3D11Device* device, const std::string& filePath, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_VertexShader>& cbvs) {
	m_texture = texture;
	m_cbvs = &cbvs;
	
	if (!LoadModel(device, filePath)) { return false; }

	SetPosition(0.0f, 0.0f, 0.0f);
	SetRotation(0.0f, 0.0f, 0.0f);
	UpdateWorldMatrix();
	return true;
}

void Model::SetTexture(ID3D11ShaderResourceView* texture) {
	m_texture = texture;
}

void Model::Draw(ID3D11DeviceContext* deviceContext, DirectX::FXMMATRIX viewProjectionMatrix) {

	DirectX::XMStoreFloat4x4(&m_cbvs->data.mat, DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&m_world_matrix), viewProjectionMatrix)));
	if (!m_cbvs->ApplyChanges(deviceContext)) { return; }
	deviceContext->VSSetConstantBuffers(0u, 1u, m_cbvs->GetAddressOf());

	deviceContext->PSSetShaderResources(0, 1, &m_texture);

	for (size_t i = 0; i < m_meshes.size(); ++i) {
		m_meshes[i].Draw(deviceContext);
	}
}

const DirectX::XMVECTOR Model::GetPosition() const {
	return DirectX::XMLoadFloat3(&m_pos);
}

const DirectX::XMFLOAT3 Model::GetPosition3() const {
	return m_pos;
}

const DirectX::XMVECTOR Model::GetRotation() const {
	return DirectX::XMLoadFloat3(&m_rot);
}

const DirectX::XMFLOAT3 Model::GetRotation3() const {
	return m_rot;
}

void Model::SetPosition(DirectX::FXMVECTOR pos) {
	DirectX::XMStoreFloat3(&m_pos, pos);
	UpdateWorldMatrix();
}

void Model::SetPosition(float x, float y, float z) {
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
	UpdateWorldMatrix();
}

void Model::AdjustPosition(DirectX::FXMVECTOR pos) {
	DirectX::XMStoreFloat3(&m_pos, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&m_pos), pos));
	UpdateWorldMatrix();
}

void Model::AdjustPosition(float x, float y, float z) {
	m_pos.x += x;
	m_pos.y += y;
	m_pos.z += z;
	UpdateWorldMatrix();
}

void Model::SetRotation(DirectX::FXMVECTOR rot) {
	DirectX::XMStoreFloat3(&m_rot, rot);
	UpdateWorldMatrix();
}

void Model::SetRotation(float p, float y, float r) {
	m_rot.x = p;
	m_rot.y = y;
	m_rot.z = r;
	UpdateWorldMatrix();
}

void Model::AdjustRotation(DirectX::FXMVECTOR rot) {
	DirectX::XMStoreFloat3(&m_rot, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&m_rot), rot));
	UpdateWorldMatrix();
}

void Model::AdjustRotation(float p, float y, float r) {
	m_rot.x += p;
	m_rot.y += y;
	m_rot.z += r;
	UpdateWorldMatrix();
}

DirectX::XMVECTOR Model::GetForward() {
	return DirectX::XMLoadFloat3(&m_forward);
}

DirectX::XMFLOAT3 Model::GetForward3() {
	return m_forward;
}

DirectX::XMVECTOR Model::GetRight() {
	return DirectX::XMLoadFloat3(&m_right);
}

DirectX::XMFLOAT3 Model::GetRight3() {
	return m_right;
}

DirectX::XMVECTOR Model::GetBackward() {
	return DirectX::XMLoadFloat3(&m_backward);
}

DirectX::XMFLOAT3 Model::GetBackward3() {
	return m_backward;
}

DirectX::XMVECTOR Model::GetLeft() {
	return DirectX::XMLoadFloat3(&m_left);
}

DirectX::XMFLOAT3 Model::GetLeft3() {
	return m_left;
}

void Model::UpdateWorldMatrix() {
	DirectX::XMStoreFloat4x4(&m_world_matrix, DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationRollPitchYaw(m_rot.x, m_rot.y, m_rot.z), DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z)));

	DirectX::XMMATRIX rotationMartix = DirectX::XMMatrixRotationRollPitchYaw(0.0f, m_rot.y, 0.0f);
	DirectX::XMStoreFloat3(&m_forward, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&DEFAULT_FORWARD_VECTOR), rotationMartix));
	DirectX::XMStoreFloat3(&m_backward, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&DEFAULT_BACKWARD_VECTOR), rotationMartix));
	DirectX::XMStoreFloat3(&m_left, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&DEFAULT_LEFT_VECTOR), rotationMartix));
	DirectX::XMStoreFloat3(&m_right, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&DEFAULT_RIGHT_VECTOR), rotationMartix));
}

bool Model::LoadModel(ID3D11Device* device, const std::string& fileName) {
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (!pScene) { return false; }

	ProcessNode(device, pScene->mRootNode, pScene);

	return true;
}

void Model::ProcessNode(ID3D11Device* device, aiNode* node, const aiScene* scene) {
	for (UINT i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(ProcessMesh(device, mesh, scene));
	}
	for (UINT i = 0; i < node->mNumChildren; ++i) {
		ProcessNode(device, node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(ID3D11Device* device, aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<DWORD> indices;

	for (UINT i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;
		vertex.color = {0.7f, 0.1f, 0.6f};
		vertex.pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		if (mesh->mTextureCoords) {
			vertex.uv = { (float)mesh->mTextureCoords[0][i].x, (float)mesh->mTextureCoords[0][i].y };
		}
		vertices.push_back(vertex);
	}
	for (UINT i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	return Mesh(device, vertices, indices);
}

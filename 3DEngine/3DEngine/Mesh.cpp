#include "Mesh.h"
#include "imgui/imgui.h"
#include "Surface.h"
#include <unordered_map>
#include <sstream>

namespace dx = DirectX;

// Mesh
Mesh::Mesh( Graphics& gfx,std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs )
{
	if( !IsStaticInitialized() )
	{
		AddStaticBind( std::make_unique<Bind::Topology>( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}

	for( auto& pb : bindPtrs )
	{
		if( auto pi = dynamic_cast<Bind::IndexBuffer*>(pb.get()) )
		{
			AddIndexBuffer( std::unique_ptr<Bind::IndexBuffer>{ pi } );
			pb.release();
		}
		else
		{
			AddBind( std::move( pb ) );
		}
	}

	AddBind( std::make_unique<Bind::TransformCbuf>( gfx,*this ) );
}
void Mesh::Draw( Graphics& gfx,dx::FXMMATRIX accumulatedTransform ) const noxnd
{
	dx::XMStoreFloat4x4( &transform,accumulatedTransform );
	Drawable::Draw( gfx );
}
dx::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return dx::XMLoadFloat4x4( &transform );
}


// Node
Node::Node( int id,const std::string& name,std::vector<Mesh*> meshPtrs,const dx::XMMATRIX& transform_in ) noxnd
	:
	id( id ),
	meshPtrs( std::move( meshPtrs ) ),
	name( name )
{
	dx::XMStoreFloat4x4( &transform,transform_in );
	dx::XMStoreFloat4x4( &appliedTransform,dx::XMMatrixIdentity() );
}

void Node::Draw( Graphics& gfx,dx::FXMMATRIX accumulatedTransform ) const noxnd
{
	const auto built =
		dx::XMLoadFloat4x4( &appliedTransform ) *
		dx::XMLoadFloat4x4( &transform ) *
		accumulatedTransform;
	for( const auto pm : meshPtrs )
	{
		pm->Draw( gfx,built );
	}
	for( const auto& pc : childPtrs )
	{
		pc->Draw( gfx,built );
	}
}

void Node::AddChild( std::unique_ptr<Node> pChild ) noxnd
{
	assert( pChild );
	childPtrs.push_back( std::move( pChild ) );
}

void Node::SetAppliedTransform( DirectX::FXMMATRIX transform ) noexcept
{
	dx::XMStoreFloat4x4( &appliedTransform,transform );
}

int Node::GetId() const noexcept
{
	return id;
}

void Node::ShowTree( Node*& pSelectedNode ) const noexcept
{	
	// if there is no selected node, set selectedId to an impossible value
	const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
	// build up flags for current node
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0 )
		| ((childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0 );
	// render this node
	const auto expanded = ImGui::TreeNodeEx( 
		(void*)(intptr_t)GetId(),node_flags,name.c_str() 
	);
	// processing for selecting node
	if ( ImGui::IsItemClicked() )
	{
		pSelectedNode = const_cast<Node*>(this);
	}

	if ( expanded )
	{
		for (const auto& pChild : childPtrs)
		{
			pChild->ShowTree( pSelectedNode );
		}
		ImGui::TreePop();
	}
}


// Model
class ModelWindow // pImpl idiom, only defined in this .cpp
{
public:
	void Show( const char* windowName,const Node& root ) noexcept
	{
		// window name defaults to "Model"
		windowName = windowName ? windowName : "Model";
		if( ImGui::Begin( windowName ) )
		{
			ImGui::Columns( 2,nullptr,true );
			root.ShowTree( pSelectedNode );

			ImGui::NextColumn();
			if (pSelectedNode != nullptr)
			{
				auto& transform = transforms[pSelectedNode->GetId()];
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
			}
		}
		ImGui::End();
	}
	dx::XMMATRIX GetTransform() const noexcept
	{
		assert( pSelectedNode != nullptr );
		const auto& transform = transforms.at( pSelectedNode->GetId() );
		return 
			dx::XMMatrixRotationRollPitchYaw( transform.roll,transform.pitch,transform.yaw ) *
			dx::XMMatrixTranslation( transform.x,transform.y,transform.z );
	}
	Node* GetSelectedNode() const noexcept
	{
		return pSelectedNode;
	}
private:
	Node* pSelectedNode;
	struct TransformParameters
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::unordered_map<int, TransformParameters> transforms;
};

Model::Model( Graphics& gfx,const std::string fileName )
	:
	pWindow( std::make_unique<ModelWindow>() )
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile( fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals
	);

	if ( pScene == nullptr )
	{
		throw ModelException( __LINE__,__FILE__,imp.GetErrorString() );
	}

	for( size_t i = 0; i < pScene->mNumMeshes; i++ )
	{
		meshPtrs.push_back( ParseMesh( gfx,*pScene->mMeshes[i],pScene->mMaterials ) );
	}

	int nextId = 0;
	pRoot = ParseNode( nextId,*pScene->mRootNode );
}

void Model::Draw( Graphics& gfx ) const noxnd
{
	if ( auto node = pWindow->GetSelectedNode() )
	{
		node->SetAppliedTransform( pWindow->GetTransform() );
	}
	pRoot->Draw( gfx,dx::XMMatrixIdentity() );
}

void Model::ShowWindow( const char* windowName ) noexcept
{
	pWindow->Show( windowName,*pRoot );
}

Model::~Model() noexcept
{}

std::unique_ptr<Mesh> Model::ParseMesh( Graphics& gfx,const aiMesh& mesh,const aiMaterial* const* pMaterial )
{
	using Dvtx::VertexLayout;

	Dvtx::VertexBuffer vbuf( std::move(
		VertexLayout{}
		.Append( VertexLayout::Position3D )
		.Append( VertexLayout::Normal )
		.Append( VertexLayout::Texture2D )
	) );

	for( unsigned int i = 0; i < mesh.mNumVertices; i++ )
	{
		vbuf.EmplaceBack(
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
		);
	}

	std::vector<unsigned short> indices;
	indices.reserve( mesh.mNumFaces * 3 );
	for( unsigned int i = 0; i < mesh.mNumFaces; i++ )
	{
		const auto& face = mesh.mFaces[i];
		assert( face.mNumIndices == 3 );
		indices.push_back( face.mIndices[0] );
		indices.push_back( face.mIndices[1] );
		indices.push_back( face.mIndices[2] );
	}

	std::vector<std::unique_ptr<Bind::Bindable>> bindablePtrs;

	if ( mesh.mMaterialIndex >= 0 )
	{
		using namespace std::string_literals;
		auto& material = *pMaterial[mesh.mMaterialIndex];
		aiString texFileName;
		material.GetTexture( aiTextureType_DIFFUSE,0,&texFileName );
		bindablePtrs.push_back( std::make_unique<Bind::Texture>( gfx,Surface::FromFile( "Models\\nano_textured\\"s + texFileName.C_Str() ) ) );
		bindablePtrs.push_back( std::make_unique<Bind::Sampler>( gfx ) );
	}

	bindablePtrs.push_back( std::make_unique<Bind::VertexBuffer>( gfx,vbuf ) );

	bindablePtrs.push_back( std::make_unique<Bind::IndexBuffer>( gfx,indices ) );

	auto pvs = std::make_unique<Bind::VertexShader>( gfx,L"PhongVS.cso" );
	auto pvsbc = pvs->GetBytecode();
	bindablePtrs.push_back( std::move( pvs ) );

	bindablePtrs.push_back( std::make_unique<Bind::PixelShader>( gfx,L"PhongPS.cso" ) );

	bindablePtrs.push_back( std::make_unique<Bind::InputLayout>( gfx,vbuf.GetLayout().GetD3DLayout(),pvsbc ) );

	struct PSMaterialConstant
	{
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[2];
	} pmc;
	bindablePtrs.push_back( std::make_unique<Bind::PixelConstantBuffer<PSMaterialConstant>>( gfx,pmc,1u ) );

	return std::make_unique<Mesh>( gfx,std::move( bindablePtrs ) );
}

std::unique_ptr<Node> Model::ParseNode( int& nextId,const aiNode& node ) noexcept
{
	const auto transform = dx::XMMatrixTranspose( dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	) );

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve( node.mNumMeshes );
	for( size_t i = 0; i < node.mNumMeshes; i++ )
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back( meshPtrs.at( meshIdx ).get() );
	}

	auto pNode = std::make_unique<Node>( nextId++,node.mName.C_Str(),std::move( curMeshPtrs ),transform );
	for( size_t i = 0; i < node.mNumChildren; i++ )
	{
		pNode->AddChild( ParseNode( nextId,*node.mChildren[i] ) );
	}

	return pNode;
}

//================================= EXCEPTIONS =================================//
ModelException::ModelException(int line, const char* file, std::string note) noexcept
	:
	ChiliException( line,file ),
	note( std::move( note ) )
{}

const char* ModelException::what() const noexcept
{
	std::ostringstream oss;
	oss << ChiliException::what() << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ModelException::GetType() const noexcept
{
	return "Chili Graphics Exception";
}

std::string ModelException::GetNote() const noexcept
{
	return note;
}

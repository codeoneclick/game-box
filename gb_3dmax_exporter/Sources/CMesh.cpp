#include "CMesh.h"
#include "CSequence.h"
#include "CSkeleton.h"

CMesh::CMesh(std::vector<IGameNode*>& gameNodes, std::vector<IGameObject*>& gameObjects) :
	m_gameNodes(gameNodes),
	m_gameObjects(gameObjects),
	m_skeleton(nullptr),
	m_sequence(nullptr)
{
	
}

CMesh::~CMesh(void)
{
	delete m_skeleton;
	m_skeleton = nullptr;
	delete m_sequence;
	m_sequence = nullptr;
}

bool CMesh::bind(void)
{
	m_skeleton = new CSkeleton(m_gameObjects);
	bool result = m_skeleton->bind();

	if(result)
	{
		result = CMesh::bindMesh();
		if(result)
		{
			m_sequence = new CSequence(m_gameObjects, m_skeleton, m_vertexData);
			result = m_sequence->bind();
		}
	}
	return result;
}

bool CMesh::bindMesh(void)
{
	if(m_gameNodes.size() == 0 || 
		m_gameObjects.size() == 0 ||
		m_gameNodes.size() != m_gameObjects.size())
	{
		return false;
	}

	i32 totalNumVertexes = 0;
	i32 totalNumTriangles = 0;

	const size_t gameObjectsCount = m_gameObjects.size();
	for(size_t k = 0; k < gameObjectsCount; ++k)
	{
		IGameNode *gameNode = m_gameNodes.at(k);
		IGameObject *gameObject = m_gameObjects.at(k);
		if(gameObject->GetIGameType() != IGameMesh::IGAME_MESH)
		{
			continue;
		}
		IGameMesh *gameMesh	= static_cast<IGameMesh*>(gameObject);
		totalNumVertexes += gameMesh->GetNumberOfVerts();
		totalNumTriangles += gameMesh->GetNumberOfFaces();
	}

	m_vertexData.clear();
	m_indexData.clear();

	m_vertexData.resize(0);
	m_indexData.resize(totalNumTriangles * 3);
	i32 indicesOffset = 0;

	for(size_t k = 0; k < gameObjectsCount; ++k)
	{
		IGameObject *gameObject = m_gameObjects.at(k);
		IGameNode *gameNode = m_gameNodes.at(k);
		std::wstring gameNodeNameW = std::wstring(gameNode->GetName());
		std::string game_node_name(gameNodeNameW.begin(), gameNodeNameW.end());
		std::cout << "name: "<< game_node_name << std::endl;
		IGameNode *parentGameNode = gameNode->GetNodeParent();
		IGameObject *parentGameObject = parentGameNode != nullptr ? parentGameNode->GetIGameObject() : nullptr;
		std::wstring parentGameNodeW = parentGameNode != nullptr ? std::wstring(parentGameNode->GetName()) : L"";
		std::string parent_game_node_name = std::string(parentGameNodeW.begin(), parentGameNodeW.end());
		std::cout <<"parent name: "<< parent_game_node_name << std::endl;

		if(gameObject->GetIGameType() != IGameMesh::IGAME_MESH)
		{
			continue;
		}

		IGameMesh *mesh = static_cast<IGameMesh*>(gameObject);

		Point3 object_position = gameNode->GetWorldTM().Translation();
		glm::vec3 parsed_object_position = glm::vec3(object_position.x, object_position.y, object_position.z);
		Quat scene_object_rotation = gameNode->GetWorldTM().Rotation();
		glm::vec3 parsed_object_rotation = glm::eulerAngles(glm::quat(scene_object_rotation.w, scene_object_rotation.x, scene_object_rotation.y, scene_object_rotation.z));
		Point3 scene_object_scale = gameNode->GetWorldTM().Scaling();
		glm::vec3 parsed_object_scale = glm::vec3(scene_object_scale.x, scene_object_scale.y, scene_object_scale.z);

		m_parsed_scene_objects.push_back(std::make_tuple(game_node_name, parent_game_node_name, parsed_object_position, parsed_object_rotation, parsed_object_scale));
		
		IGameSkin *gameSkin	= gameObject->GetIGameSkin();

		Tab<int> channels = mesh->GetActiveMapChannelNum();
		const auto channels_count = channels.Count();
		if (channels_count <= 0)
		{
			return false;
		}

		i32 num_vertices = mesh->GetNumberOfVerts();
		i32 num_faces = mesh->GetNumberOfFaces();
		
		for(i32 i = 0; i < num_faces; ++i)
		{
			FaceEx *face = mesh->GetFace(i);
			i32 face_index = face->meshFaceIndex;
			for(i32 j = 0; j < 3; ++j)
			{
				SVertex vertex;
				i32 vertex_index = face->vert[j];
				const auto position = mesh->GetVertex(vertex_index);
				vertex.m_position = glm::vec3(position.x, position.y, position.z);

				//i32 normal_index = mesh->GetFaceVertexNormal(face_index, j);
				const auto normal = mesh->GetNormal(face, j, true);
				vertex.m_normal = glm::vec3(normal.x, normal.y, normal.z);

				i32 tangent_binormal_index = mesh->GetFaceVertexTangentBinormal(face_index, j);
				const auto tangent = mesh->GetTangent(tangent_binormal_index);
				vertex.m_tangent = glm::vec3(tangent.x, tangent.y, tangent.z);

				const auto color = mesh->GetColorVertex(face->color[j]);
				vertex.m_color = glm::vec3(color.x, color.y, color.z);
				
				i32 uv_index = mesh->GetFaceTextureVertex(face_index, j);
				const auto texcoord = mesh->GetTexVertex(uv_index);
				/*const auto channel = channels[0];
				DWORD texcoord_indexes[3];
				Point3 texcoord;
				if (gameMesh->GetMapFaceIndex(channels[0], i, texcoord_indexes))
				{
					texcoord = gameMesh->GetMapVertex(channels[0], texcoord_indexes[j]);
				}
				else
				{
					texcoord = gameMesh->GetMapVertex(channels[0], indexId);
				}*/
				vertex.m_texcoord = glm::vec2(texcoord.x, texcoord.y);

				vertex.m_indexId = vertex_index;

				auto iterator = m_vertexData.begin();
				for(ui32 index = 0; iterator != m_vertexData.end(); ++iterator, ++index)
				{
					if((*iterator) == vertex)
					{
						m_indexData[3 * i + j + indicesOffset] = index;
						break;
					}
				}

				if(iterator == m_vertexData.end())
				{
					m_indexData[3 * i + j + indicesOffset] = m_vertexData.size();

					if(gameSkin != nullptr && 
						IGameSkin::IGAME_SKIN == gameSkin->GetSkinType())
					{
						i32 numWeights = gameSkin->GetNumberOfBones(vertex.m_indexId);

						if (IGameSkin::IGAME_RIGID == gameSkin->GetVertexType(vertex.m_indexId))
						{
							numWeights = 1;
						}

						for (i32 z = 0; z < numWeights; ++z)
						{
							f32 weight = gameSkin->GetWeight(vertex.m_indexId, z);
							if (numWeights == 1)
							{
								weight = 1.0f;
							}
							i32 boneId = m_skeleton->getBoneId(gameSkin->GetBoneID(vertex.m_indexId, z));
							SBoneWeight boneWeight;
							boneWeight.m_weight = weight;
							boneWeight.m_boneId = boneId;
							vertex.m_weights.push_back(boneWeight);
						}
					} else if(parentGameObject != nullptr && 
						parentGameObject->GetIGameType() == IGameMesh::IGAME_BONE)
					{
						i32 boneId =  m_skeleton->getBoneId(parentGameNode);
						SBoneWeight boneWeight;
						boneWeight.m_weight = 1.0;
						boneWeight.m_boneId = boneId;
						vertex.m_weights.push_back(boneWeight);
						std::cout<<"parent is bone"<<std::endl;
					}
					m_vertexData.push_back(vertex);
				}
			}
		}
		indicesOffset += num_faces * 3;
	}

	/*GMatrix matrix = m_gameNode->GetObjectTM(0);
	if(-1 == matrix.Parity())
	{
		std::vector<i32> swapIndexData;
		swapIndexData.resize(m_indexData.size());
		for (ui32 i = 0; i < m_indexData.size() / 3; ++i)
		{
			swapIndexData[3 * i + 0] = m_indexData[3 * i + 2];
			swapIndexData[3 * i + 1] = m_indexData[3 * i + 1];
			swapIndexData[3 * i + 2] = m_indexData[3 * i + 0];
		}
		m_indexData.swap(swapIndexData);
	}*/
	return true;
}

void CMesh::serialize(const std::string& filename)
{
	std::ofstream stream;
	stream.open(filename, std::ios::binary | std::ios::out | std::ios::trunc);
	if(!stream.is_open())
	{
		return;
	}

#if defined (__MESH__)

	i32 numVertexes = m_vertexData.size();
	i32 numIndexes = m_indexData.size();
	stream.write((char*)&numVertexes, sizeof(i32));
	stream.write((char*)&numIndexes, sizeof(i32));

	for(i32 i = 0; i < m_vertexData.size(); ++i)
	{
		stream.write((char*)&m_vertexData[i].m_position, sizeof(glm::vec3));
		stream.write((char*)&m_vertexData[i].m_normal, sizeof(glm::vec3));
		stream.write((char*)&m_vertexData[i].m_tangent, sizeof(glm::vec3));
		stream.write((char*)&m_vertexData[i].m_texcoord, sizeof(glm::vec2));
		stream.write((char*)&m_vertexData[i].m_color, sizeof(glm::vec3));

		i32 numWeights = m_vertexData[i].m_weights.size();
		stream.write((char*)&numWeights, sizeof(i32));
		for (i32 j = 0; j < numWeights; ++j)
		{
			i32 boneId = m_vertexData[i].m_weights[j].m_boneId;
			f32 weight = m_vertexData[i].m_weights[j].m_weight;
			stream.write((char*)&boneId, sizeof(i32));
			stream.write((char*)&weight, sizeof(f32));
		}
	}

	for(i32 i = 0; i < m_indexData.size(); ++i)
	{
		stream.write((char*)&m_indexData[i], sizeof(ui16));
	}

	m_skeleton->serialize(stream);
	m_sequence->serialize(stream);
	stream.close();

#elif defined(__ANIMATION__)

	m_sequence->serialize(stream);
	stream.close();

#elif defined(__SCENE__)

	i32 num_scene_objects = m_parsed_scene_objects.size();
	stream.write((char*)&num_scene_objects, sizeof(i32));

	for (auto scene_object_it = m_parsed_scene_objects.begin(); scene_object_it != m_parsed_scene_objects.end(); ++scene_object_it)
	{
		i32 name_length = std::get<0>(*scene_object_it).length();
		stream.write((char*)&name_length, sizeof(i32));
		if (name_length > 0)
		{
			stream.write(std::get<0>(*scene_object_it).c_str(), sizeof(char) * std::get<0>(*scene_object_it).length());
		}
		  
		i32 parent_name_length = std::get<1>(*scene_object_it).length();
		stream.write((char*)&parent_name_length, sizeof(i32));
		if (parent_name_length > 0)
		{
			stream.write(std::get<1>(*scene_object_it).c_str(), sizeof(char) * std::get<1>(*scene_object_it).length());
		}

		stream.write((char*)&std::get<2>(*scene_object_it), sizeof(glm::vec3));
		stream.write((char*)&std::get<3>(*scene_object_it), sizeof(glm::vec3));
		stream.write((char*)&std::get<4>(*scene_object_it), sizeof(glm::vec3));
	}
	stream.close();

#endif
}
#include "TGAFBXImporter.pch.h"
#include "FBXImporter.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "FBXImporterStructs.h"
#include "fbxsdk.h"

#include "Internal.inl"

namespace TGA
{
	//struct CrtCheckMemory
	//{
	//	_CrtMemState state1;
	//	_CrtMemState state2;
	//	_CrtMemState state3;
	//	CrtCheckMemory()
	//	{
	//		_CrtMemCheckpoint(&state1);
	//	}

	//	~CrtCheckMemory()
	//	{
	//		_CrtMemCheckpoint(&state2);

	//		if (_CrtMemDifference(&state3, &state1, &state2))
	//		{
	//			_CrtMemDumpStatistics(&state3);
	//			assert(false && "Memory leaks detected.");
	//		}
	//		_CrtMemDumpStatistics(&state3);
	//	}
	//};

	void DebugMessage(const char* fmt, ...)
	{
#ifdef _DEBUG
		va_list argList;
		va_start(argList, fmt);
		vprintf(fmt, argList);
		std::cout << std::endl;
		va_end(argList);
#endif
	}

	namespace FBX
	{
		struct VertexHash
		{
			size_t operator()(const FBXVertex& v) const
			{
				size_t result = 0;
				hash_combine(result, v.Position[0]);
				hash_combine(result, v.Position[1]);
				hash_combine(result, v.Position[2]);
				hash_combine(result, v.VertexColors[0][0]);
				hash_combine(result, v.VertexColors[0][1]);
				hash_combine(result, v.VertexColors[0][2]);
				hash_combine(result, v.VertexColors[0][3]);
				hash_combine(result, v.VertexColors[1][0]);
				hash_combine(result, v.VertexColors[1][1]);
				hash_combine(result, v.VertexColors[1][2]);
				hash_combine(result, v.VertexColors[1][3]);
				hash_combine(result, v.VertexColors[2][0]);
				hash_combine(result, v.VertexColors[2][1]);
				hash_combine(result, v.VertexColors[2][2]);
				hash_combine(result, v.VertexColors[2][3]);
				hash_combine(result, v.VertexColors[3][0]);
				hash_combine(result, v.VertexColors[3][1]);
				hash_combine(result, v.VertexColors[3][2]);
				hash_combine(result, v.VertexColors[3][3]);
				hash_combine(result, v.UVs[0][0]);
				hash_combine(result, v.UVs[0][1]);
				hash_combine(result, v.UVs[1][0]);
				hash_combine(result, v.UVs[1][1]);
				hash_combine(result, v.UVs[2][0]);
				hash_combine(result, v.UVs[2][1]);
				hash_combine(result, v.UVs[3][0]);
				hash_combine(result, v.UVs[3][1]);
				hash_combine(result, v.Normal[0]);
				hash_combine(result, v.Normal[1]);
				hash_combine(result, v.Normal[2]);
				hash_combine(result, v.Tangent[0]);
				hash_combine(result, v.Tangent[1]);
				hash_combine(result, v.Tangent[2]);
				hash_combine(result, v.Binormal[0]);
				hash_combine(result, v.Binormal[1]);
				hash_combine(result, v.Binormal[2]);

				return result;
			}

		private:

			template<typename T>
			inline void hash_combine(size_t& s, const T& v) const
			{
				std::hash<T> h;
				s^=h(v) + 0x9e3779b9 + (s<< 6) + (s>> 2);
			}
		};

		struct VertexDuplicateData
		{
			unsigned int Idx;
			size_t Hash;
		};

		unsigned int GatherMeshes(FbxNode* aNode, std::vector<FbxNode*>& outMeshes)
		{
			// For each child of our current node...
			for(int i = 0; i < aNode->GetChildCount(); i++)
			{
		        // Get the node attribute if it has one, the data that denotes what type this node is.
		        // If there is no type it's usually just an organizational node (folder).
				FbxNode* childNode = aNode->GetChild(i);
		        FbxNodeAttribute* childAttribute = childNode->GetNodeAttribute();
		        if(!childAttribute)
		        {
		            // Even if it's not a valid node, step down since it may contain meshes.
			        GatherMeshes(childNode, outMeshes);
		        }            
		        else
		        {
		            // Check if this is a mesh node.
			        FbxNodeAttribute::EType childAttributeType = childAttribute->GetAttributeType();
		            if(childAttributeType != FbxNodeAttribute::eMesh)
		            {
		                // If it's not, step down and look for meshes underneath.
			            GatherMeshes(childNode, outMeshes);
		            }
		            else
		            {
		                // Otherwise gather the node.
			            outMeshes.push_back(childNode);
		                GatherMeshes(childNode, outMeshes);
		            }
		        }
			}

		    // Good idea to return how many meshes we found. Might be useful later.
		    return static_cast<unsigned int>(outMeshes.size());
		}

		int GatherSkeletonInternal(FbxNode* aNode, FBXSkeleton& outSkeleton, std::string& outMessage, int someParentIdx = -1)
		{
			// Check if this node is a skeleton type.
		    int myIdx = someParentIdx;
		    if (aNode->GetNodeAttribute() && aNode->GetNodeAttribute()->GetAttributeType() && aNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		    {
		        // If it is we'll make a joint and add it to the skeleton.
		        // It's also important to keep track of the name of the joint
		        // since some types of data is stored with names instead of IDs.

    			FBXSkeleton::Bone bone;
		        bone.Name = aNode->GetName();
		        bone.Parent = someParentIdx;

		        outSkeleton.Bones.push_back(bone);
		        myIdx = static_cast<int>(outSkeleton.Bones.size() - 1ULL);

				if(outSkeleton.BoneNameToIndex.find(bone.Name) != outSkeleton.BoneNameToIndex.end())
				{
					char stringBuffer[256]{};
					sprintf_s(stringBuffer, 256, "Found more than one bone with the same name! Name was %s", bone.Name.c_str());
					outMessage = stringBuffer;
					return -2;
				}

		        outSkeleton.BoneNameToIndex.insert({ bone.Name, myIdx });

		        if(someParentIdx >= 0)
		        {
			        outSkeleton.Bones[someParentIdx].Children.push_back(myIdx);
		        }
		    }

		    // And keep stepping down to gather all children.
		    for (int i = 0; i < aNode->GetChildCount(); i++)
		    {
		        const int result = GatherSkeletonInternal(aNode->GetChild(i), outSkeleton, outMessage, myIdx);
				if(result != 0)
		        {
			        return result;
		        }
		    }

		    return 0;
		}

		int GatherSkeletonData(FbxNode* aNode, FBXSkeleton& outSkeleton, std::string& outMessage, int someParentIdx = -1)
		{
			GatherSkeletonInternal(aNode, outSkeleton, outMessage, someParentIdx);
			if(outMessage.empty() && outSkeleton.Bones.empty())
			{
				outMessage = "No skeleton found";
				return -1;
			}

			return 0;
		}

		void GetElementMappingdata(FbxLayerElementTemplate<FbxVector4>* anElement, int aFbxContolPointIdx, int aPolygonIdx, FbxVector4& outData)
		{
			switch(anElement->GetMappingMode())
			{
			case FbxGeometryElement::eByControlPoint:
			    {
			        switch(anElement->GetReferenceMode())
			        {
			        case FbxGeometryElement::eDirect:
			            {
			                outData.mData[0] = anElement->GetDirectArray().GetAt(aFbxContolPointIdx).mData[0];
			                outData.mData[1] = anElement->GetDirectArray().GetAt(aFbxContolPointIdx).mData[1];
			                outData.mData[2] = anElement->GetDirectArray().GetAt(aFbxContolPointIdx).mData[2];
			            }
			            break;
			        case FbxGeometryElement::eIndexToDirect:
			            {
			                const int Idx = anElement->GetIndexArray().GetAt(aFbxContolPointIdx);
			                outData.mData[0] = anElement->GetDirectArray().GetAt(Idx).mData[0];
			                outData.mData[1] = anElement->GetDirectArray().GetAt(Idx).mData[1];
			                outData.mData[2] = anElement->GetDirectArray().GetAt(Idx).mData[2];
			            }
			            break;
			        default:
			            throw std::exception("Invalid Reference Mode!");
			        }
			    }
			    break;

			case FbxGeometryElement::eByPolygonVertex:
			    {
			        switch(anElement->GetReferenceMode())
			        {
			        case FbxGeometryElement::eDirect:
			            {
			                outData.mData[0] = anElement->GetDirectArray().GetAt(aPolygonIdx).mData[0];
			                outData.mData[1] = anElement->GetDirectArray().GetAt(aPolygonIdx).mData[1];
			                outData.mData[2] = anElement->GetDirectArray().GetAt(aPolygonIdx).mData[2];
			            }
			            break;
			        case FbxGeometryElement::eIndexToDirect:
			            {
			                const int Idx = anElement->GetIndexArray().GetAt(aPolygonIdx);
			                outData.mData[0] = anElement->GetDirectArray().GetAt(Idx).mData[0];
			                outData.mData[1] = anElement->GetDirectArray().GetAt(Idx).mData[1];
			                outData.mData[2] = anElement->GetDirectArray().GetAt(Idx).mData[2];
			            }
			            break;
			        default:
			            throw std::exception("Invalid Reference Mode!");
			        }
			    }
			    break;
			}
		}

		Matrix4x4f ConvertFBXMatrix(fbxsdk::FbxAMatrix anFbxMatrix)
		{
			Matrix4x4f result = Matrix4x4f::CreateIdentityMatrix();
			
			for (int i = 0; i < 4; i++)
			{
				// The FBX Matrix might need a Transpose!
				fbxsdk::FbxVector4 row = anFbxMatrix.GetRow(i);
				result(i + 1, 1) = static_cast<float>(row[0]);
				result(i + 1, 2) = static_cast<float>(row[1]);
				result(i + 1, 3) = static_cast<float>(row[2]);
				result(i + 1, 4) = static_cast<float>(row[3]);
			}

			return result;
		}

		void ConvertFBXMatrixToArray(fbxsdk::FbxAMatrix anFbxMatrix, float anArray[16])
		{
			Matrix4x4f result = Matrix4x4f::CreateIdentityMatrix();
			
			for (int i = 0; i < 4; i++)
			{
				// The FBX Matrix might need a Transpose!
				fbxsdk::FbxVector4 row = anFbxMatrix.GetRow(i);
				result(i + 1, 1) = static_cast<float>(row[0]);
				result(i + 1, 2) = static_cast<float>(row[1]);
				result(i + 1, 3) = static_cast<float>(row[2]);
				result(i + 1, 4) = static_cast<float>(row[3]);
			}

			memcpy(&anArray[0], result.GetDataPtr(), sizeof(float) * 16);
		}

		std::string MatrixToString(const Matrix& matrix)
		{
			char buffer[256]{};
			sprintf_s(buffer, 256, "M11: \t%.3f\t M12: \t%.3f\t M13: \t%.3f\t M14: \t%.3f\t \nM21: \t%.3f\t M22: \t%.3f\t M23: \t%.3f\t M24: \t%.3f\t \nM31: \t%.3f\t M32: \t%.3f\t M33: \t%.3f\t M34: \t%.3f\t \nM41: \t%.3f\t M42: \t%.3f\t M43: \t%.3f\t M44: \t%.3f\t"
				,matrix.Data[0], matrix.Data[1],matrix.Data[2],matrix.Data[3],matrix.Data[4],matrix.Data[5],matrix.Data[6], matrix.Data[7]
				,matrix.Data[8],matrix.Data[9],matrix.Data[10],matrix.Data[11],matrix.Data[12],matrix.Data[13],matrix.Data[14],matrix.Data[15]);

			return buffer;
		}

		FBXTexture GetMaterialChannelData(const FbxSurfaceMaterial* aMaterial, const char* aChannel)
		{
			FBXTexture result;
			const FbxProperty channelProperty = aMaterial->FindProperty(aChannel);
			const int layerCount = channelProperty.GetSrcObjectCount<FbxLayeredTexture>();
			if(layerCount > 0)
			{
				// We don't use these, hua.
			}
			else
			{
				const int textureCount = channelProperty.GetSrcObjectCount<FbxTexture>();
				if(textureCount > 0)
				{
					// We should only care about the first texture here presumably.
					if(FbxTexture* texture = FbxCast<FbxTexture>(channelProperty.GetSrcObject<FbxTexture>(0)))
					{
						if(const FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(texture))
						{
							result.Path = fileTexture->GetFileName();
							result.RelativePath = fileTexture->GetRelativeFileName();

							const size_t startPos = result.RelativePath.find_last_of('\\');
							if(startPos != std::string::npos)
								result.Name = result.RelativePath.substr(startPos + 1, result.RelativePath.size() - startPos);
							else
								result.Name = result.RelativePath;
						}
					}											
				}
			}

			return result;
		}

		void GetMaterialData(const FbxSurfaceMaterial* aMaterial, FBXMaterial& outMaterial)
		{
			outMaterial.MaterialName = aMaterial->GetName();
			outMaterial.Ambient = GetMaterialChannelData(aMaterial, FbxSurfaceMaterial::sAmbient);
			outMaterial.Bump = GetMaterialChannelData(aMaterial, FbxSurfaceMaterial::sBump);
			outMaterial.Diffuse = GetMaterialChannelData(aMaterial, FbxSurfaceMaterial::sDiffuse);
			outMaterial.Displacement = GetMaterialChannelData(aMaterial, FbxSurfaceMaterial::sDisplacementColor);
			outMaterial.Emissive = GetMaterialChannelData(aMaterial, FbxSurfaceMaterial::sEmissive);
			outMaterial.NormalMap = GetMaterialChannelData(aMaterial, FbxSurfaceMaterial::sNormalMap);
			outMaterial.Reflection = GetMaterialChannelData(aMaterial, FbxSurfaceMaterial::sReflection);
			outMaterial.Specular = GetMaterialChannelData(aMaterial, FbxSurfaceMaterial::sSpecular);
			outMaterial.TransparentColor = GetMaterialChannelData(aMaterial, FbxSurfaceMaterial::sTransparentColor);
			outMaterial.VectorDisplacement = GetMaterialChannelData(aMaterial, FbxSurfaceMaterial::sVectorDisplacementColor);
			outMaterial.Shininess = GetMaterialChannelData(aMaterial, FbxSurfaceMaterial::sShininess);
		}
	}

	class Internals
	{
		Internals()
			: fbxManager(FbxManager::Create())
			, fbxIOSettings(FbxIOSettings::Create(fbxManager, "TGA"))
			, fbxImporter(FbxImporter::Create(fbxManager, ""))
			, fbxConverter(fbxManager)
			, fbxAxisSystem(FbxAxisSystem::eDirectX)
		{
			fbxIOSettings->SetBoolProp(IMP_FBX_TEXTURE, false);
			fbxIOSettings->SetBoolProp(IMP_FBX_EXTRACT_EMBEDDED_DATA, false);
		}

	public:
		static Internals& Get() { static Internals theInstance;  return theInstance;  }

		FbxManager* fbxManager = nullptr;
		FbxIOSettings* fbxIOSettings = nullptr;
		FbxGeometryConverter fbxConverter;
		FbxAxisSystem fbxAxisSystem;
		FbxImporter* fbxImporter = nullptr;

		void Destroy()
		{
			fbxManager->Destroy();
		}
	};

	bool FBXImporter::LoadModel(const std::string& someFilePath, FBXModel& outModel)
	{
		FbxImporter* fbxImporter = FbxImporter::Create(Internals::Get().fbxManager, "");

		// Used to denote a local error not detectable via FbxStatus.
		bool bImporterError = false;

		if(!fbxImporter->Initialize(someFilePath.c_str(), -1, Internals::Get().fbxIOSettings))
		{
			const FbxStatus& Status = fbxImporter->GetStatus();
			myLastError = Status.GetErrorString();
			fbxImporter->Destroy();
			return false;
		}			

		if(!fbxImporter->IsFBX())
		{
			const FbxStatus& Status = fbxImporter->GetStatus();
			myLastError = Status.GetErrorString();
			fbxImporter->Destroy();
			return false;
		}			

		FbxScene* fbxScene = FbxScene::Create(Internals::Get().fbxManager, "The Scene");

		if(fbxImporter->Import(fbxScene))
		{
			fbxScene->ConnectMaterials();

			if(!Internals::Get().fbxConverter.Triangulate(fbxScene, true, true))
	        {
		        // Failed to use the faster legacy method so try to use the new method
				DebugMessage("Model failed quick Triangulation, using advanced (slower).");
	            Internals::Get().fbxConverter.Triangulate(fbxScene, true, false);
	        }

			std::vector<FbxNode*> mdlMeshNodes;
			const unsigned int numSceneMaterials = fbxScene->GetMaterialCount();
			mdlMeshNodes.reserve(numSceneMaterials);
			outModel.Materials.resize(numSceneMaterials);
	        const unsigned int numMeshes = FBX::GatherMeshes(fbxScene->GetRootNode(), mdlMeshNodes);

			float maxExtents[3] = {0, 0, 0};
			float minExtents[3] = {0, 0, 0};

			for(FbxNode* mdlMeshNode : mdlMeshNodes)
			{
				FbxMesh* fbxMesh = mdlMeshNode->GetMesh();
				
				if(fbxMesh->GetElementBinormalCount() == 0 || fbxMesh->GetElementTangentCount() == 0)
				{
					if(!fbxMesh->GenerateTangentsData(0, true, false))
					{
						myLastError = "Failed to generate Tangent/Binormal data for the mesh " + std::string(fbxMesh->GetName()) + "!";
						fbxImporter->Destroy();
						return false;
					}
				}
			}

			// This has to happen after ALL changes have been made to the scene. I.e. you need to
			// run Triangulate and generate missing Tangents and Binormals before you run this or
			// generated things will point in the wrong direction.
			Internals::Get().fbxAxisSystem.DeepConvertScene(fbxScene);

			FBXSkeleton mdlSkeleton;
			bool hasBones = false;
			std::string gatherBonesMessage;
			switch(FBX::GatherSkeletonData(fbxScene->GetRootNode(), mdlSkeleton, gatherBonesMessage))
			{
			case 0: // A skeleton was found
				// Try to extract a sensible skeleton name.
				// First check if we have a namespace to utilize.
				if(const FBXSkeleton::Bone* rootBone = mdlSkeleton.GetRoot())
				{
					const std::string rootBoneName = rootBone->Name;
					if(const size_t pos = rootBoneName.find_first_of(':'); pos != std::string::npos)
					{
						mdlSkeleton.Name = rootBoneName.substr(0, pos);
					}
					else
					{
						// We have no namespace to use so we'll just use the model name.
						std::string filePathCopy = someFilePath;
						std::replace(filePathCopy.begin(), filePathCopy.end(), '/', '\\');
						if(const size_t slashPos = filePathCopy.find_last_of('\\'); slashPos != std::string::npos)
						{
							filePathCopy = filePathCopy.substr(slashPos + 1, filePathCopy.length() - (slashPos - 1));
						}
						mdlSkeleton.Name = filePathCopy.substr(0, filePathCopy.length() - 4);
					}
				}

				hasBones = true;
				break;
			case -1: // No skeleton found, which isn't an error in this case.
				break;
			default:
				// Something went wrong.
				myLastError = gatherBonesMessage;
				bImporterError = true;
				break;
			}

			if(!bImporterError)
			{
				std::unordered_multimap<unsigned int, std::pair<size_t, float>> ControlPointWeights;

				std::vector<FBXModel::FBXMesh> mdlMeshData;
				mdlMeshData.reserve(numMeshes * numSceneMaterials);

				for(FbxNode* mdlMeshNode : mdlMeshNodes)
				{
					const unsigned int numMeshMaterials = mdlMeshNode->GetMaterialCount();
					FbxMesh* fbxMesh = mdlMeshNode->GetMesh();

					if (hasBones)
					{
						// Load the FBX root transform. This is something provided by the modeling software.
		                // All we need to do is assemble it.
		                const FbxVector4 fbxTranslation = mdlMeshNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		                const FbxVector4 fbxRotation = mdlMeshNode->GetGeometricRotation(FbxNode::eSourcePivot);
		                const FbxVector4 fbxScale = mdlMeshNode->GetGeometricScaling(FbxNode::eSourcePivot);
		                const fbxsdk::FbxAMatrix rootTransform = FbxAMatrix(fbxTranslation, fbxRotation, fbxScale);

		                // The FBX world has many different kinds of deformers. They can be skeleton bones but
		                // they can also be several other things such as forces, effects, scripts, warps, what not.
		                // The deformer that deforms a mesh based on bones is called Skin.
		                for (int deformIdx = 0; deformIdx < fbxMesh->GetDeformerCount(); deformIdx++)
		                {
		                    FbxSkin* fbxSkin = reinterpret_cast<FbxSkin*>(fbxMesh->GetDeformer(deformIdx, FbxDeformer::eSkin));

		                    // If it's not a skin keep looking.
		                    if (!fbxSkin)
		                        continue;

		                    // The Skin deform is made up out of skin clusters. These are for all intents
		                    // and purposes the joints that affect that section of the skin.
		                    for (int clusterIdx = 0; clusterIdx < fbxSkin->GetClusterCount(); clusterIdx++)
		                    {
		                        // So we go through each cluster.
		                        FbxCluster* fbxCluster = fbxSkin->GetCluster(clusterIdx);

		                        // We get the transform of the cluster that was present at skin binding time.
		                        // This is the "resting pose" if you like.
		                        fbxsdk::FbxAMatrix meshBindTransform;
		                        fbxCluster->GetTransformMatrix(meshBindTransform);

		                        // We also need the link transform. In the case of a Skin it will be the
		                        // transform to go from local Joint Space to World Space.
		                        fbxsdk::FbxAMatrix linkTransform;
		                        fbxCluster->GetTransformLinkMatrix(linkTransform);
		                        
		                        // And finally assemble the Bind Pose Transform.
		                        // This is the default position of the vertices.
		                        fbxsdk::FbxAMatrix bindPoseInverseTransform = linkTransform.Inverse() * meshBindTransform * rootTransform;
		                        // Remember to do this or you will get very interesting results :).
		                        bindPoseInverseTransform = bindPoseInverseTransform.Transpose();

		                        // The Link in the skin cluster is the actual joint.
		                        // Because we already collected all joints we should be able to find it using
		                        // the acceleration map in the Skeleton.
		                        size_t jointIndex = mdlSkeleton.BoneNameToIndex[fbxCluster->GetLink()->GetName()];
		                        
		                        // Store the bind pose on the joint so we can access it later.
								FBX::ConvertFBXMatrixToArray(bindPoseInverseTransform, mdlSkeleton.Bones[jointIndex].BindPoseInverse.Data);

		                        // Here comes some more control point stuff.
		                        // We need to collect all the control points that this skin cluster affects.
		                        // And for those we need to store which joint affects it and its weights.
								for (int i = 0; i < fbxCluster->GetControlPointIndicesCount(); i++)
		                        {
		                            const unsigned int c = fbxCluster->GetControlPointIndices()[i];
		                            const float w = static_cast<float>(fbxCluster->GetControlPointWeights()[i]);
		                            // This inserts into a multimap.
		                            // c - control point index.
		                            // jointIndex - self explanatory.
		                            // w - the weight for this joint.
		                            ControlPointWeights.insert({ c, { jointIndex, w }});
		                        }
		                    }
		                }
					}

					// DB: Removed 220419 - Generated Bi/Tangents are in the wrong direction if you do this after running DeepConvert.
					//if(fbxMesh->GetElementBinormalCount() == 0 || fbxMesh->GetElementTangentCount() == 0)
					//{
					//    bool result = fbxMesh->GenerateTangentsData(0, true, false);
					//	// This shouldn't explode since result only returns false if there is data and overwrite is false.
					//    assert(result);
					//}

					const bool bHasMaterials = mdlMeshNode->GetMaterialCount() != 0;
					FbxSurfaceMaterial* currentSceneMaterial = nullptr;

					for(int meshMaterialIndex = 0; meshMaterialIndex < mdlMeshNode->GetMaterialCount() || meshMaterialIndex == 0; meshMaterialIndex++)
					{
						FBXModel::FBXMesh meshData = {};
						meshData.MeshName = mdlMeshNode->GetName();

            			if(bHasMaterials)
		                {
			                for( int sceneMaterialIndex = 0; sceneMaterialIndex < fbxScene->GetMaterialCount(); sceneMaterialIndex++)
			                {
		                        FbxSurfaceMaterial* sceneMaterial = fbxScene->GetMaterial(sceneMaterialIndex);
		                        FbxSurfaceMaterial* meshNodeMaterial = mdlMeshNode->GetMaterial(meshMaterialIndex);
				                if(sceneMaterial == meshNodeMaterial)
				                {
		                            currentSceneMaterial = sceneMaterial;
					                meshData.MaterialIndex = sceneMaterialIndex;
									if(outModel.Materials[sceneMaterialIndex].MaterialName.empty())
									{
										// This material probably doesn't exist so we need to create it.
										FBX::GetMaterialData(sceneMaterial, outModel.Materials[sceneMaterialIndex]);
									}

		                            break;
				                }
			                }
		                }
		                else
		                {
			                meshData.MaterialIndex = 0;
		                }

						FbxLayerElementMaterial* fbxElementMaterial = fbxMesh->GetElementMaterial();
						//FbxLayerElement::EMappingMode elementMappingMode =  fbxElementMaterial->GetMappingMode();

						const int fbxMeshPolygonCount = fbxMesh->GetPolygonCount();

						meshData.Vertices.reserve(static_cast<size_t>(fbxMeshPolygonCount) * 3ULL);

						meshData.Indices.reserve(meshData.Vertices.capacity());

						std::unordered_map<size_t, FBX::VertexDuplicateData> VertexDuplicateAccelMap;
						unsigned int IndexCounter = 0;
						VertexDuplicateAccelMap.reserve(meshData.Vertices.capacity());

						for(int p = 0; p < fbxMeshPolygonCount; p++)
						{
							if(bHasMaterials)
		                    {
		                        // This is the index of the materials in the mesh element array.
		                        // It doesn't always correspond to the scene material list since the first
		                        // material here might be material n in the scene.
		                        const int IndexAtP = fbxElementMaterial->GetIndexArray().GetAt(p);
		                        FbxSurfaceMaterial* polygonMaterial = mdlMeshNode->GetMaterial(IndexAtP);
		                        if(currentSceneMaterial != polygonMaterial)
		                        {
			                        continue;
		                        }
		                    }

							for(int v = 0; v < 3; v++)
							{
								const int fbxControlPtIndex = fbxMesh->GetPolygonVertex(p, v);
								const FbxVector4 fbxVxPos = fbxMesh->GetControlPointAt(fbxControlPtIndex);

								FbxVector2 fbxVxUVs[4];
								const int fbxNumUVs = fbxMesh->GetElementUVCount();

								const int fbxTextureUVIndex = fbxMesh->GetTextureUVIndex(p, v);
			                    for(int uv = 0; uv < fbxNumUVs && uv < 4; uv++)
			                    {
				                    FbxGeometryElementUV* fbxUvElement = fbxMesh->GetElementUV(uv);
									fbxVxUVs[uv] = fbxUvElement->GetDirectArray().GetAt(fbxTextureUVIndex);
			                    }

								const int polygonIndex = p * 3 + v;

								FbxVector4 fbxNormal;
								FbxGeometryElementNormal* fbxNormalElement = fbxMesh->GetElementNormal(0);
								FBX::GetElementMappingdata(fbxNormalElement, fbxControlPtIndex, polygonIndex, fbxNormal);

								FbxVector4 fbxTangent;
			                    FbxGeometryElementTangent* fbxTangentElement = fbxMesh->GetElementTangent(0);
								FBX::GetElementMappingdata(fbxTangentElement, fbxControlPtIndex, polygonIndex, fbxTangent);

								FbxVector4 fbxBinormal;
								FbxGeometryElementBinormal* fbxBinormalElement = fbxMesh->GetElementBinormal(0);
								FBX::GetElementMappingdata(fbxBinormalElement, fbxControlPtIndex, polygonIndex, fbxBinormal);

								// BUG: FBX SDK 2020 doesn't handle winding for these for some reason. Even when we
		                        // BUG: use DeepConvert. This has been reported and is on their fix list. Be aware
		                        // BUG: of this if there's an SDK upgrade and things look weird.

		                        int windCorrection;
		                        switch(polygonIndex)
				                {
		                			case 1:
		                                windCorrection = 2;
										break;
		                			case 2:
		                                windCorrection = 1;
										break;
									default:
		                                windCorrection = polygonIndex;
										break;
				                }

								const int windedPolygonIndex = p * 3 + windCorrection;

								FbxColor fbxColors[4];
			                    const int fbxNumVxColorChannels = fbxMesh->GetElementVertexColorCount();
			                    for(int col = 0; col < fbxNumVxColorChannels && col < 4; col++)
				                {
					                FbxGeometryElementVertexColor* colElement = fbxMesh->GetElementVertexColor(col);
									switch(colElement->GetMappingMode())
									{
									case FbxGeometryElement::eByControlPoint:
									    {
									        switch(fbxNormalElement->GetReferenceMode())
									        {
									        case FbxGeometryElement::eDirect:
									            {
									                fbxColors[col] = colElement->GetDirectArray().GetAt(fbxControlPtIndex);
									            }
									            break;
									        case FbxGeometryElement::eIndexToDirect:
									            {
									                const int Idx = colElement->GetIndexArray().GetAt(fbxControlPtIndex);
									                fbxColors[col] = colElement->GetDirectArray().GetAt(Idx);
									            }
									            break;
									        default:
									            throw std::exception("Invalid Reference Mode!");
									        }
									    }
									    break;

									case FbxGeometryElement::eByPolygonVertex:
									    {
									        switch(colElement->GetReferenceMode())
									        {
									        case FbxGeometryElement::eDirect:
									            {
													fbxColors[col] = colElement->GetDirectArray().GetAt(windedPolygonIndex);
									            }
									            break;
									        case FbxGeometryElement::eIndexToDirect:
									            {
									                const int Idx = colElement->GetIndexArray().GetAt(windedPolygonIndex);
													fbxColors[col] = colElement->GetDirectArray().GetAt(Idx);
									            }
									            break;
									        default:
									            throw std::exception("Invalid Reference Mode!");
									        }
									    }
									    break;
									}
				                }

								FBXVertex vx =
								{
									static_cast<float>(fbxVxPos[0]),
			                        static_cast<float>(fbxVxPos[1]),
			                        static_cast<float>(fbxVxPos[2]),
			                        static_cast<float>(fbxNormal[0]),
			                        static_cast<float>(fbxNormal[1]),
			                        static_cast<float>(fbxNormal[2]),
			                        static_cast<float>(fbxTangent[0]),
			                        static_cast<float>(fbxTangent[1]),
			                        static_cast<float>(fbxTangent[2]),
			                        static_cast<float>(fbxBinormal[0]),
			                        static_cast<float>(fbxBinormal[1]),
			                        static_cast<float>(fbxBinormal[2]),
			                        static_cast<float>(fbxColors[0][0]),
			                        static_cast<float>(fbxColors[0][1]),
			                        static_cast<float>(fbxColors[0][2]),
			                        static_cast<float>(fbxColors[0][3]),
			                        static_cast<float>(fbxVxUVs[0][0]),
			                        1 - static_cast<float>(fbxVxUVs[0][1])
								};

								if(vx.Position[0] > maxExtents[0])
									maxExtents[0] = vx.Position[0];
								if(vx.Position[1] > maxExtents[1])
									maxExtents[1] = vx.Position[1];
								if(vx.Position[2] > maxExtents[2])
									maxExtents[2] = vx.Position[2];

								if(vx.Position[0] < minExtents[0])
									minExtents[0] = vx.Position[0];
								if(vx.Position[1] < minExtents[1])
									minExtents[1] = vx.Position[1];
								if(vx.Position[2] < minExtents[2])
									minExtents[2] = vx.Position[2];

								unsigned int BoneIDs[] = {0, 0, 0, 0};
			                    float BoneWeights[] = { 0, 0, 0, 0};
			                    // Get animation weight data
			                    if(hasBones)
			                    {
			                        // Since we're making a bit of a complex iteration we need to define the iterator.
			                        // It's a lot less to type that way.
			                        typedef std::unordered_multimap<unsigned int, std::pair<size_t, float>>::iterator MMIter;

			                        // Then we use equal range to get all the data stored for this specific control point.
			                        std::pair<MMIter, MMIter> values = ControlPointWeights.equal_range(fbxControlPtIndex);

			                        // This idx is to loop on the 4 indices of ID and Weight.
									int idx = 0;
									for(MMIter it = values.first; it != values.second && idx < 4; ++it)
			                        {
				                        std::pair<size_t, float> BoneAndWeight = it->second;
			                            BoneIDs[idx] = static_cast<unsigned>(BoneAndWeight.first);
			                            BoneWeights[idx] = BoneAndWeight.second;
                        				idx++;
			                        }
			                    }

								vx.BoneIDs[0] = BoneIDs[0];
								vx.BoneIDs[1] = BoneIDs[1];
								vx.BoneIDs[2] = BoneIDs[2];
								vx.BoneIDs[3] = BoneIDs[3];

				                vx.BoneWeights[0] = BoneWeights[0];
								vx.BoneWeights[1] = BoneWeights[1];
								vx.BoneWeights[2] = BoneWeights[2];
								vx.BoneWeights[3] = BoneWeights[3];

			                    // A drawback of using control points is that we MAY get duplicate vertices.
			                    // This means we'll need to compare and ensure that it is a unique vert.
								FBX::VertexHash Hasher;
			                    size_t hash = Hasher(vx);
			                    if(VertexDuplicateAccelMap.find(hash) == VertexDuplicateAccelMap.end())
			                    {
			                        VertexDuplicateAccelMap[hash] = { /*{ vx },*/ IndexCounter, hash };
			                        meshData.Vertices.push_back(vx);
			                        meshData.Indices.push_back(IndexCounter++);
			                    }
			                    else
			                    {
			                        meshData.Indices.push_back(VertexDuplicateAccelMap[hash].Idx);
			                    }
							}
						}

						if(!meshData.Vertices.empty())
						{
							mdlMeshData.push_back(meshData);
						}

						VertexDuplicateAccelMap.clear();
					}

					ControlPointWeights.clear();
				}

				if(mdlMeshData.empty())
				{
					myLastError = "No mesh data was found!";
					bImporterError = true;
				}
				else
				{
					outModel.Name = someFilePath;
					outModel.Meshes = std::move(mdlMeshData);
					outModel.Skeleton = std::move(mdlSkeleton);
					float extentsCenter[3];
					extentsCenter[0] = 0.5f * minExtents[0] + maxExtents[0];
					extentsCenter[1] = 0.5f * minExtents[1] + maxExtents[1];
					extentsCenter[2] = 0.5f * minExtents[2] + maxExtents[2];

					float boxExtents[3];
					boxExtents[0] = 0.5f * maxExtents[0] - minExtents[0];
					boxExtents[1] = 0.5f * maxExtents[1] - minExtents[1];
					boxExtents[2] = 0.5f * maxExtents[2] - minExtents[2];

					float extentRadius = std::max(boxExtents[0], std::max(boxExtents[1], boxExtents[2]));

					outModel.BoxSphereBounds = {
						{boxExtents[0], boxExtents[1], boxExtents[2]},
						{extentsCenter[0], extentsCenter[1], extentsCenter[2]},
						extentRadius
					};
				}
			}
		}

		const FbxStatus& Status = fbxImporter->GetStatus();

		if(Status.Error())
			myLastError = Status.GetErrorString();
		else if (!bImporterError)
			myLastError = "Success";

		// Gotta extract this since Destroy will delete the Status object.
		const bool bResult = !(Status.Error() || bImporterError);

		fbxScene->Destroy(false);
		fbxImporter->Destroy();

		return bResult;
	}

	bool FBXImporter::IsValidModelFile(const std::string& someFilePath)
	{
		FbxImporter* fbxImporter = FbxImporter::Create(Internals::Get().fbxManager, "");

		if(!fbxImporter->Initialize(someFilePath.c_str(), -1, Internals::Get().fbxIOSettings))
		{
			const FbxStatus& Status = fbxImporter->GetStatus();
			myLastError = Status.GetErrorString();
			fbxImporter->Destroy();
			return false;
		}			

		if(!fbxImporter->IsFBX())
		{
			const FbxStatus& Status = fbxImporter->GetStatus();
			myLastError = Status.GetErrorString();
			fbxImporter->Destroy();
			return false;
		}

		FbxScene* fbxScene = FbxScene::Create(Internals::Get().fbxManager, "The Scene");

		if(fbxImporter->Import(fbxScene))
		{
			std::vector<FbxNode*> mdlMeshNodes;
	        const unsigned int numMeshes = FBX::GatherMeshes(fbxScene->GetRootNode(), mdlMeshNodes);
			return numMeshes > 0;
		}

		const FbxStatus& Status = fbxImporter->GetStatus();

		if(Status.Error())
			myLastError = Status.GetErrorString();
		else
			myLastError = "Success";

		// Gotta extract this since Destroy will delete the Status object.
		const bool bResult = !Status.Error();

		fbxScene->Destroy(false);
		fbxImporter->Destroy();

		return bResult;
	}

	bool FBXImporter::LoadAnimation(const std::string& someFilePath, const std::vector<std::string>& BoneNames, FBXAnimation& outAnimation)
	{
		FbxImporter* fbxImporter = FbxImporter::Create(Internals::Get().fbxManager, "");

		// Used to denote a local error not detectable via FbxStatus.
		bool bImporterError = false;

		if(BoneNames.empty())
		{
			myLastError = "No joint names provided!";
			return false;
		}

		if(!fbxImporter->Initialize(someFilePath.c_str(), -1, Internals::Get().fbxIOSettings))
		{
			const FbxStatus& Status = fbxImporter->GetStatus();
			myLastError = Status.GetErrorString();
			fbxImporter->Destroy();
			return false;
		}

		if(!fbxImporter->IsFBX())
		{
			const FbxStatus& Status = fbxImporter->GetStatus();
			myLastError = Status.GetErrorString();
			fbxImporter->Destroy();
			return false;
		}

		FbxScene* fbxScene = FbxScene::Create(Internals::Get().fbxManager, "The Scene");

		if(fbxImporter->Import(fbxScene))
		{
			Internals::Get().fbxAxisSystem.DeepConvertScene(fbxScene);

			// Verify that the root of the skeleton is named the same.
			// This could be more extensive but should not require a 1-1 skeleton
			// since we may have different face rigs etc.
			const ::FbxNode* skeletonRoot = fbxScene->FindNodeByName(FbxString(BoneNames[0].c_str()));

			const FbxVector4 fbxTranslation = skeletonRoot->GetGeometricTranslation(::FbxNode::eSourcePivot);
			const FbxVector4 fbxRotation = skeletonRoot->GetGeometricRotation(::FbxNode::eSourcePivot);
			const FbxVector4 fbxScale = skeletonRoot->GetGeometricScaling(::FbxNode::eSourcePivot);
			const FbxAMatrix rootTransform = FbxAMatrix(fbxTranslation, fbxRotation, fbxScale);

			const int animStackCount = fbxImporter->GetAnimStackCount();
			if(animStackCount == 0)
			{
				myLastError = "No anim stack found in file!";
				bImporterError = true;
			}
			else
			{
				if(const FbxAnimStack* fbxAnimStack = fbxScene->GetSrcObject<FbxAnimStack>(0))
				{
					// We get the name of it since Motion Builder can have several.
					FbxString animStackName = fbxAnimStack->GetName();
					std::string animationName = animStackName.Buffer();

					// Then we load the Take for this animation. This can only be a single one in Maya
					// but Motion Builder can have several! There's no standard at TGA saying we should
					// support multiple right now, though. Check with the animators!

					if (const FbxTakeInfo* fbxTakeInfo = fbxScene->GetTakeInfo(animStackName))
					{
						const FbxTime::EMode fbxGlobalTimeMode = fbxScene->GetGlobalSettings().GetTimeMode();
						const double framesPerSecond = FbxTime::GetFrameRate(fbxGlobalTimeMode);

						// Figure out the duration of the animation.
						const FbxTime fbxAnimStart = fbxTakeInfo->mLocalTimeSpan.GetStart();
						const FbxTime fbxAnimEnd = fbxTakeInfo->mLocalTimeSpan.GetStop();

						// The FBX animation may not be frames 0...N and can start at arbitrary places.
						const double fbxAnimTimeStart = fbxAnimStart.GetSecondDouble();
						const double fbxAnimTimeEnd = fbxAnimEnd.GetSecondDouble();
						const double fbxAnimTimeDuration = fbxAnimTimeEnd - fbxAnimTimeStart;

						const FbxLongLong startFrame = fbxAnimStart.GetFrameCount(fbxGlobalTimeMode);
						const FbxLongLong endFrame = fbxAnimEnd.GetFrameCount(fbxGlobalTimeMode);
						const FbxLongLong animTime = endFrame - startFrame + 1;

						outAnimation.Name = someFilePath;
						outAnimation.Length = static_cast<unsigned int>(animTime);
						outAnimation.FramesPerSecond = static_cast<float>(framesPerSecond);
						outAnimation.Frames.resize(animTime);
						outAnimation.Duration = fbxAnimTimeDuration;

						size_t localFrameCounter = 0;

						for(FbxLongLong t = startFrame; t <= endFrame && !bImporterError; t++)
						{
							FbxTime currentTime;
							currentTime.SetFrame(t, fbxGlobalTimeMode);

							outAnimation.Frames[localFrameCounter].LocalTransforms.resize(BoneNames.size());
							outAnimation.Frames[localFrameCounter].GlobalTransforms.resize(BoneNames.size());

							for(size_t j = 0; j < BoneNames.size() && !bImporterError; j++)
							{
								FbxAMatrix localFrameTransform;
								FbxAMatrix globalFrameTransform;

								// Make sure the bone exists in the animation. If not just assume Maya deleted it since it's
								// unskinned and we'll just give it Identity transform.
								if(::FbxNode* jointNode = fbxScene->FindNodeByName(FbxString(BoneNames[j].c_str())))
								{
									localFrameTransform = jointNode->EvaluateLocalTransform(currentTime);
									globalFrameTransform = jointNode->EvaluateGlobalTransform(currentTime);
								}

								FBX::ConvertFBXMatrixToArray(localFrameTransform, outAnimation.Frames[localFrameCounter].LocalTransforms[j].Data);
								FBX::ConvertFBXMatrixToArray(globalFrameTransform, outAnimation.Frames[localFrameCounter].GlobalTransforms[j].Data);
							}

							localFrameCounter++;
						}
					}
				}
			}
		}

		const FbxStatus& Status = fbxImporter->GetStatus();

		if(Status.Error())
			myLastError = Status.GetErrorString();
		else if (!bImporterError)
			myLastError = "Success";

		// Gotta extract this since Destroy will delete the Status object.
		const bool bResult = !(Status.Error() || bImporterError);

		fbxScene->Destroy(false);
		fbxImporter->Destroy();

		return bResult;
	}

	bool FBXImporter::LoadSkeleton(const std::string& someFilePath, FBXModel& outModel)
	{
		FbxImporter* fbxImporter = FbxImporter::Create(Internals::Get().fbxManager, "");

		bool bImporterError = false;

		if(!fbxImporter->Initialize(someFilePath.c_str(), -1, Internals::Get().fbxIOSettings))
		{
			const FbxStatus& Status = fbxImporter->GetStatus();
			myLastError = Status.GetErrorString();
			fbxImporter->Destroy();
			return false;
		}			

		if(!fbxImporter->IsFBX())
		{
			const FbxStatus& Status = fbxImporter->GetStatus();
			myLastError = Status.GetErrorString();
			fbxImporter->Destroy();
			return false;
		}			

		FbxScene* fbxScene = FbxScene::Create(Internals::Get().fbxManager, "The Scene");

		if(fbxImporter->Import(fbxScene))
		{
			Internals::Get().fbxAxisSystem.DeepConvertScene(fbxScene);

			std::string gatherBonesMessage;
			switch(FBX::GatherSkeletonData(fbxScene->GetRootNode(), outModel.Skeleton, gatherBonesMessage))
			{
			case 0: // A skeleton was found
				if(const FBXSkeleton::Bone* rootBone = outModel.Skeleton.GetRoot())
				{
					const std::string rootBoneName = rootBone->Name;
					if(const size_t pos = rootBoneName.find_first_of(':'); pos != std::string::npos)
					{
						outModel.Skeleton.Name = rootBoneName.substr(0, pos);
					}
					else
					{
						// We have no namespace to use so we'll just use the model name.
						std::string filePathCopy = someFilePath;
						std::replace(filePathCopy.begin(), filePathCopy.end(), '/', '\\');
						if(const size_t slashPos = filePathCopy.find_last_of('\\'); slashPos != std::string::npos)
						{
							filePathCopy = filePathCopy.substr(slashPos + 1, filePathCopy.length() - (slashPos - 1));
						}
						outModel.Skeleton.Name = filePathCopy.substr(0, filePathCopy.length() - 4);
					}
				}
				break;
			default:
				// Something went wrong.
				myLastError = gatherBonesMessage;
				bImporterError = true;
				break;
			}
		}

		const FbxStatus& Status = fbxImporter->GetStatus();

		if(Status.Error())
			myLastError = Status.GetErrorString();
		else if (!bImporterError)
			myLastError = "Success";

		// Gotta extract this since Destroy will delete the Status object.
		const bool bResult = !(Status.Error() || bImporterError);

		fbxScene->Destroy(false);
		fbxImporter->Destroy();

		return bResult;
	}

	void FBXImporter::UnloadImporter()
	{
		Internals::Get().Destroy();
	}

	//
//#ifdef _DEBUG
//	void FBXImporter::DumpJointTransforms(const FBXModel& aModel, const FBXAnimation& anAnimation, const std::string& aJointName)
//	{
//		if(!aModel.Skeleton.Joints.empty() && aModel.Skeleton.JointNameToIndex.find(aJointName) != aModel.Skeleton.JointNameToIndex.end())
//		{
//			std::cout << "Dumping joint transforms for " << aJointName << std::endl;
//			const unsigned int Idx = aModel.Skeleton.JointNameToIndex.find(aJointName)->second;
//
//			size_t vxCount = 0;
//			for(size_t m = 0; m < aModel.Meshes.size(); m++)
//			{
//				for(size_t v = 0; v < aModel.Meshes[m].Vertices.size(); v++)
//				{
//					for(int b = 0; b < 4; b++)
//					{
//						if(aModel.Meshes[m].Vertices[v].BoneIDs[b] == Idx)
//						{
//							vxCount++;
//						}
//					}
//				}
//			}
//
//			std::cout << "Vertices attached to this joint: " << std::to_string(vxCount) << std::endl;
//
//			for(size_t f = 0; f < anAnimation.Length; f++)
//			{
//				std::cout << "Frame: " << std::to_string(f) << "\nLocal xform:\n" << FBX::MatrixToString(anAnimation.Frames[f].LocalTransforms[Idx]) << std::endl;
//			}
//		}
//	}
//#endif
}

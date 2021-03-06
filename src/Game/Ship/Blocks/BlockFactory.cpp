#include "BlockFactory.h"

#include "..\..\..\Application\Application.h"

#include "BlockMain.h"
#include "BlockShield.h"
#include "BlockWeapon.h"
#include "BlockEngine.h"

#include "../../../Application/Application.h"


ObjectDataBase::ObjectDataBase()
{

}


ObjectDataBase::~ObjectDataBase()
{

}

void ObjectDataBase::Fill(std::string name)
{
	#define BLOCKTYPE(TypeOfBlock) dictionary_[#TypeOfBlock] = TypeOfBlock;
	#include "BlockTypes.h"
	#undef BLOCKTYPE	

	char model[30]          = {};
	char texture[30]        = {};
	char vertexShader[30]   = {};
	char fragmentShader[30] = {};
	char geometryShader[30] = {};
	float hits = 0.0f;
	float mass = 0.0f;

	char string[30] = {};
	float n = 0.0f;
	float rate = 0.0f;
	int integer_param = 0;

	GraphicsObjectInfo assetNames = {};

	std::string pathToDB = "../bin/resources/DB/" + name;
	FILE* file = fopen(pathToDB.c_str(), "r");
	assert(file);

	fscanf(file, "%s", string);

	while (strcmp(string, "END") != 0 && !feof(file))
	{	
		BlockType blockType = GetTypeOf(string);

		Block* newBlock = nullptr;

		fscanf(file, " hits: %f mass: %f", &hits, &mass);

		fscanf(file, " model: %s texture: %s vertexShader: %s fragmentShader: %s geometryShader: %s", model, texture, vertexShader, fragmentShader, geometryShader);
		assetNames.modelName_   = model;
		assetNames.textureName_ = texture;
		assetNames.shaderNames_._vertexShaderName   = vertexShader;
		assetNames.shaderNames_._fragmentShaderName = fragmentShader;
		assetNames.shaderNames_._geometryShaderName = geometryShader;

		

		switch (blockType)
		{
		case BlockTypeMain:
			newBlock = new BlockMain;
			break;

		case BlockTypeShield:

			newBlock = new BlockShield;

			fscanf(file, " shieldPowerMax: %f recoveryRate: %f", &n, &rate);
			
			((BlockShield*)newBlock)->shieldPowerMax_ = n;
			((BlockShield*)newBlock)->shieldPower_    = n;
			((BlockShield*)newBlock)->recoveryRate_   = rate;

			break;

		case BlockTypeWeapon:
			newBlock = new BlockWeapon;

			fscanf(file, " coolDownTime: %d", &integer_param);

			((BlockWeapon*)newBlock)->coolDownTime_  = integer_param;
			((BlockWeapon*)newBlock)->estimatedTime_ = integer_param;

			break;

		case BlockTypeEngine:
			newBlock = new BlockEngine;

			fscanf(file, " coolDownTime: %d", &integer_param);

			((BlockEngine*)newBlock)->coolDownTime_  = integer_param;
			((BlockEngine*)newBlock)->estimatedTime_ = integer_param;
			((BlockEngine*)newBlock)->power_ = 0.0f;




			break;

		default:
			break;
		}

		newBlock->relatedCoords_ = glm::vec3(0.0f);
		newBlock->isLinked_      = true;
		newBlock->ship_          = nullptr;
		newBlock->name_          = "";

		newBlock->blockType_ = blockType;
		newBlock->hits_		 = hits;
		newBlock->maxHits_	 = hits;
		newBlock->mass_      = mass;
		newBlock->isLinked_  = true;

		newBlock->graphicsAsset_ = GRAPHICSWORLD->GetManager()->Get(assetNames);

		db_[blockType] = newBlock;

		fscanf(file, " %s", string);
	}

	fclose(file);
}

Block* ObjectDataBase::GetCopyOf(BlockType blockType)
{
	return db_[blockType]->Clone();
}


BlockType ObjectDataBase::GetTypeOf(std::string blockTypeName)
{
	return dictionary_[blockTypeName];
}


BlockFactory::BlockFactory()
{
}


BlockFactory::~BlockFactory()
{
}


void BlockFactory::Init()
{
	dataBase_.Fill("BlocksDB.txt");
}


Block* BlockFactory::GetBlock(FILE* file)
{
	Block* newBlock = nullptr;

	char blockTypeName[30] = {};

	fscanf(file, "%s", blockTypeName);

	if (strcmp(blockTypeName, "END") != 0)
	{
		BlockType blockType = dataBase_.GetTypeOf(blockTypeName);
		newBlock            = dataBase_.GetCopyOf(blockType);

		fscanf(file, " (%f, %f, %f)", 
			&(newBlock->relatedCoords_.x),
			&(newBlock->relatedCoords_.y),
			&(newBlock->relatedCoords_.z));

		switch (blockType)
		{
		case BlockTypeShield:
		case BlockTypeWeapon:
		case BlockTypeEngine:
			fscanf(file, "%s", blockTypeName);

			newBlock->name_ = blockTypeName;
			break;

		default:	
			break;
		}

		switch (blockType)
		{
		case BlockTypeWeapon:
		case BlockTypeEngine:
			fscanf(file, " (%f, %f, %f)",
				&(((OrientedBlock*)newBlock)->target_.x),
				&(((OrientedBlock*)newBlock)->target_.y),
				&(((OrientedBlock*)newBlock)->target_.z));

			fscanf(file, " (%f, %f, %f)",
				&(((OrientedBlock*)newBlock)->up_.x),
				&(((OrientedBlock*)newBlock)->up_.y),
				&(((OrientedBlock*)newBlock)->up_.z));
			break;
		default:
			break;
		}

		return newBlock;
	}

	return nullptr;
}

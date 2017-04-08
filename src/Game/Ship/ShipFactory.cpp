#include "ShipFactory.h"
#include "ShipController.h"

#include <btBulletDynamicsCommon.h>

ShipFactory::ShipFactory()
{
}


ShipFactory::~ShipFactory()
{
}


void ShipFactory::Init()
{
	blockFactory_.Init();
}

Ship* ShipFactory::GenerateShip(ShipInfo shipInfo)
{
	Ship* ship = new Ship;
	
	ship->shipName_ = shipInfo.name_;

	btQuaternion rotation(
		(rand() - RAND_MAX) / 2.0f,
		(rand() - RAND_MAX) / 2.0f,
		(rand() - RAND_MAX) / 2.0f,
		(M_PI * rand()) / RAND_MAX);

	btVector3 coord_(shipInfo.coord_.x, shipInfo.coord_.y, shipInfo.coord_.z);

	ship->transform_ = btTransform(rotation, coord_);

	LoadConstruction(ship);
	LoadController(ship);

	ship->InitRigidBody();

	return ship;
}


void ShipFactory::LoadConstruction(Ship* ship)
{
	assert(ship);

	std::string constrName = "../bin/resources/construction/" + ship->shipName_ + ".txt";

	FILE* file = fopen(constrName.c_str(), "r");
	assert(file);

	Block* newBlock = nullptr;

	while (true) // TODO: REMOVE THIS FCKN SHIT !!!!!!!
	{
		newBlock = blockFactory_.GetBlock(file);

		if (newBlock == nullptr)
		{
			break;
		}

		ship->blocks_.push_back(newBlock);
	}

	fclose(file);
}


void ShipFactory::LoadController(Ship* ship)
{
	assert(ship);

	std::string scriptName = "../bin/resources/scripts/" + ship->shipName_ + ".lua";
//	std::cout << "C++: scriptName = '" << scriptName << "'\n";

	lua_State* luaThread = ship->controller_.luaThread_;
	assert(luaThread);

	int errorCode = luaL_loadfile(luaThread, scriptName.c_str());

	switch (errorCode)
	{
	case LUA_OK:
		std::cout << "LUA script successfully loaded." << std::endl;
		break;

	case LUA_ERRSYNTAX:
		std::cout << "LUA compilation error. " << lua_tostring(luaThread, -1) << std::endl;
		lua_pop(luaThread, 1);
		break;

	default:
		std::cout << "ERROR. Problems with loading LUA script. Error's code is " << errorCode << std::endl;
	}

//	if we use lua_sethook, we should call lua_resume(l, NULL, 0) while updating shipController
	lua_sethook(luaThread, ShipController::CatchLuaHook, LUA_MASKCOUNT, INSTRUCTION_LIMIT);

	luabridge::getGlobalNamespace(ship->controller_.luaThread_)
		.addFunction("GetTime",       &ShipController::GetTime)
		.addFunction("SwitchShield", &ShipController::SwitchShield)
		.addFunction("EnableShield", &ShipController::EnableShield)
		.addFunction("DisableShield", &ShipController::DisableShield);

}

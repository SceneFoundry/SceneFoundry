#include "framework.h"
#include "SceneFoundry/sandbox_game/game_object.h"


namespace sandbox_game
{


	game_object::game_object()
	{

	}


	void game_object::initialize_game_object(long long llId)
	{

		m_llId = llId;
	}


} // namespace sandbox_game




#pragma once

#include "../ECS/ECS.h"
#include "../Components/ScriptComponent.h"
#include "../Components/AudioComponent.h"
#include "../EventBus/Event.h"
#include "../Events/InputActionEvent.h"
#include <SDL.h>
//declare some native c++ function that we will bind with lua functions

void SetEntityPosition(Entity entity, const double x, const double y)
{
	if (entity.HasComponent<TransformComponent>())
	{
		TransformComponent& transform = entity.GetComponent<TransformComponent>();
		transform.position.x = x;
		transform.position.y = y;
	}
	else {
		Logger::Err("Trying to set entity position of an entity with no transform");
	}
}

std::tuple<double, double> GetEntityPosition(Entity entity)
{
	double x = 0;
	double y = 0;

	if (entity.HasComponent<TransformComponent>())
	{
		const TransformComponent transform = entity.GetComponent<TransformComponent>();

		x = transform.position.x;
		y = transform.position.y;

	}
	else 
	{
		Logger::Err("Trying to get position of entity with no transform component");
	}

	return std::make_tuple(x, y);
}

//this brings into question which is how we want to expose the audio
//or even regular systems to be used by external users
void PlayAudio(Entity entity, const std::string& assetId, int volume, bool loop = false,  bool isMusic = false)
{

	if (!entity.HasComponent<AudioComponent>())
	{
		Logger::Err("Attempting to play audio on an entity with no audio component");
		return;
	}

	AudioComponent& audio = entity.GetComponent<AudioComponent>();
	audio.assetId = assetId;
	audio.volume = volume;
	audio.loop = loop;
	audio.isMusic = isMusic;
	audio.isPlaying = false;

}


class ScriptSystem : public System
{
public:
	ScriptSystem()
	{
		RequireComponent<ScriptComponent>();
	}


	void CreateLuaBinding(sol::state& lua)
	{
		//Lua needs to know what an Entity is but in our lua we are passing an entity
		//So we need to create that type
		lua.new_usertype<Entity>("entity", 
			"get_id", &Entity::GetId, 
			"destroy", &Entity::Kill,
			"HasTag", &Entity::HasTag,
			"BelongsToGroup", &Entity::BelongsToGroup);

		lua.new_usertype<Event>("Event");

		lua.new_usertype<InputAction>("InputAction",
			"get_action", &InputAction::inputActionName);

		lua.new_usertype<InputActionEvent>("InputEvent",
			"get_event", &InputActionEvent::action
			);


		//We can create a input type or aciton type that is called via an input and expose it to lua
		//action type sounds better but more work


		//CreatAllBindingsBetween C++ and Lua
		lua.set_function("set_position", SetEntityPosition);
		lua.set_function("get_position", GetEntityPosition);
		lua.set_function("play_audio", PlayAudio);
		

	}

	void Update(double deltaTime, int ellapsedTime)
	{
		for (auto entity : GetSystemEntities())
		{
			const ScriptComponent script = entity.GetComponent<ScriptComponent>();

			//run the function
			script.func(entity, deltaTime, ellapsedTime);

		}
	}

};
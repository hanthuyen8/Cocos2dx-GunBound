#pragma once

class ICommand
{
public:
	enum class Type
	{
		CharacterMove, CharacterAim, CharacterCharge
	};

	Type type;
};
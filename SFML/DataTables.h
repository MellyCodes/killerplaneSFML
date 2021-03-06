/**
* @file
* @author Melanie Roy-Plommer
* @version 1.0
*
* @section DESCRIPTION
* <  >
*
* @section LICENSE
* Copyright 2018 - 2019
* Permission to use, copy, modify, and/or distribute this software for
* any purpose with or without fee is hereby granted, provided that the
* above copyright notice and this permission notice appear in all copies.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
* ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*
* @section Academic Integrity
* I certify that this work is solely my own and complies with
* NBCC Academic Integrity Policy (policy 1111)
*/

#pragma once
#include "TextureManager.h"
#include "Aircraft.h"
#include <map>
#include "Projectile.h"
#include "Pickup.h"

namespace GEX
{

	struct Direction
	{
		Direction(float a, float d)
			:	angle(a)
			,	distance(d)
		{}
		float angle;
		float distance;

	};
	
	struct AircraftData
	{
		int						hitpoint;
		float					speed;
		TextureID				texture;
		sf::Time				fireInterval;

		std::vector<Direction>	directions;
	};

	struct ProjectileData
	{
		int						damage;
		float					speed;
		TextureID				texture;
	};

	struct PickupData
	{
		std::function<void(Aircraft&)>	action;
		TextureID						texture;
	};

	std::map<Pickup::Type, PickupData> initializePickupData();
	std::map<Projectile::Type, ProjectileData> initializeProjectileData();
	std::map<AircraftType, AircraftData> initializeAircraftData();


}

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

#include "DataTables.h"
#include "Projectile.h"

std::map<GEX::Projectile::Type, GEX::ProjectileData> GEX::initializeProjectileData()
{
	std::map<Projectile::Type, ProjectileData> data;

	data[Projectile::Type::AlliedBullet].damage = 10;

	return std::map<Projectile::Type, ProjectileData>();
}


std::map<GEX::AircraftType, GEX::AircraftData> GEX::initializeAircraftData()
{
	std::map<GEX::AircraftType, AircraftData> data;

	//player 
	data[AircraftType::EAGLE].hitpoint = 100;
	data[AircraftType::EAGLE].speed = 200.f;
	data[AircraftType::EAGLE].texture = TextureID::Eagle;


	// enemies
	data[AircraftType::RAPTOR].hitpoint = 20;
	data[AircraftType::RAPTOR].speed = 80.f;
	data[AircraftType::RAPTOR].texture = TextureID::Raptor;
	data[AircraftType::RAPTOR].directions.emplace_back(Direction(45.f, 80.f));
	data[AircraftType::RAPTOR].directions.emplace_back(Direction(-45.f, 160.f));
	data[AircraftType::RAPTOR].directions.emplace_back(Direction(45.f, 80.f));


	data[AircraftType::AVENGER].hitpoint = 40;
	data[AircraftType::AVENGER].speed = 50.f;
	data[AircraftType::AVENGER].texture = TextureID::Avenger;
	data[AircraftType::AVENGER].directions.emplace_back(Direction(45.f, 50.f));
	data[AircraftType::AVENGER].directions.emplace_back(Direction(0.f, 50.f));
	data[AircraftType::AVENGER].directions.emplace_back(Direction(-45.f, 100.f));
	data[AircraftType::AVENGER].directions.emplace_back(Direction(0.f, 50.f));
	data[AircraftType::AVENGER].directions.emplace_back(Direction(45.f, 50.f));


	
	
	return data;
}

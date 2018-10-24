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

#include "Projectile.h"
#include "DataTables.h"
#include "Utility.h"
#include "Category.h"

namespace GEX {
	namespace
	{
		const std::map<Projectile::Type, ProjectileData> TABLE = initializeProjectileData();
	}


	Projectile::Projectile(Type type, const TextureManager & textures)
		:	Entity(1)
		,	type_(type)
		,	sprite_(textures.get(TABLE.at(type).texture), TABLE.at(type).textureRect)
	{
		centerOrigin(sprite_);
	}

	unsigned int Projectile::getCategory() const
	{
		if (type_ == Type::EnemyBulletType)
			return Category::EnemyProjectile;
		else
			return Category::AlliedProjectile;
	}

	float Projectile::getMaxSpeed() const
	{
		return TABLE.at(type_).speed;
	}

	int Projectile::getDamage() const
	{
		return TABLE.at(type_).damage;
	}

	bool Projectile::isGuided() const
	{
		return (type_ == Type::Missile);
	}

	void Projectile::guidedTowards(sf::Vector2f position)
	{
		assert(isGuided());
		targetDirection_ = unitVector(position - getWorldPosition());
	}

	sf::FloatRect Projectile::getBoundingBox() const
	{
		return getWorldTransform().transformRect(sprite_.getGlobalBounds());
	}

	void Projectile::updateCurrent(sf::Time dt, CommandQueue& commands)
	{
		if(isGuided())
		{
			const float APPROACH_RATE = 400.f;

			auto newVelocity = unitVector(APPROACH_RATE * dt.asSeconds() *
				targetDirection_ + getVelocity());

			newVelocity *= getMaxSpeed();
			setVelocity(newVelocity);

			auto angle = std::atan2(newVelocity.y, newVelocity.x);
			setRotation(toDegree(angle) + 90.f);
		}
		Entity::updateCurrent(dt, commands);
	}

	void Projectile::drawCurrent(sf::RenderTarget & target, sf::RenderStates states) const
	{
		target.draw(sprite_, states);
	}

}

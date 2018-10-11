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
		,	sprite_(textures.get(TABLE.at(type).texture))
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

	void Projectile::updateCurrent(sf::Time dt, CommandQueue& commands)
	{
		Entity::updateCurrent(dt, commands);
	}

	void Projectile::drawCurrent(sf::RenderTarget & target, sf::RenderStates states) const
	{
	}

}

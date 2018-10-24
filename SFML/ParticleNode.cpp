#include "ParticleNode.h"
#include "DataTables.h"

namespace
{
	const std::map<GEX::Particle::Type, GEX::ParticleData> TABLE = GEX::initializeParticleData();
}

namespace GEX {
	ParticleNode::ParticleNode(Particle::Type type, const GEX::TextureManager& textures)
		:	SceneNode()
		,	particles_()
		,	texture_(textures.get(GEX::TextureID::Particle))
		,	type_(type)
		,	vertexArray_(sf::Quads)
		,	needsVertexUpdate_(true)
	{}

	void ParticleNode::addParticle(sf::Vector2f position)
	{
		Particle particle;
		particle.position = position;

		particle.color = TABLE.at(type_).color;
		particle.lifetime = TABLE.at(type_).lifetime;

		particles_.push_back(particle);
	}

	Particle::Type ParticleNode::getParticleType() const
	{
		return type_;
	}

	unsigned int ParticleNode::getCategory() const
	{
		return Category::ParticleSystem;
	}

	void ParticleNode::updateCurrent(sf::Time dt, CommandQueue & commands)
	{
		// remove aged out particles
		while (!particles_.empty() && particles_.front().lifetime < sf::Time::Zero)		
			particles_.pop_front();		

		// count down lifetimes
		for (auto& p : particles_)
			p.lifetime -= dt;

		// mark for update
		needsVertexUpdate_ = true;
	}

	void ParticleNode::drawCurrent(sf::RenderTarget & target, sf::RenderStates states) const
	{
		if (needsVertexUpdate_)
		{
			computeVertices();
			needsVertexUpdate_ = false;
		}

		states.texture = &texture_;

		//draw all the vertices
		target.draw(vertexArray_, states);

	}

	void ParticleNode::addVertex(float worldx, float worldy, float texCoordU, float texCoordV, const sf::Color color) const
	{
		sf::Vertex vertex;
		vertex.position = sf::Vector2f(worldx, worldy);
		vertex.texCoords = sf::Vector2f(texCoordU, texCoordV);
		vertex.color = color;
	}

	void ParticleNode::computeVertices() const
	{
		sf::Vector2f size(texture_.getSize());
		sf::Vector2f half = size / 2.f;

		// refill vertex array;
		for (const Particle& p : particles_)
		{
			sf::Vector2f pos = p.position;
			sf::Color color = p.color;

			float ratio = p.lifetime.asSeconds() / TABLE.at(type_).lifetime.asSeconds();
			color.a = static_cast<sf::Uint8>(255 * std::max(ratio, 0.f));

			addVertex(pos.x - half.x, pos.y - half.y, 0.f, 0.f, color);
			addVertex(pos.x + half.x, pos.y - half.y, size.x, 0.f, color);
			addVertex(pos.x + half.x, pos.y + half.y, size.x, size.y, color);
			addVertex(pos.x - half.x, pos.y + half.y, 0.f, size.y, color);

		} 

	}

}


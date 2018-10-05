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
#include "Entity.h"
#include <SFML/Graphics/Sprite.hpp>
#include "TextureManager.h"
#include "TextNode.h"

namespace GEX
{
	class TextNode;
	enum class AircraftType{EAGLE, RAPTOR, AVENGER};

	class Aircraft : public Entity
	{
	public:
							Aircraft(AircraftType type, TextureManager& textures);
		void				drawCurrent(sf::RenderTarget& target, sf::RenderStates states)const override;
		unsigned int		getCategory() const override;

		void				updateTexts(); // update mini health and missile display

	protected:
		void				updateCurrent(sf::Time dt) override;

	private:
		void				updateMovementPattern(sf::Time dt);
		float				getMaxSpeed() const;

	private:
		AircraftType		type_;
		sf::Sprite			sprite_;
		TextNode*			healthDisplay_;
		TextNode*			missileDisplay_;

		float				travelDistance_;
		std::size_t			directionIndex_;
	};


}



#pragma once
#include "PostEffect.h"
#include <array>
#include <map>
#include <SFML/Graphics/RenderTexture.hpp>

namespace  GEX
{
	class BloomEffect : public GEX::PostEffect
	{
	public:
		enum class Shaders
		{
			BrightnessPass,
			DownSamplePass,
			GaussianBlurPass,
			AddPass,
		};

	private:
		typedef std::array<sf::RenderTexture, 2> RenderTextureArray;

	public:
											BloomEffect();

		void								apply(const sf::RenderTexture& input, sf::RenderTarget& output) override;

	private:
		void								prepareTextures(sf::Vector2u size);

		//effects
		void								filterBright(const sf::RenderTexture& input, sf::RenderTexture& output);
		void								blurMultipass(RenderTextureArray& renderTextures);
		void								blur(const sf::RenderTexture& input, sf::RenderTexture& output, sf::Vector2f offsetFactor);
		void								downSample(const sf::RenderTexture& input, sf::RenderTexture& output);
		void								add(const sf::RenderTexture& source, const sf::RenderTexture& bloom, sf::RenderTarget& target);

	private:
		std::map < Shaders, std::unique_ptr<sf::Shader>>	shaders_;

		sf::RenderTexture									brightnessTexture_;
		RenderTextureArray									firstPassTexture_;
		RenderTextureArray									secondPassTexture_;


	};

}



#pragma once

// Core
#include <unordered_map>
#include <memory>

// SFML
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

// Others
#include "ResourceID.hpp"

namespace ResourceManager
{
	// Generic resource holder.
	template <typename ID, typename Resource>
	class ResourceHolder
	{
	public:

		void loadResource(ID id, const std::string& path)
		{
			// Try finding.
			auto found = mResourceHolder.find(id);

			// Check if found
			if (found != mResourceHolder.end())
			{
				throw("Error loading redundant texture");
			}
			
			// Create unqiue resource, default initialized.
			auto texture = std::make_unique<Resource>();

			// Try loading the resource into the object.
			if (!texture->loadFromFile(path))
			{
				throw("Error loading file.");
			}

			// If we make it here, resource does not 
			// exist on the map yet, we can insert it.
			mResourceHolder.emplace(id, std::move(texture));
		}

		[[nodiscard]] Resource& get(ID id)
		{
			// Try finding.
			auto found = mResourceHolder.find(id);

			// Check if not found.
			if (found == mResourceHolder.end())
			{
				throw("Error resource ID does not exist.");
			}

			// If we make it here, it means that the resource
			// exists, we can just return it.
			return *mResourceHolder[id].get();
		}

		[[nodiscard]] const Resource& get(ID id) const
		{
			// Try finding.
			auto found = mResourceHolder.find(id);

			// Check if not found.
			if (found == mResourceHolder.end())
			{
				throw("Error resource ID does not exist.");
			}

			// If we make it here, it means that the resource
			// exists, we can just return it.
			return *mResourceHolder.at(id);
		}
	private: // Attributes
		std::unordered_map<ID, std::unique_ptr<Resource>> mResourceHolder;
	};

	// Declare aliases
	using TextureHolder = ResourceHolder<TextureID, sf::Texture>;
	using FontHolder = ResourceHolder<FontID, sf::Font>;
}

#pragma once

// Core
#include <cassert>

// SFML
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

// Others
#include "Common.hpp"
#include "Debug.hpp"
#include "ResourceHolder.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"

// Temp
#include "Button.hpp"

// GUI
#include "../gui/board_gui.hpp"


namespace app
{
	using ushort = unsigned short;

	// Basic context for the window.
	struct WindowContext
	{
		ushort width;
		ushort height;
		std::string title;
		ushort target_fps = 60;
	};

	class Application final
	{
	public: // Public methods.

		// Delete default Ctor.
		Application() = delete;

		// Ctor.
		explicit Application(const WindowContext& ctx) noexcept;

		// Open and run the window.
		void run() noexcept;

	private: // Private methods.

		// Private Ctor.
		Application(ushort width, ushort height, const std::string& title = "Unnamed Application", ushort targetFps = 60) noexcept;

		// Handle events.
		void handleEvents() noexcept;

		// Update functions.
		void fixedTimeUpdate() noexcept;
		void realTimeUpdate() noexcept;
		void update(const sf::Time& dt) noexcept;

		// Render.
		void render() noexcept;

		// Update delta time.
		void updateDt() noexcept;

		// Initialize Resources
		void initTextures();
		void initFonts();

		// Debug
		void initDebug();

	private:
		// Window and context.
		sf::VideoMode		mVideoMode;
		sf::RenderWindow	mWindow;
		ushort				mTargetFps;

		// Time and clock.
		sf::Time		mTimeSinceLastUpdate = sf::Time::Zero;
		const sf::Time	TIME_PER_FRAME;
		sf::Clock		mClock;

		// Resource Holder
		ResourceManager::TextureHolder	mTextureHolder;
		ResourceManager::FontHolder		mFontHolder;

		// Inputs.
		Mouse		mouse;
		Keyboard	keyboard;

		std::unique_ptr<Debug> debugger;

		// Temp (For buttons)
		std::unique_ptr<Button> button;

 private: // GUI stuff...
    BoardGui board;
	};
}

namespace app
{
    // Setup the application window given the context.
    inline Application::Application(const WindowContext& ctx) noexcept
        : Application(ctx.width, ctx.height, ctx.title, ctx.target_fps)
    {
        // Initialize textures.
        initTextures();

        // Intialize fonts.
        initFonts();

        // Initialize debug
        initDebug();

        // Temp (init button)
        ButtonConfig btn_cfg { 0.f, 0.f, 500.f, 400.f, sf::Color(100,100,100), sf::Color(200,200,200), sf::Color(150, 150, 150)};
        int i = 0;
        this->button = std::make_unique<Button>(btn_cfg, mFontHolder.get(FontID::HELVETICA), "Button", true);
        // button->setFunction([&]() {
        //     if (i++ % 2 == 0)
        //     {
        //         button->setLabel("PRESSED");
        //     }
        //     else
        //     {
        //         button->setLabel("READY");
        //     }
            
        //     });
    }

    // Main run loop.
    inline void Application::run() noexcept
    {
        while (mWindow.isOpen())
        {
            // Update delta time.
            updateDt();

            // Handle events.
            handleEvents();

            // Real time update. (Updates which do not require DT)
            realTimeUpdate();

            // Fixed time update.
            fixedTimeUpdate();

            // Render.
            render();
        }
    }

    // Private Ctor.
    inline Application::Application(const ushort width, const ushort height, const std::string& title, const ushort targetFps) noexcept
        : mVideoMode(width, height)
        , mWindow(mVideoMode, title)
        , mTargetFps(targetFps)
        , TIME_PER_FRAME(sf::seconds(1.f / static_cast<float>(mTargetFps)))
        , board(mWindow)
    {}

    // Handle SFML events.
    inline void Application::handleEvents() noexcept
    {
        sf::Event event{};
        while (mWindow.pollEvent(event))
        {
            // TODO: Input Base Class.
             
            // Handle mouse input.
            mouse.handleEvent(event);

            // Handle keyboard input.
            keyboard.handleEvent(event);

            // TODO: Replace with switch statement
            // refactor into another method handleEvent().
            if (event.type == sf::Event::Closed)
                mWindow.close();
        }
    }

    // Update with fixed steps.
    inline void Application::fixedTimeUpdate() noexcept
    {
        while (mTimeSinceLastUpdate >= TIME_PER_FRAME)
        {
            mTimeSinceLastUpdate -= TIME_PER_FRAME;
            update(TIME_PER_FRAME);
        }
    }

    // Real time update.
    inline void Application::realTimeUpdate() noexcept
    {
        // Update mouse position.
    	mouse.updatePosition(mWindow);

        // Temp (update button)
        button->update(mouse);

        // Update previous mouse position for motion_vector
        mouse.updatePrevPosition(mouse.position());


        // Update debugger
        #ifdef DEBUG_MODE_ON
        debugger->update();
        #endif
    }

    // Update components function.
    inline void Application::update(const sf::Time& dt) noexcept
    {
    }

    // Render scene.
    inline void Application::render() noexcept
    {
        // Clear scene.
        mWindow.clear();

        // Objects to render here.


        mWindow.draw(board);

        // Draw debugger
        // #ifdef DEBUG_MODE_ON
        // mWindow.draw(*debugger);
        // #endif 

        // Display scene.
        mWindow.display();
    }

    // Update delta time.
    inline void Application::updateDt() noexcept
    {
        mTimeSinceLastUpdate = mClock.restart();
    }

    // Initialize texture
    inline void Application::initTextures()
    {
    }

    inline void Application::initFonts()
    {
        mFontHolder.loadResource(FontID::HELVETICA, "resources/HelveticaNeueLTStd-It.otf");
    }


    inline void Application::initDebug()
    {
        // Place items here that you want to debug.
        
        #ifdef DEBUG_MODE_ON
        debugger = std::make_unique<Debug>(mFontHolder.get(FontID::HELVETICA));

        // Register them.
        debugger->reg(mouse);
        debugger->reg(keyboard);

        #endif // DEBUG
    }
}
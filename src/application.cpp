#include "application.h"

using namespace gunshot;

Application::Application() : Shared::Application(PROJECT_NAME, { Flag::Audio, Flag::Scene, Flag::Network })
{
	PLATFORM->setTitle(PRODUCT_NAME);
#if defined(PLATFORM_MAC)
	PLATFORM->resize(720, 1280);
#else
	PLATFORM->resize(360, 640);
#endif
#if defined(PLATFORM_WINDOWS)
	PLATFORM->rescale(1.5f);
#endif
	RENDERER->setVsync(true);

	STATS->setAlignment(Shared::StatsSystem::Align::BottomRight);

	FRAME->addOne([this] {
		initialize();
	});
}

Application::~Application()
{
}

void Application::initialize()
{
	auto screen = std::make_shared<Shared::SceneHelpers::StandardScreen>();
	screen->setTouchable(true);
	SCENE_MANAGER->switchScreen(screen);

	auto world = std::make_shared<Shared::PhysHelpers::World>();
	world->setAnchor(0.5f);
	world->setPivot(0.5f);
	world->setDebug(true);
	screen->getContent()->attach(world);

	auto floor = std::make_shared<Shared::PhysHelpers::Entity>();
	floor->setSize({ 768.0f, 16.0f });
	floor->setPivot(0.5f);
	floor->setPosition({ 0.0f, 192.0f });
	world->attach(floor);

	auto enableAutoSuicide = [](std::shared_ptr<Scene::Node> node){
		node->runAction(Actions::Collection::ExecuteInfinite([node]{
			if (node->getY() > 1000.0f)
				node->runAction(Actions::Collection::Kill(node));
		}));
	};

	auto spawnBox = [world, enableAutoSuicide]{
		auto box = std::make_shared<Shared::PhysHelpers::Entity>();
		box->setType(Shared::PhysHelpers::Entity::Type::Dynamic);
		box->setSize({ 24.0f, 24.0f });
		box->setPivot(0.5f);
		enableAutoSuicide(box);
		world->attach(box);
	};

	auto spawnBall = [world, enableAutoSuicide]{
		auto ball = std::make_shared<Shared::PhysHelpers::Entity>();
		ball->setType(Shared::PhysHelpers::Entity::Type::Dynamic);
		ball->setShape(Shared::PhysHelpers::Entity::Shape::Circle);
		ball->setSize({ 24.0f, 24.0f });
		ball->setPivot(0.5f);
		enableAutoSuicide(ball);
		world->attach(ball);
	};

	static bool auto_spawn = true;

	screen->runAction(Actions::Collection::Pausable([]{ return auto_spawn; }, Actions::Collection::RepeatInfinite([spawnBox, spawnBall]() -> Actions::Collection::UAction {
		const auto delay = 0.125f;

		return Actions::Collection::MakeSequence(
			Actions::Collection::Wait(delay),
			Actions::Collection::Execute([spawnBox, spawnBall]{
				if (Common::Helpers::Chance(0.5f))
					spawnBox();
				else
					spawnBall();
			})
		);
	})));

	static auto framer = Common::FrameSystem::Framer();
	
	framer.setCallback([spawnBox, spawnBall]{
		ImGui::Begin("Options", nullptr, ImGui::User::ImGuiWindowFlags_ControlPanel);
		ImGui::SetWindowPos(ImGui::User::BottomLeftCorner());

		if (ImGui::Button("Spawn 1 box"))
		{
			spawnBox();
		}

		if (ImGui::Button("Spawn 25 boxes"))
		{
			for (int i = 0; i < 25; i++)
			{
				spawnBox();
			}
		}

		if (ImGui::Button("Spawn 100 boxes"))
		{
			for (int i = 0; i < 100; i++)
			{
				spawnBox();
			}
		}

		ImGui::Spacing();

		if (ImGui::Button("Spawn 1 ball"))
		{
			spawnBall();
		}

		if (ImGui::Button("Spawn 25 ball"))
		{
			for (int i = 0; i < 25; i++)
			{
				spawnBall();
			}
		}

		if (ImGui::Button("Spawn 100 ball"))
		{
			for (int i = 0; i < 100; i++)
			{
				spawnBall();
			}
		}

		ImGui::Separator();

		ImGui::Checkbox("Auto spawn", &auto_spawn);

		ImGui::End();
	});
}

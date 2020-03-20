#include <application.h>
#include <eventSystem.h>
#include <console.h>
#include <render/renderModel.h>
#include <render/renderModel.h>
#include <render/vertexArray.h>
#include <render/texture.h>
#include <render/shaderProgram.h>
#include <render/textCreator.h>
#include <render/renderer.h>

#include <control/utils.h>
#include <control/camera.h>
#include <control/transform.h>
#include <control/noise.h>
#include <list>
#include <iostream>

using namespace sp;



class MainMenu : public ApplicationLayer
{
public:
	TextCreator *tc;
	Timer *timer;
	DocSetting ds;
	MainMenu() :ApplicationLayer("mainmenu") {};
	void onInit() override {
		tc = new TextCreator();
		tc->submitFont("font1.ttf");
		timer = new Timer(1.f);
	}

	void onLoad() override {
		timer->reset();
		RenderCommand::setClearColor(ColorReferance::Color::Steel);
		
		
	}

	void onUpdate(float dt) override{
		if (EventSystem::isPressedKey(KeyCode::escape)) Application::close();
		if (EventSystem::isPressedKey(KeyCode::space)) Application::switchLayer("game");
	}
	void onRender() {
		ds.defaultParaSettings.textAlignment = TextAlignment::center;
		ds.defaultParaSettings.textSettings.color = ColorReferance::Color::CerulianBlue;
		ds.defaultParaSettings.textSettings.fontSize = 0.09;
		ParaSettings ps;
		ps.textSettings.color = ColorReferance::Color::GrassGreen;
		ps.textSettings.fontSize = 0.04;
		ps.textAlignment = TextAlignment::center;
		tc->beginDocument(ds);
		tc->paragraph("MENU");
		ds.defaultParaSettings.textSettings.fontSize = 0.06;
		tc->moveCursorVertical(0.3f);
		if(timer->fraction() > 0.2f)
			tc->paragraph("press space to continue", ps);
		else
			tc->paragraph("  ", ps);
		tc->moveCursorVertical(1.1f);
		ds.defaultParaSettings.textSettings.color = ColorReferance::Color::Tomato;
		ps.textSettings.color = ColorReferance::Color::Red;
		ps.textSettings.fontSize = 0.04;
		tc->paragraph("press esc to exit", ps);
		tc->endDocument();
		tc->drawDocument();
	}
};

class Game : public ApplicationLayer
{
public:
	Game() :ApplicationLayer("game") {};
	void onInit() override {

	}

	void onLoad() override {
		RenderCommand::setClearColor(ColorReferance::Color::InkBlack);
	}

	void onUpdate(float dt) override {
		if (EventSystem::isPressedKey(KeyCode::escape)) Application::switchLayer("mainmenu");
	}
	void onRender() {

	}
};


int main(int argc, char** argv) {

	Application::create(600, 400, "platformer");
	Application::pushLayer((new MainMenu()));
	Application::pushLayer((new Game()));
	Application::run();
	return 0;
}
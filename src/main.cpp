// ===================================================
// main.cpp GM21制作用プロジェクト
// 
// 制作者：		日付：
// ===================================================

#include "application.h"
#include "main.h"

// =========================================================
// main関数
// =========================================================
extern "C" void nnMain()
{
	foo::Application* app = new foo::Application();

	app->Initialize();
	app->Run();
	app->Finalize();

	delete app;
}

int main()
{
	foo::Application* app = new foo::Application();

	app->Initialize();
	app->Run();
	app->Finalize();

	delete app;
	return 0;
}
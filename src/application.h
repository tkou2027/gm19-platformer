#pragma once

namespace foo
{
	class Application
	{
	public:
		void Initialize();
		void Run();
		void Finalize();
	private:
		void Update();
		void Draw();
	};
}
#pragma once

#include "../../Headers/Context/Context.h"
#include <chrono>

namespace osomi{
	class Game{
	public:
		static Game *create(u32 w, u32 h, std::string name, void (*update)(double), void (*render)(), void (*postRender)());
		~Game(){ if (c != nullptr) delete c; }
		void start();
		
		void setSize(u32 x, u32 y){
			if (prevW == x && prevH == y)return;
			newW = x;
			newH = y;
		}
	private:
		Game(){}
		
		Context *c;
		
		u32 prevW, prevH;
		u32 newW, newH;
		
		u64 frames;
		f64 time;
		
		void(*update)(double);
		void(*render)();
		void(*postRender)();
		
		std::chrono::high_resolution_clock::time_point t, t0;
	};
}
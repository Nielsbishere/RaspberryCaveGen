#include "../../Headers/Context/Game.h"
#include "../../Headers/Context/Context.h"

using namespace osomi;
using namespace std;

Game *Game::create(u32 w, u32 h, std::string title, void(*update)(double), void (*render)(), void (*postRender)()) {
	if (update == nullptr || render == nullptr || postRender == nullptr || w == 0 || h == 0 || title == ""){
		printf("Couln't create window; invalid size, title, render or update function!\n");
		return nullptr;
	}
	Context *c = Context::createContext(w, h, title);
	if (c == nullptr){
		printf("Couldn't create window; invalid context!\n");
		return nullptr;
	}
	
	Game *g = new Game();
	g->c = c;
	g->prevW = w;
	g->prevH = h;
	g->update = update;
	g->render = render;
	g->postRender = postRender;
	
	return g;
}

void Game::start(){
	printf("Started game\n");
	t0 = chrono::high_resolution_clock::now();
	t = t0;
	float dt = 0;
	bool init = false;
	while (true){
		if (newW != 0 && newH != 0){
			prevW = newW;
			prevH = newH;
			prevW = prevH = 0;
			glViewport(0, 0, prevW, prevH);
		}
		
		chrono::high_resolution_clock::time_point now = chrono::high_resolution_clock::now();
		dt = chrono::duration_cast<chrono::duration<double>>(now - t).count();
		
		if (!init) {
			init = true;
			continue;
		}
		
		update(dt);
		
		t = chrono::high_resolution_clock::now();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render();
		
		#ifdef __EGL__
		eglSwapBuffers(c->t->display, c->t->surface);
		#endif
		
		postRender();
		
		frames++;
		time += dt;
	}
}
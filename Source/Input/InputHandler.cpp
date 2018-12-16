#include "../../Headers/Input/InputHandler.h"

#include <string>
using namespace std;

#ifdef __UNIX__
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <linux/keyboard.h>
#include <linux/kd.h>
#include <pthread.h>
#include <dirent.h>
#endif

using namespace osomi;

InputHandler::InputHandler(){
	for (u32 i = 0; i < 256; i++)
		currKeys[i] = prevKeys[i] = false;
	for (u32 i = 0; i < 8; i++)
		currMouse[i] = prevMouse[i] = false;
	cursorX = cursorY = 0;
	ded = false;
}

InputHandler::~InputHandler(){}

void InputHandler::update() {
	for (u32 i = 0; i < 256; i++)
		prevKeys[i] = currKeys[i];
	for (u32 i = 0; i < 8; i++)
		prevMouse[i] = currMouse[i];
}
		
#ifdef __UNIX__

string pickKeyboard() {
	std::string pattern = "event-kbd";
	std::string file = "";
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir("/dev/input/by-path/")) != nullptr) {
		while ((ent = readdir(dir)) != nullptr) {
			file = std::string(ent->d_name);
			if (!file.compare(".") || !file.compare("..")) continue;
			if (file.substr(file.length() - pattern.length()) == pattern) 
				return "/dev/input/by-path/" + file;
		}
	}
	return "";
}

void *handleMouse(void *arg){
	FILE *fmouse;
	fmouse = fopen("/dev/input/mouse0", "r");
	if (fmouse == nullptr)fmouse = fopen("/dev/input/mouse1", "r");
	if (fmouse == nullptr)fmouse = fopen("/dev/input/mouse2", "r");
	if (fmouse == nullptr)fmouse = fopen("/dev/input/mouse3", "r");
	if (fmouse == nullptr)fmouse = fopen("/dev/input/mouse4", "r");

	if (fmouse != nullptr) {
		printf("Starting mouse thread\n");
		while (((InputHandler*)arg)->isAlive()) {
			signed char b[3];
			fread(b, sizeof(char), 3, fmouse);

			((InputHandler*)arg)->addCursorPos(b[1] / 100.f, -b[2] / 100.f);
			((InputHandler*)arg)->updateMouse(0, b[0] & 0x1);
			((InputHandler*)arg)->updateMouse(2, b[0] & 0x4);
			((InputHandler*)arg)->updateMouse(1, b[0] & 0x2);
		}
		fclose(fmouse);
	}
	printf("Closing mouse thread\n");

	pthread_exit(nullptr);
}

void *handleKey(void *arg){
	FILE *fkey;
	string str = pickKeyboard();
	if (str == "" || (fkey = fopen(str.c_str(), "r")) == nullptr){
		printf("Couldn't open key thread!\n");
		return nullptr;
	}
	printf("Starting key thread!\n");
	while (((InputHandler*)arg)->isAlive()){
		//Still alive... Still alive
		struct input_event ev;
		
		fread(&ev, sizeof(struct input_event), 1, fkey);
		if (ev.type == (__u16)EV_KEY) 
			((InputHandler*)arg)->updateKey(InputHandler::getKeyCode(ev.code), ev.value);
	}
	printf("Ending key thread!\n");
}

InputHandler *InputHandler::init(){
	InputHandler *ih = new InputHandler();
	
	int mouseErr = pthread_create(&ih->mouse, NULL, &handleMouse, ih);
	if (mouseErr != 0) printf("Couldn't create mouse thread!\n");
	
	int keyErr = pthread_create(&ih->keyboard, NULL, &handleKey, ih);
	if (keyErr != 0)printf("Couldn't create key thread!\n");

	return ih;
}
u8 InputHandler::getKeyCode(u32 someCode){
	if (someCode == KEY_UP)return CODE_UP;
	if (someCode == KEY_DOWN)return CODE_DOWN;
	if (someCode == KEY_LEFT)return CODE_LEFT;
	if (someCode == KEY_RIGHT)return CODE_RIGHT;
	
	if (someCode == KEY_ESC)
		return CODE_ESC;
	if (someCode == KEY_BACKSPACE)
		return CODE_BACKSPACE;
	if (someCode == KEY_ENTER)
		return CODE_ENTER;
	if (someCode == KEY_TAB)
		return CODE_TAB;
	if (someCode == KEY_LEFTCTRL)
		return CODE_LCTRL;
	if (someCode == KEY_LEFTSHIFT)
		return CODE_LSHIFT;
	if (someCode == KEY_RIGHTCTRL)
		return CODE_RCTRL;
	if (someCode == KEY_RIGHTSHIFT)
		return CODE_RSHIFT;
	if (someCode == KEY_RIGHTALT)
		return CODE_RALT;
	if (someCode == KEY_LEFTALT)
		return CODE_LALT;
	if (someCode == KEY_SCROLLLOCK)
		return CODE_SCRLLCK;
	if (someCode == KEY_CAPSLOCK)
		return CODE_CAPS;
	if (someCode == KEY_NUMLOCK)
		return CODE_NUMLCK;
	if (someCode >= KEY_1 && someCode <= KEY_0)
		return someCode == KEY_0 ? 0 : (someCode - KEY_1 + 1);
	if (someCode >= KEY_Q && someCode <= KEY_P){
		string str("qwertyuiop");
		return str[someCode - KEY_Q] - 'a' + 10;
	}
	if (someCode >= KEY_A && someCode <= KEY_L){
		string str("asdfghjkl");
		return str[someCode - KEY_A] - 'a' + 10;
	}
	if (someCode >= KEY_Z && someCode <= KEY_M) {
		string str("zxcvbnm");
		return str[someCode - KEY_Z] - 'a' + 10;
	}
	if (someCode >= KEY_F1 && someCode <= KEY_F10)
		return CODE_F1 + (someCode - KEY_F1);
	if (someCode == KEY_F11)return CODE_F11;
	if (someCode == KEY_F12)return CODE_F12;
	if (someCode == KEY_SPACE)return CODE_SPACE;
	if (someCode == KEY_MINUS)return CODE_MINUS;
	if (someCode == KEY_EQUAL)return CODE_EQUALS;
	if (someCode == KEY_LEFTBRACE)return CODE_LBRACE;
	if (someCode == KEY_RIGHTBRACE)return CODE_RBRACE;
	if (someCode == KEY_SEMICOLON)return CODE_SEMICOLON;
	if (someCode == KEY_APOSTROPHE)return CODE_APOSTROPHE;
	if (someCode == KEY_GRAVE)return CODE_TILDE;
	if (someCode == KEY_BACKSLASH)return CODE_BACKSLASH;
	if (someCode == KEY_COMMA)return CODE_COMMA;
	if (someCode == KEY_DOT)return CODE_DOT;
	if (someCode == KEY_SLASH)return CODE_SLASH;
	if (someCode >= KEY_KP7 && someCode <= KEY_KP9)
		return CODE_KP7 + (someCode - KEY_KP7);
	if (someCode == KEY_KPMINUS)
		return CODE_KPMINUS;
	if (someCode == KEY_KPPLUS)
		return CODE_KPPLUS;
	if (someCode >= KEY_KP4 && someCode <= KEY_KP6)
		return CODE_KP4 + (someCode - KEY_KP4);
	if (someCode >= KEY_KP1 && someCode <= KEY_KP3)
		return CODE_KP1 + (someCode - KEY_KP1);
	if (someCode == KEY_KP0)
		return CODE_KP0;
	if (someCode == KEY_KPDOT)
		return CODE_KPDOT;
	if (someCode == KEY_KPENTER)
		return CODE_KPENTER;
	if (someCode == KEY_KPSLASH)
		return CODE_KPSLASH;
	if (someCode == KEY_KPCOMMA)
		return CODE_KPCOMMA;
	
	if (someCode >= KEY_F13 && someCode <= KEY_F24)
		return CODE_F13 + (someCode - KEY_F13);
	
	if (someCode == KEY_PAGEDOWN)return CODE_PAGEDOWN;
	if (someCode == KEY_PAGEUP)return CODE_PAGEUP;
	if (someCode == KEY_HOME)return CODE_HOME;
	if (someCode == KEY_END)return CODE_END;
	if (someCode == KEY_INSERT)return CODE_INSERT;
	if (someCode == KEY_DELETE)return CODE_DELETE;
	
	return CODE_UNDEF;
//	return someCode;
}
#endif

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>
#include "chat_multiplayer.h"
#include "game_multiplayer_js_import.h"

extern "C" {
	void SendChatMessage(const char* msg) {
		EM_ASM({
			SendMessageString(UTF8ToString($0));
		}, msg);
	};
}
#else 
extern "C" {
	void SendChatMessage(const char* msg) {
	};
}
#endif
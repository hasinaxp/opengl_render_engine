#include "eventSystem.h"
#include <SDL.h>
#include <iostream>
#include <map>
#include "application.h"

namespace sp {

	MouseEventInfo EventSystem::Mouse;
	KeyEventInfo EventSystem::Keyboard;

	bool EventSystem::_shouldQuit = false;
	uint EventSystem::_controlWord = 0x00010000;
	std::vector<EventHandlerInfo> EventSystem::_events = {};
	std::vector<KeyCode> EventSystem::_pressedKeys = {};
	std::vector<KeyCode> EventSystem::_releasedKeys = {};
	std::vector<KeyCode> EventSystem::_heldKeys = {};

	
	void EventSystem::resetControlWord()
	{
		_controlWord = 0x00010000;
	}

	void EventSystem::addControlWord(uint word)
	{
		_controlWord |= word;
	}

	void EventSystem::pollFromWindow(void* window)
	{
		SDL_Window* win = reinterpret_cast<SDL_Window*>(window);
		if (!win) {
			std::cout << "err: nullptr is passed while polling events\n";
			return;
		}
		_pressedKeys = {};
		_releasedKeys = {};
		std::vector<KeyCode>::iterator it;
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT)
				_shouldQuit = true;

			switch (ev.type)
			{
			case SDL_KEYDOWN:
				addControlWord(static_cast<uint>(EventType::key_down));
				it = std::find(_heldKeys.begin(), _heldKeys.end(), static_cast<KeyCode>(ev.key.keysym.sym));
				if (it == _heldKeys.end()) {
					_pressedKeys.push_back(static_cast<KeyCode>(ev.key.keysym.sym));
					_heldKeys.push_back(static_cast<KeyCode>(ev.key.keysym.sym));
				}
				break;
			case SDL_KEYUP:
				addControlWord(static_cast<uint>(EventType::key_up));
				_releasedKeys.push_back(static_cast<KeyCode>(ev.key.keysym.sym));
				it = std::find(_heldKeys.begin(), _heldKeys.end(), static_cast<KeyCode>(ev.key.keysym.sym));
				if (it != _heldKeys.end())
					_heldKeys.erase(it);
				break;
			case SDL_MOUSEMOTION:
				addControlWord(static_cast<uint>(EventType::mouse_move));
				Mouse.x = (float)ev.motion.x * 2 /Application::getWidth() - 1.0f;
				Mouse.y = 1.0f - (float)ev.motion.y * 2 /Application::getHeight() ;
				Mouse.dx = (float)ev.motion.xrel;
				Mouse.dy = (float)ev.motion.yrel;
				Mouse.screenX = ev.motion.x;
				Mouse.screenY = ev.motion.y;
				break;
			case SDL_MOUSEBUTTONDOWN:

				if (ev.button.button == SDL_BUTTON_LEFT)
				{
					addControlWord(static_cast<uint>(EventType::mouse_down_left));
				}
				else if (ev.button.button == SDL_BUTTON_RIGHT)
				{
					addControlWord(static_cast<uint>(EventType::mouse_down_right));
				}
				else if (ev.button.button == SDL_BUTTON_MIDDLE)
				{
					addControlWord(static_cast<uint>(EventType::mouse_down_middle));
				}
				break;
			case SDL_MOUSEBUTTONUP:

				if (ev.button.button == SDL_BUTTON_LEFT)
				{
					addControlWord(static_cast<uint>(EventType::mouse_up_left));
				}
				else if (ev.button.button == SDL_BUTTON_RIGHT)
				{
					addControlWord(static_cast<uint>(EventType::mouse_up_right));
				}
				else if (ev.button.button == SDL_BUTTON_MIDDLE)
				{
					addControlWord(static_cast<uint>(EventType::mouse_up_middle));
				}
				break;
			case SDL_MOUSEWHEEL:
				break;
				addControlWord(static_cast<uint>(EventType::mouse_wheel));
			}
		} // while loop for polling events ends here

	}

	void EventSystem::resolve()
	{
		std::map<EventType, bool> mask_flags = {};
		std::map<EventType, byte> priority_flags = {};

		if (_heldKeys.size() > 0)
			addControlWord(static_cast<uint>(EventType::key_hold));





		for (auto listener : _events){
			if (mask_flags.count(listener.type) == 0){
				mask_flags[listener.type] = false;
				priority_flags[listener.type] = listener.priority;
			}
			

			if ((mask_flags[listener.type] == true) && (priority_flags[listener.type] > listener.priority)){

				continue;
			}
			if (matchSequence(_controlWord, static_cast<uint>(listener.type))) {
				bool satisfies = true;
				switch (listener.type)
				{
				case EventType::key_down:
				{
					KeyEventInfo* ef = new KeyEventInfo();
					for (auto k : _pressedKeys) {
						ef->timeStamp = SDL_GetTicks();
						ef->type = EventType::key_down;
						ef->keyCode = k;
						ef->state = 1;
						ef->heldKeys = _heldKeys;
						Keyboard = *ef;
						listener.handler(ef);
					}
					delete ef;
				}
					break;
				case EventType::key_up:
				{
					KeyEventInfo* ef = new KeyEventInfo();
					for (auto k : _releasedKeys) {
						ef->timeStamp = SDL_GetTicks();
						ef->type = EventType::key_up;
						ef->keyCode = k;
						ef->state = 2;
						ef->heldKeys = _heldKeys;
						Keyboard = *ef;
						listener.handler(ef);
					}
					delete ef;
				}
				case EventType::key_hold:
				{
					KeyEventInfo* ef = new KeyEventInfo();
					for (auto k : _heldKeys) {
						ef->timeStamp = SDL_GetTicks();
						ef->type = EventType::key_hold;
						ef->keyCode = k;
						ef->state = 3;
						ef->heldKeys = _heldKeys;
						Keyboard = *ef;
						listener.handler(ef);
					}
					delete ef;
				}
					break;
				case EventType::mouse_move:
				case EventType::mouse_down:
				case EventType::mouse_up:
				case EventType::mouse_down_left:
				case EventType::mouse_down_middle:
				case EventType::mouse_down_right:
				case EventType::mouse_up_left:
				case EventType::mouse_up_middle:
				case EventType::mouse_up_right:
				{
					Mouse.type = listener.type;
					Mouse.timeStamp = SDL_GetTicks();
					listener.handler(&Mouse);
				}
				break;
				default:
					break;
				}
				
			}

			if (listener.mask == true){
				mask_flags[listener.type] = true;
				priority_flags[listener.type] = listener.priority;
			}
		}
	}

	bool EventSystem::shouldQuit()
	{
		return _shouldQuit;
	}


	EventHandlerInfo EventSystem::addEventListener(EventType ev, std::function<void(EventInfo *)> handler, bool mask, uint id, byte priority)
	{
		id = getValidId(id);

		EventHandlerInfo evnt;
		evnt.id = id;
		evnt.type = ev;
		evnt.handler = handler;
		evnt.mask = mask;
		evnt.priority = priority;

		_events.push_back(evnt);
		std::sort(_events.begin(), _events.end(), [](auto a, auto b) {
			return a.priority > b.priority;
		});
		return evnt;

	}

	void EventSystem::removeEventListener(uint id)
	{
		for (int i = 0; i < _events.size(); i++)
		{
			if (_events[i].id == id)
			{
				_events[i] = _events[_events.size() - 1];
				_events.pop_back();
				break;
			}
		}
		std::sort(_events.begin(), _events.end(), [](auto a, auto b) {
			return a.priority > b.priority;
			});
	}

	byte EventSystem::getListenerPriority(int id)
	{
		byte priority = 0;
		for (int i = 0; i < _events.size(); i++)
		{
			if (_events[i].id == id)
			{
				priority = _events[i].priority;
				break;
			}
		}
		return priority;
	}

	bool EventSystem::getListenerMask(int id)
	{
		bool mask = false;
		for (int i = 0; i < _events.size(); i++)
		{
			if (_events[i].id == id)
			{
				mask = _events[i].mask;
				break;
			}
		}
		return mask;
	}

	void EventSystem::setListenerPriority(int id, byte priority)
	{
		for (int i = 0; i < _events.size(); i++)
		{
			if (_events[i].id == id)
			{
				_events[i].priority = priority;
				break;
			}
		}
	}

	void EventSystem::setListenerMask(int id, bool mask)
	{
		for (int i = 0; i < _events.size(); i++)
		{
			if (_events[i].id == id)
			{
				_events[i].mask = mask;
				break;
			}
		}
	}

	MouseEventInfo EventSystem::getMouse(EventInfo* info)
	{
		MouseEventInfo ef = *(static_cast<MouseEventInfo *>(info));
		return ef;
	}

	KeyEventInfo EventSystem::getKeyboard(EventInfo* info)
	{
		KeyEventInfo ef = *(static_cast<KeyEventInfo*>(info));
		return ef;
	}

	char EventSystem::getKey(EventInfo* info)
	{
		KeyEventInfo ef = *(static_cast<KeyEventInfo*>(info));
		return static_cast<char>(ef.keyCode);
	}

	char EventSystem::getKey(KeyEventInfo info)
	{
		return static_cast<char>(info.keyCode);
	}

	char EventSystem::getKey(KeyCode code)
	{
		return static_cast<char>(code);
	}

	bool EventSystem::isHoldingKey(KeyCode code)
	{
		auto it = std::find(_heldKeys.begin(), _heldKeys.end(), code);
		return (it != _heldKeys.end());
	}

	bool EventSystem::isPressedKey(KeyCode code)
	{
		auto it = std::find(_pressedKeys.begin(), _pressedKeys.end(), code);
		return (it != _pressedKeys.end());
	}

	bool EventSystem::isReleasedKey(KeyCode code)
	{
		auto it = std::find(_releasedKeys.begin(), _releasedKeys.end(), code);
		return (it != _releasedKeys.end());
	}



	int EventSystem::getValidId(int id)
	{
		bool is_present = false;
		if (id == 0)
			is_present = true;
		else
			for (auto ev : _events)
			{
				if (ev.id == id)
				{
					is_present = true;
					break;
				}
			}

		if (!is_present) {
			return id;
		}
		else {
			int new_id = rand();
			return getValidId(new_id);
		}

	}

	bool EventSystem::matchSequence(uint expression, uint sequence)
	{
		sequence = ~sequence;
		expression |= sequence;
		return ((~expression) == 0);
	}



};
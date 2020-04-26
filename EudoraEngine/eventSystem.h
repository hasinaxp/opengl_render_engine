#pragma once
#include "api.h"
#include <algorithm>
#include <functional>
#include <vector>


#define sp_convert_event_handler(x) std::bind(&x, this, std::placeholders::_1)

namespace sp {

	enum class EventType
	{
		none = 0x00010000,
		key_down = 0x00010001,
		key_up = 0x00010002,
		mouse_move = 0x00010004,
		mouse_down = 0x00010008,
		mouse_up = 0x00010010,
		mouse_wheel = 0x00010020,

		mouse_down_left = 0x00010108,
		mouse_up_left = 0x00010210,
		mouse_down_right = 0x00010408,
		mouse_up_right = 0x00010810,
		mouse_down_middle = 0x00011008,
		mouse_up_middle = 0x00012010,
		
		key_hold = 0x00110001,
		click = 0x00020001,
	};

	//all keycodes (values are same as sdl)
	enum class KeyCode
	{
		unknown = 0x00,
		backspace = 0x08,
		tab = 0x09,
		rtn = 0x0D,
		escape = 0x1B,
		space = 0x20,
		exclaim = 0x21,
		quotedbl = 0x22,
		hash = 0x23,
		dollar = 0x24,
		percent = 0x25,
		ampersand = 0x26,
		quote = 0x27,
		leftparen = 0x28,
		rightparen = 0x29,
		asterisk = 0x2A,
		plus = 0x2B,
		comma = 0x2C,
		minus = 0x2D,
		period = 0x2E,
		slash = 0x2F,
		n0 = 0x30,
		n1 = 0x31,
		n2 = 0x32,
		n3 = 0x33,
		n4 = 0x34,
		n5 = 0x35,
		n6 = 0x36,
		n7 = 0x37,
		n8 = 0x38,
		n9 = 0x39,
		colon = 0x3A,
		semicolon = 0x3B,
		less = 0x3C,
		equals = 0x3D,
		greater = 0x3E,
		question = 0x3F,
		at = 0x40,
		leftbracket = 0x5B,
		backslash = 0x5C,
		rightbracket = 0x5D,
		caret = 0x5E,
		underscore = 0x5F,
		backquote = 0x60,
		a = 0x61,
		b = 0x62,
		c = 0x63,
		d = 0x64,
		e = 0x65,
		f = 0x66,
		g = 0x67,
		h = 0x68,
		i = 0x69,
		j = 0x6A,
		k = 0x6B,
		l = 0x6C,
		m = 0x6D,
		n = 0x6E,
		o = 0x6F,
		p = 0x70,
		q = 0x71,
		r = 0x72,
		s = 0x73,
		t = 0x74,
		u = 0x75,
		v = 0x76,
		w = 0x77,
		x = 0x78,
		y = 0x79,
		z = 0x7A,
		del = 0x7F,
		capslock = 0x40000039,
		f1 = 0x4000003A,
		f2 = 0x4000003B,
		f3 = 0x4000003C,
		f4 = 0x4000003D,
		f5 = 0x4000003E,
		f6 = 0x4000003F,
		f7 = 0x40000040,
		f8 = 0x40000041,
		f9 = 0x40000042,
		f10 = 0x40000043,
		f11 = 0x40000044,
		f12 = 0x40000045,
		printscreen = 0x40000046,
		scrolllock = 0x40000047,
		pause = 0x40000048,
		insert = 0x40000049,
		home = 0x4000004A,
		pageup = 0x4000004B,
		end = 0x4000004D,
		pagedown = 0x4000004E,
		right = 0x4000004F,
		left = 0x40000050,
		down = 0x40000051,
		up = 0x40000052,
		numlockclear = 0x40000053,
		kp_divide = 0x40000054,
		kp_multiply = 0x40000055,
		kp_minus = 0x40000056,
		kp_plus = 0x40000057,
		kp_enter = 0x40000058,
		kp_1 = 0x40000059,
		kp_2 = 0x4000005A,
		kp_3 = 0x4000005B,
		kp_4 = 0x4000005C,
		kp_5 = 0x4000005D,
		kp_6 = 0x4000005E,
		kp_7 = 0x4000005F,
		kp_8 = 0x40000060,
		kp_9 = 0x40000061,
		kp_0 = 0x40000062,
		kp_period = 0x40000063,
		application = 0x40000065,
		power = 0x40000066,
		kp_equals = 0x40000067,
		shift_left = 0x400000E1,
		shift_right = 0x400000E5,
		ctrl_left = 0x400000E0,
		ctrl_right = 0x400000E4,
		alt_left = 0x400000E2,
		alt_right = 0x400000E6,
	};

	
	//base ckass for event information
	struct SP_API EventInfo
	{
		uint timeStamp = 0;
		EventType type = EventType::none;

	};
	//structure containing mouse event info
	struct SP_API MouseEventInfo: public EventInfo
	{
		real x = 0.0f;
		real y = 0.0f;
		real dx = 0.0f;
		real dy = 0.0f;
		int screenX = 0;
		int screenY = 0;
	};

	//structure containing keyboard event info
	struct SP_API KeyEventInfo : public EventInfo
	{
		std::vector<KeyCode> heldKeys;
		KeyCode keyCode;
		byte state; // 1: pressed 2: released 3:holding
		bool isPressed(KeyCode code) {
			auto it = std::find(heldKeys.begin(), heldKeys.end(), code);
			return (it != heldKeys.end());
		}
	};

	//structure containing information about a window Event
	struct SP_API EventHandlerInfo
	{
		uint id = 0;
		EventType type = EventType::none;
		std::function<void(EventInfo *)> handler;
		byte priority = 20;
		bool mask = false; // if true no other event after this should be executed
	};

	//singletone class for translating and handling events
	class SP_API EventSystem 
	{
	public:
		static MouseEventInfo Mouse;
		static KeyEventInfo Keyboard;
	private:
		static bool _shouldQuit;
		static uint _controlWord;
		static std::vector<EventHandlerInfo> _events;
		static std::vector<KeyCode> _pressedKeys;
		static std::vector<KeyCode> _releasedKeys;
		static std::vector<KeyCode> _heldKeys;

	public:
		static void resetControlWord();
		static void addControlWord(uint word);
		static void pollFromWindow(void * window);
		static void resolve();
		static bool shouldQuit();
		
		static EventHandlerInfo addEventListener(EventType ev, std::function<void(EventInfo*)> handler, bool mask = true, uint id = 0, byte priority = 10);
		static void removeEventListener(uint id);
		static byte getListenerPriority(int id);
		static bool getListenerMask(int id);
		static void setListenerPriority(int id, byte priority);
		static void setListenerMask(int id, bool mask);

		static MouseEventInfo getMouse(EventInfo* info);
		static KeyEventInfo getKeyboard(EventInfo* info);
		static char getKey(EventInfo* info);
		static char getKey(KeyEventInfo info);
		static char getKey(KeyCode code);

		static bool isHoldingKey(KeyCode code);
		static bool isPressedKey(KeyCode code);
		static bool isReleasedKey(KeyCode code);


	private:
		static int getValidId(int id);
		static bool matchSequence(uint expression, uint sequence);
	};

};
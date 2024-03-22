#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <time.h>
#include <string>

using namespace geode::prelude;

#ifdef GEODE_IS_WINDOWS
namespace matdash {
	struct Console {
		std::ofstream out, in;
		Console() {
			AllocConsole();
			out = decltype(out)("CONOUT$", std::ios::out);
			in = decltype(in)("CONIN$", std::ios::in);
			std::cout.rdbuf(out.rdbuf());
			std::cin.rdbuf(in.rdbuf());

			FILE* dummy;
			freopen_s(&dummy, "CONOUT$", "w", stdout);
		}
		~Console() {
			out.close();
			in.close();
		}
	};

	inline void create_console() {
		static Console console;
	}
}
#endif

namespace Utils
{
	const char* getNameObj(cocos2d::CCNode* obj);

	cocos2d::CCSize WinSize();

	template <class R, class T>
	R& from(T base, intptr_t offset) {
		return *reinterpret_cast<R*>(reinterpret_cast<uintptr_t>(base) + offset);
	}

	template<typename T>
	std::string vectorToString(const std::vector<T>& vec) {
		std::ostringstream oss;
		for (size_t i = 0; i < vec.size(); ++i) {
			if (i != 0) {
				oss << " ";
			}
			oss << vec[i];
		}
		return oss.str();
	}

	template<typename T>
	std::vector<T> stringToVector(const std::string& str) {
		std::vector<T> vec;
		std::istringstream iss(str);
		T val;
		while (iss >> val) {
			vec.push_back(val);
		}
		return vec;
	}

	bool hasParentWithID(cocos2d::CCNode* node, const std::string& parentID);
	cocos2d::CCNode* getParentWithID(cocos2d::CCNode* node, const std::string& parentID);

}

#define public_cast(value, member) [](auto* v) { \
	class FriendClass__; \
	using T = typename std::remove_pointer<decltype(v)>::type; \
	class FriendeeClass__: public T { \
	protected: \
		friend FriendClass__; \
	}; \
	class FriendClass__ { \
	public: \
		auto& get(FriendeeClass__* v) { return v->member; } \
	} c; \
	return c.get(reinterpret_cast<FriendeeClass__*>(v)); \
}(value)
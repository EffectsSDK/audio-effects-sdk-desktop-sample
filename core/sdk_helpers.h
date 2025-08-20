#ifndef CORE_SDK_HELPERS_H
#define CORE_SDK_HELPERS_H

#include <memory>

namespace audio_effects_sdk { class IRelease; }

namespace core {

struct SDKReleaser
{
	void operator()(audio_effects_sdk::IRelease* obj);
};

template<class T>
using SDKUniquePtr = std::unique_ptr<T, SDKReleaser>;

template<class T>
std::shared_ptr<T> wrapSDKObject(T* obj)
{
	return std::shared_ptr<T>(obj, SDKReleaser());
}

} // namespace core

#endif // CORE_SDK_HELPERS_H

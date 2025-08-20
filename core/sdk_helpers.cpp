#include "sdk_helpers.h"

#include <audio_effects_sdk/irelease.h>

namespace core {

void SDKReleaser::operator()(audio_effects_sdk::IRelease* obj) {
	if (nullptr != obj) {
		obj->release();
	}
}

}
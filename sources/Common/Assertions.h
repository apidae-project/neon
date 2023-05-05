#pragma once

#include <Core/Panic.h>

namespace Neon {
void Assert(const bool Assertion, const char *ErrorMessage);
void Assert(const bool Assertion, const char *ErrorMessage) {
	if(!Assertion) AssertionFail(ErrorMessage, __FILE__, __LINE__, __PRETTY_FUNCTION);
	else return;
}
} // namespace Neon

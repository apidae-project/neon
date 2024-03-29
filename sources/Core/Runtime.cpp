#include <Core/Panic.h>
#include <Core/Runtime.h>
#include <cstdint>

using namespace Neon;

extern "C"
{
	atexit_func_entry_t __atexit_funcs[ATEXIT_MAX_FUNCS];
	uarch_t __atexit_func_count = 0;
	void *__dso_handle = nullptr;

	int __cxa_atexit(void (*func)(void *), void *objptr, void *dso)
	{
		if (__atexit_func_count >= ATEXIT_MAX_FUNCS) return -1;

		__atexit_funcs[__atexit_func_count].destructor_func = func;
		__atexit_funcs[__atexit_func_count].obj_ptr = objptr;
		__atexit_funcs[__atexit_func_count].dso_handle = dso;
		__atexit_func_count++;

		return 0;
	}

	void __cxa_finalize(void *func)
	{
		uarch_t i = __atexit_func_count;
		if (func == nullptr)
		{
			while (i--)
			{
				if (__atexit_funcs[i].destructor_func)
				{
					(*__atexit_funcs[i].destructor_func)(
					    __atexit_funcs[i].obj_ptr);
				}
			}
			return;
		}

		while (i--)
		{
			if (__atexit_funcs[i].destructor_func == func)
			{
				(*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
				__atexit_funcs[i].destructor_func = 0;
			}
		}
	}

	void __cxa_pure_virtual() { Panic("Pure virtual function called!"); }

	namespace __cxxabiv1
	{
	int __cxa_guard_acquire(uint64_t *guard)
	{
		if ((*guard) & 0x0001) return 0;
		if ((*guard) & 0x0100) abort();
		*guard |= 0x0100;
		return 1;
	}

	void __cxa_guard_release(uint64_t *guard) { *guard |= 0x01; }

	void __cxa_guard_abort(uint64_t *guard)
	{
		Panic("__cxa_guard_abort() called!");
	}
	} // namespace __cxxabiv1

	uintptr_t __stack_chk_guard = 0x595E9FBD94FDA766;
	[[noreturn]] void __stack_chk_fail() { Panic("Stack smashing detected!"); }
}

namespace std
{
[[gnu::noreturn]] void terminate() noexcept
{
	Panic("std::terminate() called.");
}
} // namespace std
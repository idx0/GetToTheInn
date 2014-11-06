#pragma once

#undef NDEBUG
#include <assert.h>
#include <new>

namespace Utils
{
	template<typename T>
    bool isValid(T* obj) {
        return (obj != static_cast<T*>(0));
    }

    template<typename T>
    class Singleton
    {
    public:
        static T* getInstance() {
            if (!isValid(m_instance)) {
                m_instance = new (std::nothrow) T;
            }

            assert(m_instance);

            return m_instance;
        }
    private:
        static T* m_instance;
    };

    template<typename T>
    T* Singleton<T>::m_instance = static_cast<T*>(0);

	template<typename T>
	void clamp(T& var, const T& mn, const T& mx)
	{
		if (var < mn) { var = mn; }
		if (var > mx) { var = mx; }
	}
}

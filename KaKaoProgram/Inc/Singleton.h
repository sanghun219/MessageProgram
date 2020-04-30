#pragma once
#include "ServLibrary.h"

template<typename T>
class Singleton
{
private:
	~Singleton() {}
	Singleton() {}
	Singleton(const T& other) {}
	T& operator =(const T& other) {}
public:
	inline static T* GetInst();
private:
	static std::shared_ptr<T> m_pInst;
	static std::recursive_mutex m_rm;
};
template<typename T> std::shared_ptr<T>	Singleton<T>::m_pInst = nullptr;
template<typename T> std::recursive_mutex Singleton<T>::m_rm;

template<typename T>
inline T * Singleton<T>::GetInst()
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);
	if (m_pInst == nullptr)
	{
		m_pInst.reset(new T);
	}
	return m_pInst.get();
}

template <typename T>
T Singleton<T>::_instance;

template <typename T>
inline Singleton<T>::Singleton()
{
}

template <typename T>
inline Singleton<T>::~Singleton()
{
}

template<typename T>
inline T* Singleton<T>::getInstancePtr()
{
    return &_instance;
}

template<typename T>
inline T& Singleton<T>::getInstance()
{
    return _instance;
}

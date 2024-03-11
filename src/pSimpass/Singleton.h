#ifndef _SINGLETON_H_
#define _SINGLETON_H_

template <typename T>
class Singleton
{
public:
    static T* getInstancePtr();
    static T& getInstance();

protected:
    Singleton();
    virtual ~Singleton();

private:
    Singleton( const Singleton<T>& instance );
    T& operator=( const Singleton<T>& instance );
    static T _instance;
};


#include "Singleton.inl"

#endif //#ifndef _SINGLETON_H_

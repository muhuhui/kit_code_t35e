#ifndef SINGLETON_H
#define SINGLETON_H

#include <stdlib.h>
#include <utility>

template <class T>
class SingleTon
{
protected:
    SingleTon(){}

private:
    SingleTon(const SingleTon&){}
    SingleTon& operator=(const SingleTon&){}
    static T* m_instance;

public:
    static T* GetInstance();
};


template <class T>
T* SingleTon<T>::GetInstance()
{
    return m_instance;
}

template <class T>
T* SingleTon<T>::m_instance = new T();


#endif // SINGLETON_H

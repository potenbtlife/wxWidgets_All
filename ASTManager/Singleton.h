#ifndef Singleton_h
#define Singleton_h

//not used
template<class T>
class Singleton
{
    static T* singleton;
    static unsigned int n;
protected:
public:
    Singleton()
    {
        if(!singleton)
            singleton = new T;
        n++;
    };
    virtual ~Singleton(){
        if(!--n)
            delete singleton;
    };
    T* operator->()
    {
        return singleton;
    }
    T& GetInstance()
    {
        return *singleton;
    }
};

template<class T> T* Singleton<T>::singleton = NULL;
template<class T> unsigned int Singleton<T>::n =0;
#endif

#ifndef SINGLETON_H
#define SINGLETON_H
#include <stddef.h>

template <typename T> 
class singleton
{
public:
    virtual ~singleton(){}
    static T* getInstance()
    {
        if (m_Instance == NULL)m_Instance = new T();
        return m_Instance;
    }
   
    
    static void deleteInstance()
    {
        if ( m_Instance != NULL)
        {
            delete m_Instance;
            m_Instance = NULL;
        }
    }

protected:
    singleton(){}
private:
    static T* m_Instance;
};

template <typename T>
T* singleton<T>::m_Instance = NULL;
#endif

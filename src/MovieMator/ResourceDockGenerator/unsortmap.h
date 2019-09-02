#ifndef UNSORTMAP_H
#define UNSORTMAP_H

#include <QMap>

template <class Key, class T>
class UnsortMap
{
public:
    explicit UnsortMap();

    virtual ~UnsortMap();

    // 是否含有 key
    bool contains(const Key &key);

    // 通过 key获取 value
    T value(const Key &key);

    // 获取排序列表
    QList<Key> keys();

    // 把 key-value存入到数据中指定位置 nIndex
    void insert(int nIndex, const Key &key, const T &value);

    // 把 key-value追加到数据中
    void append(const Key &key, const T &value);

    // 获取数据个数
    int count() const;

private:
    QMap<Key, T> m_values;

    QList<Key>   m_keys;
};

template <class Key, class T>
UnsortMap<Key, T>::UnsortMap()
{

}

template <class Key, class T>
UnsortMap<Key, T>::~UnsortMap()
{

}

template <class Key, class T>
bool UnsortMap<Key, T>::contains(const Key &key)
{
    return m_keys.contains(key);
}

template <class Key, class T>
T UnsortMap<Key, T>::value(const Key &key)
{
    return m_values.value(key);
}

template <class Key, class T>
QList<Key> UnsortMap<Key, T>::keys()
{
    return m_keys;
}

template <class Key, class T>
void UnsortMap<Key, T>::insert(int nIndex, const Key &key, const T &value)
{
    m_keys.insert(nIndex, key);
    m_values.insert(key, value);
}

template <class Key, class T>
void UnsortMap<Key, T>::append(const Key &key, const T &value)
{
    m_keys.append(key);
    m_values.insert(key, value);
}

template <class Key, class T>
int UnsortMap<Key, T>::count() const
{
    return m_keys.count();
}

#endif // UNSORTMAP_H

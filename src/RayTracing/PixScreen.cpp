#include "PixScreen.h"

#include "ColorLight.h"
template class PixScreenT<ColorLight>; // Explicit instantiation of ColorLight

template<typename T>
PixScreenT<T>::PixScreenT() : PixScreenT(0, 0)
{}
template<typename T>
PixScreenT<T>::PixScreenT(const QSize &size) : w(size.width()), h(size.height()), screen(new T *[w])
{
    int x = 0;
    for (x = 0; x < w; x++) {
        screen[x] = new T[h];
    }
}
template<typename T>
PixScreenT<T>::PixScreenT(const int &width, const int &height) : w(width), h(height), screen(new T *[w])
{
    int x = 0;
    for (x = 0; x < w; x++) {
        screen[x] = new T[h];
    }
}
template<typename T>
PixScreenT<T>::PixScreenT(const PixScreenT<T> &that) : w(that.w), h(that.h), screen(new T *[w])
{
    int x = 0;
    for (x = 0; x < w; x++) {
        screen[x] = new T[h];
        memcpy(screen[x], that.screen[x], h * sizeof(T));
    }
}

template<typename T>
PixScreenT<T>::~PixScreenT()
{
    int x = 0;
    for (x = 0; x < w; x++) {
        delete[] screen[x];
    }
    delete[] screen;
}

template<typename T>
PixScreenT<T> &PixScreenT<T>::operator=(const PixScreenT<T> &ps)
{
    int x = 0;
    if (w != ps.w) {
        for (x = 0; x < w; x++) {
            delete[] screen[x];
        }
        delete[] screen;
        w = ps.w;
        h = ps.h;
        screen = new T *[w];
        for (x = 0; x < w; x++) {
            screen[x] = new T[h];
            memcpy(screen[x], ps.screen[x], h * sizeof(T));
        }
    } else if (h != ps.h) {
        h = ps.h;
        for (x = 0; x < w; x++) {
            delete[] screen[x];
            screen[x] = new T[w];
            memcpy(screen[x], ps.screen[x], h * sizeof(T));
        }
    } else {
        for (x = 0; x < w; x++) {
            memcpy(screen[x], ps.screen[x], h * sizeof(T));
        }
    }
    return *this;
}

template<typename T>
constexpr const T &PixScreenT<T>::at(int x, const int y) const
{
    return screen[x][y];
}

template<typename T>
constexpr T &PixScreenT<T>::operator()(int x, const int y)
{
    return screen[x][y];
}

template<typename T>
void PixScreenT<T>::setColumn(int x, const T *column)
{
    memcpy(screen[x], column, h * sizeof(T));
}

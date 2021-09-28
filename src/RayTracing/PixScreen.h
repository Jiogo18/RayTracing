#ifndef PIXSCREEN_H
#define PIXSCREEN_H

#include "../Qt_compat/QSize"

template<typename T>
class PixScreenT
{
public: // il faut au moins un PixScreen<T> dans ce fichier pour en avoir avec le même type dans d'autres fichiers
    PixScreenT();
    PixScreenT(const QSize &size);
    PixScreenT(const int &width, const int &height);
    PixScreenT(const PixScreenT<T> &that);
    ~PixScreenT();
    PixScreenT<T> &operator=(const PixScreenT<T> &ps);

    constexpr inline void set(int x, int y, const T &v);
    constexpr const T &at(int x, const int y) const;
    constexpr T &operator()(int x, const int y);

    constexpr inline const int &width() const { return w; }
    constexpr inline const int &height() const { return h; }
    constexpr inline QSize size() const { return QSize{w, w}; }
    constexpr inline int rowNumber() const { return w * h; }

    constexpr inline T *operator[](int x);
    constexpr inline const T *getColumn(int x) const;
    void setColumn(int x, const T *column);

    constexpr T *beginColumn(const int &x) { return &screen[x][0]; }
    constexpr T *endColumn(const int &x) { return &screen[x][h]; }

private:
    int w;
    int h;
    T **screen;
};

template<typename T>
constexpr inline void PixScreenT<T>::set(int x, int y, const T &v)
{
    screen[x][y] = v;
}

template<typename T>
constexpr inline T *PixScreenT<T>::operator[](int x)
{
    return screen[x];
}
template<typename T>
constexpr inline const T *PixScreenT<T>::getColumn(int x) const
{
    return screen[x];
}

// template<typename T>
// class PixScreenQList
// {
// public: // il faut au moins un PixScreenQ<T> dans ce fichier pour en avoir avec le même type dans d'autres fichiers
//     constexpr inline PixScreenQList() : screen(0) {}
//     constexpr inline PixScreenQList(const QSize &size) : screen(size.width()), h(size.height())
//     {
//         for (int i = 0; i < width(); i++)
//             screen[i] = QList<T>(h);
//     }
//     constexpr inline PixScreenQList(const int &width, const int &height) : screen(width), h(height)
//     {
//         for (int i = 0; i < width; i++)
//             screen[i] = QList<T>(h);
//     }
//     constexpr inline PixScreenQList *operator=(const PixScreenQList &ps)
//     {
//         screen = ps.screen;
//         h = ps.h;
//         return this;
//     }

//     constexpr inline void set(const int &x, const int &y, const T &pos) { screen[x][y] = pos; }
//     constexpr inline const T &at(const int &x, const int &y) const { return screen[x][y]; }
//     T &operator()(const int &x, const int &y) { return screen[x][y]; }

//     constexpr inline int width() const { return screen.size(); }
//     constexpr inline const int &height() const { return h; }
//     constexpr inline QSize size() const { return QSize(width(), h); }
//     constexpr inline int rowNumber() const { return width() * h; }

//     constexpr inline QList<T> &operator[](const int &x) { return screen[x]; }
//     constexpr inline const QList<T> &getColumn(int x) const { return screen.at(x); }
//     constexpr inline void setColumn(const int &x, const QList<T> &column) { screen[x] = column; }

// private:
//     QList<QList<T>> screen;
//     int h;
// };

#define PIXSCREEN 2
#if PIXSCREEN == 1
template<typename T>
using PixScreen = PixScreenQList<T>;
#else
template<typename T>
using PixScreen = PixScreenT<T>;
#endif

#endif // PIXSCREEN_H

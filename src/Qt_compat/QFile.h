#pragma once

#include <string>

typedef std::string QByteArray;

class QIODevice
{
public:
    enum Type {
        ReadOnly = 1,
        Text = 2
    };
};

class QFile
{
public:
    QFile(std::string filename) {}
    bool open(int type) { return false; }
    void close() {}

    QByteArray readLine() { return ""; };

    int pos() { return 0; };
    bool atEnd() const { return true; }

    std::string errorString() const { return "No error"; };
};
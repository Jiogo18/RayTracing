#pragma once

#include "QSize.h"
#include <windows.h>
#include <wingdi.h>
#include <opencv2/core/mat.hpp>

#define RGBA(r, g, b, a) ((cv::Vec4b){(uchar)(b), (uchar)(g), (uchar)(r), (uchar)(a)})

class Image : public cv::Mat
{
public:
    enum Format {
        Format_RGB32 = CV_8UC3,
        Format_RGBA32 = CV_8UC4,
        Format_RGB64 = CV_16UC3,
        Format_RGBA64 = CV_16UC4
    };
    Image();
    Image(unsigned int width, unsigned int height, Image::Format format = Format::Format_RGB32);
    Image(const QSize &size);
    Image(const uchar *data, int width, int height, Image::Format format);
    Image(const Image &image);

    constexpr int width() const { return cols; }
    constexpr int height() const { return rows; }

    bool isNull() const { return empty(); }
    Image scaled(const QSize &size) const;
    Image::Format format() const { return (Image::Format)type(); }
    bool load(std::string filename);

    // RGB : if (channel() == 3)
    const cv::Vec3b &pixelColor3(int x, int y) const;
    void fill3(cv::Vec3b color);
    void setPixelColor3(int x, int y, cv::Vec3b color);

    // Only RGBA : if (channel() == 4)
    const cv::Vec4b &pixelColor(int x, int y) const;
    void fill(cv::Vec4b color);
    void setPixelColor(int x, int y, cv::Vec4b color);

    void fillBitmapPixels(BYTE *bytes, int size) const;
    int getDataLength() const { return cols * rows * channels(); }
    uchar *operator()(int x, int y);

private:
    int getPixels() const { return cols * rows; }
};
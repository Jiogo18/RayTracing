#include "Image.h"
#include <opencv2/opencv.hpp>

Image::Image() : cv::Mat(0, 0, Image::Format_RGB32)
{}

Image::Image(unsigned int width, unsigned int height, Image::Format format) : cv::Mat(height, width, format)
{}

Image::Image(const SIZE &size, Image::Format format) : cv::Mat(size.cx, size.cy, format)
{}

Image::Image(const uchar *data, int width, int height, Image::Format format) : cv::Mat(height, width, format)
{
    memcpy(cv::Mat::data, data, width * height * 3);
}

Image::Image(const Image &image) : cv::Mat(image)
{}

Image Image::scaled(const SIZE &size) const
{
    Image image(size.cx, size.cy, format());
    cv::resize(*this, image, cv::Size(size.cx, size.cy));
    return image;
}

bool Image::load(std::string filename)
{
    cv::Mat::operator=(cv::imread(filename, cv::IMREAD_UNCHANGED));
    if (empty()) {
        return false;
    }
    if (channels() == 4) {
        cv::cvtColor(*this, *this, cv::COLOR_BGRA2RGBA);
    } else {
        cv::cvtColor(*this, *this, cv::COLOR_BGR2RGB);
    }
    return true;
}

const cv::Vec3b &Image::pixelColor3(int x, int y) const
{
    return at<cv::Vec3b>(y, x);
}

void Image::fill3(cv::Vec3b color)
{
    int pixels = getPixels();
    for (int i = 0; i < pixels; i++) {
        at<cv::Vec3b>(i) = color;
    }
}

void Image::setPixelColor3(int x, int y, cv::Vec3b color)
{
    at<cv::Vec3b>(y, x) = color;
}

const cv::Vec4b &Image::pixelColor(int x, int y) const
{
    return at<cv::Vec4b>(y, x);
}

void Image::fill(cv::Vec4b color)
{
    int pixels = getPixels();
    for (int i = 0; i < pixels; i++) {
        at<cv::Vec4b>(i) = color;
    }
}

void Image::setPixelColor(int x, int y, cv::Vec4b color)
{
    at<cv::Vec4b>(y, x) = color;
}

void Image::fillBitmapPixels(BYTE *bytes, int size) const
{
    int x, y;
    if (channels() == 4) {
        for (y = 0; y < rows; y++) {
            for (x = 0; x < cols; x++) {
                memcpy(&bytes[(x + y * cols) * 4], &at<cv::Vec4b>(y, x), sizeof(cv::Vec4b)); // copy RGBA
            }
        }
    } else {
        for (y = 0; y < rows; y++) {
            for (x = 0; x < cols; x++) {
                memcpy(&bytes[(x + y * cols) * 4], &at<cv::Vec3b>(y, x), sizeof(cv::Vec3b)); // copy RGB
            }
        }
    }
}

uchar *Image::operator()(int x, int y)
{
    ASSERT(0 <= x && x < width() && 0 <= y && y < height());
    return data + (x + (width()) * y) * channels();
}

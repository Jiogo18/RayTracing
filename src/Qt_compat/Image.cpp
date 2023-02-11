#include "Image.h"
#include <opencv2/opencv.hpp>

Image::Image() : cv::Mat(0, 0, Image::Format_RGB32)
{}

Image::Image(unsigned int width, unsigned int height, Image::Format format) : cv::Mat(height, width, format)
{}

Image::Image(const QSize &size) : cv::Mat(size.height(), size.width(), Image::Format_RGB32)
{}

Image::Image(const uchar *data, int width, int height, Image::Format format) : cv::Mat(height, width, format)
{
    memcpy(cv::Mat::data, data, width * height * 3);
}

Image::Image(const Image &image) : cv::Mat(image)
{}

Image Image::scaled(const QSize &size) const
{
    Image image(size.width(), size.height(), format());
    cv::resize(*this, image, cv::Size(size.width(), size.height()));
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

/**
 * @file UEye.cpp
 * @author Yutaka Kondo <yutaka.kondo@youtalk.jp>
 * @date Apr 14, 2014
 */

#include "rgbd/camera/UEye.h"

namespace rgbd {

UEye::UEye(const uint deviceNo, const std::string& file) :
        _deviceNo(deviceNo), _driver(deviceNo, "uEye"), _size(752, 480) {
    if (_driver.connectCam() != IS_SUCCESS) {
        std::cerr << "UEye: failed to initialize UEye camera" << std::endl;
        std::exit(-1);
    }
    _driver.loadCamConfig(file);
}

UEye::~UEye() {
    _driver.disconnectCam();
}

cv::Size UEye::colorSize() const {
    return _size;
}

void UEye::start() {
    if (_driver.setFreeRunMode() != IS_SUCCESS) {
        std::cerr << "UEye: failed to start capturing UEye camera" << std::endl;
        std::exit(-1);
    }
}

void UEye::captureColor(cv::Mat& buffer) {
    boost::mutex::scoped_lock lock(_mutex);

    // Wait for up to 1000ms to capture next frame.
    const char* data = _driver.processNextFrame(1000);
    std::memcpy(buffer.data, data,
                3 * sizeof (uchar) * _size.width * _size.height);
}

bool UEye::adjustColorGain(int red, int green, int blue) {
    bool autoGain = false;
    int masterGain = 0;
    bool gainBoost = false;
    return _driver.setGain(autoGain, masterGain, red, green, blue, gainBoost)
            == IS_SUCCESS;
}

}

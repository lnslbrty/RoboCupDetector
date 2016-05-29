/**
 * Date: 25.05.2016
 * Author: Toni Uhlig <matzeton@googlemail.com>
 */

#ifndef RC_BLOBDETECTORFACTORY_H
#define RC_BLOBDETECTORFACTORY_H 1

#include <thread>


namespace {
class BlobDetectorFactory {

  public:
    BlobDetectorFactory(int numThreads) {
    }
    ~BlobDetectorFactory() {
    }

  private:
    static std::Thread *thrds;

};
}

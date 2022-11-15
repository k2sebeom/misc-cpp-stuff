#include <iostream>
#include "shape.h"
#include "portaudio.h"

int main() {
    Rectangle r = Rectangle(5, 5);
    std::cout << "Size: " << r.GetSize() <<  std::endl;

    PaError err = Pa_Initialize();
    std::cout << err << std::endl;

    int numDevices;
    numDevices = Pa_GetDeviceCount();
    std::cout << numDevices << std::endl;

    const PaDeviceInfo *deviceInfo;
    for(int i = 0; i < numDevices; i++ )
    {
        deviceInfo = Pa_GetDeviceInfo(i);
        std::cout << deviceInfo->name << std::endl;
    }

    err = Pa_Terminate();
    std::cout << err << std::endl;
    return 0;
}

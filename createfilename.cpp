#include "createfilename.h"

#include <cstdio>
#include <ctime>

void CreateFilename::createFilename()
{
    std::time_t rawtime;
    std::tm* timeinfo;
    char timestamp [80];
    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);
    std::strftime(timestamp,80,"%Y-%m-%d-%H-%M-%S",timeinfo);
    std::puts(timestamp);

    QString tmstmp = timestamp;

    filename_new2 = tmstmp+"-background.jpg";
}

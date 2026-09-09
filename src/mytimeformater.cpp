#include "mytimeformater.h"

myTimeFormater::myTimeFormater(double value) {
    setValue(value);
}

void myTimeFormater::setValue(double value) {
    if (value < 0.0)
        return;

    this->hour = value / 3600.0;
    value -= this->hour * 3600.0;
    this->minute = value / 60.0;
    value -= this->minute * 60.0;
    this->second = value;
}

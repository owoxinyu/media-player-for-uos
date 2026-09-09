#ifndef MYTIMEFORMATER_H
#define MYTIMEFORMATER_H

class myTimeFormater
{
public:
    myTimeFormater();
    myTimeFormater(double value = 0.0);
    void setValue(double value);
    int hour, minute;
    double second;
};

#endif // MYTIMEFORMATER_H

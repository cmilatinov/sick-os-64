#pragma once

#include "common/types.h"

#define MAX_DRIVERS 256

class Driver {

    public:
    virtual const char * DisplayName() = 0;

    virtual void Load() = 0;
    virtual void Reset() = 0;
    virtual void Destroy() = 0;

};

class DriverManager {

    private:
    Driver ** drivers;
    uint32_t numDrivers = 0;

    public:
    DriverManager();
    ~DriverManager();

    void RegisterDriver(Driver * driver);
    void LoadAll();

};
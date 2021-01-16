#include "modules/drivers.h"
#include "modules/lib.h"
#include "modules/memory.h"

DriverManager::DriverManager() {
    drivers = reinterpret_cast<Driver **>(malloc(MAX_DRIVERS * sizeof(Driver*)));
    memset(drivers, 0, MAX_DRIVERS * sizeof(Driver*));
}

DriverManager::~DriverManager() {
    free(drivers);
}

void DriverManager::RegisterDriver(Driver * driver) {
    if (drivers == nullptr)
        return;

    if (driver != nullptr)
        drivers[numDrivers++] = driver;
}

void DriverManager::LoadAll() {
    if (drivers == nullptr)
        return;

    for (uint32_t i = 0; i < numDrivers; i++) {
        if (drivers[i] != nullptr) {
            printf("Loading Driver: ");
            printf(drivers[i]->DisplayName());
            printf();
            drivers[i]->Load();
        }
    }
}
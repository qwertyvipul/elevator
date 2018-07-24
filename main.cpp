#include "elev.h"

int main()
{
    Building building;
    while(true){
        building.master_tick();
        wait(1000);
        building.record_floor_requests();
    }
    return 0;
}

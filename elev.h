//elev.h
//header files for elevators -- contains class declarations
#if !defined(VIP_ELEV_H)
#define VIP_ELEV_H
#include "app.h" //provided by the client
#include "msoftcon.h" //for console graphics
#include<iostream>
#include<iomanip> //for setw()
#include<conio.h> //for screen output
#include<stdlib.h> //for itoa()
#include<process.h> //for exit()



using namespace std;

enum Direction{UP, DOWN, STOP};
const int LOAD_TIME = 3; //loading/unloading time (ticks)
const int SPACING = 7; //visual spacing between the cars
const int BUFF_LENGTH = 80; //length of utility string buffer

class Building; //forward declaration

class Elevator{
private:
    Building* buildingPtr; //pointer to parent building
    const int car_number;
    int current_floor;
    int old_floor;
    Direction current_direction;
    bool destination[NUM_FLOORS];
    int loading_timer; //non-zero if loading
    int unloading_timer; //non-zero if unloading

public:
    Elevator(Building*, int); //constructor
    void car_tick1(); //time tick-1 for each car
    void car_tick2(); //time tick-2 for each car
    void car_display();
    void display_destinations();
    void decide();
    void move();
    void get_destinations();
    int get_floor() const;
    Direction get_direction() const;
};

class Building{
private:
    Elevator* car_list[NUM_CARS]; //pointer to cars
    int num_cars;

    bool floor_request[2][NUM_FLOORS]; //false = UP, true = DOWN

public:
    Building(); //constructor
    ~Building(); //destructor
    void master_tick();
    int get_car_floor(const int) const;
    Direction get_car_direction(const int) const;
    bool check_floor_request(const int, const int) const;
    void set_floor_request(const int, const int, const bool);
    void record_floor_requests();
    void show_floor_requests();
};

#endif

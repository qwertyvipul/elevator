//elev.h
//header files for elevators -- contains class declarations
#if !defined(VIP_ELEV_H) //to avoid multiple includes
#define VIP_ELEV_H
#include "app.h" //provided by the client
#include "msoftcon.h" //for console graphics
#include<iostream>
#include<iomanip> //for setw() - sets the field width to be used on the output operations
#include<conio.h> //for screen output
#include<stdlib.h> //for itoa() - converts an integer into a null-terminated string specified by given base
                    //ans stores the result in an array given by the str parameter
#include<process.h> //for exit() - returns 1 to the operating system i.e terminates the program



using namespace std;

enum Direction{UP, DOWN, STOP};
const int LOAD_TIME = 3; //loading/unloading time (ticks)
const int SPACING = 7; //visual spacing between the cars
const int BUFF_LENGTH = 80; //length of utility string buffer

class Building; //forward declaration

class Elevator{
private:
    Building* buildingPtr; //pointer to parent building - each elevator is in a building
    const int car_number; //each elevator has a number
    int current_floor; //it is at some floor
    int old_floor; //the last floor from which it moved on
    Direction current_direction; //the direction in which the lift is heading
    bool destination[NUM_FLOORS]; //array of all the destinations the lift has to stop
    int loading_timer; //non-zero if loading - gate opening
    int unloading_timer; //non-zero if unloading - gate closing

public:
    Elevator(Building*, int); //constructor
    void car_tick1(); //time tick-1 for each car
    void car_tick2(); //time tick-2 for each car
    void car_display(); //this will display all the cars
    void display_destinations(); //display all the destinations the lift has to stop
    void decide();
    void move();
    void get_destinations();
    int get_floor() const;
    Direction get_direction() const; //returns the current direction - 
    /*
    Using const keyword after the function declaration ensures that one cannot change the instance members. At least,
    not until the member is declared mutable.
    */
};

class Building{
private:
    Elevator* car_list[NUM_CARS]; //pointer to cars - each building has number of elevators.
    int num_cars; //total number of elevators

    bool floor_request[2][NUM_FLOORS]; //false = UP, true = DOWN - either a person on that floor wants to go up or down

public:
    Building(); //constructor
    ~Building(); //destructor
    void master_tick();
    int get_car_floor(const int) const;
    Direction get_car_direction(const int) const; //get the direction of the given car
    bool check_floor_request(const int, const int) const;
    void set_floor_request(const int, const int, const bool);
    void record_floor_requests();
    void show_floor_requests();
};

#endif

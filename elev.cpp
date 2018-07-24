//elev.cpp
//contains class data and member function definitions

#include"elev.h"

//////////////////////////////////////////////////////////
Building::Building(){
    char ustring[BUFF_LENGTH]; //string for floor numbers
    init_graphics(); //initialize graphics
    clear_screen(); //clear screen
    num_cars = 0;

    //make elevators
    for(int k=0; k<NUM_CARS; k++){
        car_list[k] = new Elevator(this, num_cars);
        num_cars++;
    }

    //make floors
    for(int j=0; j<NUM_FLOORS; j++){
        set_cursor_pos(3, NUM_FLOORS-j);
        itoa(j+1, ustring, 10);
        cout<<setw(3)<<ustring;
        floor_request[UP][j] = false;
        floor_request[DOWN][j] = false;
    }
}

Building::~Building(){
    for(int k=0; k<NUM_CARS; k++){
        delete car_list[k];
    }
}

//master time tick
void Building::master_tick(){
    int j;
    show_floor_requests();
    for(j=0; j<NUM_CARS; j++){
        car_list[j]->car_tick1();
        car_list[j]->car_tick2();
    }
}

//display floor requests
void Building::show_floor_requests(){
    for(int j=0; j<NUM_FLOORS; j++){
        set_cursor_pos(SPACING, NUM_FLOORS-j);
        if(floor_request[UP][j]==true){
            cout<<'\x1E';
        }else{
            cout<<' ';
        }
        set_cursor_pos(SPACING+3, NUM_FLOORS-j);

        if(floor_request[DOWN][j]==true){
            cout<<'\x1F';
        }else{
            cout<<' ';
        }
    }
}

//get requests from users outside car
void Building::record_floor_requests(){
    char ch = 'x'; //utility char for input
    char ustring[BUFF_LENGTH]; //utility string for input
    int iFloor; //floor for which request made
    char chDirection; //'u' or 'd' for up or down

    set_cursor_pos(1, 22); //bottom of screen
    cout<<"Press [Enter] to call an elevator: ";
    if(!kbhit()){ //wait for keypress (must be CR)
        return;
    }

    cin.ignore(10, '\n');
    if(ch == '\x1B'){ //if escape key, end program
        exit(0);
    }

    set_cursor_pos(1, 22); clear_line(); //clear old text
    set_cursor_pos(1, 22);
    cout<<"Enter the floor you're on: ";
    cin.get(ustring, BUFF_LENGTH); //get floor
    cin.ignore(10, '\n'); //eat chars, including newline
    iFloor = atoi(ustring);

    cout<<"Enter direction you want to go (u or d): ";
    cin.get(chDirection);
    cin.ignore(10, '\n');

    if(chDirection=='u' || chDirection=='U'){
        floor_request[UP][iFloor-1] = true;
    }
    if(chDirection=='d' || chDirection=='D'){
        floor_request[DOWN][iFloor-1] = true;
    }
    set_cursor_pos(1, 22); clear_line();
    set_cursor_pos(1, 23); clear_line();
    set_cursor_pos(1, 24); clear_line();
}

bool Building::check_floor_request(const int dir, const int floor) const{
    return floor_request[dir][floor];
}

void Building::set_floor_request(const int dir, const int floor, const bool value){
    floor_request[dir][floor] = value;
}

int Building::get_car_floor(const int carnum) const{
    return car_list[carnum]->get_floor();
}

Direction Building::get_car_direction(const int carnum) const{
    return car_list[carnum]->get_direction();
}

/////////////////////////////////////////////////////////////////
Elevator::Elevator(Building* ptrB, int nc):
buildingPtr(ptrB), car_number(nc){
    current_floor = 0;
    old_floor = 0;
    current_direction = STOP;
    for(int j=0; j<NUM_FLOORS; j++){
        destination[j] = false;
    }
    loading_timer = 0;
    unloading_timer = 0;
}

int Elevator::get_floor() const{
    return current_floor;
}

Direction Elevator::get_direction() const{
    return current_direction;
}

void Elevator::car_tick1(){
    car_display();
    display_destinations();
    if(loading_timer){
        --loading_timer;
    }

    if(unloading_timer){
        --unloading_timer;
    }

    decide();
}

void Elevator::car_tick2(){
    move();
}

void Elevator::car_display(){
    set_cursor_pos(SPACING + (car_number + 1) * SPACING, NUM_FLOORS - old_floor);
    cout<<"    ";
    set_cursor_pos(SPACING - 1 + (car_number + 1) * SPACING, NUM_FLOORS - current_floor);

    switch(loading_timer){
    case 3:
        cout<<"\x01\xDB\xDB";
        break;
    case 2:
        cout<<"\xDB\x01\xDB";
        get_destinations();
        break;
    case 1:
        cout<<"\xDB\xDB\xDB";
        break;
    case 0:
        cout<<"\xDB\xDB\xDB";
        break;
    }
    set_cursor_pos(SPACING + (car_number + 1) * SPACING, NUM_FLOORS - current_floor);

    switch(unloading_timer){
    case 3:
        cout<<"\xDB\x01\xDB";
        break;
    case 2:
        cout<<"\xDB\xDB\x01";
        break;
    case 1:
        cout<<"\xDB\xDB\xDB";
        break;
    case 0:
        cout<<"\xDB\xDB\xDB";
        break;
    }

    old_floor = current_floor;
}

void Elevator::display_destinations(){
    for(int j=0; j<NUM_FLOORS; j++){
        set_cursor_pos(SPACING - 2 + (car_number + 1) * SPACING, NUM_FLOORS - j);
        if(destination[j] == true){
            cout<<'\xFE';
        }else{
            cout<<' ';
        }
    }
}

void Elevator::decide(){
    int j;
    bool destins_above, destins_below;
    bool requests_above, request_below;
    int nearest_higher_req = 0;
    int nearest_lower_req = 0;
    bool car_between_up, car_between_down;
    bool car_opposite_up, car_opposite_down;
    int ofloor;
    Direction odir;

    if(current_floor == NUM_FLOORS - 1 && current_direction == UP){
        current_direction = STOP;
    }

    if(destination[current_floor] == true){
        destination[current_floor] = false;
        if(!unloading_timer){
            unloading_timer = LOAD_TIME;
        }
        return;
    }

    if(buildingPtr->check_floor_request(UP, current_floor) && current_direction != DOWN){
        current_direction = UP;
        buildingPtr->set_floor_request(current_direction, current_floor, false);
        if(!loading_timer){
            loading_timer = LOAD_TIME;
        }
        return;
    }

    if(buildingPtr->check_floor_request(DOWN, current_floor) && current_direction != UP){
        current_direction = DOWN;
        buildingPtr->set_floor_request(current_direction, current_floor, false);
        if(!loading_timer){
            loading_timer = LOAD_TIME;
        }
        return;
    }

    destins_above = destins_below = false;
    requests_above = request_below = false;
    for(j=current_floor+1; j<NUM_FLOORS; j++){
        if(destination[j]){
            destins_above = true;
        }
        if(buildingPtr->check_floor_request(UP, j) || buildingPtr->check_floor_request(DOWN, j)){
            requests_above = true;
            if(!nearest_higher_req){
                nearest_higher_req = j;
            }
        }
    }

    for(j=current_floor-1; j>=0; j--){
        if(destination[j]){
            destins_below = true;
        }
        if(buildingPtr->check_floor_request(UP, j) || buildingPtr->check_floor_request(DOWN, j)){
            request_below = true;
            if(!nearest_lower_req){
                nearest_lower_req = j;
            }
        }
    }

    if(!destins_above && !requests_above && !destins_below && !request_below){
        current_direction = STOP;
        return;
    }

    if(destins_above && (current_direction == STOP || current_direction == UP)){
        current_direction = UP;
        return;
    }

    if(destins_below && (current_direction == STOP || current_direction == DOWN)){
        current_direction = DOWN;
        return;
    }

    car_between_up = car_between_down = false;
    car_opposite_up = car_opposite_down = false;

    for(j=0; j<NUM_CARS; j++){
        if(j!=car_number){
            ofloor = buildingPtr->get_car_floor(j);
            odir = buildingPtr->get_car_direction(j);

            if((odir == UP || odir == STOP) && requests_above){
                if((ofloor>current_floor && ofloor<=nearest_higher_req) ||
                   (ofloor==current_floor && j<car_number)){
                        car_between_up = true;
                   }
            }

            if((odir == DOWN || odir == STOP) && request_below){
                if((ofloor<current_floor && ofloor >= nearest_lower_req) ||
                (ofloor == current_floor && j<car_number)){
                    car_between_down = true;
                }
            }

            if((odir == UP || odir == STOP) && request_below){
                if(nearest_lower_req >= ofloor && nearest_lower_req - ofloor < current_floor - nearest_lower_req){
                    car_opposite_up = true;
                }
            }

            if((odir == DOWN || odir == STOP) && requests_above){
                if(ofloor >= nearest_higher_req && ofloor - nearest_higher_req < nearest_higher_req - current_floor){
                    car_between_down = true;
                }
            }
        }
    }

    if((current_direction == UP || current_direction == STOP) &&
       requests_above && !car_between_up && !car_opposite_down){
            current_direction = UP;
            return;
       }

    if((current_direction == DOWN || current_direction == STOP) &&
       request_below && !car_between_down && !car_opposite_up){
            current_direction = DOWN;
            return;
       }
    current_direction = STOP;
}

void Elevator::move(){
    if(loading_timer || unloading_timer){
        return;
    }

    if(current_direction == UP){
        current_floor++;
    }else if(current_direction == DOWN){
        current_floor--;
    }
}

void Elevator::get_destinations(){
    char ustring[BUFF_LENGTH];
    int dest_floor;

    set_cursor_pos(1, 22); clear_line();
    set_cursor_pos(1, 22);

    cout<<"Car "<<car_number+1<<" has stopped at floor "<<(current_floor + 1)<<endl;
    cout<<"Enter destination floors (0 when finished)";

    for(int j=1; j<NUM_FLOORS; j++){
        set_cursor_pos(1, 24);
        cout<<"Destination "<<j<<" (or press 0 to confirm): ";
        cin.get(ustring, BUFF_LENGTH);
        cin.ignore(10, '\n');
        dest_floor = atoi(ustring);
        set_cursor_pos(1, 24); clear_line();

        if(dest_floor == 0){
            set_cursor_pos(1, 22); clear_line();
            set_cursor_pos(1, 23); clear_line();
            set_cursor_pos(1, 24); clear_line();
            return;
        }
        --dest_floor;
        if(dest_floor == current_floor){
            --j; continue;
        }

        if(j==1 && current_direction == STOP){
            current_direction = (dest_floor < current_floor)?DOWN:UP;
        }
        destination[dest_floor] = true;
        display_destinations();
    }
}

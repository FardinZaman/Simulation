#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<time.h>
#include<math.h>
#include <bits/stdc++.h>
#include<fstream>

using namespace std;

int total_batch = 900;
double mean_interarrival_time = 0.0;
int max_batch_size = 0;
int number_of_floors = 0;
int number_of_elevators = 0;
double simulation_termination = 0;
int elevator_capacity = 0;

double door_holding_time = 0.0;
double time_of_elevator_for_each_floor = 0.0;
double time_to_open_door = 0.0;
double time_to_close_door = 0.0;
double time_of_passenger_embark = 0.0;
double time_of_passenger_disembark = 0.0;

vector<double> A;

int queues_first_customer;
int queue_length;
double start_time_of_current_queue;
int limit;
int max_floor;
int remaining_customer;
int queue_total;

double TIME;
double AVG_DELIVARY_TIME;
double AVG_ELIVATOR_TIME;
double MAX_DELIVARY_TIME;
double MAX_ELIVATOR_TIME;
int QUEUE_LENGTH_LONGEST;
double AVG_QUEUE_TIME;
double MAX_QUEUE_TIME;

int current_customer;
int current_elevator;

int destination = 0;

ofstream output;
ofstream output2;

class Customer
{
public:

    double between;
    double arrive_time;
    int destination_floor;
    double elevator_time;
    double wait_time;
    double delivary_time;

    Customer()
    {
        wait_time = 0.0;
    }
};

class Elevator
{
public:

    vector<int> selected_floors;
    vector<int> floors_they_pressed;
    int occupants_currently;
    double return_time;
    int first_customer;
    int stop_count;
    double elevator_delivary_time;
    double operate_time;


    Elevator()
    {
        this->selected_floors.clear();
        this->floors_they_pressed.clear();

        for(int i=0 ; i<number_of_floors ; i++)
        {
            selected_floors.push_back(0);
            floors_they_pressed.push_back(0);
        }
    }

    void initialize_vectors()
    {
        this->selected_floors.clear();
        this->floors_they_pressed.clear();

        for(int i=0 ; i<number_of_floors ; i++)
        {
            this->selected_floors.push_back(0);
            this->floors_they_pressed.push_back(0);
        }
    }

};


vector<Customer> customers;
vector<Elevator> elevators;



void generate_batch_arrival()
{
    A.clear();

    for(int i=0 ; i<total_batch ; i++)
    {
        double a_random = (float) rand()/RAND_MAX;
        double iat_of_this_batch = -mean_interarrival_time * log(a_random);

        int size_of_this_batch = (rand() % max_batch_size) + 1;
        //cout<<size_of_this_batch<<endl;

        A.push_back(iat_of_this_batch);

        for(int j=0 ; j<size_of_this_batch-1 ; j++)
        {
            A.push_back(0.0);
        }
    }

    cout<<A.size()<<endl;
}

void calculate_customer_data(int k)
{
    int N = customers.at(k).destination_floor - 1;
    double time_to_floor = N * time_of_elevator_for_each_floor;
    int sum_flrvec_j = 0;
    int sum_selvec_j = 0;
    for(int m=1 ; m<N ; m++)
    {
        sum_flrvec_j += elevators.at(current_elevator).floors_they_pressed.at(m);
        sum_selvec_j += elevators.at(current_elevator).selected_floors.at(m);
    }
    double time_to_drop_others = sum_flrvec_j * time_of_passenger_disembark;
    double time_door_before = sum_selvec_j * (2*time_to_close_door);
    customers.at(k).elevator_time = time_to_floor + time_to_drop_others + time_of_passenger_disembark + time_door_before + time_to_open_door;

    customers.at(k).delivary_time += customers.at(k).elevator_time;

    AVG_DELIVARY_TIME += customers.at(k).delivary_time;

    if(customers.at(k).delivary_time > MAX_DELIVARY_TIME)
    {
        MAX_DELIVARY_TIME = customers.at(k).delivary_time;
    }

    if(customers.at(k).elevator_time > MAX_ELIVATOR_TIME)
    {
        //cout<<customers.at(k).elevator_time<<endl;
        MAX_ELIVATOR_TIME = customers.at(k).elevator_time;
    }
}

//int count_stop = 0;
void calculate_elevator_data()
{
    //count_stop++;
    int sum_selvec_j_all = 0;
    int sum_flrvec_j_all = 0;
    int Max;
    for(int m=1 ; m<number_of_floors ; m++)
    {
        if(elevators.at(current_elevator).selected_floors.at(m) == 1)
        {
            Max = m+1;
        }
        sum_selvec_j_all += elevators.at(current_elevator).selected_floors.at(m);
        sum_flrvec_j_all += elevators.at(current_elevator).floors_they_pressed.at(m);
    }
    elevators.at(current_elevator).stop_count += sum_selvec_j_all;
    //cout<<current_elevator<<"->"<<sum_selvec_j_all<<endl;

    double travel_time = (2*time_of_elevator_for_each_floor) * (Max-1);
    double passenger_drop_time = sum_flrvec_j_all * time_of_passenger_disembark;
    double door_time = sum_selvec_j_all * (2*time_to_close_door);
    elevators.at(current_elevator).elevator_delivary_time = travel_time + passenger_drop_time + door_time;
    elevators.at(current_elevator).return_time = TIME + elevators.at(current_elevator).elevator_delivary_time;
    elevators.at(current_elevator).operate_time += elevators.at(current_elevator).elevator_delivary_time;
}

void simulate()
{
    AVG_DELIVARY_TIME = 0.0;
    AVG_ELIVATOR_TIME = 0.0;
    MAX_DELIVARY_TIME = 0.0;
    MAX_ELIVATOR_TIME = 0.0;
    QUEUE_LENGTH_LONGEST = 0.0;
    AVG_QUEUE_TIME = 0.0;
    MAX_QUEUE_TIME = 0.0;
    queue_total = 0;
    remaining_customer = 0;

    customers.clear();
    elevators.clear();

    current_customer = 0;
    Customer customer;
    customer.between = A.at(current_customer);
    //customer.delivary_time = 15;
    customer.delivary_time = door_holding_time;
    customer.destination_floor = (rand() % (number_of_floors-1)) + 2;
    customers.push_back(customer);

    TIME = customers.at(current_customer).between;
    for(int k=0 ; k<number_of_elevators ; k++)
    {
        Elevator elevator;
        elevator.return_time = TIME;
        elevator.stop_count = 0;
        elevator.operate_time = 0;

        elevators.push_back(elevator);
    }

    while(TIME <= simulation_termination)
    {
        bool broken = false;

        for(int k=0 ; k<number_of_elevators ; k++)
        {
            if(TIME >= elevators.at(k).return_time)
            {
                current_elevator = k;
                broken = true;
                break;
            }
        }

        if(broken)
        {
            elevators.at(current_elevator).first_customer = current_customer;
            elevators.at(current_elevator).occupants_currently = 0;
            elevators.at(current_elevator).initialize_vectors();

            while(1)
            {
                destination = customers.at(current_customer).destination_floor;
                elevators.at(current_elevator).selected_floors.at(destination - 1) = 1;
                elevators.at(current_elevator).floors_they_pressed.at(destination - 1) += 1;
                elevators.at(current_elevator).occupants_currently += 1;

            breaker:

                current_customer++;
                Customer customer;
                //cout<<"before"<<endl;
                customer.between = A.at(current_customer);
                //cout<<"after"<<endl;
                //customer.delivary_time = 15;
                customer.delivary_time = door_holding_time;
                customer.destination_floor = (rand() % (number_of_floors-1)) + 2;
                customers.push_back(customer);
                TIME = TIME + customers.at(current_customer).between;

                for(int k=0 ; k<number_of_elevators ; k++)
                {
                    if(TIME >= elevators.at(k).return_time)
                    {
                        elevators.at(k).return_time = TIME;
                    }
                }

                double between_i = customers.at(current_customer).between;
                int occup_j = elevators.at(current_elevator).occupants_currently;
                if(between_i <= door_holding_time && occup_j < elevator_capacity)
                {
                    int first_j = elevators.at(current_elevator).first_customer;
                    for(int k=first_j ; k<=current_customer-1 ; k++)
                    {
                        customers.at(k).delivary_time += between_i;
                    }
                }
                else
                {
                    limit = current_customer - 1;
                    break;
                }
            }

            int first_j = elevators.at(current_elevator).first_customer;
            for(int k=first_j ; k<=limit ; k++)
            {
                calculate_customer_data(k);
                /*int N = customers.at(k).destination_floor - 1;
                double time_to_floor = N * time_of_elevator_for_each_floor;
                int sum_flrvec_j = 0;
                int sum_selvec_j = 0;
                for(int m=1 ; m<N ; m++)
                {
                    sum_flrvec_j += elevators.at(current_elevator).floors_they_pressed.at(m);
                    sum_selvec_j += elevators.at(current_elevator).selected_floors.at(m);
                }
                double time_to_drop_others = sum_flrvec_j * time_of_passenger_disembark;
                double time_door_before = sum_selvec_j * (2*time_to_close_door);
                customers.at(k).elevator_time = time_to_floor + time_to_drop_others + time_of_passenger_disembark + time_door_before + time_to_open_door;

                customers.at(k).delivary_time += customers.at(k).elevator_time;

                AVG_DELIVARY_TIME += customers.at(k).delivary_time;

                if(customers.at(k).delivary_time > MAX_DELIVARY_TIME)
                {
                    MAX_DELIVARY_TIME = customers.at(k).delivary_time;
                }

                if(customers.at(k).elevator_time > MAX_ELIVATOR_TIME)
                {
                    MAX_ELIVATOR_TIME = customers.at(k).elevator_time;
                }*/
            }

            calculate_elevator_data();

            /*int sum_selvec_j_all = 0;
            int sum_flrvec_j_all = 0;
            int Max;
            for(int m=1 ; m<number_of_floors ; m++)
            {
                if(elevators.at(current_elevator).selected_floors.at(m) == 1)
                {
                    Max = m+1;
                }
                sum_selvec_j_all += elevators.at(current_elevator).selected_floors.at(m);
                sum_flrvec_j_all += elevators.at(current_elevator).floors_they_pressed.at(m);
            }
            elevators.at(current_elevator).stop_count += sum_selvec_j_all;

            double travel_time = (2*time_of_elevator_for_each_floor) * (Max-1);
            double passenger_drop_time = sum_flrvec_j_all * time_of_passenger_disembark;
            double door_time = sum_selvec_j_all * (2*time_to_close_door);
            elevators.at(current_elevator).elevator_delivary_time = travel_time + passenger_drop_time + door_time;
            elevators.at(current_elevator).return_time = TIME + elevators.at(current_elevator).elevator_delivary_time;
            elevators.at(current_elevator).operate_time += elevators.at(current_elevator).elevator_delivary_time;*/
        }

        else
        {
            queues_first_customer = current_customer;
            start_time_of_current_queue = TIME;
            queue_length = 1;
            customers.at(current_customer).arrive_time = TIME;

            while(1)
            {
                current_customer++;
                Customer customer;
                //cout<<"before"<<endl;
                customer.between = A.at(current_customer);
                //cout<<"after"<<endl;
                //customer.delivary_time = 15;
                customer.destination_floor = (rand() % (number_of_floors-1)) + 2;
                TIME = TIME + customer.between;
                customer.arrive_time = TIME;
                customers.push_back(customer);

                /*if(TIME >= simulation_termination)
                {
                    break;
                }*/

                queue_length++;

                bool broken2 = false;

                for(int k=0 ; k<number_of_elevators ; k++)
                {
                    if(TIME >= elevators.at(k).return_time)
                    {
                        current_elevator = k;
                        broken2 = true;
                        break;
                    }
                }
                if(!broken2)
                {
                    continue;
                }

                elevators.at(current_elevator).initialize_vectors();
                remaining_customer = queue_length - elevator_capacity;

                int R;
                if(remaining_customer <= 0)
                {
                    R = current_customer;
                    elevators.at(current_elevator).occupants_currently = queue_length;

                }
                else
                {
                    R = queues_first_customer + (elevator_capacity - 1);
                    elevators.at(current_elevator).occupants_currently = elevator_capacity;
                }

                for(int k=queues_first_customer ; k<=R ; k++)
                {
                    destination = customers.at(k).destination_floor;
                    elevators.at(current_elevator).selected_floors.at(destination - 1) = 1;
                    elevators.at(current_elevator).floors_they_pressed.at(destination - 1) += 1;
                }

                if(queue_length >= QUEUE_LENGTH_LONGEST)
                {
                    QUEUE_LENGTH_LONGEST = queue_length;
                }

                queue_total += elevators.at(current_elevator).occupants_currently;
                double sum_queue_delay = 0;
                for(int m=queues_first_customer ; m<=R ; m++)
                {
                    double arrive_time = customers.at(m).arrive_time;
                    sum_queue_delay += (TIME-arrive_time);
                }
                AVG_QUEUE_TIME += sum_queue_delay;

                if((TIME - start_time_of_current_queue) >= MAX_QUEUE_TIME)
                {
                    MAX_QUEUE_TIME = TIME - start_time_of_current_queue;
                }

                elevators.at(current_elevator).first_customer = queues_first_customer;

                int first_j = elevators.at(current_elevator).first_customer;
                for(int k=first_j ; k<= R ; k++)
                {
                    customers.at(k).delivary_time = door_holding_time + (TIME-customers.at(k).arrive_time);
                    customers.at(k).wait_time = TIME-customers.at(k).arrive_time;
                }

                if(remaining_customer <= 0)
                {
                    queue_length = 0;
                    //cout<<"bingo"<<endl;
                    goto breaker;
                }

                //cout<<"bingo"<<endl;

                limit = R;
                for(int k=first_j ; k<=limit ; k++)
                {
                    calculate_customer_data(k);
                }
                calculate_elevator_data();

                queue_length = remaining_customer;
                queues_first_customer = R + 1;
                start_time_of_current_queue = customers.at(R+1).arrive_time;

                if(TIME >= simulation_termination)
                {
                    break;
                }
            }
        }
    }
}

void calculate_data()
{
    int N = (current_customer+1) - queue_length;
    cout<<"Customers served : "<<N<<endl;
    output<<N<<",";

    AVG_DELIVARY_TIME = AVG_DELIVARY_TIME/N;
    cout<<"Avg delivary time : "<<AVG_DELIVARY_TIME<<endl;
    output<<round(AVG_DELIVARY_TIME)<<",";

    cout<<"Maximum delivary time : "<<MAX_DELIVARY_TIME<<endl;
    output<<round(MAX_DELIVARY_TIME)<<",";

    //cout<<queue_length<<endl;

    AVG_ELIVATOR_TIME = 0;
    for(int m=0 ; m<=limit ; m++)
    {
        AVG_ELIVATOR_TIME += customers.at(m).elevator_time;
    }
    AVG_ELIVATOR_TIME = AVG_ELIVATOR_TIME/(double)limit;
    cout<<"Average time spent on elevator : "<<AVG_ELIVATOR_TIME<<endl;
    output<<round(AVG_ELIVATOR_TIME)<<",";

    cout<<"Maximum time spent on elevator : "<<MAX_ELIVATOR_TIME<<endl;
    output<<round(MAX_ELIVATOR_TIME)<<",";

    cout<<"Longest queue : "<<QUEUE_LENGTH_LONGEST<<endl;
    output<<QUEUE_LENGTH_LONGEST<<",";

    AVG_QUEUE_TIME = AVG_QUEUE_TIME/queue_total;
    cout<<"Average time in queue : "<<AVG_QUEUE_TIME<<endl;
    output<<round(AVG_QUEUE_TIME)<<",";

    cout<<"Maximum time in queue : "<<MAX_QUEUE_TIME<<endl;
    output<<round(MAX_QUEUE_TIME)<<",";

    for(int k=0 ; k<number_of_elevators ; k++)
    {
        cout<<"Stop count of elevator "<<k<<" : "<<elevators.at(k).stop_count<<endl;
        cout<<"Transport percentage of elevator "<<k<<" : "<<elevators.at(k).operate_time/simulation_termination<<endl;
        //cout<<"Transport percentage of elevator "<<k<<" : "<<elevators.at(k).operate_time<<endl;
    }
    for(int k=0 ; k<number_of_elevators ; k++)
    {
        output2<<elevators.at(k).stop_count<<",";
    }
    for(int k=0 ; k<number_of_elevators ; k++)
    {
        output2<<round((elevators.at(k).operate_time/simulation_termination) * 100)<<",";
    }

    cout<<"TIME : "<<TIME<<endl;

    //cout<<count_stop<<endl;
    /*for(int i=0 ; i<customers.size() ; i++)
    {
        cout<<customers.at(i).between<<endl;
    }*/
}

int main()
{
    /*mean_interarrival_time = 1.5*12;
    max_batch_size = 6;
    number_of_floors = 12;
    number_of_elevators = 4;
    simulation_termination = 10000;
    elevator_capacity = 12;

    door_holding_time = 15.0;
    time_of_elevator_for_each_floor = 5.0;
    time_to_open_door = 3.0;
    time_to_close_door = 3.0;
    time_of_passenger_embark = 3.0;
    time_of_passenger_disembark = 3.0;*/

    double mit;
    ifstream input("input.txt");
    input>>simulation_termination;
    input>>number_of_floors;
    input>>number_of_elevators;
    input>>elevator_capacity;
    input>>max_batch_size;
    input>>door_holding_time;
    input>>time_of_elevator_for_each_floor;
    input>>time_to_open_door;
    input>>time_to_close_door;
    input>>time_of_passenger_embark;
    input>>time_of_passenger_disembark;
    input>>mit;
    mean_interarrival_time = mit*12;

    output.open("output.csv" , ios::app);
    output2.open("output_elevator.csv" , ios::app);
    output<<"Simulation Number,Customers Served,Average Delivery,Maximum Delivery,Average Time in Elevator,Maximum Time in Elevator,"
          <<"Longest Queue,Average Queue Time,Longest Queue Time"<<endl;

    output2<<"Simulation Number,";
    for(int i=1 ; i<=number_of_elevators ; i++)
    {
        output2<<"Stop Count of - "<<i<<",";
    }
    for(int i=1 ; i<=number_of_elevators ; i++)
    {
        output2<<"Usage Percentage of - "<<i<<",";
    }
    output2<<endl;

    /*for(int i=0 ; i<A.size() ; i++)
    {
        A.at(i) = A.at(i) + 1;
        cout<<A.at(i)<<endl;
    }*/
    //Elevator elevator;
    //cout<<elevator.selected_floors.at(12);
    /*generate_batch_arrival();
    simulate();
    calculate_data();*/

    for(int i=1 ; i<=10 ; i++)
    {
        output<<i<<",";
        output2<<i<<",";

        generate_batch_arrival();
        simulate();
        calculate_data();

        output<<endl;
        output2<<endl;
    }
}

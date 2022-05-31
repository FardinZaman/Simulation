#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<time.h>
#include<math.h>
#include <bits/stdc++.h>
#include<fstream>

#define queue_size 100
#define arrival 1
#define departure 2
#define busy 1
#define idle 0

using namespace std;

int server_status;
int next_event_type;
double simulation_time;
double total_delay;
double area_under_Q_t;
double area_under_B_t;
int customers_delayed;
int customers_in_queue;
double time_of_last_event;

int customers_arrived;
int customers_served;

double mean_interarrival_time;
double mean_service_time;
int number_of_customers;

double arrival_time[queue_size + 1];
double time_of_next_event[3];

vector<double> A;
vector<double> S;
vector<double> randoms;
vector<double> randoms2;

ofstream output;
ofstream output2;

double randoms_min;
double randoms_max;
double randoms_median;
double randoms2_min;
double randoms2_max;
double randoms2_median;
double A_min;
double A_max;
double A_median;
double S_min;
double S_max;
double S_median;

double randoms_P_x[10];
double randoms2_P_x[10];
double A_P_x[4];
double S_P_x[4];

double randoms_F_x[10];
double randoms2_F_x[10];
double A_F_x[4];
double S_F_x[4];

ofstream output3;
ofstream output4;
ofstream output5;

//ifstream input;

void random_generator()
{
    randoms.clear();
    randoms2.clear();

    for(int i=0 ; i<number_of_customers ; i++)
    {
        randoms.push_back((float) rand()/RAND_MAX);
    }
    for(int i=0 ; i<number_of_customers ; i++)
    {
        randoms2.push_back((float) rand()/RAND_MAX);
    }

    //cout<<mean_service_time<<endl;
    A.clear();
    S.clear();

    for(int i=0 ; i<number_of_customers ; i++)
    {
        A.push_back(-mean_interarrival_time * log(randoms.at(i)));
        S.push_back(-mean_service_time * log(randoms2.at(i)));
    }

    /*for(int i=0 ; i<number_of_customers ; i++)
    {
        cout<<randoms.at(i)<<" "<<randoms2.at(i)<<endl;
    }
    cout<<endl;
    for(int i=0 ; i<number_of_customers ; i++)
    {
        cout<<A.at(i)<<" "<<S.at(i)<<endl;
    }
    cout<<endl;*/
}

void determine_next_event()
{
    int i;
    double time_of_immediate_next_event = 1.0e+29;

    next_event_type = 0;

    for(i=1 ; i<=2 ; i++)
    {
        if(time_of_next_event[i] < time_of_immediate_next_event)
        {
            time_of_immediate_next_event = time_of_next_event[i];
            next_event_type = i;
        }
    }

    //cout<<next_event_type<<endl;
    //cout<<time_of_immediate_next_event<<endl;

    if(next_event_type == 0)
    {
        exit(1);
    }

    simulation_time = time_of_immediate_next_event;
}

void update_data()
{
    double time_since_last_event;
    time_since_last_event = simulation_time - time_of_last_event;
    time_of_last_event = simulation_time;

    area_under_Q_t = area_under_Q_t + customers_in_queue*time_since_last_event;
    area_under_B_t = area_under_B_t + server_status*time_since_last_event;
}

void arrive()
{
    //double delay;
    //cout<<customers_arrived<<"  ";
    if(customers_arrived < number_of_customers)
    {
        time_of_next_event[arrival] = simulation_time + A.at(customers_arrived);
        //time_of_next_event[arrival] = simulation_time + 1;
        customers_arrived++;
    }
    else
    {
        time_of_next_event[arrival] = 1.0e+30;
    }
    //cout<<customers_arrived<<endl;

    if(server_status == busy)
    {
        customers_in_queue++;
        if(customers_in_queue > queue_size)
        {
            exit(2);
        }

        arrival_time[customers_in_queue] = simulation_time;
    }
    else
    {
        //cout<<simulation_time<<endl;

        customers_delayed++;
        server_status = busy;

        time_of_next_event[departure] = simulation_time + S.at(customers_served);
        customers_served++;

        //cout<<"    "<<time_of_next_event[departure]<<endl;

        //cout<<customers_served<<endl;

    }
}

void depart()
{
    double delay;

    if(customers_in_queue == 0)
    {
        server_status = idle;
        time_of_next_event[departure] = 1.0e+30;
    }
    else
    {
        //cout<<customers_in_queue<<endl;

        customers_in_queue--;

        delay = simulation_time - arrival_time[1];
        total_delay = total_delay + delay;

        customers_delayed++;
        time_of_next_event[departure] = simulation_time + S.at(customers_served);
        customers_served++;

        //cout<<time_of_next_event[departure]<<endl;
        //cout<<customers_served<<endl;

        for(int i=1 ; i<= customers_in_queue ; i++)
        {
            arrival_time[i] = arrival_time[i+1];
        }
    }
}

void show_data()
{
    cout<<total_delay/customers_delayed<<endl;
    cout<<area_under_Q_t/simulation_time<<endl;
    cout<<area_under_B_t/simulation_time<<endl;
    cout<<simulation_time<<endl;
    cout<<endl;
}

void to_csv_file()
{
    output<<mean_service_time<<","<<total_delay/customers_delayed<<","<<area_under_Q_t/simulation_time<<","<<area_under_B_t/simulation_time<<","<<simulation_time<<endl;
}

void to_text_file()
{
    output2<<"Mean interarrival time    "<<mean_interarrival_time<<"  minutes\n\n";
    output2<<"Mean service time         "<<mean_service_time<<"  minutes\n\n";
    output2<<"Number of customers       "<<(int)number_of_customers<<"\n\n";

    output2<<endl;
    output2<<"Average delay in queue    "<<total_delay/customers_delayed<<"  minutes\n\n";
    output2<<"Average number in queue   "<<area_under_Q_t/simulation_time<<"\n\n";
    output2<<"Server utilization        "<<area_under_B_t/simulation_time<<"\n\n";
    output2<<"Time simulation ended     "<<simulation_time<<"  minutes\n\n";
}

void simulate()
{
    server_status = idle;
    simulation_time = 0.0;
    total_delay = 0.0;
    area_under_B_t = 0.0;
    area_under_Q_t = 0.0;
    customers_delayed = 0;
    customers_in_queue = 0;
    time_of_last_event = 0.0;
    customers_arrived = 0;
    customers_served = 0;

    time_of_next_event[arrival] = simulation_time + A.at(customers_arrived);
    //cout<<customers_arrived<<endl;
    customers_arrived++;
    time_of_next_event[departure] = 1.0e+30;

    int tracker = 0;

    while(customers_delayed < number_of_customers)
    {
        //cout<<customers_in_queue<<endl;
        //tracker++;
        //cout<<tracker<<endl;
        determine_next_event();

        update_data();

        if(next_event_type == 1)
        {
            //cout<<tracker<<endl;
            //tracker++;
            arrive();
            //break;
        }
        else if(next_event_type == 2)
        {
            depart();
            //break;
        }

        //cout<<customers_delayed<<endl;
    }

    show_data();
    //to_text_file();
}

void various_statistics()
{
    sort(randoms.begin() , randoms.end());
    sort(randoms2.begin() , randoms2.end());
    sort(A.begin() , A.end());
    sort(S.begin() , S.end());

    /*for(int i=0 ; i<number_of_customers ; i++)
    {
        cout<<randoms.at(i)<<" "<<randoms2.at(i)<<endl;
    }
    cout<<endl;
    for(int i=0 ; i<number_of_customers ; i++)
    {
        cout<<A.at(i)<<" "<<S.at(i)<<endl;
    }
    cout<<endl;*/

    randoms_min = randoms.front();
    randoms2_min = randoms2.front();
    A_min = A.front();
    S_min = S.front();

    randoms_max = randoms.back();
    randoms2_max = randoms2.back();
    A_max = A.back();
    S_max = S.back();

    if(number_of_customers%2 == 0)
    {
        randoms_median = (randoms.at(number_of_customers/2) + randoms.at(number_of_customers/2 - 1)) / 2;
        randoms2_median = (randoms2.at(number_of_customers/2) + randoms2.at(number_of_customers/2 - 1)) / 2;
        A_median = (A.at(number_of_customers/2) + A.at(number_of_customers/2 - 1)) / 2;
        S_median = (S.at(number_of_customers/2) + S.at(number_of_customers/2 - 1)) / 2;
    }
    else
    {
        randoms_median = randoms.at((number_of_customers + 1)/2 - 1);
        randoms2_median = randoms2.at((number_of_customers + 1)/2 - 1);
        A_median = A.at((number_of_customers + 1)/2 - 1);
        S_median = S.at((number_of_customers + 1)/2 - 1);
    }

    //cout<<randoms_min<<" "<<randoms_max<<" "<<randoms_median<<endl;
    //cout<<randoms2_min<<" "<<randoms2_max<<" "<<randoms2_median<<endl;

    for(int i=0 ; i<10 ; i++)
    {
        randoms_P_x[i] = 0;
        randoms2_P_x[i] = 0;
    }

    for(int i=0 ; i<4 ; i++)
    {
        A_P_x[i] = 0;
        S_P_x[i] = 0;
    }

    for(int i=0 ; i<number_of_customers ; i++)
    {
        double x = randoms.at(i);

        if(x <= 0.1)
        {
            randoms_P_x[0]++;
        }
        else if(x <= 0.2)
        {
            randoms_P_x[1]++;
        }
        else if(x <= 0.3)
        {
            randoms_P_x[2]++;
        }
        else if(x <= 0.4)
        {
            randoms_P_x[3]++;
        }
        else if(x <= 0.5)
        {
            randoms_P_x[4]++;
        }
        else if(x <= 0.6)
        {
            randoms_P_x[5]++;
        }
        else if(x <= 0.7)
        {
            randoms_P_x[6]++;
        }
        else if(x <= 0.8)
        {
            randoms_P_x[7]++;
        }
        else if(x <= 0.9)
        {
            randoms_P_x[8]++;
        }
        else
        {
            randoms_P_x[9]++;
        }
    }

    for(int i=0 ; i<number_of_customers ; i++)
    {
        double x = randoms2.at(i);

        if(x <= 0.1)
        {
            randoms2_P_x[0]++;
        }
        else if(x <= 0.2)
        {
            randoms2_P_x[1]++;
        }
        else if(x <= 0.3)
        {
            randoms2_P_x[2]++;
        }
        else if(x <= 0.4)
        {
            randoms2_P_x[3]++;
        }
        else if(x <= 0.5)
        {
            randoms2_P_x[4]++;
        }
        else if(x <= 0.6)
        {
            randoms2_P_x[5]++;
        }
        else if(x <= 0.7)
        {
            randoms2_P_x[6]++;
        }
        else if(x <= 0.8)
        {
            randoms2_P_x[7]++;
        }
        else if(x <= 0.9)
        {
            randoms2_P_x[8]++;
        }
        else
        {
            randoms2_P_x[9]++;
        }
    }

    /*for(int i=0 ; i<10 ; i++)
    {
        cout<<randoms_P_x[i]<<"  "<<randoms2_P_x[i]<<endl;
    }*/

    for(int i=0 ; i<number_of_customers ; i++)
    {
        double x = A.at(i);
        double beta = mean_interarrival_time;

        if(x <= beta/2)
        {
            A_P_x[0]++;
        }
        else if(x <= beta)
        {
            A_P_x[1]++;
        }
        else if(x <= 2*beta)
        {
            A_P_x[2]++;
        }
        else
        {
            A_P_x[3]++;
        }
    }

    for(int i=0 ; i<number_of_customers ; i++)
    {
        double x = S.at(i);
        double beta = mean_service_time;

        if(x <= beta/2)
        {
            S_P_x[0]++;
        }
        else if(x <= beta)
        {
            S_P_x[1]++;
        }
        else if(x <= 2*beta)
        {
            S_P_x[2]++;
        }
        else
        {
            S_P_x[3]++;
        }
    }

    /*for(int i=0 ; i<4 ; i++)
    {
        cout<<A_P_x[i]<<"  "<<S_P_x[i]<<endl;
    }*/

    randoms_F_x[0] = randoms_P_x[0];
    randoms2_F_x[0] = randoms2_P_x[0];

    for(int i=1 ; i<10 ; i++)
    {
        randoms_F_x[i] = randoms_F_x[i-1] + randoms_P_x[i];
        randoms2_F_x[i] = randoms2_F_x[i-1] + randoms2_P_x[i];
    }

    /*for(int i=0 ; i<10 ; i++)
    {
        cout<<randoms_F_x[i]<<"  "<<randoms2_F_x[i]<<endl;
    }*/

    A_F_x[0] = A_P_x[0];
    S_F_x[0] = S_P_x[0];

    for(int i=1 ; i<4 ; i++)
    {
        A_F_x[i] = A_F_x[i-1] + A_P_x[i];
        S_F_x[i] = S_F_x[i-1] + S_P_x[i];
    }

    /*for(int i=0 ; i<4 ; i++)
    {
        cout<<A_F_x[i]<<"  "<<S_F_x[i]<<endl;
    }*/

    output3<<"min"<<","<<randoms_min<<","<<randoms2_min<<","<<A_min<<","<<S_min<<endl;
    output3<<"max"<<","<<randoms_max<<","<<randoms2_max<<","<<A_max<<","<<S_max<<endl;
    output3<<"median"<<","<<randoms_median<<","<<randoms2_median<<","<<A_median<<","<<S_median<<endl;

    double temp = 0.0;
    for(int i=0 ; i<10 ; i++)
    {
        output4<<temp<<" - "<<temp+0.1<<","<<randoms_P_x[i]/number_of_customers<<","<<randoms_F_x[i]/number_of_customers<<","<<randoms2_P_x[i]/number_of_customers<<","<<randoms2_F_x[i]/number_of_customers<<endl;
        temp = temp + 0.1;
    }

    output5<<"0 - B/2"<<","<<A_P_x[0]/number_of_customers<<","<<A_F_x[0]/number_of_customers<<","<<S_P_x[0]/number_of_customers<<","<<S_F_x[0]/number_of_customers<<endl;
    output5<<"B/2 - B"<<","<<A_P_x[1]/number_of_customers<<","<<A_F_x[1]/number_of_customers<<","<<S_P_x[1]/number_of_customers<<","<<S_F_x[1]/number_of_customers<<endl;
    output5<<"B - 2B"<<","<<A_P_x[2]/number_of_customers<<","<<A_F_x[2]/number_of_customers<<","<<S_P_x[2]/number_of_customers<<","<<S_F_x[2]/number_of_customers<<endl;
    output5<<"2B - "<<","<<A_P_x[3]/number_of_customers<<","<<A_F_x[3]/number_of_customers<<","<<S_P_x[3]/number_of_customers<<","<<S_F_x[3]/number_of_customers<<endl;
}

void simulate2()
{
    double arr[] = {0.5 , 0.6 , 0.7 , 0.8 , 0.9};
    for(int i=0 ; i<5 ; i++)
    {
        mean_service_time = arr[i];
        //cout<<mean_service_time<<endl;
        random_generator();
        simulate();

        to_csv_file();

        cout<<endl;
    }
}

int main()
{
    output.open("output.csv");
    output<<"k,average delay in queue,average number in queue,server utilization,time the simulation ended, "<<endl;
    output<<std::fixed<<setprecision(3);

    output2.open("output.txt");
    output2<<"Single-Server queueing system\n\n";
    output2<<std::fixed<<setprecision(3);

    output3.open("Statistics-1.csv");
    output3<<"Type,Uniform-randoms,Uniform-randoms2,Expo-A,Expo-S, "<<endl;
    //output3<<std::fixed<<setprecision(3);

    output4.open("Statistics-2-Uni.csv");
    output4<<"Range,randoms_P(x),randoms_F(x),randoms2_P(x),randoms2_F(x), "<<endl;
    //output4<<std::fixed<<setprecision(1);

    output5.open("Statistics-2-Expo.csv");
    output5<<"Range,A_P(x),A_F(x),S_P(x),S_F(x), "<<endl;
    //output5<<std::fixed<<setprecision(3);

    ifstream input("input.txt");
    if(!input)
        cout<<"what?"<<endl;
    input>>mean_interarrival_time;
    input>>mean_service_time;
    input>>number_of_customers;
    cout<<mean_interarrival_time<<" "<<mean_service_time<<" "<<number_of_customers<<endl;

    random_generator();
    simulate();
    to_text_file();

    various_statistics();

    simulate2();
}


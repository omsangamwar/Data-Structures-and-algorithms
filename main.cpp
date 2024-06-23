#include <bits/stdc++.h>
#include "parser.h"

#define all(v) v.begin(), v.end()

using namespace std;

/** Global Constants **/
const string TRACE = "trace";
const string SHOW_STATISTICS = "stats";
const string ALGORITHMS[9] = {"", "FCFS", "RR-", "SPN", "SRT", "HRRN", "FB-1", "FB-2i", "AGING"};

bool sortByServiceTime(const tuple<string, int, int> &a, const tuple<string, int, int> &b)
{
    return (get<2>(a) < get<2>(b));
}
bool sortByArrivalTime(const tuple<string, int, int> &a, const tuple<string, int, int> &b)
{
    return (get<1>(a) < get<1>(b));
}

bool descendingly_by_response_ratio(tuple<string, double, int> a, tuple<string, double, int> b)
{
    return get<1>(a) > get<1>(b);
}

bool byPriorityLevel (const tuple<int,int,int>&a,const tuple<int,int,int>&b){
    if(get<0>(a)==get<0>(b))
        return get<2>(a)> get<2>(b);
    return get<0>(a) > get<0>(b);
}

void clear_timeline()
{
    for(int i=0; i<last_instant; i++)
        for(int j=0; j<process_count; j++)
            timeline[i][j] = ' ';
}

string getProcessName(tuple<string, int, int> &a)
{
    return get<0>(a);
}

int getArrivalTime(tuple<string, int, int> &a)
{
    return get<1>(a);
}

int getServiceTime(tuple<string, int, int> &a)
{
    return get<2>(a);
}

int getPriorityLevel(tuple<string, int, int> &a)
{
    return get<2>(a);
}

double calculate_response_ratio(int wait_time, int service_time)
{
    return (wait_time + service_time)*1.0 / service_time;
}

void fillInWaitTime(){
    for (int i = 0; i < process_count; i++)
    {
        int arrivalTime = getArrivalTime(processes[i]);
        for (int k = arrivalTime; k < finishTime[i]; k++)
        {
            if (timeline[k][i] != '*')
                timeline[k][i] = '.';
        }
    }
}

void firstComeFirstServe()
{
    int time = getArrivalTime(processes[0]);
    for (int i = 0; i < process_count; i++)
    {
        int processIndex = i;
        int arrivalTime = getArrivalTime(processes[i]);
        int serviceTime = getServiceTime(processes[i]);

        finishTime[processIndex] = (time + serviceTime);
        turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
        normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);

        for (int j = time; j < finishTime[processIndex]; j++)
            timeline[j][processIndex] = '*';
        for (int j = arrivalTime; j < time; j++)
            timeline[j][processIndex] = '.';
        time += serviceTime;
    }
}

void roundRobin(int originalQuantum)
{
    queue<pair<int,int>>q;
    int j=0;
    if(getArrivalTime(processes[j])==0){
        q.push(make_pair(j,getServiceTime(processes[j])));
        j++;
    }
    int currentQuantum = originalQuantum;
    for(int time =0;time<last_instant;time++){
        if(!q.empty()){
            int processIndex = q.front().first;
            q.front().second = q.front().second-1;
            int remainingServiceTime = q.front().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            currentQuantum--;
            timeline[time][processIndex]='*';
            while(j<process_count && getArrivalTime(processes[j])==time+1){
                q.push(make_pair(j,getServiceTime(processes[j])));
                j++;
            }

            if(currentQuantum==0 && remainingServiceTime==0){
                finishTime[processIndex]=time+1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
                currentQuantum=originalQuantum;
                q.pop();
            }else if(currentQuantum==0 && remainingServiceTime!=0){
                q.pop();
                q.push(make_pair(processIndex,remainingServiceTime));
                currentQuantum=originalQuantum;
            }else if(currentQuantum!=0 && remainingServiceTime==0){
                finishTime[processIndex]=time+1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
                q.pop();
                currentQuantum=originalQuantum;
            }
        }
        while(j<process_count && getArrivalTime(processes[j])==time+1){
            q.push(make_pair(j,getServiceTime(processes[j])));
            j++;
        }
    }
    fillInWaitTime();
}

void shortestProcessNext()
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // pair of service time and index
    int j = 0;
    for (int i = 0; i < last_instant; i++)
    {
        while(j<process_count && getArrivalTime(processes[j]) <= i){
            pq.push(make_pair(getServiceTime(processes[j]), j));
            j++;
        }
        if (!pq.empty())
        {
            int processIndex = pq.top().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            pq.pop();

            int temp = arrivalTime;
            for (; temp < i; temp++)
                timeline[temp][processIndex] = '.';

            temp = i;
            for (; temp < i + serviceTime; temp++)
                timeline[temp][processIndex] = '*';

            finishTime[processIndex] = (i + serviceTime);
            turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
            normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
            i = temp - 1;
        }
    }
}

void shortestRemainingTime()
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    int j = 0;
    for (int i = 0; i < last_instant; i++)
    {
        while(j<process_count &&getArrivalTime(processes[j]) == i){
            pq.push(make_pair(getServiceTime(processes[j]), j));
            j++;
        }
        if (!pq.empty())
        {
            int processIndex = pq.top().second;
            int remainingTime = pq.top().first;
            pq.pop();
            int serviceTime = getServiceTime(processes[processIndex]);
            int arrivalTime = getArrivalTime(processes[processIndex]);
            timeline[i][processIndex] = '*';

            if (remainingTime == 1) // process finished
            {
                finishTime[processIndex] = i + 1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
            }
            else
            {
                pq.push(make_pair(remainingTime - 1, processIndex));
            }
        }
    }
    fillInWaitTime();
}

void highestResponseRatioNext()
{

    // Vector of tuple <process_name, process_response_ratio, time_in_service> for processes that are in the ready queue
    vector<tuple<string, double, int>> present_processes;
    int j=0;
    for (int current_instant = 0; current_instant < last_instant; current_instant++)
    {
        while(j<process_count && getArrivalTime(processes[j])<=current_instant){
            present_processes.push_back(make_tuple(getProcessName(processes[j]), 1.0, 0));
            j++;
        }
        // Calculate response ratio for every process
        for (auto &proc : present_processes)
        {
            string process_name = get<0>(proc);
            int process_index = processToIndex[process_name];
            int wait_time = current_instant - getArrivalTime(processes[process_index]);
            int service_time = getServiceTime(processes[process_index]);
            get<1>(proc) = calculate_response_ratio(wait_time, service_time);
        }

        // Sort present processes by highest to lowest response ratio
        sort(all(present_processes), descendingly_by_response_ratio);

        if (!present_processes.empty())
        {
            auto &first_proc = present_processes[0];
            int process_index = processToIndex[get<0>(first_proc)];

            // Add to timeline and update remaining service time
            timeline[current_instant][process_index] = '*';
            get<2>(first_proc)++;

            // If process is finished, remove from queue and compute statistics
            if (get<2>(first_proc) == getServiceTime(processes[process_index]))
            {
                finishTime[process_index] = current_instant + 1;
                turnAroundTime[process_index] = finishTime[process_index] - getArrivalTime(processes[process_index]);
                normTurn[process_index] = (turnAroundTime[process_index] * 1.0 / getServiceTime(processes[process_index]));
                present_processes.erase(present_processes.begin());
            }
        }
    }
    fillInWaitTime();
}

void feedback_with_queues(vector<int>&algorithms,int feedback_count,int queue_count,vector<int>&quantum,int boostTime){
    vector<queue<tuple<int,int,int>>>queues(queue_count);
    int j=0;
    int currentQuantum = quantum[0];
    for(int time =0;time<last_instant;time++){
        while(j<process_count && getArrivalTime(processes[j])==time){
            queues[0].push(make_tuple(0,j,getServiceTime(processes[j])));
            j++;
        }
        if(time>0 && time%boostTime==0){
            for(int l=queue_count-1;l>=0;l--){
                queue<tuple<int,int,int>>empty;
                swap(queues[l],empty);
            }
            for(int k=0;k<process_count;k++){
                if(finishTime[k]==-1){
                    queues[0].push(make_tuple(0,k,getServiceTime(processes[k])));
                }
            }
        }
        for(int i=0;i<queue_count;i++){
            if(!queues[i].empty()){
                int queue_level = get<0>(queues[i].front());
                int processIndex = get<1>(queues[i].front());
                queues[i].front() = make_tuple(get<0>(queues[i].front()),get<1>(queues[i].front()),get<2>(queues[i].front())-1);
                int remainingServiceTime = get<2>(queues[i].front());
                int arrivalTime = getArrivalTime(processes[processIndex]);
                int serviceTime = getServiceTime(processes[processIndex]);
                currentQuantum--;
                timeline[time][processIndex]='*';
                while(j<process_count && getArrivalTime(processes[j])==time+1){
                    queues[0].push(make_tuple(0,j,getServiceTime(processes[j])));
                    j++;
                }
                if(currentQuantum==0 && remainingServiceTime==0){
                    finishTime[processIndex]=time+1;
                    turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                    normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
                    currentQuantum=quantum[queue_level];
                    queues[i].pop();
                    break;
                }else if(currentQuantum==0 && remainingServiceTime!=0 && queue_level==queue_count-1){
                    queues[i].pop();
                    queues[i].push(make_tuple(queue_level,processIndex,remainingServiceTime));
                    currentQuantum=quantum[queue_level];
                    break;
                }else if(currentQuantum==0 && remainingServiceTime!=0 && queue_level<queue_count-1){
                    queues[i].pop();
                    queues[i+1].push(make_tuple(queue_level+1,processIndex,remainingServiceTime));
                    currentQuantum=quantum[queue_level+1];
                    break;
                }else if(currentQuantum!=0 && remainingServiceTime==0){
                    finishTime[processIndex]=time+1;
                    turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                    normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
                    queues[i].pop();
                    currentQuantum=quantum[queue_level];
                    break;
                }
            }
        }
    }
    fillInWaitTime();
}

void printTrace()
{
    string buffer;
    int t, i;

    /* Print Timeline */
    for (t = 0; t < last_instant; t++)
    {
        buffer += to_string(t % 10);
    }
    cout << " ";
    for (int j = 0; j < process_count - 1; j++)
        cout << " ";
    cout << buffer << endl;

    for (i = 0; i < process_count; i++)
    {
        cout << getProcessName(processes[i]) << " ";
        for (t = 0; t < last_instant; t++)
            cout << timeline[t][i];
        cout << endl;
    }
    cout << endl;
}

void printStats()
{
    int i;
    int total_turnaround = 0;
    double total_normturn = 0.0;

    cout << "Process  ";
    for (i = 0; i < process_count; i++)
        cout << getProcessName(processes[i]) << " ";
    cout << endl;

    cout << "Arrival  ";
    for (i = 0; i < process_count; i++)
        cout << getArrivalTime(processes[i]) << " ";
    cout << endl;

    cout << "Service  ";
    for (i = 0; i < process_count; i++)
    {
        cout << getServiceTime(processes[i]) << " ";
    }
    cout << "Mean" << endl;

    cout << "Finish   ";
    for (i = 0; i < process_count; i++)
    {
        cout << finishTime[i] << " ";
    }
    cout << endl;

    cout << "Turnaround ";
    for (i = 0; i < process_count; i++)
    {
        cout << turnAroundTime[i] << " ";
        total_turnaround += turnAroundTime[i];
    }
    cout << setprecision(2) << fixed << total_turnaround * 1.0 / process_count << endl;

    cout << "NormTurn ";
    for (i = 0; i < process_count; i++)
    {
        cout << setprecision(2) << fixed << normTurn[i] << " ";
        total_normturn += normTurn[i];
    }
    cout << setprecision(2) << fixed << total_normturn / process_count << endl;
}

int main(int argc, char **argv)
{
    readInput();
    clear_timeline();
    string arg1 = argv[1];
    string arg2 = argv[2];
    int algorithm=0,quantum;
    if(arg2.find("-")== string::npos){
        for(int i=0;i<9;i++){
            if(ALGORITHMS[i]==arg2){
                algorithm = i;
                break;
            }
        }
    }else if(arg2.find("RR")!=string::npos || arg2.find("FB-")!=string::npos){
        string q="";
        for(int i=0;i<arg2.length();i++){
            if(arg2[i]=='-'){
                for(int j=i+1;j<arg2.length();j++){
                    q+=arg2[j];
                }
                break;
            }
        }
        quantum = stoi(q);
        if(arg2.find("RR")!=string::npos){
            algorithm = 2;
        }else if(arg2.find("FB-")!=string::npos){
            algorithm = 6;
        }
    }
    if (algorithm == 1)
    {
        firstComeFirstServe();
    }
    else if (algorithm == 2)
    {
        roundRobin(quantum);
    }
    else if (algorithm == 3)
    {
        shortestProcessNext();
    }
    else if (algorithm == 4)
    {
        shortestRemainingTime();
    }
    else if (algorithm == 5)
    {
        highestResponseRatioNext();
    }
    else if (algorithm == 6)
    {
        vector<int>algorithms(1,algorithm);
        int feedback_count=1;
        vector<int>quantums(1,quantum);
        feedback_with_queues(algorithms,feedback_count,1,quantums,last_instant);
    }
    else if (algorithm == 7)
    {
        vector<int>algorithms(1,algorithm);
        int feedback_count=2;
        vector<int>quantums{quantum,2*quantum};
        feedback_with_queues(algorithms,feedback_count,2,quantums,last_instant);
    }
    else if (algorithm == 8)
    {
        vector<int>algorithms(1,algorithm);
        int feedback_count=4;
        vector<int>quantums{1,2,4,8};
        feedback_with_queues(algorithms,feedback_count,4,quantums,last_instant);
    }
    else
    {
        cout << "Please enter a valid argument" << endl;
        exit(0);
    }
    if (arg1 == TRACE)
    {
        printTrace();
    }
    else if (arg1 == SHOW_STATISTICS)
    {
        printStats();
    }
    return 0;
}

/*
    Optimazation:
    1. reflect stars number to charactor a-y while black hole to z
    2. h2 have two stages 
       1) when heristic is small use direct reflecting
       2) when heristic is big build map and just read from map
*/

#include<vector>
#include<iostream>
#include<string.h>
#include<fstream>
#include<queue>
#include<map>
#include<utility>
#include<algorithm>
#include"chrono"

using namespace std;

// enum
enum DIRECTION{
    left,
    right,
    up,
    down
};

// global variables
bool usingmap;
string start_string(25,0);
string target_string(25,0);
    
map<string, int> closed_list;
map<string, pair<string,char>> came_from;
map<string, int> cost_so_far;

map<pair<int,int>,int> manhatten_map;

// heristic function h1
int h1(const string current_string){
    /* 
        Input: current string
        Output: heristic value from current status to target
        Stradegy: Number of stallars misplaced
    */
    int heristic_value = 0;

    for(int i=0;i<25;i++)
    {
        if(current_string[i] != target_string[i])
            heristic_value++;
    }

    return heristic_value-1;
}

bool NotContrast(char DIR,int direction)
{
    switch (DIR)
    {
        case('U'):
        {
            if(direction == down)
                return false;
            break;
        }
        case('D'):
        {
            if(direction == up)
                return false;
            break;
        }
        case('L'):
        {
            if(direction == DIRECTION::right)
                return false;
            break;
        }
        case('R'):
        {
            if(direction == DIRECTION::left)
                return false;
            break;
        }
    }

    return true;
}

// self-def sort struct for h1 function
struct cmp_f1{
    bool operator()(const string &a, const string &b)
    {
        return h1(a) + cost_so_far.find(a)->second > h1(b) + cost_so_far.find(b)->second;
    }
};

// building mapping function
void buildmap(void)
{
    /*
        Input: Nothing
        Ouptut: Manhatten Map, mapping (current_pos , target_pos) into manhatten value
        Stradegy: bfs for each current_pos
    */

    // calculate manhatten distance for each current_pos
    for(int i=0;i<5;i++)
    {
        for(int j=0;j<5;j++)
        {
            // Starting from node (i,j)

            // mapping for bfs
            vector<vector<int> > vec(5, vector<int>(5, 10));
            queue<pair<int,int>> worklist;

            worklist.push({i,j});
            vec[i][j] = 0;

            while (!worklist.empty())
            {
                // get front element
                auto current_node = worklist.front();
                worklist.pop();

                auto next_cost = vec[current_node.first][current_node.second] + 1;

                // special position (using space channel)
                if(current_node.first == 0 && current_node.second == 2)
                {
                    if(next_cost < vec[4][2])
                    {
                        vec[4][2] = next_cost;
                        worklist.push({4,2});
                    }
                }
                
                if(current_node.first == 4 && current_node.second == 2)
                {
                    if(next_cost < vec[0][2])
                    {
                        vec[0][2] = next_cost;
                        worklist.push({0,2});
                    }
                }

                if(current_node.first == 2 && current_node.second == 0)
                {
                    if(next_cost < vec[2][4])
                    {
                        vec[2][4] = next_cost;
                        worklist.push({2,4});
                    }
                }

                if(current_node.first == 2 && current_node.second == 4)
                {
                    if(next_cost < vec[2][0])
                    {
                        vec[2][0] = next_cost;
                        worklist.push({2,0});
                    }
                }
                

                //general position (without space channel)
                // up
                if(current_node.first - 1 >= 0 && next_cost < vec[current_node.first-1][current_node.second])
                {
                    vec[current_node.first-1][current_node.second] = next_cost;
                    worklist.push({current_node.first-1,current_node.second});
                }
                // down
                if(current_node.first + 1 <= 4 && next_cost < vec[current_node.first+1][current_node.second])
                {
                    vec[current_node.first+1][current_node.second] = next_cost;
                    worklist.push({current_node.first+1,current_node.second});
                }
                // left
                if(current_node.second - 1 >= 0 && next_cost < vec[current_node.first][current_node.second-1])
                {
                    vec[current_node.first][current_node.second-1] = next_cost;
                    worklist.push({current_node.first,current_node.second-1});
                }
                // right
                if(current_node.second + 1 <= 4 && next_cost < vec[current_node.first][current_node.second+1])
                {
                    vec[current_node.first][current_node.second+1] = next_cost;
                    worklist.push({current_node.first,current_node.second+1});
                }
            
            }

            //build map
            for(int ii =0; ii < 5;ii++)
            {
                for(int jj=0;jj<5;jj++)
                {
                    manhatten_map[{5*i+j,5*ii+jj}] = vec[ii][jj];
                }
            }
            
        }

    }
    return;
}

// heristic function h2
int h2(const string current_string){
    /*
        Input: Current string
        Output: Heristic value from current status to target
        Stradegy: Modified Manhattan Distance, Using the min distance from current to target with space channel
    */

    int heristic_value = 0;
    int target_pos;

    for(int current_pos=0;current_pos<25;current_pos++)
    {
        // get current_pos && target_pos
        auto current_star = current_string[current_pos];
        for(target_pos=0;target_pos<25;target_pos++)
        {
            if(target_string[target_pos] == current_star)
                break;
        }

        // No map method 
        if(usingmap == false)
        {
            auto heigh = abs(target_pos%5 - current_pos%5);
            auto width = abs(target_pos/5 - current_pos/5);
            auto direct_manhatten = heigh + width;
            auto manhatten = 0;

            // cout << "direct_manhatten: " <<  direct_manhatten << endl;

            if(current_pos == 0 || current_pos == 4 || current_pos == 20 || current_pos == 24)
            {
                if(direct_manhatten == 5)
                    manhatten = 4;
                else if(direct_manhatten == 6)
                    manhatten = 3;
                else if(direct_manhatten == 7)
                    manhatten = 4;
                else if(direct_manhatten == 8)
                    manhatten = 5;
                else 
                    manhatten = direct_manhatten;
            }

            if(current_pos == 1 || current_pos == 3 || current_pos == 9 || current_pos == 19 || current_pos == 23 
                || current_pos == 21 || current_pos == 15 || current_pos == 5)
            {
                if(direct_manhatten == 4)
                    manhatten = 3;
                else if(direct_manhatten == 5)
                    manhatten = 2;
                else if(direct_manhatten == 6)
                    manhatten = 3;
                else if(direct_manhatten == 7)
                    manhatten = 4;
                else 
                    manhatten = direct_manhatten;
            }

            if(current_pos == 2 || current_pos == 10 || current_pos == 22 || current_pos == 24)
            {
                if(direct_manhatten == 3)
                    manhatten = 2;
                else if(direct_manhatten == 4)
                    manhatten = 1;
                else if(direct_manhatten == 5)
                    manhatten = 2;
                else if(direct_manhatten == 6)
                    manhatten = 3;
                else 
                    manhatten = direct_manhatten;
            }

            if(current_pos == 6 || current_pos == 8 || current_pos == 16 || current_pos == 18)
            {
                if(direct_manhatten == 4)
                    manhatten = 3;
                else if(direct_manhatten == 5)
                    manhatten = 4;
                else if(direct_manhatten == 6)
                    manhatten = 5;
                else 
                    manhatten = direct_manhatten;
            }

            if(current_pos == 7 || current_pos == 11 || current_pos == 17 || current_pos == 13)
            {
                if(direct_manhatten == 3)
                    manhatten = 2;
                else if(direct_manhatten == 4)
                    manhatten = 3;
                else if(direct_manhatten == 5)
                    manhatten= 4;
                else 
                    manhatten = direct_manhatten;
            }

            if(current_pos == 12)
                manhatten = direct_manhatten;

            // cout << "real_manhatten: " <<  manhatten << endl;

            heristic_value += manhatten;
        }

        // get manhatten distance from map
        if(usingmap == true)
        {
            heristic_value += manhatten_map.find({current_pos,target_pos})->second;
        }
    }



    if(usingmap == false && heristic_value > 10){
        usingmap = true;
        buildmap();
    }

#ifdef DEBUG
    cout << "Heristic Value: " << heristic_value << endl;
#endif

    return heristic_value;
}

// self-def sort struct for h2 function
struct cmp_f2{
    bool operator()(const string &a, const string &b)
    {
        return h2(a) + cost_so_far.find(a)->second > h2(b) + cost_so_far.find(b)->second;
    }
};

// goal test function
bool goal_test(const string current_string)
{
    /*  Input: current string
        Output: boolean value
        Stradegy: If current status is target status,return true,
        else return false;
    */

    for(int i=0;i<25;i++){
        if(current_string[i] != target_string[i])
            return false;
    }

    return true;
}

// neighbor function
bool neighbor(const string &current_string, string & neighbor_string,int direction)
{
    /*
        Input: current_string and direction
        Output: neigbor_string and status
        Stradegy: Change current_status according to direction, if the next status
            is valid, then take back using neighbor_string, return true. If not,return false
    */

    int current_pos;
    int next_pos;

    /* get current position of 0 */
    for(int i=0;i<25;i++)
    {
        if(current_string[i] == 'a')
            current_pos = i;
    }    

    /* valid neighbor check only considering space channel */
    switch (direction)
    {
        /* calculate next position accroding to current position and direction */
        case DIRECTION::up :
        {
            if(current_pos == 0 || current_pos == 1 || current_pos == 3 || current_pos == 4)
            {
                return false;
            }
            else if(current_pos == 2)
            {
                next_pos = 22;
            }
            else
            {
                next_pos = current_pos - 5;
            }
            break;
        }
        case DIRECTION::down :
        {
            if(current_pos == 20 || current_pos == 21 || current_pos == 23 || current_pos == 24)
            {
                return false;
            }
            else if(current_pos == 22)
            {
                next_pos = 2;
            }
            else
            {
                next_pos = current_pos + 5;
            }
            break;
        }
        case DIRECTION::left :
        {
            if(current_pos == 0 || current_pos == 5 || current_pos == 15 || current_pos == 20)
            {
                return false;
            }
            else if(current_pos == 10)
            {
                next_pos = 14;
            }
            else
            {
                next_pos = current_pos - 1;
            }
            break;
        }
        case DIRECTION::right :
        {
            if(current_pos == 4 || current_pos == 9 || current_pos == 19 || current_pos == 24)
            {
                return false;
            }
            else if(current_pos == 14)
            {
                next_pos = 10;
            }
            else
            {
                next_pos = current_pos + 1;
            }
            break;
        }

        default:
            break;
        }

    /* valid neighbor check considering black hole */
    if(current_string[next_pos] == 'z'){
        return false;
    }
    else
    {
        /* generate neighbor string */
        for(int i=0;i<25;i++)
        {
            neighbor_string[i] = current_string[i];
        }
        swap(neighbor_string[current_pos],neighbor_string[next_pos]);
    }

    return true;
}

// reflect function
void status2str(vector<vector<int>> status, string &str){
    /*
        Input: vector format status
        Output: String format representation
        Stradegy: Reflect stars number to charactor a-y while black hole to z
    */
    for(int i=0;i<5;i++)
    {
        for(int j=0;j<5;j++)
        {
            if(status[i][j] >= 0)
                str[5*i+j] = 'a' + status[i][j];
            else
                str[5*i+j] = 'z';
        }
    }
    return;
}

// search stradegy
void A_h1(const vector<vector<int>>  &start, const vector<vector<int>>  &target){
    
    /* reflect start to target */
    status2str(start , start_string);
    status2str(target, target_string);

#ifdef DEBUG
    cout << "start string:" << start_string << endl;
    cout << "target string:" << target_string << endl;
#endif

    priority_queue<string,vector<string>,cmp_f1> open_list;

    /* Set Start Status */
    open_list.push(start_string);
    came_from[start_string] = {"NONE",'$'};
    cost_so_far[start_string] = 0;

    /* run A* algorithm */
    while(!open_list.empty())
    {
        // get the status with lowest f_cost and add to close_list
        auto current_string = open_list.top();
        closed_list[current_string] = 1;

#ifdef DEBUG
    cout << current_string << endl;
#endif

        // goal test
        if(goal_test(current_string) == true)
        {
            break;
        }
        else
        {
            open_list.pop();
        }
        
        // adding neighbor
        for(int direction=0;direction<4;direction++){
            // get neighbor string according to directions
            string neighbor_string(25,0);
            auto flag = neighbor(current_string,neighbor_string,direction);

            // skip if neighbor is invalid or is already in close_list 
            if(flag == false || closed_list.find(neighbor_string) != closed_list.end())
            {
                continue;
            }

            // recalculate cost_so_far
            auto new_cost = cost_so_far.find(current_string)->second + 1;

            // set direction
            char DIR;
            switch (direction)
            {
                case DIRECTION::up:
                {
                    DIR = 'U';
                    break;
                }
                
                case DIRECTION::down:
                {
                    DIR = 'D';
                    break;
                }

                case DIRECTION::left:
                {
                    DIR = 'L';
                    break;
                }

                case DIRECTION::right:
                {
                    DIR = 'R';
                    break;
                }

            }

            // calculate new cost
            auto  iter = cost_so_far.find(neighbor_string);
            if(iter == cost_so_far.end() || iter->second > new_cost)
            {
                // set new cost
                cost_so_far[neighbor_string] = new_cost;

                // set f_cost to neighbor
                open_list.push(neighbor_string);

                // set parent for neighbor
                came_from[neighbor_string] = {current_string,DIR};
            }
        }

#ifdef ListElement
    while (!open_list.empty())
    {
        cout << open_list.top() <<endl;
        open_list.pop();
    }
    exit(0);
#endif

    }

    return;
}

void A_h2(const vector<vector<int>>  &start, const vector<vector<int>>  &target){
    
    // default no using map
    usingmap = false;

    /* reflect start to target */
    status2str(start , start_string);
    status2str(target, target_string);

#ifdef DEBUG
    cout << "start string:" << start_string << endl;
    cout << "target string:" << target_string << endl;
#endif

    priority_queue<string,vector<string>,cmp_f2> open_list;

    /* Set Start Status */
    open_list.push(start_string);
    came_from[start_string] = {"NONE",'$'};
    cost_so_far[start_string] = 0;

    /* run A* algorithm */
    while(!open_list.empty())
    {
        // get the status with lowest f_cost and add to close_list
        auto current_string = open_list.top();
        closed_list[current_string] = 1;

#ifdef DEBUG
    cout << current_string << endl;
#endif

        // goal test
        if(goal_test(current_string) == true)
        {
            break;
        }
        else
        {
            open_list.pop();
        }
        
        // adding neighbor
        for(int direction=0;direction<4;direction++){
            // get neighbor string according to directions
            string neighbor_string(25,0);
            auto flag = neighbor(current_string,neighbor_string,direction);

            // skip if neighbor is invalid or is already in close_list 
            if(flag == false || closed_list.find(neighbor_string) != closed_list.end())
            {
                continue;
            }

            // recalculate cost_so_far
            auto new_cost = cost_so_far.find(current_string)->second + 1;

            // set direction
            char DIR;
            switch (direction)
            {
                case DIRECTION::up:
                {
                    DIR = 'U';
                    break;
                }
                
                case DIRECTION::down:
                {
                    DIR = 'D';
                    break;
                }

                case DIRECTION::left:
                {
                    DIR = 'L';
                    break;
                }

                case DIRECTION::right:
                {
                    DIR = 'R';
                    break;
                }

            }

            // calculate new cost
            auto  iter = cost_so_far.find(neighbor_string);
            if(iter == cost_so_far.end() || iter->second > new_cost)
            {
                // set new cost
                cost_so_far[neighbor_string] = new_cost;

                // set f_cost to neighbor
                open_list.push(neighbor_string);

                // set parent for neighbor
                came_from[neighbor_string] = {current_string,DIR};
            }
        }

#ifdef ListElement
    while (!open_list.empty())
    {
        cout << open_list.top() <<endl;
        open_list.pop();
    }
    exit(0);
#endif

    }

    return;
}

void IDA_h1(const vector<vector<int>>  &start, const vector<vector<int>>  &target){
    
    int (*hf)(const string);
    hf = h1;

    /* reflect start to target */
    status2str(start , start_string);
    status2str(target, target_string);

    // set initial d_limit and infinite_d_limit
    int d_limit = hf(start_string);
    int infinite_d_limit = INT16_MAX;

    // run IDA*
    bool stop_flag = false;
    while (d_limit < INT16_MAX)
    {
        // list for work
        queue<pair<string,int>> worklist;
        map<string,bool> InPath;

        // push initial node into list
        auto next_d_limit = infinite_d_limit;
        came_from[start_string] = {"NONE",'$'};
        
        worklist.push({start_string,hf(start_string)+cost_so_far[start_string]});

        while (!worklist.empty())
        {
            auto current_node = worklist.front();
            worklist.pop();

            auto next_cost = cost_so_far[current_node.first] + 1;

            if(current_node.second > d_limit)
            {
                next_d_limit = min(next_d_limit,current_node.second);
            }
            else
            {
                // goal test
                if(goal_test(current_node.first) == true)
                {
                    stop_flag = true;
                    break;
                }
                
                // adding neighbor
                for(int direction=0;direction<4;direction++){
                    // get neighbor string according to directions
                    string neighbor_string(25,0);
                    auto flag = neighbor(current_node.first,neighbor_string,direction);

                    // skip if neighbor is invalid , if neoghbor in map 
                    auto DIR = came_from.find(neighbor_string)->second.second;
                    auto iter = cost_so_far.find(neighbor_string);

                    if(flag == false || NotContrast(DIR,direction) == false || ( iter != cost_so_far.end() && iter->second < next_cost ))
                    {
                        continue;
                    }

                    // set direction
                    switch (direction)
                    {
                        case DIRECTION::up:
                        {
                            DIR = 'U';
                            break;
                        }
                        
                        case DIRECTION::down:
                        {
                            DIR = 'D';
                            break;
                        }

                        case DIRECTION::left:
                        {
                            DIR = 'L';
                            break;
                        }

                        case DIRECTION::right:
                        {
                            DIR = 'R';
                            break;
                        }
                    }

                    // insert neighbor node
                    worklist.push({neighbor_string,hf(neighbor_string)+next_cost});
                    came_from[neighbor_string] = {current_node.first,DIR};
                    cost_so_far[neighbor_string] = next_cost;

#ifdef DEBUG
    cout << neighbor_string << endl;
#endif                
                }
            }

        }

        // if get goal
        if(stop_flag == true)
            break;
        
        // if do not get goal
        d_limit = next_d_limit;
        
    }

    return;
}

void IDA_h2(const vector<vector<int>>  &start, const vector<vector<int>>  &target){

    int (*hf)(const string);
    hf = h2;

    /* reflect start to target */
    status2str(start , start_string);
    status2str(target, target_string);

    // set initial d_limit and infinite_d_limit
    int d_limit = hf(start_string);
    int infinite_d_limit = INT16_MAX;

    // run IDA*
    bool stop_flag = false;
    while (d_limit < INT16_MAX)
    {
        // list for work
        queue<pair<string,int>> worklist;
        map<string,bool> InPath;

        // push initial node into list
        auto next_d_limit = infinite_d_limit;
        came_from[start_string] = {"NONE",'$'};
        
        worklist.push({start_string,hf(start_string)+cost_so_far[start_string]});

        while (!worklist.empty())
        {
            auto current_node = worklist.front();
            worklist.pop();

            auto next_cost = cost_so_far[current_node.first] + 1;

            if(current_node.second > d_limit)
            {
                next_d_limit = min(next_d_limit,current_node.second);
            }
            else
            {
                // goal test
                if(goal_test(current_node.first) == true)
                {
                    stop_flag = true;
                    break;
                }
                
                // adding neighbor
                for(int direction=0;direction<4;direction++){
                    // get neighbor string according to directions
                    string neighbor_string(25,0);
                    auto flag = neighbor(current_node.first,neighbor_string,direction);

                    // skip if neighbor is invalid , if neoghbor in map 
                    auto DIR = came_from.find(neighbor_string)->second.second;
                    auto iter = cost_so_far.find(neighbor_string);

                    if(flag == false || NotContrast(DIR,direction) == false || ( iter != cost_so_far.end() && iter->second < next_cost ))
                    {
                        continue;
                    }

                    // set direction
                    switch (direction)
                    {
                        case DIRECTION::up:
                        {
                            DIR = 'U';
                            break;
                        }
                        
                        case DIRECTION::down:
                        {
                            DIR = 'D';
                            break;
                        }

                        case DIRECTION::left:
                        {
                            DIR = 'L';
                            break;
                        }

                        case DIRECTION::right:
                        {
                            DIR = 'R';
                            break;
                        }
                    }

                    // insert neighbor node
                    worklist.push({neighbor_string,hf(neighbor_string)+next_cost});
                    came_from[neighbor_string] = {current_node.first,DIR};
                    cost_so_far[neighbor_string] = next_cost;

#ifdef DEBUG
    cout << neighbor_string << endl;
#endif                
                }
            }

        }

        // if get goal
        if(stop_flag == true)
            break;
        
        // if do not get goal
        d_limit = next_d_limit;
        
    }

    return;
}

int main(int argc, char* argv[])
{
    // get options from commandline
    char* search_stradegy = argv[1];
    char* start_file = argv[2];
    char* target_file = argv[3];

    // test command line input
#ifdef DEBUG
    cout << search_stradegy << ' ' << start_file << ' ' << target_file << endl;
#endif

    /* set start and target status */
    vector<vector<int>> start(5,vector<int>(5,0));
    vector<vector<int>> target(5,vector<int>(5,0));

    // read start status from input.txt
    ifstream startFile;
    startFile.open(start_file,ios::in);
    if(startFile)
    {
        for(int i=0;i<5;i++)
        {
            for(int j=0;j<5;j++)
            {
                startFile >> start[i][j];
            }
        }
        startFile.close();
    }
    else
    {
        cout << "Input.txt does not exist!\nExiting......" << endl;
        exit(1);
    }

    // read target status from output.txt
    ifstream targetFile;
    targetFile.open(target_file,ios::in);
    if(targetFile)
    {
        for(int i=0;i<5;i++)
        {
            for(int j=0;j<5;j++)
            {
                targetFile >> target[i][j];
            }
        }
        targetFile.close();
    }
    else
    {
        cout << "Target.txt does not exist!\nExiting......" << endl;
        exit(1);
    }

#ifdef DEBUG
    cout << "Start Map:" << endl;
    for(int i=0;i<5;i++)
    {
        for(int j=0;j<5;j++)
            cout << start[i][j] << ' ';
        cout << endl;
    }
    cout << "Target Map:" << endl;
    for(int i=0;i<5;i++)
    {
        for(int j=0;j<5;j++)
            cout << target[i][j] << ' ';
        cout << endl;
    }
#endif

    /* set search stradegy */
    void (*search_func)(const vector<vector<int>>  &start,const vector<vector<int>>  &target);
    if(!strcmp(search_stradegy,"A_h1"))
        search_func = &A_h1;
    else if (!strcmp(search_stradegy,"A_h2"))
        search_func = &A_h2;
    else if (!strcmp(search_stradegy,"IDA_h1"))
        search_func = &IDA_h1;
    else if (!strcmp(search_stradegy,"IDA_h2"))
        search_func = &IDA_h2;
    else{
        cout << "Wrong type for heristic function! \nExiting......" << endl;
        exit(1);
    }

    /* run search function */
    auto start_time = chrono::system_clock::now();
    search_func(start,target);
    auto finish_time = chrono::system_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(finish_time - start_time);
    auto time_cost = double(duration.count())*chrono::microseconds::period::num/ chrono::microseconds::period::den;

    // rebuild action path
    if(came_from.find(target_string) != came_from.end())
    {
        vector<char> action_path;
        auto iter = came_from.find(target_string);
        auto parent = iter->second.first;
        auto action = iter->second.second;

        while (parent != "NONE")
        {
            action_path.push_back(action);
            iter = came_from.find(parent);
            parent = iter->second.first;
            action = iter->second.second;
            
        }   

        reverse(action_path.begin(),action_path.end());
        

        for(int i=0;i<action_path.size();i++)
            cout << action_path[i];

        ofstream resultfile;
        if(!strcmp(search_stradegy,"A_h1"))
            resultfile.open("output_A_h1.txt",ios::out | ios::app);
        else if (!strcmp(search_stradegy,"A_h2"))
            resultfile.open("output_A_h2.txt",ios::out| ios::app);
        else if (!strcmp(search_stradegy,"IDA_h1"))
            resultfile.open("output_IDA_h1.txt",ios::out| ios::app);
        else if (!strcmp(search_stradegy,"IDA_h2"))
            resultfile.open("output_IDA_h2.txt",ios::out| ios::app);
        
        for(int i=0;i<action_path.size();i++){
            resultfile << action_path[i];
        }
        resultfile << "," << time_cost << endl;
    }
    else{
        cout << "x,x" << endl;
        return 0;
    }  

    cout << "," << time_cost << endl;
    
}

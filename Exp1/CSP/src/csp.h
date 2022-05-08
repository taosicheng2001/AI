#include<iostream>
#include<map>
#include<memory>
#include<utility>

// #include<boost/smart_ptr.hpp>
// #include<boost/make_shared.hpp>

enum Rank{
    senior,
    junior
};

enum type{
    notwork,
    work,
    either,
    none
};



class csp;

class variable
{
private:
    int order;  // oreder number of worker
    int rank;   // rank of workrer
    int time;   // date of this variable
    bool if_assigned;   // flag for whether assigned
    int remain_assignment; // -1 reprensent 0 or 1

public:
    variable(int order, int rank, int time):order(order),rank(rank),time(time)
    {
        if_assigned = false;
        remain_assignment = type::either;
        // std::cout << "rank:" << rank << std::endl;
    };

    int get_order()
    {
        return order;
    }

    int get_rank()
    {
        return rank;
    }

    int get_time()
    {
        return time;
    }

    bool assigned_check()
    {
        return if_assigned;
    }

    int get_remain_assignment()
    {
        return remain_assignment;
    }

    void set_flag(bool flag){
        if_assigned = flag;
    }

    void print()
    {
        std::cout << "Order:" << order << " Time:" << time << " Assign:" << if_assigned << std::endl;
    }
};

class constraint
{
public:
    virtual bool test(csp* csp) const = 0;
};

class csp
{
private:
    int ** assignment;
    int worker_num;
    std::map<std::pair<int,int>,variable*> variables;
    std::map<int,constraint*> constraints;
    int assigned_variable_num;
    int cnt=0;

public:
    csp(int worker_num):worker_num(worker_num)
    {
        assigned_variable_num = 0;
        initial_assignment();
    }

    csp(int worker_num, int** assignment):worker_num(worker_num),assignment(assignment){};

    void initial_assignment()
    {
        assignment = (int **)malloc(sizeof(int*)*worker_num);

        // malloc assigment
        for(int i=0;i<worker_num;i++)
            assignment[i] = (int*)malloc(sizeof(int)*7);
        
        // initial assignment
        for(int i=0;i<worker_num;i++)
        {
            for(int j=0;j<7;j++)
                assignment[i][j] = -1;
        }
    }

    int** get_assignment()
    {
        return assignment;
    }

    int get_workernum()
    {
        return worker_num;
    }
    
    int get_assigned_num()
    {
        return assigned_variable_num;
    }

    std::map<std::pair<int,int>,variable*> get_variables()
    {
        return variables;
    }

    std::map<int,constraint*> get_constraint()
    {
        return constraints;
    }

    void add_variable(variable* new_variable)
    {
        //std::cout << new_variable->get_order() << " " << new_variable->get_time() << std::endl;
        variables[{new_variable->get_order(),new_variable->get_time()}] = new_variable;
    }

    void add_constraint(constraint* new_constraint)
    {
        constraints[cnt] = new_constraint;
        std::cout << "Constraint " << cnt++ << " Active" << std::endl;
    }

    void drop_constraint(int order)
    {
        constraints.erase(order);
    }

    variable* select_var(csp* csp)
    {
        // select unassignde  variable and return

        // day first
        for(int j=0;j<7;j++)
        {
            for(int i=0;i<csp->get_workernum();i++)
            {
                auto var = variables.find({i,j})->second;

                if(var->assigned_check() == false)
                    return var;
            }
        }

        // column
        // for(int i=0;i<csp->get_workernum();i++)
        // {
        //     for(int j=0;j<7;j++)
        //     {
        //         auto var = variables.find({i,j})->second;

        //         if(var->assigned_check() == false)
        //             return var;
        //     }

        // }       
 

        return nullptr;
    }

    void set_assigned_variable_num(int change)
    {
        assigned_variable_num += change;
        return;
    }

    void set_assign(int assign,variable* var)
        {
            auto order = var->get_order();
            auto time = var->get_time();
            switch (assign)
            {
                case type::work :
                {
                    assignment[order][time] = work;
                    break;
                }
                case type::notwork :
                {
                    assignment[order][time] = notwork;
                    break;
                }
            }

            // change csp and flag
            var->set_flag(true);
            set_assigned_variable_num(1);
        }

    void unset_assign(variable* var)
    {
        auto order = var->get_order();
        auto time = var->get_time();

        assignment[order][time] = -1;

        var->set_flag(false);
        set_assigned_variable_num(-1);
    }

    void print_assign()
    {
         for(int i=0;i<worker_num;i++)
        {
            for(int j=0;j<7;j++)
                std::cout << assignment[i][j] << " ";
            std::cout << std::endl;
        }
    }

    int get_constraint_num()
    {
        return cnt;
    }
};

// every worker should relax more than min_relax_day
class min_relax_constraint: public constraint
{
private:
    int min_relax_day;

public:
    min_relax_constraint(int input_relax_day):min_relax_day(input_relax_day){};
    virtual bool test(csp* csp) const
    {
        // std::cout << "min relax day test!" << std::endl;
        auto assignment = csp->get_assignment();
        for(int i=0;i<csp->get_workernum();i++)
        {
            // get work day for each worker
            auto work_day = 0;
            for(int j=0;j<7;j++)
            {
                if(assignment[i][j] == 1)
                    work_day++;
            }

            //std::cout << work_day << std::endl;

            // not satisfied
            if(work_day + min_relax_day > 7)
            {
                return false;
            }
        }

        return true;
    }
};

// each worker should not continue relax than max_continue_relax_day
class max_continue_relax_constraint: public constraint
{
private:
    int max_continue_relax_day;

public:
    max_continue_relax_constraint(int input_day):max_continue_relax_day(input_day){};
    virtual bool test(csp* csp) const
    {
        // std::cout << "max rest test!" << std::endl;
        auto assignment = csp->get_assignment();
        for(int i=0;i<csp->get_workernum();i++)
        {
            auto continue_relax = 0;
            for(int j=0;j<7;j++)
            {
                // get continue relax day for each worker
                if(assignment[i][j] == 0)
                    continue_relax++;
                if(assignment[i][j] == 1)
                    continue_relax = 0;

                // not satisfied
                if(continue_relax >= max_continue_relax_day)
                    return false;
            }
        }
        return true;
    }  
};

// worker each day should be more than min_workernum_eachday 
class min_workernum_eachday_constraint: public constraint
{
private:
    int min_workernum_eachday;
public:
    min_workernum_eachday_constraint(int input_num):min_workernum_eachday(input_num){};
    virtual bool test(csp* csp) const
    {
        // std::cout << "min worker test!" << std::endl;
        auto assignment = csp->get_assignment();
        auto not_complete = false;

        for(int j=0;j<7;j++)
        {
            // only check whether false when one day all assigned
            for(int i=0;i<csp->get_workernum();i++)
            {
                if(assignment[i][j] == -1)
                    not_complete = true;
            }

            auto workernum = 0;
            for(int i=0;i<csp->get_workernum();i++)
            {
                if(assignment[i][j] == 1)
                    workernum++;
            }

            if(not_complete == true ||  workernum >= min_workernum_eachday)
                continue;
            else
                return false;
        }

        return true;
    }

};

// each day there should be a senior worker
class senior_worker_eachday_constraint:public constraint
{
private:
    int min_workernum;

public:
    senior_worker_eachday_constraint(int minworker):min_workernum(minworker){};
    virtual bool test(csp* csp) const
    {
        // std::cout << "senior worker test!" << std::endl;

        auto assignment = csp->get_assignment();
        auto variables = csp->get_variables();
        auto not_complete = false;

        for(int j=0;j<7;j++)
        {
            // only check whether false when one day all assigned
            for(int i=0;i<csp->get_workernum();i++)
            {
                if(assignment[i][j] == -1)
                    not_complete = true;
            }

            auto flag = false;
            for(int i=0;i<csp->get_workernum();i++)
            {   
                // 
                if(assignment[i][j] == 1)
                {
                    //std::cout << i << " " << j << std::endl;
                    //std::cout <<  variables.find({i,j})->second->get_order() << " " << variables.find({i,j})->second->get_time() << std::endl << std::endl;
                    if(variables.find({i,j})->second->get_rank() == senior)
                    {
                        //std::cout << i << " is senior" << std::endl;
                        flag = true;
                    }
                }
            }

            if(not_complete == true ||  flag == true)
                continue;
            else
                return false;
        }

        return true;
    }
};

// worker1 and worker2 should not work in one day
class not_one_day_constraint: public constraint
{
private:
    int workernum1;
    int workernum2;

public:
    not_one_day_constraint(int worker1, int worker2):workernum1(worker1-1),workernum2(worker2-1){};
    virtual bool test(csp* csp) const
    {
        // std::cout << "not one day test!" << std::endl;
        auto assignment = csp->get_assignment();
        for(int j=0;j<7;j++)
        {
            // only check whether false when one day all assigned
            for(int i=0;i<csp->get_workernum();i++)
            {
                if(assignment[i][j] == -1)
                    return true;
            }

            if(assignment[workernum1][j] == 1 && assignment[workernum2][j] == 1)
                return false;
        }

        return true;
    }
};
#include"./csp.h"
#include<memory>
#include<fstream>
#include<ctime>

// #define DEBUG

using namespace std;

bool Inference(csp* CSP)
{
    auto flag = true;

    // constraint checking
    for(int i=0;i<CSP->get_constraint_num();i++)
    {
        flag = CSP->get_constraint().find(i)->second->test(CSP) && flag;
    }

    return flag;
}

bool BackTracking_Search(csp* CSP)
{
    // if finish assignment
    if(CSP->get_assigned_num() == (CSP->get_workernum()*7) )
    {
        return Inference(CSP);
    }
    
    // select an unassignde var
    auto current_var = CSP->select_var(CSP);
#ifdef DEBUG
    current_var->print();
#endif
    // try to assign for this var
    for(int assign=0;assign<=1;assign++)
    {
        // there is no valid assignment for this var
        auto valid_assignment = current_var->get_remain_assignment();
        if(valid_assignment == type::none)
        {
            return false;
        }
        // there is only one valid assignment for this var
        else if(valid_assignment <= 1 && valid_assignment != assign){
            continue;
        }

        // valid assign 
        CSP->set_assign(assign,current_var);
#ifdef DEBUG
        CSP->print_assign();
#endif

        // inference
        if(Inference(CSP) == true)
        {
            auto result = BackTracking_Search(CSP);
            
            if(result == true)
                return result;
        }

        CSP->unset_assign(current_var);
  
    }

    return false;

}


int main(){

    csp CSP(7);
    // add variables
    for(int j=0;j<7;j++){
        variable* worker1 = new variable(0,Rank::senior,j);
        variable* worker2 = new variable(1,Rank::senior,j);
        variable* worker3 = new variable(2,Rank::junior,j);
        variable* worker4 = new variable(3,Rank::junior,j);
        variable* worker5 = new variable(4,Rank::junior,j);
        variable* worker6 = new variable(5,Rank::junior,j);
        variable* worker7 = new variable(6,Rank::junior,j);

        CSP.add_variable(worker1);
        CSP.add_variable(worker2);
        CSP.add_variable(worker3);
        CSP.add_variable(worker4);
        CSP.add_variable(worker5);
        CSP.add_variable(worker6);
        CSP.add_variable(worker7);
    }

    // add constraint
    {
        min_relax_constraint constraint1(2);
        max_continue_relax_constraint constraint2(3);
        min_workernum_eachday_constraint constraint3(4);
        senior_worker_eachday_constraint constraint4(1);
        not_one_day_constraint constraint5(1,4);
        not_one_day_constraint constraint6(2,3);
        not_one_day_constraint constraint7(3,6);

        CSP.add_constraint(&constraint1);
        CSP.add_constraint(&constraint2);
        CSP.add_constraint(&constraint3);
        CSP.add_constraint(&constraint4);
        CSP.add_constraint(&constraint5);
        CSP.add_constraint(&constraint6);
        CSP.add_constraint(&constraint7);
    }

    // csp CSP(10);
    // // add variables
    // for(int j=0;j<7;j++){
    //     variable* worker1 = new variable(0,Rank::senior,j);
    //     variable* worker2 = new variable(1,Rank::senior,j);
    //     variable* worker3 = new variable(2,Rank::junior,j);
    //     variable* worker4 = new variable(3,Rank::junior,j);
    //     variable* worker5 = new variable(4,Rank::junior,j);
    //     variable* worker6 = new variable(5,Rank::junior,j);
    //     variable* worker7 = new variable(6,Rank::junior,j);
    //     variable* worker8 = new variable(7,Rank::senior,j);
    //     variable* worker9 = new variable(8,Rank::junior,j);
    //     variable* worker10 = new variable(9,Rank::senior,j);

    //     CSP.add_variable(worker1);
    //     CSP.add_variable(worker2);
    //     CSP.add_variable(worker3);
    //     CSP.add_variable(worker4);
    //     CSP.add_variable(worker5);
    //     CSP.add_variable(worker6);
    //     CSP.add_variable(worker7);
    //     CSP.add_variable(worker8);
    //     CSP.add_variable(worker9);
    //     CSP.add_variable(worker10);
    // }

    // // add constraint
    // {
    //     min_relax_constraint constraint1(2);
    //     max_continue_relax_constraint constraint2(3);
    //     min_workernum_eachday_constraint constraint3(5);
    //     senior_worker_eachday_constraint constraint4(1);
    //     not_one_day_constraint constraint5(1,5);
    //     not_one_day_constraint constraint6(2,6);
    //     not_one_day_constraint constraint7(8,10);

    //     CSP.add_constraint(&constraint1);
    //     CSP.add_constraint(&constraint2);
    //     CSP.add_constraint(&constraint3);
    //     CSP.add_constraint(&constraint4);
    //     CSP.add_constraint(&constraint5);
    //     CSP.add_constraint(&constraint6);
    //     CSP.add_constraint(&constraint7);
    // }

    clock_t start = clock();
    BackTracking_Search(&CSP);
    clock_t end   = clock();
    
    auto assignment = CSP.get_assignment();
    CSP.print_assign();
    
    cout << "Time Cost: " << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;

    fstream outfile;
    outfile.open("../output/output2.txt",ios::out);
    if(outfile){

        for(int j=0;j<7;j++)
        {
        
            for(int i=0;i<CSP.get_workernum();i++)
            {
                if(assignment[i][j] == 1)
                    outfile << i+1 << " ";
            }
            outfile << endl;    
        }
            
        
    }


    exit(0);
    return 0;

}
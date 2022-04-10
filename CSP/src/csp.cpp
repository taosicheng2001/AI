#include"./csp.h"
#include<memory>

#define DEBUG

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

    // test
    // int** assignment = new int*[7];
    // for(int i=0;i<5;i++)
    //     assignment[i] = new int[7];
    // int arr0[7] = {0,1,1,1,1,1,0};
    // assignment[0] = arr0;
    // int arr1[7] = {0,1,0,1,0,1,0};
    // assignment[1] = arr1;
    // int arr2[7] = {1,1,0,1,1,0,1};
    // assignment[2] = arr2;
    // int arr3[7] = {1,0,1,0,1,0,1};
    // assignment[3] = arr3;
    // int arr4[7] = {1,1,1,0,0,1,1};
    // assignment[4] = arr4;
    // csp CSP(5,assignment);

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

    // shared ptr
    std::shared_ptr<min_relax_constraint> ptr_1(&constraint1); 
    std::shared_ptr<constraint> ptr_save_1;
    ptr_save_1 = std::dynamic_pointer_cast<constraint>(ptr_1);
    CSP.add_constraint(ptr_save_1);

    std::shared_ptr<max_continue_relax_constraint> ptr_2(&constraint2); 
    std::shared_ptr<constraint> ptr_save_2;
    ptr_save_2 = std::dynamic_pointer_cast<constraint>(ptr_2);
    CSP.add_constraint(ptr_save_2);
    
    std::shared_ptr<min_workernum_eachday_constraint> ptr_3(&constraint3); 
    std::shared_ptr<constraint> ptr_save_3;
    ptr_save_3 = std::dynamic_pointer_cast<constraint>(ptr_3);
    CSP.add_constraint(ptr_save_3);

    std::shared_ptr<senior_worker_eachday_constraint> ptr_4(&constraint4); 
    std::shared_ptr<constraint> ptr_save_4;
    ptr_save_4 = std::dynamic_pointer_cast<constraint>(ptr_4);
    CSP.add_constraint(ptr_save_4);

    std::shared_ptr<not_one_day_constraint> ptr_5(&constraint5); 
    std::shared_ptr<constraint> ptr_save_5;
    ptr_save_5 = std::dynamic_pointer_cast<constraint>(ptr_5);
    CSP.add_constraint(ptr_save_5);

    std::shared_ptr<not_one_day_constraint> ptr_6(&constraint6); 
    std::shared_ptr<constraint> ptr_save_6;
    ptr_save_6 = std::dynamic_pointer_cast<constraint>(ptr_6);
    CSP.add_constraint(ptr_save_6);   

    std::shared_ptr<not_one_day_constraint> ptr_7(&constraint7); 
    std::shared_ptr<constraint> ptr_save_7;
    ptr_save_7 = std::dynamic_pointer_cast<constraint>(ptr_7);
    CSP.add_constraint(ptr_save_7);     

    }

    BackTracking_Search(&CSP);
    auto assignment = CSP.get_assignment();

        
    // test
    if(CSP.get_constraint().find(0)->second->test(&CSP))
        cout << "C1 True!" << endl;
    if(CSP.get_constraint().find(1)->second->test(&CSP))
        cout << "C2 True!" << endl;
    if(CSP.get_constraint().find(2)->second->test(&CSP))
        cout << "C3 True!" << endl;
    if(CSP.get_constraint().find(3)->second->test(&CSP))
        cout << "C4 True!" << endl;
    if(CSP.get_constraint().find(4)->second->test(&CSP))
        cout << "C5 True!" << endl;
    if(CSP.get_constraint().find(5)->second->test(&CSP))
        cout << "C6 True!" << endl;
    if(CSP.get_constraint().find(6)->second->test(&CSP))
        cout << "C7 True!" << endl;
    
    exit(0);
    return 0;

}
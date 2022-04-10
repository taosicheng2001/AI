#include"./csp.h"

using namespace std;

void BackTracking_Search()
{
    ;
}

void BackTrack()
{
    
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

    static csp CSP(7);

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
    
    // test
    // if(CSP.get_constraint().find(&constraint1)->first->test(&CSP))
    //     cout << "C1 True!" << endl;
    // if(CSP.get_constraint().find(&constraint2)->first->test(&CSP))
    //     cout << "C2 True!" << endl;
    // if(CSP.get_constraint().find(&constraint3)->first->test(&CSP))
    //     cout << "C3 True!" << endl;
    // if(CSP.get_constraint().find(&constraint4)->first->test(&CSP))
    //     cout << "C4 True!" << endl;
    // if(CSP.get_constraint().find(&constraint5)->first->test(&CSP))
    //     cout << "C5 True!" << endl;




}
#include <iostream>
using namespace std;
class Demo{
    public:
        Demo()
        {
            total_objects ++;
        }
        Demo(int a) : aa(a)
        {
            total_objects ++;
        }
        ~Demo()
        {
            total_objects --;
        }
        
        static int gettotal_objects()
        {
            return total_objects;    
        }
        
    private:
        int aa;
        static int total_objects;  
};

int Demo::total_objects = 0;  


int main()
{
    Demo demo(10);
    cout << "Demo= " << Demo::gettotal_objects() << endl; 
    Demo demo1(10);
    cout << "Demo= " << Demo::gettotal_objects() << endl; 
    Demo demo2(10);
    cout << "Demo= " << Demo::gettotal_objects() << endl; 
    Demo demo3(10);
    cout << "Demo= " << Demo::gettotal_objects() << endl; 
    Demo demo4(10);
    cout << "Demo= " << Demo::gettotal_objects() << endl; 
    Demo *p = new Demo[10];
    cout << "Demo= " << Demo::gettotal_objects() << endl; 

    delete [] p;
    cout << "Demo= " << Demo::gettotal_objects() << endl; 
    
    
    


    return 0;
}
/*********************************************
* @FileName: driver_license_test.cpp
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @CreatedTime: 周四 7月 20 16:04:00 2023
* @Descript:
*/
#include <iostream>
#include <memory>
#include <ostream>
using namespace std;

class Examiner;
class Student{
    public:
    Student(string name, string id) : name(name), id(id), score(0){}
    bool test(Student&, Examiner&);
    int gettExamResult();
    
    private:
    string name;
    string id; // 20230720
    int score;  // 90分
    friend class Examiner;

};

class Examiner
{
    public:
    bool chickId(Student&);
    void setScore(Student&);
};



bool Student::test(Student& stu, Examiner& ex)
{
    cout << "准备考试" << endl;
    cout << "进行身份验证" << endl;
    if(ex.chickId(stu))
    {
        cout << "身份验证成功" << endl;
        cout << "开始考试" << endl;
        cout << "考试完成" << endl;
        ex.setScore(stu);
        if(stu.gettExamResult() > 90)
        {
            cout << "考试通过" << endl;
            return true;
        }else 
        {
            cout << "考试不合格" << endl;
            return false;
        }
        
    }else {
        cout << "身份验证失败" << endl;
        return false;
    }
}

int Student::gettExamResult()
{
    return score;
}

bool Examiner::chickId(Student&stu)
{
    if(stu.id == "20230720")
    {
        cout << "考生的身份为 姓名: " << stu.name << ",  " << "id: " << stu.id << endl;
        return true;
    }
    return false;
}

void Examiner::setScore(Student &stu)
{
    stu.score = 99;
    cout << "考官：考生最终的分数为: " << stu.score << endl;
}

int main(int argc, char *argv[])
{
    Student xiaoming("小明", "20230720");
    Examiner kaoguan;
    
    if(xiaoming.test(xiaoming, kaoguan))
    {
        cout << "成功" << endl;
    }
    else {
        cout << "失败" << endl;
    }


    
    return 0;
}
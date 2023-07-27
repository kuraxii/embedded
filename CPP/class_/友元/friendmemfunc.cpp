#include <iostream>
#include <memory>

using namespace std;

class Address;

class Student
{
  public:
    Student(string n, int a, float s) : name(n), age(a), score(s)
    {
    }

    void show_home(const Address &add);

  private:
    string name;
    int age;
    float score;
};

class Address
{
  public:
    Address(string p, string c, string d, string s) : province(p), city(c), district(d), street(s)
    {
    }
    // 让student成为Address类的朋友
    friend void Student::show_home(const Address &add);

  private:
    string province;
    string city;
    string district;
    string street;
};

void Student::show_home(const Address &add)
{
    cout << name << "今年" << age << "岁，成绩是" << score << endl;
    cout << "家庭地址是" << add.province << add.city << add.district << add.street << endl;
}

int main(int argc, char *argv[])
{
    Address add("四川省", "成都市", "金牛区", "金牛大道1223号");
    Student xiaoming("小明", 13, 99.9);

    return 0;
}
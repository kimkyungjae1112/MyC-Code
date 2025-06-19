#include <iostream>
#include <map>
#include <string>
#include <functional>

namespace ObjectFunc
{


typedef int (*MathFunc)(int, int);
using ModernMathFunc = int(*)(int, int);

struct StructFunc 
{ 
    virtual std::string GetId() const = 0;
    
    virtual int operator()(int, int) = 0;
};

struct StructAddFunc : public StructFunc
{
    virtual std::string GetId() const override { return "Add : "; }

    virtual int operator()(int a, int b) { return a + b; }
};

struct StructSubFunc : public StructFunc
{
    virtual std::string GetId() const override { return "Sub : "; }

    virtual int operator()(int a, int b) { return a - b; }
};

int Add(int a, int b) { return a + b; }
int Sub(int a, int b) { return a - b; }

std::map<MathFunc, std::string> FuncMap = {
   { Add, "Add : " },
   { Sub, "Subtract : " },
};

MathFunc Funcs[] = { Add, Sub };

void Apply(MathFunc Func)
{
    std::cout << "---- Start Apply FuncPointer ----" << '\n';

    std::cout << FuncMap[Func] << Func(10, 3) << '\n';

    for (auto F : Funcs)
    {
        std::cout << FuncMap[F] << F(10, 3) << '\n';
    }

    std::cout << "---- End Apply FuncPointer ----" << '\n';
}

void Apply(StructFunc* Func)
{
    std::cout << "---- Start Apply Struct Func ----" << '\n';
    std::cout << Func->GetId() << Func->operator()(1, 3) << '\n';
    std::cout << "---- End Apply Struct Func ----" << '\n';
}

std::function<void()> Apply()
{
    return []() { std::cout << "Lambda Function" << '\n'; };
}

void Main()
{
    Apply(Add);
    Apply(Sub);
    StructFunc* AddFunc = new StructAddFunc();
    StructFunc* SubFunc = new StructSubFunc();

    Apply(AddFunc);
    Apply(SubFunc);

    auto Function = Apply();
    Function();


    // 언리얼의 Delegate, 혹은 Input에 함수를 연결해줄 때 이런 과정을 사용했구나
    std::function<int(StructSubFunc&, int, int)> StructFunction = &StructSubFunc::operator();
    std::cout << "멤버 함수를 저장하기 : " << StructFunction(*static_cast<StructSubFunc*>(SubFunc), 3, 2) << '\n';

    delete AddFunc;
    delete SubFunc;
}

}

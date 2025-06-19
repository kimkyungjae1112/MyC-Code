#include "ObjectFunc.hpp"
#include "MultiThread.hpp"
#include "SmartPointer.hpp"

#define OBJECTFUNC ObjectFunc::Main()
#define FALLINGSTAR FallingStar::Main()
#define SMARTPOINTER SmartPointer::Main()

int main()
{
    std::cout << "확인하고 싶은 코드 결과 : " << "ObjectFunc, FallingStar, SmartPointer" << '\n' << "위 목록 중 입력 : ";
    
    std::string Str;
    std::cin >> Str;

    if(Str == "ObjectFunc") OBJECTFUNC;
    else if(Str == "FallingStar") FALLINGSTAR;
    else if(Str == "SmartPointer") SMARTPOINTER;
    else { std::cout << "잘못된 입력";}

    return 0;
}
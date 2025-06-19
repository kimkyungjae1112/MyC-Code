#pragma once
#include <utility>
#include <iostream>

/*
	일반 생성 방법
	UniquePtr<T> Ptr = UniquePtr<T>(new T());

	Make 팩토리 함수
	UniquePtr<T> Ptr = Make_Functions<T>(value of T);
*/

namespace SmartPointer
{

template <typename T>
class UniquePtr
{
public:
	explicit UniquePtr(T* InPtr = nullptr) : PrimitivePtr(InPtr) {}
	
    // 복사 방지
	UniquePtr(const UniquePtr&) = delete;
	UniquePtr& operator=(const UniquePtr&) = delete;
	
    // 이동 생성자
	UniquePtr(const UniquePtr&& Rhs) noexcept : PrimitivePtr(Rhs.PrimitivePtr)
	{
		Rhs.PrimitivePtr = nullptr;
	}

	UniquePtr& operator=(UniquePtr&& Rhs) noexcept
	{
		if (this != &Rhs)
		{
			delete PrimitivePtr;
			PrimitivePtr = Rhs.PrimitivePtr;
			Rhs.PrimitivePtr = nullptr;
		}
		return *this;
	}

	~UniquePtr()
	{
        std::cout << "Unique Pointer 객체 메모리 반환" << '\n';
		delete PrimitivePtr;
	}

	// 포인터 연산자 기능 구현
	T& operator*() const { return *PrimitivePtr; }
	T* operator->() const { return PrimitivePtr; }
	
    // 소유권 Handle
	void Reset(T* InPtr = nullptr)
	{
		delete PrimitivePtr;
		PrimitivePtr = InPtr;
		InPtr = nullptr;
	}

	T* Get() const
	{
		return PrimitivePtr;
	}

private:
	T* PrimitivePtr;
};

template<typename T>
class SharedPtr
{
public:
	SharedPtr(T* InPtr) : PrimitivePtr(InPtr) { RefCount = new int(1); }
	
    // 복사 연산 허용
	SharedPtr(const SharedPtr& InSharedPtr) : PrimitivePtr(InSharedPtr.PrimitivePtr), RefCount(InSharedPtr.RefCount)
	{
		++(*RefCount);
	}

	SharedPtr& operator=(const SharedPtr& InSharedPtr)
	{
		if (this != &InSharedPtr)
		{
			PrimitivePtr = InSharedPtr.PrimitivePtr;
			RefCount = InSharedPtr.RefCount;
			++(*RefCount);
		}
		return *this;
	}

	// 이동 연산 제거
	// 복사 생성자를 정의하면 기본 이동 생성자는 생성 안되지만, 명시적으로 표현
	SharedPtr(const SharedPtr&&) = delete;
	SharedPtr& operator=(const SharedPtr&&) = delete;

	~SharedPtr()
	{
		--(*RefCount);
		if (*RefCount == 0)
		{
            std::cout << "Shared Pointer 객체 메모리 반환" << '\n';
			delete PrimitivePtr;
			delete RefCount;
		}
	}

	// 포인터 연산자 기능
	T& operator*() const { return *PrimitivePtr; }
	T* operator->() const { return PrimitivePtr; }
	
    // 소유권 Handle
	T* Get() const
	{
		return PrimitivePtr;
	}

private:
	T* PrimitivePtr;
	int* RefCount;
};

template <typename T, typename... Args>
UniquePtr<T> Make_Unique(Args&&... args)
{
	return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
SharedPtr<T> Make_Shared(Args&&... args)
{
	return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

struct Dummy
{
    void DummyFunc()
    {
        std::cout << "I am Dummy!" << '\n';
    }
};

void Main()
{
    UniquePtr<Dummy> DummyUniquePtr = UniquePtr<Dummy>(new Dummy());
    // UniquePtr<Dummy> DummyUniquePtr1 = DummyUniquePtr; 오류
    DummyUniquePtr->DummyFunc();
    

    SharedPtr<Dummy> DummySharedPtr1 = Make_Shared<Dummy>(Dummy());
    SharedPtr<Dummy> DummySharedPtr2 = DummySharedPtr1;
    
    DummySharedPtr1->DummyFunc();
    DummySharedPtr2->DummyFunc();
}

}
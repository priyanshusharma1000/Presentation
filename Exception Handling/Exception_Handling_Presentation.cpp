//stack unwinding
//When the exception propogate from caller function to called function then the fully constructed object of caller function goes out of scope due to stack unwinding


//When C++ throws an exception :
//Normal execution stops immediately
//The runtime searches for a matching catch
//All stack objects between throw → catch are destroyed
//This process is called Stack Unwinding

//#include <iostream>
//using namespace std;
//
//struct A {
//    A() { cout << "A created\n"; }
//    ~A() { cout << "A destroyed\n"; }
//};
//
//void func() {
//    A obj;
//    throw runtime_error("Error!");
//}
//
//int main() {
//    try {
//        func();
//    }
//    catch (const runtime_error&e) {
//        cout << "Exception caught\n" << e.what() << std::endl;
//    }
//    return 0;
//}






//2) Exceptions + RAII
//RAII = Resource Acquisition Is Initialization
//Resources = memory, file, mutex, sockets
//
//RAII ensures :
//Acquire resource in constructor
//Release in destructor
//Because destructors run during stack unwinding →
//RAII makes code exception safe automatically



//BAD EXAMPLE(NO RAII)
//#include <iostream>
//using namespace std;
//
//struct A {
//    A() { cout << "A created\n"; }
//    ~A() { cout << "A destroyed\n"; }
//};
//void fun() {
//    A* ptr = new A();
//    throw runtime_error("exception occur");
//    delete ptr; //this line never reach and hence it leads to memory leak
//}
//int main() {
//    try {
//        fun();
//    }
//    catch (...) {
//        cout << "Exception caught\n";
//    }
//    return 0;
//}


//Good Example(with RAII)
//#include <iostream>
//#include<memory>
//using namespace std;
//
//struct A {
//    A() { cout << "A created\n"; }
//    ~A() { cout << "A destroyed\n"; }
//};
//void fun() {
//    unique_ptr<A>ptr(new A());
//    throw runtime_error("exception occur");
//    
//}
//int main() {
//    try {
//        fun();
//    }
//    catch (...) {
//        cout << "Exception caught\n";
//    }
//    return 0;
//}





//3) Throwing Exceptions in Constructors
//Constructor can throw if object cannot be created.
//
//
//Example:
//(1)File cannot open
//(2)DB connection fails
//(3)Memory allocation fails
//
//If constructor throws :
//Object is NOT created
//Already created members are destroyed


//code demo
//#include<iostream>
//using namespace std;
//
//struct File {
//    File() {
//        cout << "Opening file\n";
//        throw runtime_error("File failed");
//    }
//    ~File() { cout << "Closing file\n"; }
//};
//
//int main() {
//    try {
//        File f;
//    }
//    catch (...) {
//        cout << "Failed to create File object\n";
//    }
//}
//A failed constructor means a failed object.





//4) noexcept and Why It’s CRITICAL
//noexcept = function promises no exceptions
//void func() noexcept;
//
//If exception escapes noexcept function →
//std::terminate() is called → program crashes
//
//
//Why it matters
//1) Performance
//Compiler optimizes better.
//2) STL uses it heavily
//Example : vector reallocation uses move only if noexcept.

//#include<iostream>
//#include<vector>
//using namespace std;
//
//struct A {
//    A() { cout << "ctor\n"; }
//    A(const A&) { cout << "copy\n"; }
//    A(A&&) noexcept{ cout << "move\n"; }
//};
//int main() {
//    vector<A> v;
//
//    v.reserve(1);        // capacity = 1
//
//    cout << "---- push 1 ----\n";
//    v.push_back(A());    // construct first element
//
//    cout << "---- push 2 ----\n";
//    v.push_back(A());    // triggers REALLOCATION
//
//    cout << "---- push 3 ----\n";
//    v.push_back(A());    // triggers REALLOCATION again
//}







//5) catch (...)
//catch (...) = catch ANY exception
//Universal catch handler
//
//try {}
//catch (...) {}
//
//Catches:
//
//   std exceptions
//   user exceptions
//   unknown exceptions
//
// What it CANNOT do:
//  Cannot access exception object
//  Cannot know what error occurred
//
//
//It is the last safety net and should be place at last of catch statement


//#include <iostream>
//using namespace std;
//
//void riskyFunction()
//{
//    throw 10;      // throwing int
//    // throw "error";  // could be anything
//    // throw 3.14;     // could be double
//}
//
//int main()
//{
//    try
//    {
//        riskyFunction();
//    }
//    catch (...)
//    {
//        cout << "Some exception caught!" << endl;
//    }
//
//    cout << "Program continues safely";
//}



//example where it won't work
//case 1: divide by 0
//#include <iostream>
//using namespace std;
//
//int main()
//{
//    try
//    {
//        int a = 10;
//        int b = 0;
//        int c = a / b;   // runtime crash (SIGFPE)
//        cout << c;
//    }
//    catch (...)
//    {
//        cout << "Exception caught!";
//    }
//}


//case 2:segmentation fault
//#include <iostream>
//using namespace std;
//
//int main()
//{
//    try
//    {
//        int* ptr = nullptr;
//        *ptr = 5;    // segmentation fault
//    }
//    catch (...)
//    {
//        cout << "Caught!";
//    }
//}









//6) Rethrowing Exceptions
//Catching an exception and throwing the same exception again so it continues propagating up the call stack.
//
//
//Why is it important ?
//
//Used when a lower layer can :
//    log / cleanup / add context
//   but cannot fully handle the error
//
//So it passes the same original exception to higher layers.
//Example: DB layer logs → Service layer decides what to do.
//
//
//Correct way to rethrow(preserve original exception)
//try {
//        risky();
//    }
//catch (const std::exception& e) {
//        log(e.what());
//        throw;   // <-- CORRECT rethrow
//    }

//
//❌ Wrong way(creates copy / slicing)
//try {
//    risky();
//}
//catch (std::exception e) {
//    log(e.what());
//    throw e;   // <-- WRONG
//}


//code example
//#include <iostream>
//#include <stdexcept>
//using namespace std;
//
//void lowLevel()
//{
//    cout << "lowLevel(): throwing exception\n";
//    throw runtime_error("Database connection failed");
//}
//
//void midLevel()
//{
//    try {
//        lowLevel();
//    }
//    catch (const exception& e) {
//        cout << "midLevel(): logging -> " << e.what() << endl;
//
//        // RETHROW SAME EXCEPTION
//        throw;
//    }
//}
//
//int main()
//{
//    try {
//        midLevel();
//    }
//    catch (const exception& e) {
//        cout << "main(): final handler -> " << e.what() << endl;
//    }
//}






//7) Can destructor throw exception
//While stack unwinding is happening, obj destructor runs.
//If destructor ALSO throws → now we have two active exceptions.
//C++ has no way to handle two simultaneous exceptions
//runtime calls :
//std::terminate(); //program crashes
//
//Demo of the crash
//
//#include <iostream>
//#include <stdexcept>
//using namespace std;
//
//class Test {
//public:
//    ~Test() {
//        cout << "Destructor called\n";
//        throw runtime_error("Destructor error");
//    }
//};
//
//int main() {
//    try {
//        Test t;
//        throw runtime_error("Main error");
//    }
//    catch (...) {
//        cout << "Caught in main\n";
//    }
//}

//IT IS ALWAYS ADVISABLE TO MAKE DESTRUCTOR AS A NOEXCEPT FUNCTION














//std::exception is the base class for all the exception in c++
//some examples
//#include<iostream>
//#include<exception>
//#include<vector>
//using namespace std;
//void fun() {
//	vector<int>v;
//	v.push_back(1);
//	std::cout << v.at(1);
//}
//int main() {
//	try {
//		fun();
//	}
//	catch (const out_of_range& e) { //const is necessary otherwise copy is created
//		std::cout << "Exception :" << e.what() << std::endl;
//	}
//	catch(const exception& e) {
//		std::cout << "Exception :" << e.what() << std::endl;
//	}
//	return 0;
//}




//we can create our own custom exception class by inheriting from exception class and overload its virtual function what()
//#include <iostream>
//#include <exception>
//using namespace std;
//
//class InvalidAgeException : public std::exception
//{
//public:
//    const char* what() const noexcept override
//    {
//        return "Age cannot be negative!";
//    }
//};
//
//void checkAge(int age)
//{
//    if (age < 0)
//        throw InvalidAgeException();
//}
//
//int main()
//{
//    try
//    {
//        checkAge(-5);
//    }
//    catch (const exception& e)
//    {
//        cout << e.what();
//    }
//}

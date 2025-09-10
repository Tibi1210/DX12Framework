#pragma once

#include <iostream>



#define KBs(x) (x)*1024


#define EVAL_HRES(hr, err) if (hr != S_OK){std::cout << "API Error: " << err << ". At file: " << __FILE__ << std::endl;}
#define PRINT_W_N(msg) std::wcout << msg << std::endl;
#define PRINT_N(msg) std::cout << msg << std::endl;


#ifdef _DEBUG

#define ASSERT(exp) if (!(exp)) { std::cout << "Critical error in file:" << __FILE__ << std::endl; throw; }

#else

#define ASSERT(exp)

#endif 

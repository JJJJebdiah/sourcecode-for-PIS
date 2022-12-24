#include "product.h"


bool Product::add(int num)
{
    number +=num;
    return true;
}

bool Product::delet(int num)
{
    if(number<num)
        return false;
    number -=num;
    return true;
}

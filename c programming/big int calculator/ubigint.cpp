// $Id: ubigint.cpp,v 1.14 2016-06-23 17:21:26-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <cmath>
using namespace std;

#include "ubigint.h"
#include "debug.h"

ubigint::ubigint (unsigned long that) {
    
    stringstream sso;
    sso << that;
    
    string numbers;
    sso >> numbers;
    
    for (char digit: numbers)
    {
        ubig_value.insert(ubig_value.begin(), digit);
    }
}

ubigint::ubigint (const string& that) {
   for (char digit: that) {
      /*if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }*/
      ubig_value.insert(ubig_value.begin(), digit);
   }
}

ubigint ubigint::operator+ (const ubigint& that) const {
   //return ubigint (uvalue + that.uvalue);
    ubigint result;
    unsigned int digit = 0;
    int num1, num2;
    int carry = 0;
    int remainder;
    bool this_longer = false;
    bool that_longer = false;
    
   while (true)
   {
       num1 = this->ubig_value[digit] - 48;
       num2 = that.ubig_value[digit] - 48;
       int sum = num1 + num2 + carry;
       
       remainder = sum % 10;
       unsigned char rem = remainder + '0';
       result.ubig_value.push_back(rem);
       if (sum >= 10)
       {
           carry = 1;
       }
       else
       {
           carry = 0;
       }
       
       ++digit;
       if (digit == this->ubig_value.size())
       {
           if (digit == that.ubig_value.size())
           {
               if(carry == 1)
               {
                   unsigned char co = 1 + '0';
                   result.ubig_value.push_back(co);
               }
               return result;
           }
           else
           {
               that_longer = true;
               break;
           }
       }
       
       if (digit == that.ubig_value.size())
       {
           this_longer = true;
           break;
       }
   }
    
    if (this_longer)
    {
        for (unsigned int num = digit;
             num < this->ubig_value.size(); num++)
        {
            num1 = this->ubig_value[num] - 48;
            if (carry == 1)
            {
                num1 += carry;
                if (num1 >= 10)
                {
                    num1 = num1 % 10;
                    carry = 1;
                }
                else
                {
                    carry = 0;
                }
            }
            unsigned char rem = num1 + '0';
            result.ubig_value.push_back(rem);
        }
    }
    
    if (that_longer)
    {
        for (unsigned int num = digit;
             num < that.ubig_value.size(); num++)
        {
            num1 = that.ubig_value[num] - 48;
            if (carry == 1)
            {
                num1 += carry;
                if (num1 >= 10)
                {
                    num1 = num1 % 10;
                    carry = 1;
                }
                else
                {
                    carry = 0;
                }
            }
            unsigned char rem = num1 + '0';
            result.ubig_value.push_back(rem);
        }
    }
    if (carry == 1)
    {
        unsigned char rem = carry + '0';
        result.ubig_value.push_back(rem);
        carry = 0;
    }
    
    return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   //if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   //return ubigint (uvalue - that.uvalue);
    
    ubigint result;
    int digit = 0;
    int num1, num2;
    int carry = 0;
    bool this_bigger = false;
    bool that_bigger = false;
    
    int this_size = this->ubig_value.size();
    int that_size = that.ubig_value.size();
    
    if (*this > that)
        this_bigger = true;
    if (*this < that)
        that_bigger = true;
    
    if(this_bigger == false && that_bigger == false)
    {
        unsigned char zero = 0 + '0';
        result.ubig_value.push_back(zero);
        return result;
    }
    
    if (this_bigger)
    {
        for (int i = 0; i < that_size; i++)
        {
            num1 = this->ubig_value[i] - 48 - carry;
            num2 = that.ubig_value[i] - 48;
            
            if (num1 < num2)
            {
                num1 += 10;
                carry = 1;
            }
            else
            {
                carry = 0;
            }
            
            int sub = num1 - num2;
            unsigned char subt = sub + '0';
            result.ubig_value.push_back(subt);
            digit++;
        }
        
        for (int j = digit; j < this_size; j++)
        {
            num1 = this->ubig_value[j] - 48 - carry;
            
            if(num1 < 0)
            {
                num1 += 10;
                carry = 1;
            }
            else{
                carry = 0;
            }
            
            unsigned char subt = num1 + '0';
            result.ubig_value.push_back(subt);
        }
    }
    
    if (that_bigger)
    {
        for (int i = 0; i < this_size; i++)
        {
            num1 = that.ubig_value[i] - 48 - carry;
            num2 = this->ubig_value[i] - 48;
            
            if (num1 < num2)
            {
                num1 += 10;
                carry = 1;
            }
            else
            {
                carry = 0;
            }
            
            int sub = num1 - num2;
            unsigned char subt = sub + '0';
            result.ubig_value.push_back(subt);
            digit++;
        }
        
        for (int j = digit; j < that_size; j++)
        {
            num1 = that.ubig_value[j] - 48 - carry;
            
            if(num1 < 0)
            {
                num1 += 10;
                carry = 1;
            }
            else{
                carry = 0;
            }
            
            unsigned char subt = num1 + '0';
            result.ubig_value.push_back(subt);
        }
    }
    
    while( result.ubig_value.size() > 0
          && result.ubig_value.back() == 48)
    {
        result.ubig_value.pop_back();
    }
    return result;
}

ubigint ubigint::operator* (const ubigint& that) const {
   //return ubigint (uvalue * that.uvalue);
    ubigint result = ubigint(0);
    int num1, num2;
    int carry = 0;
    int remainder;
    vector<ubigint> myvec;
    unsigned char digit = 0 + '0';
    
    for (unsigned int i = 0; i < this->ubig_value.size(); i++)
    {
        ubigint temp;
        
        for (unsigned int k = 0; k < i; k++)
        {
            temp.ubig_value.push_back(digit);
        }
        
        for (unsigned int j = 0; j < that.ubig_value.size(); j++)
        {
            num1 = this->ubig_value[i] - 48;
            num2 = that.ubig_value[j] - 48;
            int mult = num1 * num2 + carry;
            remainder = mult % 10;
            carry = mult / 10;
            
            unsigned char rem = remainder + '0';
            temp.ubig_value.push_back(rem);
        }
        
        if (carry > 0)
        {
            unsigned char car = carry + '0';
            temp.ubig_value.push_back(car);
            carry = 0;
        }
        
        myvec.push_back(temp);
    }
    
    for (unsigned int x = 0; x < myvec.size(); x++)
    {
        result = result + myvec[x];
    }
    
    return result;
}

void ubigint::multiply_by_2() {
   //uvalue *= 2;
    ubigint result;
    ubigint two = ubigint(2);
    ubigint temp;
    
    for (unsigned int i = 0; i < this->ubig_value.size(); i++)
    {
        int num = this->ubig_value[i] - 48;
        unsigned char cha = num + '0';
        temp.ubig_value.push_back(cha);
    }
    
    result = temp * two;
    
    this->ubig_value.clear();
    
    for(unsigned int j = 0; j < result.ubig_value.size(); j++)
    {
        int num1 = result.ubig_value[j] - 48;
        unsigned char cha1 = num1 + '0';
        this->ubig_value.push_back(cha1);
    }
}

void ubigint::divide_by_2() {
   //uvalue /= 2;
    ubigint one = ubigint(1);
    ubigint zero = ubigint(0);
    if (*this == one)
    {
        *this = zero;
        return;
    }
    
    if (*this == zero)
    {
        return;
    }
    
    ubigint result;
    int digit = this->ubig_value.size() - 1;
    int num, div;
    int remainder = 0;
    
    for (unsigned int i = 0; i < this->ubig_value.size(); i++)
    {
        num = this->ubig_value[digit - i] - 48;
        num = num + remainder * 10;
        
        div = num / 2;
        remainder = num % 2;
        
        unsigned char cha = div + '0';
        result.ubig_value.insert(result.ubig_value.begin(), cha);
    }
    
    while( result.ubig_value.size() > 0
          && result.ubig_value.back() == 48)
    {
        result.ubig_value.pop_back();
    }
    
    this->ubig_value.clear();
    
    for(unsigned int j = 0; j < result.ubig_value.size(); j++)
    {
        int num1 = result.ubig_value[j] - 48;
        unsigned char cha1 = num1 + '0';
        this->ubig_value.push_back(cha1);
    }
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, ubigint divisor) {
   // Note: divisor is modified so pass by value (copy).
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
       //cout << "pow of 2 : " << power_of_2 << endl;
       //cout << "divisor : " << divisor << endl;
       //cout << "remainder : " << remainder << endl;
      if (divisor <= remainder) {
          remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   //return uvalue == that.uvalue;
    int this_size = this->ubig_value.size();
    int that_size = that.ubig_value.size();
    int num1, num2;
    
    if (this_size != that_size)
    {
        return false;
    }
    else
    {
        for (int i = 0; i < this_size; i++)
        {
            num1 = this->ubig_value[i];
            num2 = that.ubig_value[i];
            
            if (num1 != num2)
            {
                return false;
            }
        }
        return true;
    }
}

bool ubigint::operator< (const ubigint& that) const {
   //return uvalue < that.uvalue;
    int this_size = this->ubig_value.size();
    int that_size = that.ubig_value.size();
    int num1, num2;
    
    if (this_size > that_size)
    {
        return false;
    }
    else if (this_size < that_size)
    {
        return true;
    }
    else if (*this == that)
    {
        return false;
    }
    else
    {
        for (int i = 1; i <= this_size; i++)
        {
            num1 = this->ubig_value[this_size - i];
            num2 = that.ubig_value[this_size - i];
            
            if (num1 < num2)
                return true;
            else if (num1 > num2)
                return false;
        }
        
        return false;
    }
}

bool ubigint::operator> (const ubigint& that) const {
    if (*this < that)
        return false;
    else if (*this == that)
        return false;
    else
        return true;
}

bool ubigint::operator<= (const ubigint& that) const {
    if (*this > that)
        return false;
    else
        return true;
}

bool ubigint::operator>= (const ubigint& that) const {
    if (*this < that)
        return false;
    else
        return true;
}

ostream& operator<< (ostream& out, const ubigint& that) { 
   //return out << "ubigint(" << that.uvalue << ")";
    int counter = 0;
    
    for (int digit = that.ubig_value.size() - 1; digit >=0; --digit)
    {
        if (counter == 69)
        {
            out << "\\" << endl;
            counter = 0;
        }
        out << that.ubig_value[digit];
        counter++;
    }
    return out;
}


#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdint.h>
#include "math.h"
#include <iomanip>
#include <algorithm>
#include <chrono>


#define MAX(a,b) (((a) > (b)) ? (a) : (b))
using namespace std;



#define print(a) cout << a << endl;


class BigNumber
{
public:
    int length;
    BigNumber(vector<uint32_t> & );
    BigNumber(uint32_t);
    BigNumber(string, char);
    BigNumber(string);
    void PrintBigNumber();
    vector<uint32_t> MainVector;
};


BigNumber::BigNumber(vector<uint32_t> &vect)
{
  MainVector = vect;
}


BigNumber::BigNumber(uint32_t number)
{
  MainVector.push_back(number);
}




BigNumber::BigNumber(string str, char mode)
{
    int RealSize = str.size();
    int AdjSize;
    string subs;

    if(mode == 'h' && all_of(str.cbegin(), str.cend(), [](char i){ return isxdigit(i); }))
    {
        AdjSize = RealSize / 8;

        MainVector.resize(AdjSize + 1);
        for (int i = AdjSize; i >= 0; i--)
        {
          RealSize = MAX(RealSize - 8, 0);
          subs = str.substr(RealSize);
          MainVector[AdjSize - i] = stoi(subs, nullptr, 0x10);
          str.erase(RealSize);
        }
        return;
    }

    if(mode == 'd' && all_of(str.cbegin(), str.cend(), [](char i){ return isdigit(i); }))
    {
        cout << "convert from decimal string to big number yet to be implemented" << endl;
        return;
    }
    if(mode == 'b' && all_of(str.cbegin(), str.cend(), [](char i){ return i == '0' ||  i == '1'; }))
    {
        AdjSize = RealSize / (8 * 4);
        MainVector.resize(AdjSize + 1);
        for ( int i = AdjSize; i>= 0; i--)
        {
            RealSize = MAX(RealSize - 32, 0);
            subs = str.substr(RealSize);
            MainVector[AdjSize - i] = stol(subs, nullptr, 2);
            str.erase(RealSize);
        }
        return;
    }
}




BigNumber::BigNumber(string str)
{
				BigNumber(str, 'h');
}

void ClearZero(BigNumber* A);


BigNumber LongAdd(BigNumber A, BigNumber B)
{
  uint32_t carry = 0;
  uint64_t temp = 0;
  BigNumber C((uint32_t)0);
  int n = MAX(A.MainVector.size(), B.MainVector.size());
  vector<uint32_t> a(n),b(n);
  a = A.MainVector;
  b = B.MainVector;
  C.MainVector.resize(n+1);
  uint64_t max_val = (uint64_t)pow(2,32);
  for (int i = 0; i < n; i++)
  {
    temp = (uint64_t)a[i] + (uint64_t)b[i] + carry;
    C.MainVector[i] = (uint64_t)temp % max_val;
    carry = temp >> 32;
  }
  return C;
}


BigNumber LongSub(BigNumber A, BigNumber B, uint32_t* borrow)
{
  ClearZero(&A);
  ClearZero(&B);
  uint32_t n = MAX(A.MainVector.size(), B.MainVector.size());
  int64_t temp = 0;
  uint64_t max_val = (uint64_t)pow(2,32);
  BigNumber c((uint32_t)0);
  *borrow = 0;
  c.MainVector.resize(n+1);
  for(int i = 0; i <= n-1 ; i++)
  {
    temp = (uint64_t)A.MainVector[i] - (uint64_t)B.MainVector[i] - *borrow;
    if (temp >= 0)
    {
         c.MainVector[i] = temp;
         *borrow = 0;
    }
    else
    {
         c.MainVector[i] = max_val + temp;
         *borrow = 1;
    }
  }
  return c;
}

int LongCmp(BigNumber A, BigNumber B)
{
     ClearZero(&A);
     ClearZero(&B);
     int i = -1 + MAX(A.MainVector.size(), B.MainVector.size());
     while (A.MainVector[i] == B.MainVector[i])
     {
          i--;
          if (i == -1)
          {
               return 0;
          }
     }
     if (i == -1)
     {
          return 0;
     }
     else
     {
          if(A.MainVector[i] > B.MainVector[i])
          {
               return 1;
          }
          else
          {
               return -1;
          }
     }
}


BigNumber LongMulOneDigit(BigNumber A, uint32_t b)
{
     uint64_t carry = 0;
     uint64_t temp = 0;
     BigNumber C((uint32_t)0);
     int n = A.MainVector.size();
     C.MainVector.resize(n+sizeof(uint32_t));
     uint64_t max_val = pow(2,32);
     for (int i = 0; i <= n-1; i++)
     {
          temp = (uint64_t)A.MainVector[i] * (uint64_t)b + carry;
          C.MainVector[i] = temp & (max_val - 1);
          carry = temp >> 32;
     }
     C.MainVector[n] = carry;
     return C;
}

BigNumber LongShiftDigitsToHigh(BigNumber* in, int shift)
{
				uint64_t temp = 0;
				uint64_t max_val = (uint64_t)pow(2,32);
				ClearZero(in);
				(*in).MainVector.resize((*in).MainVector.size() + (shift/8)+1);
				for (int j = 0; j < shift; j++)
				{
						 for (int i = (*in).MainVector.size() - 1; i >= 0; i--)
						 {
										 temp = ((uint64_t)(*in).MainVector[i]<<1)/max_val;
										 (*in).MainVector[i] = (*in).MainVector[i]<<1;
										 if (temp != 0)
										 {
														 (*in).MainVector[i+1]= (*in).MainVector[i+1] + temp;
										 }
						 }
				}
				vector<uint32_t> VecToReturn = (*in).MainVector;
				return BigNumber(VecToReturn);
}

BigNumber LongShiftDigitsToLow(BigNumber* in, int shift)
{
    uint64_t temp = 0;
    uint64_t max_val = (uint64_t)pow(2, 32);
    ClearZero(in);
    for(int j = 0; j < shift; j++)
    {
        for(int i = 0; i < (*in).MainVector.size(); i++)
        {
            temp = (*in).MainVector[i] & 0x1;
            (*in).MainVector[i] = (*in).MainVector[i]>>1;
            if(i != 0)
            {
                (*in).MainVector[i-1] = (*in).MainVector[i-1] | (temp << 31);
            }


        }
    }
    vector<uint32_t> VecToReturn = (*in).MainVector;
    return BigNumber(VecToReturn);
}


BigNumber LongMul(BigNumber A, BigNumber B)
{
     uint64_t carry = 0;
     BigNumber temp((uint32_t)0);
     BigNumber C((uint32_t)0);
     int n =  B.MainVector.size();
     C.MainVector.resize(A.MainVector.size()+B.MainVector.size());
     temp.MainVector.resize(A.MainVector.size()+B.MainVector.size());
     uint64_t max_val = pow(2,32);
     for(int i = 0; i <= n - 1; i++)
     {
          temp.MainVector = LongMulOneDigit(A,B.MainVector[i]).MainVector;
          temp = LongShiftDigitsToHigh(&temp, i*32);
          C = LongAdd(C, temp);
     }
     return C;
}


void ClearZero(BigNumber *A)
{

     int i = (*A).MainVector.size();
		 if (all_of((*A).MainVector.begin(), (*A).MainVector.end(), [](int i) { return i==0; }))
		 {
						 (*A).MainVector.resize(1);
						 return;
		 }
     while ((*A).MainVector[i-1] == 0)
     {
          i--;
     }
     (*A).MainVector.resize(i);
     return;
}

BigNumber LongPower(BigNumber A, BigNumber B)
{
     BigNumber C((uint32_t)1);
     C.MainVector = A.MainVector;
     uint32_t carry = 0;
     BigNumber temp((uint32_t)1);
     if (LongCmp(B, BigNumber((uint32_t)0)) == 0)
     {
						 return BigNumber((uint32_t)1);
		 }
     while (LongCmp(B,temp) != 0)
     {
          C = LongMul(C, A);
          ClearZero(&C);
          B = LongSub(B, temp, &carry);
     }
     return C;
}


BigNumber LongDivMod(BigNumber A, BigNumber B, BigNumber* R)
{
     uint32_t k = B.MainVector.size();
     uint32_t t;
     BigNumber Q((uint32_t)0), C((uint32_t)0);
     (*R).MainVector = A.MainVector;
     uint32_t DummyBorrow = 0;

     while (LongCmp((*R), B) != -1)
     {
          t = (*R).MainVector.size();
          C = LongShiftDigitsToHigh(&B, t - k);
          if ( LongCmp((*R), C) == -1)
          {
               t--;
               C = LongShiftDigitsToHigh(&B, t - k);
          }
          *R = LongSub((*R), C, &DummyBorrow);
          Q = LongAdd(Q,LongPower(BigNumber((uint32_t)2), BigNumber((uint32_t)(t - k))));
					ClearZero(R);
     }
		 return Q;
}

void BigNumber::PrintBigNumber()
{
  cout << "0x";
  for(int i = MainVector.size(); i > 0; i--){
    cout << setw(8) << setfill('0') << hex << MainVector[i-1];
  }
  cout << endl;
}

int GetHighDigitNumber(BigNumber in)
{
    int size = in.MainVector.size();
    uint32_t LastNumber = in.MainVector.back();
    int i = 0;
    while(LastNumber != 0)
    {
        LastNumber = LastNumber >> 1;
        i++;
    }
    return (size - 1) * 32 + i;
}


int main()
{
  vector<uint32_t> testvec = {0xfffffffa,0x124};
  BigNumber test(testvec);
  vector<uint32_t> testvec2 = { 0x1dab8744, 0x31};
  BigNumber test2(testvec2);
  BigNumber remain(0);
  auto time_start = std::chrono::high_resolution_clock::now();
  auto time_end = std::chrono::high_resolution_clock::now();
  cout << "big number a: ";

  test.PrintBigNumber();
  time_end = std::chrono::high_resolution_clock::now();
  cout << "big number b: ";
  test2.PrintBigNumber();
  uint32_t temp = 0;


  cout << "adding two big numbers: ";
  time_start = std::chrono::high_resolution_clock::now();
  LongAdd(test, test2);
  time_end = std::chrono::high_resolution_clock::now();
  chrono::duration<double, std::milli> ms_double = time_end - time_start;
  LongAdd(test, test2).PrintBigNumber();
  cout << "adding took " << ms_double.count() << "ms\n";


  cout << "substruction big numbers: ";
  time_start = std::chrono::high_resolution_clock::now();
  LongSub(test, test2, &temp);
  time_end = std::chrono::high_resolution_clock::now();
  LongSub(test, test2, &temp).PrintBigNumber();
  ms_double = time_end - time_start;
  cout << "substruction took " << ms_double.count() << "ms\n";





  cout << "dividing big numbers: ";
  time_start = std::chrono::high_resolution_clock::now();
  LongDivMod(test, test2, &remain);
  time_end = std::chrono::high_resolution_clock::now();
  LongDivMod(test, test2, &remain).PrintBigNumber();
  ms_double = time_end - time_start;
  cout << "dividing took " << ms_double.count() << "ms\n";



  cout << "multiplying big numbers: ";
  time_start = std::chrono::high_resolution_clock::now();
  LongMul(test, test2);


  time_end = std::chrono::high_resolution_clock::now();
  LongMul(test, test2).PrintBigNumber();
  ms_double = time_end - time_start;
  cout << "multiplying took " << ms_double.count() << "ms\n";

  BigNumber ToPower(5);
  cout << "raising BigNumber to power: ";
  time_start = std::chrono::high_resolution_clock::now();
  LongPower(test, ToPower);
  time_end = std::chrono::high_resolution_clock::now();
  LongPower(test, ToPower).PrintBigNumber();
  ms_double = time_end - time_start;
  cout << "rasing to power took " << ms_double.count() << "ms\n";

  cout << GetHighDigitNumber(test) << endl;
  return 0;
}

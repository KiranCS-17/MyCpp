#include<iostream>
#include<vector>
#include<algorithm>
#include<iterator>
using namespace std;
const unsigned DECIMAL = 10;
void printVec(std::vector<int> & v)
{
    for(unsigned i = 0;i<v.size();i++)
    {
        cout<<v[i]<<endl;
    }
}
 
void breakNum(unsigned int num,std::vector<int> & v)
{
   unsigned l_num = num;
   unsigned rem =0;
   while( l_num != 0)
   {
      rem = l_num % 10;   
      v.push_back(rem);
      l_num = l_num / 10;
   } 
}
 
unsigned makeNum(std::vector<int> & v)
{
    unsigned num = 0;
    unsigned placeVal = 1;
    std::vector<int>::iterator it = v.begin();
    for(;it!=v.end();it++)
    {
        num = num + (*it) * (placeVal);
        placeVal = placeVal * DECIMAL;
    }
    return num;
}
 
void getAscDscNum(unsigned int inputNum,unsigned int& ascNum,unsigned int& dscNum)
{
  std::vector<int> v;
  breakNum(inputNum,v);
  sort(v.begin(), v.end(), greater<int>());
  ascNum = makeNum(v);
  reverse(v.begin(), v.end());
  dscNum = makeNum(v);
}
 
 
unsigned KaprekarIterations(unsigned int num)
{
    unsigned num_iter = 0;
    unsigned l_num = num;
    unsigned ascNum =0 , dscNum=0 ;
    getAscDscNum(l_num,ascNum,dscNum);
    //cout<<l_num <<"::"<<ascNum<<"::"<<dscNum<<endl;
    while(l_num != 6174)
    {
        l_num = dscNum - ascNum;
	num_iter++;
	cout<<num_iter<<")"<<dscNum<<"-"<<ascNum<<"="<<l_num<<endl;
        getAscDscNum(l_num,ascNum,dscNum);
 
    }
    cout<< "----------------------------------------\n";
    return num_iter;
}
 
int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <unsigned integer>" << std::endl;
        return 1;
    }

    unsigned int number = std::stoul(argv[1]);
    unsigned iterations = KaprekarIterations(number);
    cout << "Number of iterations "<< iterations<<endl;
}

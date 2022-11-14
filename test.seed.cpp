#include <helib/helib.h>
#include <NTL/ZZ.h>
#include <string>
#include <iostream>

using namespace std;
using namespace helib;


string JSONFromSeed(Context& ctx, unsigned char seed[32]){
    NTL::SetSeed(seed,32);
    SecKey sk(ctx);
    sk.GenSecKey();
    ostringstream oss;
    sk.writeOnlySecKeyToJSON(oss);
    return oss.str();
}

int main(int argc, char* argv[])
{
    Context ctx = ContextBuilder<CKKS>().m(256).bits(32).precision(16).c(2).build();
    unsigned char seed[32];
    NTL::GetCurrentRandomStream().get(seed,32);
    string sks1 = JSONFromSeed(ctx,seed);
    string sks2 = JSONFromSeed(ctx,seed);
    assertEq(sks1,sks2,"deterministic key generation failed");
    cout<<sks1<<endl<<sks2<<endl;
}
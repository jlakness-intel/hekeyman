#include <helib/helib.h>
#include <NTL/ZZ.h>
#include <string>
#include <iostream>
#include <immintrin.h>

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
    array<unsigned long long,4> seed;
    for(auto& s:seed){
        int e = _rdrand64_step(&s);
        assertEq(e,1,"random seed generator error: " + to_string(e));
    }
    unsigned char *seed_data = reinterpret_cast<unsigned char *>(seed.data());
    string sks1 = JSONFromSeed(ctx,seed_data);
    string sks2 = JSONFromSeed(ctx,seed_data);
    assertEq(sks1,sks2,"deterministic key generation failed");
    cout<<sks1<<endl<<sks2<<endl;
}
#include <helib/helib.h>
#include <NTL/ZZ.h>
#include <string>
#include <iostream>
#include <random>
#include <immintrin.h>

using namespace std;
using namespace helib;

typedef array<uint32_t,32/sizeof(uint32_t)> Seed;

SecKey genSK(Context& ctx, Seed seed){
    long phim = ctx.getPhiM();
    double bound = sqrt(phim * log(phim) / 2.0);
    zzX poly;
    poly.SetLength(phim);
    seed_seq seq(seed.begin(),seed.end());
    do {
        vector<int32_t> vals(phim,0);
        seq.generate(vals.begin(),vals.end());
        transform(vals.begin(),vals.end(),poly.begin(),[](int32_t v){return v/(INT32_MAX/2);});
    } while (
        embeddingLargestCoeff(poly, ctx.getZMStar()) > bound
    );
    DoubleCRT dcrt(poly, ctx,ctx.getCtxtPrimes()|ctx.getSpecialPrimes());
    SecKey sk(ctx);
    sk.ImportSecKey(dcrt,bound);
    return sk;
}

string SKtoJSON(SecKey& sk){
    ostringstream oss;
    sk.writeOnlySecKeyToJSON(oss);
    return oss.str();
}

int main(int argc, char* argv[])
{
    Context ctx = ContextBuilder<CKKS>().m(256).bits(32).precision(16).c(2).build();
    random_device rd;
    Seed seed;
    for(auto& s:seed) s=rd();
    SecKey sk1 = genSK(ctx,seed);
    SecKey sk2 = genSK(ctx,seed);
    string s1 = SKtoJSON(sk1);
    string s2 = SKtoJSON(sk2);
    assertEq(s1,s2,"deterministic key generation failed");
    cout<<endl<<s1<<endl<<endl<<s2<<endl;
}
#include <helib/helib.h>
#include <NTL/ZZ.h>
#include <string>
#include <iostream>
#include <random>
#include <functional>
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
        transform(vals.begin(),vals.end(),poly.begin(),[](int32_t v)->double{
            return v/(INT32_MAX/2);
            });
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

    vector<double> vals(ctx.getNSlots()),v1,v2;
    generate(vals.begin(),vals.end(),[](){
        return static_cast<double>(rand())/RAND_MAX;
        });
    cout<<vals<<endl;

    PtxtArray pt(ctx,vals),pt1(ctx),pt2(ctx);
    Ctxt ct1(sk1),ct2(sk2);
    pt.encrypt(ct1);
    pt.encrypt(ct2);
    pt1.decrypt(ct1,sk1);
    pt2.decrypt(ct2,sk2);
    pt1.store(v1);
    pt2.store(v2);

    cout<<v1<<endl<<v2<<endl;

    double sse = inner_product(v1.begin(),v1.end(),v2.begin(),0.0,plus(),[](double a,double b)->double {
        double d = a-b;
        return d*d;
    });

    assertTrue(sse < vals.size()*1e-5 , "decrypted arrays do not match");

    return 0;
}




//////////////////////////////////////////////////////////////////////
// Base32.cpp
// (c) Vasian Cepa http://madebits.com
//////////////////////////////////////////////////////////////////////

#include <string.h>


int GetEncode32Length(int bytes)
{
    int bits = bytes * 8;
    int length = bits / 5;
    if((bits % 5) > 0)
    {
        length++;
    }
    return length;
}

int GetDecode32Length(int bytes)
{
    int bits = bytes * 5;
    int length = bits / 8;
    return length;
}

static bool Encode32Block(unsigned char* in5, unsigned char* out8)
{
    // pack 5 bytes
    unsigned long long buffer = 0;
    for(int i = 0; i < 5; i++)
    {
        if(i != 0)
        {
            buffer = (buffer << 8);
        }
        buffer = buffer | in5[i];
    }
    // output 8 bytes
    for(int j = 7; j >= 0; j--)
    {
        buffer = buffer << (24 + (7 - j) * 5);
        buffer = buffer >> (24 + (7 - j) * 5);
        unsigned char c = (unsigned char)(buffer >> (j * 5));
        // self check
        if(c >= 32) return false;
        out8[7 - j] = c;
    }
    return true;
}

bool Encode32(unsigned char* in, int inLen, unsigned char* out)
{
    if((in == 0) || (inLen <= 0) || (out == 0)) return false;
    
    int d = inLen / 5;
    int r = inLen % 5;
    
    unsigned char outBuff[8];
    
    for(int j = 0; j < d; j++)
    {
        if(!Encode32Block(&in[j * 5], &outBuff[0])) return false;
        memmove(&out[j * 8], &outBuff[0], sizeof(unsigned char) * 8);
    }
    
    unsigned char padd[5];
    memset(padd, 0, sizeof(unsigned char) * 5);
    for(int i = 0; i < r; i++)
    {
        padd[i] = in[inLen - r + i];
    }
    if(!Encode32Block(&padd[0], &outBuff[0])) return false;
    memmove(&out[d * 8], &outBuff[0], sizeof(unsigned char) * GetEncode32Length(r));
    
    return true;
}

static bool Decode32Block(unsigned char* in8, unsigned char* out5)
{
    // pack 8 bytes
    unsigned long buffer = 0;
    for(int i = 0; i < 8; i++)
    {
        // input check
        if(in8[i] >= 32) return false;
        if(i != 0)
        {
            buffer = (buffer << 5);
        }
        buffer = buffer | in8[i];
    }
    // output 5 bytes
    for(int j = 4; j >= 0; j--)
    {
        out5[4 - j] = (unsigned char)(buffer >> (j * 8));
    }
    return true;
}

bool Decode32(unsigned char* in, int inLen, unsigned char* out)
{
    if((in == 0) || (inLen <= 0) || (out == 0)) return false;
    
    int d = inLen / 8;
    int r = inLen % 8;
    
    unsigned char outBuff[5];
    
    for(int j = 0; j < d; j++)
    {
        if(!Decode32Block(&in[j * 8], &outBuff[0])) return false;
        memmove(&out[j * 5], &outBuff[0], sizeof(unsigned char) * 5);
    }
    
    unsigned char padd[8];
    memset(padd, 0, sizeof(unsigned char) * 8);
    for(int i = 0; i < r; i++)
    {
        padd[i] = in[inLen - r + i];
    }
    if(!Decode32Block(&padd[0], &outBuff[0])) return false;
    memmove(&out[d * 5], &outBuff[0], sizeof(unsigned char) * GetDecode32Length(r));
    
    return true;
}

bool Map32(unsigned char* inout32, int inout32Len, unsigned char* alpha32)
{
    if((inout32 == 0) || (inout32Len <= 0) || (alpha32 == 0)) return false;
    for(int i = 0; i < inout32Len; i++)
    {
        if(inout32[i] >=32) return false;
        inout32[i] = alpha32[inout32[i]];
    }
    return true;
}

static void ReverseMap(unsigned char* inAlpha32, unsigned char* outMap)
{
    memset(outMap, 0, sizeof(unsigned char) * 256);
    for(int i = 0; i < 32; i++)
    {
        outMap[(int)inAlpha32[i]] = i;
    }
}

bool Unmap32(unsigned char* inout32, int inout32Len, unsigned char* alpha32)
{
    if((inout32 == 0) || (inout32Len <= 0) || (alpha32 == 0)) return false;
    unsigned char rmap[256];
    ReverseMap(alpha32, rmap);
    for(int i = 0; i < inout32Len; i++)
    {
        inout32[i] = rmap[(int)inout32[i]];
    }
    return true;
}





//
//  EccRegister.m
//  EccTest
//
//  Created by Chin ping Hsu on 3/4/15.
//  Copyright (c) 2015 Chin ping Hsu. All rights reserved.
//

#import "EccRegister.h"
#include <openssl/crypto.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/ecdsa.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/rc4.h>
//#include <conio.h>
//#include "Base32.h"



static unsigned char cBlackList[4][64];
//const int	KEYLENGTH = 64;

#define SHORTHASH_BYTES 64
#define SIGNATURE_BYTES (PRIME_BYTES * 2)
#define PRIME_BYTES 14
#define SECRETKEY_BYTES (PRIME_BYTES * 3)
#define PUBLICKEY_BYTES (PRIME_BYTES * 2)
#define  NID NID_secp112r1

const char alphabet[] = "123456789ABCDEFGHJKMNPQRSTUVWXYZ";



static unsigned char s_publicKey_Personal[] =
{
0x1a,0xab,0xcd,0x49,0xbc,0x2d,0x92,0xa1,0xf5,0x23,0x83,0x03,0xc7,0xb0,0x76,0xde,0x5d,0xb5,
0xdf,0x7a,0x8a,0x4b,0xd3,0x21,0xa8,0xa6,0xab,0xba
};




static unsigned char s_publicKey_Com[] =
{
    0x00,0x4a,0x70,0xb6,0xa5,0xf8,0x2d,0x47,0x66,0x52,0x17,0x88,0x34,0x1a,0x94,0xcf,
    0xb8,0x06,0x8c,0x8e,0x95,0x44,0x8c,0x28,0xd3,0xf8,0x6f,0xfc
};



//家庭版
static unsigned char s_publicKey_Fam[] =
{
0x66,0x78,0x6a,0x4b,0x4b,0xa8,0x05,0x31,0x6c,0x67,0x03,0xab,0x58,0x6e,0x2d,0x79,
0x76,0x2c,0x99,0x4e,0xcd,0xf1,0xe4,0x55,0x08,0xfe,0xf3,0xcd
};





static unsigned long long pklen = 28;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int VerifyShortMessage(
                       const unsigned char m[SHORTHASH_BYTES],const unsigned long long mlen,
                       const unsigned char sm[SIGNATURE_BYTES],const unsigned long long smlen,
                       const unsigned char pk[PUBLICKEY_BYTES],const unsigned long long pklen
                       )
{
    //  unsigned char h[20];
    EC_GROUP *group;
    EC_KEY *k;
    EC_POINT *kxy;
    BIGNUM *kx;
    BIGNUM *ky;
    ECDSA_SIG *rs;
    int len;
    
    if (smlen != SIGNATURE_BYTES) return -1;
    if (mlen > SHORTHASH_BYTES) return -1;
    // SHA1(m,mlen,h);
    
    group = EC_GROUP_new_by_curve_name(NID);
    if (!group) return -1;
    
    kx = BN_new(); if (!kx) return -1;
    ky = BN_new(); if (!ky) { BN_free(kx); return -1; }
    kxy = EC_POINT_new(group); if (!kxy) { BN_free(ky); BN_free(kx); return -1; }
    k = EC_KEY_new(); if (!k) { EC_POINT_free(kxy); BN_free(ky); BN_free(kx); return -1; }
    rs = ECDSA_SIG_new(); if (!rs) { EC_KEY_free(k); EC_POINT_free(kxy); BN_free(ky); BN_free(kx); return -1; }
    
    if (!EC_KEY_set_group(k,group)) goto error;
    if (!BN_bin2bn(pk,PRIME_BYTES,kx)) goto error; pk += PRIME_BYTES;
    if (!BN_bin2bn(pk,PRIME_BYTES,ky)) goto error;
#ifdef PRIME_FIELD
    if (!EC_POINT_set_affine_coordinates_GFp(group,kxy,kx,ky,0)) goto error;
#else
    if (!EC_POINT_set_affine_coordinates_GF2m(group,kxy,kx,ky,0)) goto error;
#endif
    if (!EC_KEY_set_public_key(k,kxy)) goto error;
    
    if (!BN_bin2bn(sm,PRIME_BYTES,rs->r)) goto error; sm += PRIME_BYTES;
    if (!BN_bin2bn(sm,PRIME_BYTES,rs->s)) goto error;
    
    len = ECDSA_do_verify(m, 4, rs,k);//h,20,rs,k);
    ECDSA_SIG_free(rs);
    EC_KEY_free(k);
    EC_POINT_free(kxy);
    BN_free(ky);
    BN_free(kx);
    EC_GROUP_clear_free(group);
    
    if (len == 1) return 0;
    if (len == 0) return -100;
    return -1;
    
error:
    ECDSA_SIG_free(rs);
    EC_KEY_free(k);
    EC_POINT_free(kxy);
    BN_free(ky);
    BN_free(kx);
    EC_GROUP_clear_free(group);
    return -1;
}




@implementation EccRegister


+(RegistrationType)CheckRegisterInfo:(NSString*) strRegisterCode
{
    if([strRegisterCode length] == 0)
    {
        return Registration_None;
    }
    
    if([self DoRsaChangeCommon:strRegisterCode])
    {
        return  Registration_Personal;
    }
    else if ([self DoRsaChangeCommercial:strRegisterCode])
    {
        return Registration_Commercial;
    }
    else if ([self DoRsaChangeFamily:strRegisterCode])
    {
        return Registration_Family;
    }
    else
    {
        return Registration_None;
    }
    
    return Registration_None;
    
}




//
+(int)DoRsaChangeCommon:(NSString*) strRegisterCode
{
    int i, n = [strRegisterCode length];//, hexValue

    s_publicKey_Personal[15] = 0xbd;
    s_publicKey_Personal[4] = 0xbe;
    s_publicKey_Personal[20] = 0x7a;
    s_publicKey_Personal[10] = 0x3e;

    unsigned char msg[SHORTHASH_BYTES];
    unsigned long long msglen = 0;
    unsigned char sm[SIGNATURE_BYTES];
    unsigned long long smlen = 0;
    char mappedtext[50];
    int mappedtextlen = 0;
    memset(mappedtext, 0, 50);
    memset(msg,0,SHORTHASH_BYTES);
    memset(sm,0,SIGNATURE_BYTES);

    for(i = 0; i < n; i ++)
    {
        if(i < 18 || i >= 27 )
        {
            mappedtext[mappedtextlen] = [strRegisterCode characterAtIndex:i];
            mappedtextlen ++;
        }
        if(i >= 18 && i < 24)
        {
            msg[msglen] = [strRegisterCode characterAtIndex:i];
            msglen ++;
        }
    }
    Unmap32((unsigned char *)mappedtext, mappedtextlen, (unsigned char *)alphabet);
    Decode32((unsigned char *)mappedtext, mappedtextlen, sm);

    smlen = GetDecode32Length(mappedtextlen);
    if(VerifyShortMessage(msg, msglen, sm, smlen, s_publicKey_Personal, pklen) == 0)
        return 1;
    return 0;
}



//增加企业版
+(int)DoRsaChangeCommercial:(NSString*) strRegisterCode
{
    int i, n = [strRegisterCode length];//, hexValue
    
    
    
    s_publicKey_Com[8] = 0x41;
    s_publicKey_Com[1] = 0x94;
    s_publicKey_Com[5] = 0x1b;
    s_publicKey_Com[20] = 0xc2;
    s_publicKey_Com[12] = 0x12;
    
    
    unsigned char msg[SHORTHASH_BYTES];
    unsigned long long msglen = 0;
    unsigned char sm[SIGNATURE_BYTES];
    unsigned long long smlen = 0;
    char mappedtext[50];
    int mappedtextlen = 0;
    memset(mappedtext, 0, 50);
    memset(msg,0,SHORTHASH_BYTES);
    memset(sm,0,SIGNATURE_BYTES);
    
    for(i = 0; i < n; i ++)
    {
        if(i < 18 || i >= 27 )
        {
            mappedtext[mappedtextlen] = [strRegisterCode characterAtIndex:i];
            mappedtextlen ++;
        }
        if(i >= 18 && i < 24)
        {
            msg[msglen] = [strRegisterCode characterAtIndex:i];
            msglen ++;
        }
    }
    Unmap32((unsigned char *)mappedtext, mappedtextlen, (unsigned char *)alphabet);
    Decode32((unsigned char *)mappedtext, mappedtextlen, sm);
    
    smlen = GetDecode32Length(mappedtextlen);
    if(VerifyShortMessage(msg, msglen, sm, smlen, s_publicKey_Com, pklen) == 0)
        return 1;
    return 0;
}


//增加家庭版
+(int)DoRsaChangeFamily:(NSString*) strRegisterCode
{
    int i, n = [strRegisterCode length];//, hexValue
    
    s_publicKey_Fam[9] = 0xbd;
    s_publicKey_Fam[1] = 0x18;
    s_publicKey_Fam[11] = 0x12;
    s_publicKey_Fam[18] = 0xca;
    
    
    unsigned char msg[SHORTHASH_BYTES];
    unsigned long long msglen = 0;
    unsigned char sm[SIGNATURE_BYTES];
    unsigned long long smlen = 0;
    char mappedtext[50];
    int mappedtextlen = 0;
    memset(mappedtext, 0, 50);
    memset(msg,0,SHORTHASH_BYTES);
    memset(sm,0,SIGNATURE_BYTES);
    
    for(i = 0; i < n; i ++)
    {
        if(i < 18 || i >= 27 )
        {
            mappedtext[mappedtextlen] = [strRegisterCode characterAtIndex:i];
            mappedtextlen ++;
        }
        if(i >= 18 && i < 24)
        {
            msg[msglen] = [strRegisterCode characterAtIndex:i];
            msglen ++;
        }
    }
    Unmap32((unsigned char *)mappedtext, mappedtextlen, (unsigned char *)alphabet);
    Decode32((unsigned char *)mappedtext, mappedtextlen, sm);
    
    smlen = GetDecode32Length(mappedtextlen);
    if(VerifyShortMessage(msg, msglen, sm, smlen, s_publicKey_Fam, pklen) == 0)
        return 1;
    return 0;
}



+(RegistrationType)registration:(NSString *)strRegisterCode
{
    //企业版本跟家庭版本注册
            RegistrationType type = [EccRegister CheckRegisterInfo:[strRegisterCode stringByReplacingOccurrencesOfString:@"-" withString:@""]];
            return type;

}
@end




//c interface
//返回值 对应RegisterVersion枚举类型
//1 未注册
RegistrationType ecc_verify_register_info(char *license)
{
    return [EccRegister registration:[NSString stringWithUTF8String:license]];
}

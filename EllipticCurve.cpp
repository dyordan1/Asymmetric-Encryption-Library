#include "EllipticCurve.h"

namespace AsymmEL
{

char* EllipticCurve::names[6] =
{
	"secp160r1",
	"P-192",
	"secp224r1",
	"P-256",
	"P-384",
	"P-521"
};

int EllipticCurve::sizes[6] = {160,192,224,256,384,521};

char* EllipticCurve::bases[6] =
{
	"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFFFF",
	"fffffffffffffffffffffffffffffffeffffffffffffffff",
	"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000000001",
	"ffffffff00000001000000000000000000000000ffffffffffffffffffffffff",
	"fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000ffffffff",
	"1ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
};

char* EllipticCurve::coefficients[6][3] =
{
	{
		"1",
		"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFFFC",
		"1C97BEFC54BD7A8B65ACF89F81D4D4ADC565FA45"
	},
	{
		"1",
		"fffffffffffffffffffffffffffffffefffffffffffffffc",
		"64210519e59c80e70fa7e9ab72243049feb8deecc146b9b1"
	},
	{
		"1",
		"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFE",
		"B4050A850C04B3ABF54132565044B0B7D7BFD8BA270B39432355FFB4"
	},
	{
		"1",
		"ffffffff00000001000000000000000000000000fffffffffffffffffffffffc",
		"5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b"
	},
	{
		"1",
		"fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000fffffffc",
		"b3312fa7e23ee7e4988e056be3f82d19181d9c6efe8141120314088f5013875ac656398d8a2ed19d2a85c8edd3ec2aef"
	},
	{
		"1",
		"1fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffc",
		"51953eb9618e1c9a1f929a21a0b68540eea2da725b99b315f3b8b489918ef109e156193951ec7e937b1652c0bd3bb1bf073573df883d2c34f1ef451fd46b503f00"
	}
};

char* EllipticCurve::points[6][2] =
{
	{
		"4A96B5688EF573284664698968C38BB913CBFC82",
		"23A628553168947D59DCC912042351377AC5FB32"
	},
	{
		"188da80eb03090f67cbf20eb43a18800f4ff0afd82ff1012",
		"07192b95ffc8da78631011ed6b24cdd573f977a11e794811"
	},
	{
		"B70E0CBD6BB4BF7F321390B94A03C1D356C21122343280D6115C1D21",
		"BD376388B5F723FB4C22DFE6CD4375A05A07476444D5819985007E34"
	},
	{
		"6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296",
		"4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5"
	},
	{
		"aa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a385502f25dbf55296c3a545e3872760ab7",
		"3617de4a96262c6f5d9e98bf9292dc29f8f41dbd289a147ce9da3113b5f0b8c00a60b1ce1d7e819d7a431d7c90ea0e5f"
	},
	{
		"c6858e06b70404e9cd9e3ecb662395b4429c648139053fb521f828af606b4d3dbaa14b5e77efe75928fe1dc127a2ffa8de3348b3c1856a429bf97e7e31c2e5bd66",
		"11839296a789a3bc0045c8a5fb42c7d1bd998f54449579b446817afbd17273e662c97ee72995ef42640c550b9013fad0761353c7086a272c24088be94769fd16650"
	}
};

//end namespace
}

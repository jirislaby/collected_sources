union dbl_ul {
	double dbl;
	struct {
#if (__BYTE_ORDER == __BIG_ENDIAN) || defined(__arm__)
		u32 ms;
		u32 ls;
#else
		u32 ls;
		u32 ms;
#endif
	} ul;
};

#define GET_MSDW(x) ({ union dbl_ul u = { .dbl = (x) }; u.ul.ms; })
#define GET_LSDW(x) ({ union dbl_ul u = { .dbl = (x) }; u.ul.ls; })
#define SET_MSDW(x,y) ({ union dbl_ul u = { .dbl = (x) }; u.ul.ms = (y);u.dbl;})
#define SET_LSDW(x,y) ({ union dbl_ul u = { .dbl = (x) }; u.ul.ls = (y);u.dbl;})
#define DW_TO_DBL(x, y) ({ union dbl_ul u = { .ul = { .ms = x, .ls = y } }; \
		u.dbl; });

inline double fabs(double x)
{
	return ({ union dbl_ul u = { .dbl = x }; u.ul.ms &= 0x7fffffff;u.dbl;});
}

static inline double copysign(double arg2, double arg1)
{
	return SET_MSDW(arg2, (GET_MSDW(arg2) & 0x7fffffff) |
			(GET_MSDW(arg1) & 0x80000000));
}

double scalbn(double x, int n)
{
	static const double
		two54 = 1.80143985094819840000e+16,  // 0x43500000, 0x00000000
		twom54 = 5.55111512312578270212e-17, // 0x3C900000, 0x00000000
		huge = 1.0e+300, tiny = 1.0e-300;

	s32 k, hx, lx;
	hx = GET_MSDW(x);
	lx = GET_LSDW(x);

	k = (hx & 0x7ff00000) >> 20;	/* extract exponent */
	if (k == 0) {		/* 0 or subnormal x */
		if ((lx | (hx & 0x7fffffff)) == 0)
			return x;	/* +-0 */
		x *= two54;
		hx = GET_MSDW(x);
		k = ((hx & 0x7ff00000) >> 20) - 54;
		if (n < -50000)
			return tiny * x;	/*underflow */
	}
	if (k == 0x7ff)
		return x + x;	/* NaN or Inf */
	k = k + n;
	if (k > 0x7fe)
		return huge * copysign(huge, x);	/* overflow  */
	if (k > 0)		/* normal result */
		return SET_MSDW(x, (hx & 0x800fffff) | (k << 20));
	if (k <= -54) {
		if (n > 50000)	/* in case integer overflow in n+k */
			return huge * copysign(huge, x);	/*overflow */
		else
			return tiny * copysign(tiny, x);	/*underflow */
	}

	k += 54;		/* subnormal result */

	return SET_MSDW(x, (hx & 0x800fffff) | (k << 20)) * twom54;
}

double floor(double x)
{
	static const double huge = 1.0e300;
	s32 i0, i1, j0;
	u32 i, j;

	i0 = GET_MSDW(x);
	i1 = GET_LSDW(x);

	j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
	if (j0 < 20) {
		if (j0 < 0) {	/* raise inexact if x != 0 */
			if (huge + x > 0.0) {	/* return 0*sign(x) if |x|<1 */
				if (i0 >= 0)
					i0 = i1 = 0;
				else if (((i0 & 0x7fffffff) | i1) != 0) {
					i0 = 0xbff00000;
					i1 = 0;
				}
			}
		} else {
			i = (0x000fffff) >> j0;
			if (((i0 & i) | i1) == 0)
				return x;	/* x is integral */
			if (huge + x > 0.0) {	/* raise inexact flag */
				if (i0 < 0)
					i0 += (0x00100000) >> j0;
				i0 &= (~i);
				i1 = 0;
			}
		}
	} else if (j0 > 51) {
		if (j0 == 0x400)
			return x + x;	/* inf or NaN */
		else
			return x;	/* x is integral */
	} else {
		i = ((u32) (0xffffffff)) >> (j0 - 20);
		if ((i1 & i) == 0)
			return x;	/* x is integral */
		if (huge + x > 0.0) {	/* raise inexact flag */
			if (i0 < 0) {
				if (j0 == 20)
					i0 += 1;
				else {
					j = i1 + (1 << (52 - j0));
					if (j < i1)
						i0 += 1; /* got a carry */
					i1 = j;
				}
			}
			i1 &= (~i);
		}
	}

	return DW_TO_DBL(i0, i1);
}

static int __rem_pio2(double *x, double *y, int e0, int nx, int prec,
		      const s32 * ipio2)
{
	static const int init_jk[] = { 2, 3, 4, 6 }; /* initial value for jk */

	static const double PIo2[] = {
		1.57079625129699707031e+00,	/* 0x3FF921FB, 0x40000000 */
		7.54978941586159635335e-08,	/* 0x3E74442D, 0x00000000 */
		5.39030252995776476554e-15,	/* 0x3CF84698, 0x80000000 */
		3.28200341580791294123e-22,	/* 0x3B78CC51, 0x60000000 */
		1.27065575308067607349e-29,	/* 0x39F01B83, 0x80000000 */
		1.22933308981111328932e-36,	/* 0x387A2520, 0x40000000 */
		2.73370053816464559624e-44,	/* 0x36E38222, 0x80000000 */
		2.16741683877804819444e-51,	/* 0x3569F31D, 0x00000000 */
	};

	static const double
		zero = 0.0,
		one = 1.0,
		two24 =  1.67772160000000000000e+07, // 0x41700000, 0x00000000
		twon24 = 5.96046447753906250000e-08; // 0x3E700000, 0x00000000

	s32 jz, jx, jv, jp, jk, carry, n, iq[20], i, j, k, m, q0, ih;
	double z, fw, f[20], fq[20], q[20];

	/* initialize jk */
	jk = init_jk[prec];
	jp = jk;

	/* determine jx,jv,q0, note that 3>q0 */
	jx = nx - 1;
	jv = (e0 - 3) / 24;
	if (jv < 0)
		jv = 0;
	q0 = e0 - 24 * (jv + 1);

	/* set up f[0] to f[jx+jk] where f[jx+jk] = ipio2[jv+jk] */
	j = jv - jx;
	m = jx + jk;
	for (i = 0; i <= m; i++, j++)
		f[i] = (j < 0) ? zero : (double)ipio2[j];

	/* compute q[0],q[1],...q[jk] */
	for (i = 0; i <= jk; i++) {
		for (j = 0, fw = 0.0; j <= jx; j++)
			fw += x[j] * f[jx + i - j];
		q[i] = fw;
	}

	jz = jk;
recompute:
	/* distill q[] into iq[] reversingly */
	for (i = 0, j = jz, z = q[jz]; j > 0; i++, j--) {
		fw = (double)((s32) (twon24 * z));
		iq[i] = (s32) (z - two24 * fw);
		z = q[j - 1] + fw;
	}

	/* compute n */
	z = scalbn(z, q0);	/* actual value of z */
	z -= 8.0 * floor(z * 0.125);	/* trim off integer >= 8 */
	n = (s32) z;
	z -= (double)n;
	ih = 0;
	if (q0 > 0) {		/* need iq[jz-1] to determine n */
		i = (iq[jz - 1] >> (24 - q0));
		n += i;
		iq[jz - 1] -= i << (24 - q0);
		ih = iq[jz - 1] >> (23 - q0);
	} else if (q0 == 0)
		ih = iq[jz - 1] >> 23;
	else if (z >= 0.5)
		ih = 2;

	if (ih > 0) {		/* q > 0.5 */
		n += 1;
		carry = 0;
		for (i = 0; i < jz; i++) {	/* compute 1-q */
			j = iq[i];
			if (carry == 0) {
				if (j != 0) {
					carry = 1;
					iq[i] = 0x1000000 - j;
				}
			} else
				iq[i] = 0xffffff - j;
		}
		if (q0 > 0) {	/* rare case: chance is 1 in 12 */
			switch (q0) {
			case 1:
				iq[jz - 1] &= 0x7fffff;
				break;
			case 2:
				iq[jz - 1] &= 0x3fffff;
				break;
			}
		}
		if (ih == 2) {
			z = one - z;
			if (carry != 0)
				z -= scalbn(one, q0);
		}
	}

	/* check if recomputation is needed */
	if (z == zero) {
		j = 0;
		for (i = jz - 1; i >= jk; i--)
			j |= iq[i];
		if (j == 0) {	/* need recomputation */
			for (k = 1; iq[jk - k] == 0; k++)
				;	/* k = no. of terms needed */

			/* add q[jz+1] to q[jz+k] */
			for (i = jz + 1; i <= jz + k; i++) {
				f[jx + i] = (double)ipio2[jv + i];
				for (j = 0, fw = 0.0; j <= jx; j++)
					fw += x[j] * f[jx + i - j];
				q[i] = fw;
			}
			jz += k;
			goto recompute;
		}
	}

	/* chop off zero terms */
	if (z == 0.0) {
		jz -= 1;
		q0 -= 24;
		while (iq[jz] == 0) {
			jz--;
			q0 -= 24;
		}
	} else {		/* break z into 24-bit if necessary */
		z = scalbn(z, -q0);
		if (z >= two24) {
			fw = (double)((s32) (twon24 * z));
			iq[jz] = (s32) (z - two24 * fw);
			jz += 1;
			q0 += 24;
			iq[jz] = (s32) fw;
		} else
			iq[jz] = (s32) z;
	}

	/* convert integer "bit" chunk to floating-point value */
	fw = scalbn(one, q0);
	for (i = jz; i >= 0; i--) {
		q[i] = fw * (double)iq[i];
		fw *= twon24;
	}

	/* compute PIo2[0,...,jp]*q[jz,...,0] */
	for (i = jz; i >= 0; i--) {
		for (fw = 0.0, k = 0; k <= jp && k <= jz - i; k++)
			fw += PIo2[k] * q[i + k];
		fq[jz - i] = fw;
	}

	/* compress fq[] into y[] */
	switch (prec) {
	case 0:
		fw = 0.0;
		for (i = jz; i >= 0; i--)
			fw += fq[i];
		y[0] = (ih == 0) ? fw : -fw;
		break;
	case 1:
	case 2:
		fw = 0.0;
		for (i = jz; i >= 0; i--)
			fw += fq[i];
		y[0] = (ih == 0) ? fw : -fw;
		fw = fq[0] - fw;
		for (i = 1; i <= jz; i++)
			fw += fq[i];
		y[1] = (ih == 0) ? fw : -fw;
		break;
	case 3:		/* painful */
		for (i = jz; i > 0; i--) {
			fw = fq[i - 1] + fq[i];
			fq[i] += fq[i - 1] - fw;
			fq[i - 1] = fw;
		}
		for (i = jz; i > 1; i--) {
			fw = fq[i - 1] + fq[i];
			fq[i] += fq[i - 1] - fw;
			fq[i - 1] = fw;
		}
		for (fw = 0.0, i = jz; i >= 2; i--)
			fw += fq[i];
		if (ih == 0) {
			y[0] = fq[0];
			y[1] = fq[1];
			y[2] = fw;
		} else {
			y[0] = -fq[0];
			y[1] = -fq[1];
			y[2] = -fw;
		}
	}
	return n & 7;
}

s32 rem_pio2(double x, double *y)
{
	static const s32 two_over_pi[] = {
		0xA2F983, 0x6E4E44, 0x1529FC, 0x2757D1, 0xF534DD, 0xC0DB62,
		0x95993C, 0x439041, 0xFE5163, 0xABDEBB, 0xC561B7, 0x246E3A,
		0x424DD2, 0xE00649, 0x2EEA09, 0xD1921C, 0xFE1DEB, 0x1CB129,
		0xA73EE8, 0x8235F5, 0x2EBB44, 0x84E99C, 0x7026B4, 0x5F7E41,
		0x3991D6, 0x398353, 0x39F49C, 0x845F8B, 0xBDF928, 0x3B1FF8,
		0x97FFDE, 0x05980F, 0xEF2F11, 0x8B5A0A, 0x6D1F6D, 0x367ECF,
		0x27CB09, 0xB74F46, 0x3F669E, 0x5FEA2D, 0x7527BA, 0xC7EBE5,
		0xF17B3D, 0x0739F7, 0x8A5292, 0xEA6BFB, 0x5FB11F, 0x8D5D08,
		0x560330, 0x46FC7B, 0x6BABF0, 0xCFBC20, 0x9AF436, 0x1DA9E3,
		0x91615E, 0xE61B08, 0x659985, 0x5F14A0, 0x68408D, 0xFFD880,
		0x4D7327, 0x310606, 0x1556CA, 0x73A8C9, 0x60E27B, 0xC08C6B,
	};

	static const s32 npio2_hw[] = {
		0x3FF921FB, 0x400921FB, 0x4012D97C, 0x401921FB, 0x401F6A7A,
		0x4022D97C, 0x4025FDBB, 0x402921FB, 0x402C463A, 0x402F6A7A,
		0x4031475C, 0x4032D97C, 0x40346B9C, 0x4035FDBB, 0x40378FDB,
		0x403921FB, 0x403AB41B, 0x403C463A, 0x403DD85A, 0x403F6A7A,
		0x40407E4C, 0x4041475C, 0x4042106C, 0x4042D97C, 0x4043A28C,
		0x40446B9C, 0x404534AC, 0x4045FDBB, 0x4046C6CB, 0x40478FDB,
		0x404858EB, 0x404921FB,
	};
	static const double
		zero = 0.00000000000000000000e+00,    // 0x00000000, 0x00000000
		half = 5.00000000000000000000e-01,    // 0x3FE00000, 0x00000000
		two24 = 1.67772160000000000000e+07,   // 0x41700000, 0x00000000
		invpio2 = 6.36619772367581382433e-01, // 0x3FE45F30, 0x6DC9C883
		pio2_1 = 1.57079632673412561417e+00,  // 0x3FF921FB, 0x54400000
		pio2_1t = 6.07710050650619224932e-11, // 0x3DD0B461, 0x1A626331
		pio2_2 = 6.07710050630396597660e-11,  // 0x3DD0B461, 0x1A600000
		pio2_2t = 2.02226624879595063154e-21, // 0x3BA3198A, 0x2E037073
		pio2_3 = 2.02226624871116645580e-21,  // 0x3BA3198A, 0x2E000000
		pio2_3t = 8.47842766036889956997e-32; // 0x397B839A, 0x252049C1

	double z, w, t, r, fn;
	double tx[3];
	s32 e0, i, j, nx, n, ix, hx;

	hx = GET_MSDW(x);	/* high word of x */
	ix = hx & 0x7fffffff;

	if (ix <= 0x3fe921fb) {	/* |x| ~<= pi/4 , no need for reduction */
		y[0] = x;
		y[1] = 0;
		return 0;
	}

	if (ix < 0x4002d97c) {	/* |x| < 3pi/4, special case with n=+-1 */
		if (hx > 0) {
			z = x - pio2_1;
			if (ix != 0x3ff921fb) {	// 33+53 bit pi is good enough
				y[0] = z - pio2_1t;
				y[1] = (z - y[0]) - pio2_1t;
			} else {	/* near pi/2, use 33+33+53 bit pi */
				z -= pio2_2;
				y[0] = z - pio2_2t;
				y[1] = (z - y[0]) - pio2_2t;
			}
			return 1;
		} else {	/* negative x */
			z = x + pio2_1;
			if (ix != 0x3ff921fb) {	// 33+53 bit pi is good enough
				y[0] = z + pio2_1t;
				y[1] = (z - y[0]) + pio2_1t;
			} else {	/* near pi/2, use 33+33+53 bit pi */
				z += pio2_2;
				y[0] = z + pio2_2t;
				y[1] = (z - y[0]) + pio2_2t;
			}
			return -1;
		}
	}

	if (ix <= 0x413921fb) {	/* |x| ~<= 2^19*(pi/2), medium size */
		t = fabs(x);
		n = (s32) (t * invpio2 + half);
		fn = (double)n;
		r = t - fn * pio2_1;
		w = fn * pio2_1t;	/* 1st round good to 85 bit */
		if (n < 32 && ix != npio2_hw[n - 1])
			y[0] = r - w;	/* quick check no cancellation */
		else {
			u32 high;
			j = ix >> 20;
			y[0] = r - w;
			high = GET_MSDW(y[0]);
			i = j - ((high >> 20) & 0x7ff);
			if (i > 16) {	/* 2nd iteration needed, good to 118 */
				t = r;
				w = fn * pio2_2;
				r = t - w;
				w = fn * pio2_2t - ((t - r) - w);
				y[0] = r - w;
				high = GET_MSDW(y[0]);
				i = j - ((high >> 20) & 0x7ff);
				/* 3rd iteration needed, 151 bits acc */
				if (i > 49) {
					t = r; // will cover all possible cases
					w = fn * pio2_3;
					r = t - w;
					w = fn * pio2_3t - ((t - r) - w);
					y[0] = r - w;
				}
			}
		}
		y[1] = (r - y[0]) - w;
		if (hx < 0) {
			y[0] = -y[0];
			y[1] = -y[1];
			return -n;
		} else
			return n;
	}
	/* 
	 * all other (large) arguments
	 */
	if (ix >= 0x7ff00000) {	/* x is inf or NaN */
		y[0] = y[1] = x - x;
		return 0;
	}
	/* set z = scalbn(|x|,ilogb(x)-23) */
	z = SET_LSDW(z, GET_LSDW(x));
	e0 = (ix >> 20) - 1046;	/* e0 = ilogb(z)-23; */
	z = SET_MSDW(z, ix - ((s32) (e0 << 20)));

	for (i = 0; i < 2; i++) {
		tx[i] = (double)((s32) (z));
		z = (z - tx[i]) * two24;
	}

	tx[2] = z;
	nx = 3;

	while (tx[nx - 1] == zero)
		nx--;		/* skip zero term */

	n = __rem_pio2(tx, y, e0, nx, 2, two_over_pi);

	if (hx < 0) {
		y[0] = -y[0];
		y[1] = -y[1];
		return -n;
	}

	return n;
}

double fmod(double x, double y)
{
	static const double one = 1.0, Zero[] = { 0.0, -0.0, };
	int n, hx, hy, hz, ix, iy, sx, i;
	unsigned int lx, ly, lz;

	hx = GET_MSDW(x);
	lx = GET_LSDW(x);
	hy = GET_MSDW(y);
	ly = GET_LSDW(y);
	sx = hx & 0x80000000;	/* sign of x */
	hx ^= sx;		/* |x| */
	hy &= 0x7fffffff;	/* |y| */

	/* purge off exception values */
	if ((hy | ly) == 0 || (hx >= 0x7ff00000) || /* y=0,or x not finite */
	    ((hy | ((ly | -ly) >> 31)) > 0x7ff00000))	/* or y is NaN */
		return (x * y) / (x * y);
	if (hx <= hy) {
		if ((hx < hy) || (lx < ly))
			return x;			/* |x|<|y| return x */
		if (lx == ly)
			return Zero[(u32) sx >> 31];	/* |x|=|y| return x*0 */
	}

	/* determine ix = ilogb(x) */
	if (hx < 0x00100000) {	/* subnormal x */
		if (hx == 0)
			for (ix = -1043, i = lx; i > 0; i <<= 1)
				ix -= 1;
		else
			for (ix = -1022, i = (hx << 11); i > 0; i <<= 1)
				ix -= 1;
	} else
		ix = (hx >> 20) - 1023;

	/* determine iy = ilogb(y) */
	if (hy < 0x00100000) {	/* subnormal y */
		if (hy == 0)
			for (iy = -1043, i = ly; i > 0; i <<= 1)
				iy -= 1;
		else
			for (iy = -1022, i = (hy << 11); i > 0; i <<= 1)
				iy -= 1;
	} else
		iy = (hy >> 20) - 1023;

	/* set up {hx,lx}, {hy,ly} and align y to x */
	if (ix >= -1022)
		hx = 0x00100000 | (0x000fffff & hx);
	else {			/* subnormal x, shift x to normal */
		n = -1022 - ix;
		if (n <= 31) {
			hx = (hx << n) | (lx >> (32 - n));
			lx <<= n;
		} else {
			hx = lx << (n - 32);
			lx = 0;
		}
	}

	if (iy >= -1022)
		hy = 0x00100000 | (0x000fffff & hy);
	else {			/* subnormal y, shift y to normal */
		n = -1022 - iy;
		if (n <= 31) {
			hy = (hy << n) | (ly >> (32 - n));
			ly <<= n;
		} else {
			hy = ly << (n - 32);
			ly = 0;
		}
	}

	/* fix point fmod */
	n = ix - iy;
	while (n--) {
		hz = hx - hy;
		lz = lx - ly;
		if (lx < ly)
			hz -= 1;
		if (hz < 0) {
			hx = hx + hx + (lx >> 31);
			lx = lx + lx;
		} else {
			if ((hz | lz) == 0)	/* return sign(x)*0 */
				return Zero[(u32) sx >> 31];
			hx = hz + hz + (lz >> 31);
			lx = lz + lz;
		}
	}

	hz = hx - hy;
	lz = lx - ly;
	if (lx < ly)
		hz -= 1;

	if (hz >= 0) {
		hx = hz;
		lx = lz;
	}

	/* convert back to floating value and restore the sign */
	if ((hx | lx) == 0)	/* return sign(x)*0 */
		return Zero[(u32) sx >> 31];
	while (hx < 0x00100000) {	/* normalize x */
		hx = hx + hx + (lx >> 31);
		lx = lx + lx;
		iy -= 1;
	}
	if (iy >= -1022) {	/* normalize output */
		hx = ((hx - 0x00100000) | ((iy + 1023) << 20));
		x = DW_TO_DBL(hx | sx, lx);
	} else {		/* subnormal output */
		n = -1022 - iy;
		if (n <= 20) {
			lx = (lx >> n) | ((u32) hx << (32 - n));
			hx >>= n;
		} else if (n <= 31) {
			lx = (hx << (32 - n)) | (lx >> n);
			hx = sx;
		} else {
			lx = hx >> (n - 32);
			hx = sx;
		}
		x = DW_TO_DBL(hx | sx, lx);
		x *= one;	/* create necessary signal */
	}

	return x;
}

static double __sin(double x, double y, int iy)
{
	static double
		half = 5.00000000000000000000e-01,/* 0x3FE00000, 0x00000000 */
		S1 = -1.66666666666666324348e-01, /* 0xBFC55555, 0x55555549 */
		S2 =  8.33333333332248946124e-03, /* 0x3F811111, 0x1110F8A6 */
		S3 = -1.98412698298579493134e-04, /* 0xBF2A01A0, 0x19C161D5 */
		S4 =  2.75573137070700676789e-06, /* 0x3EC71DE3, 0x57B1FE7D */
		S5 = -2.50507602534068634195e-08, /* 0xBE5AE5E6, 0x8A2B9CEB */
		S6 =  1.58969099521155010221e-10; /* 0x3DE5D93A, 0x5ACFD57C */

	double z, r, v;
	int ix = GET_MSDW(x);

	ix &= 0x7fffffff;			/* high word of x */
	if (ix < 0x3e400000 && (int)x == 0)	/* |x| < 2**-27 */
		return x;			/* generate inexact */

	z = x * x;
	v = z * x;
	r = S2 + z * (S3 + z * (S4 + z * (S5 + z * S6)));

	if (!iy)
		return x + v * (S1 + z * r);
	else
		return x - ((z * (half * y - v * r) - y) - v * S1);
}

static double __cos(double x, double y)
{
	static const double
		one = 1.00000000000000000000e+00, /* 0x3FF00000, 0x00000000 */
		C1 =  4.16666666666666019037e-02, /* 0x3FA55555, 0x5555554C */
		C2 = -1.38888888888741095749e-03, /* 0xBF56C16C, 0x16C15177 */
		C3 =  2.48015872894767294178e-05, /* 0x3EFA01A0, 0x19CB1590 */
		C4 = -2.75573143513906633035e-07, /* 0xBE927E4F, 0x809C52AD */
		C5 =  2.08757232129817482790e-09, /* 0x3E21EE9E, 0xBDB4B1C4 */
		C6 = -1.13596475577881948265e-11; /* 0xBDA8FAE9, 0xBE8838D4 */

	double a, hz, z, r, qx;
	int ix = GET_MSDW(x);

	ix &= 0x7fffffff;			/* ix = |x|'s high word */
	if (ix < 0x3e400000 && (int)x == 0)	/* if x < 2**27 */
		return one;			/* generate inexact */

	z = x * x;
	r = z * (C1 + z * (C2 + z * (C3 + z * (C4 + z * (C5 + z * C6)))));
	if (ix < 0x3FD33333)			/* if |x| < 0.3 */
		return one - (0.5 * z - (z * r - x * y));
	else {
		if (ix > 0x3fe90000)		/* x > 0.78125 */
			qx = 0.28125;
		else
			qx = DW_TO_DBL(ix - 0x00200000, 0);	/* x/4 */

		hz = 0.5 * z - qx;
		a = one - qx;

		return a - (hz - (z * r - x * y));
	}
}

double sin(double x)
{
	double y[2], z = 0.0;
	int ix;

	ix = GET_MSDW(x);

	ix &= 0x7fffffff;
	if (ix <= 0x3fe921fb)
		return __sin(x, z, 0);
	else if (ix >= 0x7ff00000)
		return x - x;
	else {
		int n = rem_pio2(x, y);
		switch (n & 3) {
		case 0:
			return __sin(y[0], y[1], 1);
		case 1:
			return __cos(y[0], y[1]);
		case 2:
			return -__sin(y[0], y[1], 1);
		default:
			return -__cos(y[0], y[1]);
		}
	}
}

double cos(double x)
{
	double y[2], z = 0.0;
	s32 ix = GET_MSDW(x);

	/* |x| ~< pi/4 */
	ix &= 0x7fffffff;
	if (ix <= 0x3fe921fb)
		return __cos(x, z);
	/* cos(Inf or NaN) is NaN */
	else if (ix >= 0x7ff00000)
		return x - x;
	/* argument reduction needed */
	else {
		s32 n = rem_pio2(x, y);
		switch (n & 3) {
		case 0:
			return __cos(y[0], y[1]);
		case 1:
			return -__sin(y[0], y[1], 1);
		case 2:
			return -__cos(y[0], y[1]);
		default:
			return __sin(y[0], y[1], 1);
		}
	}
}

int main()
{
	double a;

	for (a = 0.0; a < 3 * M_PI; a += 0.1)
		printf("sin(%.3f)=%14.10f\n", a, sin(a));

	return 0;
}

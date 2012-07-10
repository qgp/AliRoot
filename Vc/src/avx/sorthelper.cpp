/*  This file is part of the Vc library.

    Copyright (C) 2011 Matthias Kretz <kretz@kde.org>

    Vc is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    Vc is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Vc.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <avx/intrinsics.h>
#include <avx/casts.h>
#include <avx/sorthelper.h>
#include <avx/macros.h>

namespace Vc
{
namespace AVX
{

template<> __m128i SortHelper<short>::sort(__m128i x)
{
    __m128i lo, hi, y;
    // sort pairs
    y = _mm_shufflelo_epi16(_mm_shufflehi_epi16(x, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
    lo = _mm_min_epi16(x, y);
    hi = _mm_max_epi16(x, y);
    x = _mm_blend_epi16(lo, hi, 0xaa);

    // merge left and right quads
    y = _mm_shufflelo_epi16(_mm_shufflehi_epi16(x, _MM_SHUFFLE(0, 1, 2, 3)), _MM_SHUFFLE(0, 1, 2, 3));
    lo = _mm_min_epi16(x, y);
    hi = _mm_max_epi16(x, y);
    x = _mm_blend_epi16(lo, hi, 0xcc);
    y = _mm_srli_si128(x, 2);
    lo = _mm_min_epi16(x, y);
    hi = _mm_max_epi16(x, y);
    x = _mm_blend_epi16(lo, _mm_slli_si128(hi, 2), 0xaa);

    // merge quads into octs
    y = _mm_shuffle_epi32(x, _MM_SHUFFLE(1, 0, 3, 2));
    y = _mm_shufflelo_epi16(y, _MM_SHUFFLE(0, 1, 2, 3));
    lo = _mm_min_epi16(x, y);
    hi = _mm_max_epi16(x, y);

    x = _mm_unpacklo_epi16(lo, hi);
    y = _mm_srli_si128(x, 8);
    lo = _mm_min_epi16(x, y);
    hi = _mm_max_epi16(x, y);

    x = _mm_unpacklo_epi16(lo, hi);
    y = _mm_srli_si128(x, 8);
    lo = _mm_min_epi16(x, y);
    hi = _mm_max_epi16(x, y);

    return _mm_unpacklo_epi16(lo, hi);
}
template<> __m128i SortHelper<unsigned short>::sort(__m128i x)
{
    __m128i lo, hi, y;
    // sort pairs
    y = _mm_shufflelo_epi16(_mm_shufflehi_epi16(x, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
    lo = _mm_min_epu16(x, y);
    hi = _mm_max_epu16(x, y);
    x = _mm_blend_epi16(lo, hi, 0xaa);

    // merge left and right quads
    y = _mm_shufflelo_epi16(_mm_shufflehi_epi16(x, _MM_SHUFFLE(0, 1, 2, 3)), _MM_SHUFFLE(0, 1, 2, 3));
    lo = _mm_min_epu16(x, y);
    hi = _mm_max_epu16(x, y);
    x = _mm_blend_epi16(lo, hi, 0xcc);
    y = _mm_srli_si128(x, 2);
    lo = _mm_min_epu16(x, y);
    hi = _mm_max_epu16(x, y);
    x = _mm_blend_epi16(lo, _mm_slli_si128(hi, 2), 0xaa);

    // merge quads into octs
    y = _mm_shuffle_epi32(x, _MM_SHUFFLE(1, 0, 3, 2));
    y = _mm_shufflelo_epi16(y, _MM_SHUFFLE(0, 1, 2, 3));
    lo = _mm_min_epu16(x, y);
    hi = _mm_max_epu16(x, y);

    x = _mm_unpacklo_epi16(lo, hi);
    y = _mm_srli_si128(x, 8);
    lo = _mm_min_epu16(x, y);
    hi = _mm_max_epu16(x, y);

    x = _mm_unpacklo_epi16(lo, hi);
    y = _mm_srli_si128(x, 8);
    lo = _mm_min_epu16(x, y);
    hi = _mm_max_epu16(x, y);

    return _mm_unpacklo_epi16(lo, hi);
}

template<> __m256i SortHelper<int>::sort(__m256i hgfedcba)
{
    const __m128i hgfe = hi128(hgfedcba);
    const __m128i dcba = lo128(hgfedcba);
    __m128i l = _mm_min_epi32(hgfe, dcba); // ↓hd ↓gc ↓fb ↓ea
    __m128i h = _mm_max_epi32(hgfe, dcba); // ↑hd ↑gc ↑fb ↑ea

    __m128i x = _mm_unpacklo_epi32(l, h); // ↑fb ↓fb ↑ea ↓ea
    __m128i y = _mm_unpackhi_epi32(l, h); // ↑hd ↓hd ↑gc ↓gc

    l = _mm_min_epi32(x, y); // ↓(↑fb,↑hd) ↓hfdb ↓(↑ea,↑gc) ↓geca
    h = _mm_max_epi32(x, y); // ↑hfdb ↑(↓fb,↓hd) ↑geca ↑(↓ea,↓gc)

    x = _mm_min_epi32(l, Reg::permute<X2, X2, X0, X0>(h)); // 2(hfdb) 1(hfdb) 2(geca) 1(geca)
    y = _mm_max_epi32(h, Reg::permute<X3, X3, X1, X1>(l)); // 4(hfdb) 3(hfdb) 4(geca) 3(geca)

    __m128i b = Reg::shuffle<Y0, Y1, X0, X1>(y, x); // b3 <= b2 <= b1 <= b0
    __m128i a = _mm_unpackhi_epi64(x, y);           // a3 >= a2 >= a1 >= a0

    if (VC_IS_UNLIKELY(_mm_extract_epi32(x, 2) >= _mm_extract_epi32(y, 1))) {
        return concat(Reg::permute<X0, X1, X2, X3>(b), a);
    } else if (VC_IS_UNLIKELY(_mm_extract_epi32(x, 0) >= _mm_extract_epi32(y, 3))) {
        return concat(a, Reg::permute<X0, X1, X2, X3>(b));
    }

    // merge
    l = _mm_min_epi32(a, b); // ↓a3b3 ↓a2b2 ↓a1b1 ↓a0b0
    h = _mm_max_epi32(a, b); // ↑a3b3 ↑a2b2 ↑a1b1 ↑a0b0

    a = _mm_unpacklo_epi32(l, h); // ↑a1b1 ↓a1b1 ↑a0b0 ↓a0b0
    b = _mm_unpackhi_epi32(l, h); // ↑a3b3 ↓a3b3 ↑a2b2 ↓a2b2
    l = _mm_min_epi32(a, b);      // ↓(↑a1b1,↑a3b3) ↓a1b3 ↓(↑a0b0,↑a2b2) ↓a0b2
    h = _mm_max_epi32(a, b);      // ↑a3b1 ↑(↓a1b1,↓a3b3) ↑a2b0 ↑(↓a0b0,↓a2b2)

    a = _mm_unpacklo_epi32(l, h); // ↑a2b0 ↓(↑a0b0,↑a2b2) ↑(↓a0b0,↓a2b2) ↓a0b2
    b = _mm_unpackhi_epi32(l, h); // ↑a3b1 ↓(↑a1b1,↑a3b3) ↑(↓a1b1,↓a3b3) ↓a1b3
    l = _mm_min_epi32(a, b); // ↓(↑a2b0,↑a3b1) ↓(↑a0b0,↑a2b2,↑a1b1,↑a3b3) ↓(↑(↓a0b0,↓a2b2) ↑(↓a1b1,↓a3b3)) ↓a0b3
    h = _mm_max_epi32(a, b); // ↑a3b0 ↑(↓(↑a0b0,↑a2b2) ↓(↑a1b1,↑a3b3)) ↑(↓a0b0,↓a2b2,↓a1b1,↓a3b3) ↑(↓a0b2,↓a1b3)

    return concat(_mm_unpacklo_epi32(l, h), _mm_unpackhi_epi32(l, h));
}

template<> __m256i SortHelper<unsigned int>::sort(__m256i hgfedcba)
{
    const __m128i hgfe = hi128(hgfedcba);
    const __m128i dcba = lo128(hgfedcba);
    __m128i l = _mm_min_epu32(hgfe, dcba); // ↓hd ↓gc ↓fb ↓ea
    __m128i h = _mm_max_epu32(hgfe, dcba); // ↑hd ↑gc ↑fb ↑ea

    __m128i x = _mm_unpacklo_epi32(l, h); // ↑fb ↓fb ↑ea ↓ea
    __m128i y = _mm_unpackhi_epi32(l, h); // ↑hd ↓hd ↑gc ↓gc

    l = _mm_min_epu32(x, y); // ↓(↑fb,↑hd) ↓hfdb ↓(↑ea,↑gc) ↓geca
    h = _mm_max_epu32(x, y); // ↑hfdb ↑(↓fb,↓hd) ↑geca ↑(↓ea,↓gc)

    x = _mm_min_epu32(l, Reg::permute<X2, X2, X0, X0>(h)); // 2(hfdb) 1(hfdb) 2(geca) 1(geca)
    y = _mm_max_epu32(h, Reg::permute<X3, X3, X1, X1>(l)); // 4(hfdb) 3(hfdb) 4(geca) 3(geca)

    __m128i b = Reg::shuffle<Y0, Y1, X0, X1>(y, x); // b3 <= b2 <= b1 <= b0
    __m128i a = _mm_unpackhi_epi64(x, y);           // a3 >= a2 >= a1 >= a0

    if (VC_IS_UNLIKELY(_mm_extract_epu32(x, 2) >= _mm_extract_epu32(y, 1))) {
        return concat(Reg::permute<X0, X1, X2, X3>(b), a);
    } else if (VC_IS_UNLIKELY(_mm_extract_epu32(x, 0) >= _mm_extract_epu32(y, 3))) {
        return concat(a, Reg::permute<X0, X1, X2, X3>(b));
    }

    // merge
    l = _mm_min_epu32(a, b); // ↓a3b3 ↓a2b2 ↓a1b1 ↓a0b0
    h = _mm_max_epu32(a, b); // ↑a3b3 ↑a2b2 ↑a1b1 ↑a0b0

    a = _mm_unpacklo_epi32(l, h); // ↑a1b1 ↓a1b1 ↑a0b0 ↓a0b0
    b = _mm_unpackhi_epi32(l, h); // ↑a3b3 ↓a3b3 ↑a2b2 ↓a2b2
    l = _mm_min_epu32(a, b);      // ↓(↑a1b1,↑a3b3) ↓a1b3 ↓(↑a0b0,↑a2b2) ↓a0b2
    h = _mm_max_epu32(a, b);      // ↑a3b1 ↑(↓a1b1,↓a3b3) ↑a2b0 ↑(↓a0b0,↓a2b2)

    a = _mm_unpacklo_epi32(l, h); // ↑a2b0 ↓(↑a0b0,↑a2b2) ↑(↓a0b0,↓a2b2) ↓a0b2
    b = _mm_unpackhi_epi32(l, h); // ↑a3b1 ↓(↑a1b1,↑a3b3) ↑(↓a1b1,↓a3b3) ↓a1b3
    l = _mm_min_epu32(a, b); // ↓(↑a2b0,↑a3b1) ↓(↑a0b0,↑a2b2,↑a1b1,↑a3b3) ↓(↑(↓a0b0,↓a2b2) ↑(↓a1b1,↓a3b3)) ↓a0b3
    h = _mm_max_epu32(a, b); // ↑a3b0 ↑(↓(↑a0b0,↑a2b2) ↓(↑a1b1,↑a3b3)) ↑(↓a0b0,↓a2b2,↓a1b1,↓a3b3) ↑(↓a0b2,↓a1b3)

    return concat(_mm_unpacklo_epi32(l, h), _mm_unpackhi_epi32(l, h));
}

template<> __m256 SortHelper<float>::sort(__m256 hgfedcba)
{
    const __m128 hgfe = hi128(hgfedcba);
    const __m128 dcba = lo128(hgfedcba);
    __m128 l = _mm_min_ps(hgfe, dcba); // ↓hd ↓gc ↓fb ↓ea
    __m128 h = _mm_max_ps(hgfe, dcba); // ↑hd ↑gc ↑fb ↑ea

    __m128 x = _mm_unpacklo_ps(l, h); // ↑fb ↓fb ↑ea ↓ea
    __m128 y = _mm_unpackhi_ps(l, h); // ↑hd ↓hd ↑gc ↓gc

    l = _mm_min_ps(x, y); // ↓(↑fb,↑hd) ↓hfdb ↓(↑ea,↑gc) ↓geca
    h = _mm_max_ps(x, y); // ↑hfdb ↑(↓fb,↓hd) ↑geca ↑(↓ea,↓gc)

    x = _mm_min_ps(l, Reg::permute<X2, X2, X0, X0>(h)); // 2(hfdb) 1(hfdb) 2(geca) 1(geca)
    y = _mm_max_ps(h, Reg::permute<X3, X3, X1, X1>(l)); // 4(hfdb) 3(hfdb) 4(geca) 3(geca)

    __m128 a = _mm_castpd_ps(_mm_unpackhi_pd(_mm_castps_pd(x), _mm_castps_pd(y))); // a3 >= a2 >= a1 >= a0
    __m128 b = Reg::shuffle<Y0, Y1, X0, X1>(y, x); // b3 <= b2 <= b1 <= b0

    // merge
    l = _mm_min_ps(a, b); // ↓a3b3 ↓a2b2 ↓a1b1 ↓a0b0
    h = _mm_max_ps(a, b); // ↑a3b3 ↑a2b2 ↑a1b1 ↑a0b0

    a = _mm_unpacklo_ps(l, h); // ↑a1b1 ↓a1b1 ↑a0b0 ↓a0b0
    b = _mm_unpackhi_ps(l, h); // ↑a3b3 ↓a3b3 ↑a2b2 ↓a2b2
    l = _mm_min_ps(a, b);      // ↓(↑a1b1,↑a3b3) ↓a1b3 ↓(↑a0b0,↑a2b2) ↓a0b2
    h = _mm_max_ps(a, b);      // ↑a3b1 ↑(↓a1b1,↓a3b3) ↑a2b0 ↑(↓a0b0,↓a2b2)

    a = _mm_unpacklo_ps(l, h); // ↑a2b0 ↓(↑a0b0,↑a2b2) ↑(↓a0b0,↓a2b2) ↓a0b2
    b = _mm_unpackhi_ps(l, h); // ↑a3b1 ↓(↑a1b1,↑a3b3) ↑(↓a1b1,↓a3b3) ↓a1b3
    l = _mm_min_ps(a, b); // ↓(↑a2b0,↑a3b1) ↓(↑a0b0,↑a2b2,↑a1b1,↑a3b3) ↓(↑(↓a0b0,↓a2b2) ↑(↓a1b1,↓a3b3)) ↓a0b3
    h = _mm_max_ps(a, b); // ↑a3b0 ↑(↓(↑a0b0,↑a2b2) ↓(↑a1b1,↑a3b3)) ↑(↓a0b0,↓a2b2,↓a1b1,↓a3b3) ↑(↓a0b2,↓a1b3)

    return concat(_mm_unpacklo_ps(l, h), _mm_unpackhi_ps(l, h));
}

template<> __m256 SortHelper<sfloat>::sort(__m256 hgfedcba)
{
    return SortHelper<float>::sort(hgfedcba);
}

template<> void SortHelper<double>::sort(__m256d &VC_RESTRICT x, __m256d &VC_RESTRICT y)
{
    __m256d l = _mm256_min_pd(x, y); // ↓x3y3 ↓x2y2 ↓x1y1 ↓x0y0
    __m256d h = _mm256_max_pd(x, y); // ↑x3y3 ↑x2y2 ↑x1y1 ↑x0y0
    x = _mm256_unpacklo_pd(l, h); // ↑x2y2 ↓x2y2 ↑x0y0 ↓x0y0
    y = _mm256_unpackhi_pd(l, h); // ↑x3y3 ↓x3y3 ↑x1y1 ↓x1y1
    l = _mm256_min_pd(x, y); // ↓(↑x2y2,↑x3y3) ↓x3x2y3y2 ↓(↑x0y0,↑x1y1) ↓x1x0y1y0
    h = _mm256_max_pd(x, y); // ↑x3x2y3y2 ↑(↓x2y2,↓x3y3) ↑x1x0y1y0 ↑(↓x0y0,↓x1y1)
    x = _mm256_unpacklo_pd(l, h); // ↑(↓x2y2,↓x3y3) ↓x3x2y3y2 ↑(↓x0y0,↓x1y1) ↓x1x0y1y0
    y = _mm256_unpackhi_pd(h, l); // ↓(↑x2y2,↑x3y3) ↑x3x2y3y2 ↓(↑x0y0,↑x1y1) ↑x1x0y1y0
    l = _mm256_min_pd(x, y); // ↓(↑(↓x2y2,↓x3y3) ↓(↑x2y2,↑x3y3)) ↓x3x2y3y2 ↓(↑(↓x0y0,↓x1y1) ↓(↑x0y0,↑x1y1)) ↓x1x0y1y0
    h = _mm256_max_pd(x, y); // ↑(↑(↓x2y2,↓x3y3) ↓(↑x2y2,↑x3y3)) ↑x3x2y3y2 ↑(↑(↓x0y0,↓x1y1) ↓(↑x0y0,↑x1y1)) ↑x1x0y1y0
    __m256d a = Reg::permute<X2, X3, X1, X0>(Reg::permute128<X0, X1>(h, h)); // h0 h1 h3 h2
    __m256d b = Reg::permute<X2, X3, X1, X0>(l);                             // l2 l3 l1 l0

    // a3 >= a2 >= b1 >= b0
    // b3 <= b2 <= a1 <= a0

    // merge
    l = _mm256_min_pd(a, b); // ↓a3b3 ↓a2b2 ↓a1b1 ↓a0b0
    h = _mm256_min_pd(a, b); // ↑a3b3 ↑a2b2 ↑a1b1 ↑a0b0

    x = _mm256_unpacklo_pd(l, h); // ↑a2b2 ↓a2b2 ↑a0b0 ↓a0b0
    y = _mm256_unpackhi_pd(l, h); // ↑a3b3 ↓a3b3 ↑a1b1 ↓a1b1
    l = _mm256_min_pd(x, y);      // ↓(↑a2b2,↑a3b3) ↓a2b3 ↓(↑a0b0,↑a1b1) ↓a1b0
    h = _mm256_min_pd(x, y);      // ↑a3b2 ↑(↓a2b2,↓a3b3) ↑a0b1 ↑(↓a0b0,↓a1b1)

    x = Reg::permute128<Y0, X0>(l, h); // ↑a0b1 ↑(↓a0b0,↓a1b1) ↓(↑a0b0,↑a1b1) ↓a1b0
    y = Reg::permute128<Y1, X1>(l, h); // ↑a3b2 ↑(↓a2b2,↓a3b3) ↓(↑a2b2,↑a3b3) ↓a2b3
    l = _mm256_min_pd(x, y);      // ↓(↑a0b1,↑a3b2) ↓(↑(↓a0b0,↓a1b1) ↑(↓a2b2,↓a3b3)) ↓(↑a0b0,↑a1b1,↑a2b2,↑a3b3) ↓b0b3
    h = _mm256_min_pd(x, y);      // ↑a0a3 ↑(↓a0b0,↓a1b1,↓a2b2,↓a3b3) ↑(↓(↑a0b0,↑a1b1) ↓(↑a2b2,↑a3b3)) ↑(↓a1b0,↓a2b3)

    x = _mm256_unpacklo_pd(l, h); // h2 l2 h0 l0
    y = _mm256_unpackhi_pd(l, h); // h3 l3 h1 l1
}
template<> __m256d SortHelper<double>::sort(__m256d dcba)
{
    /*
     * to find the second largest number find
     * max(min(max(ab),max(cd)), min(max(ad),max(bc)))
     *  or
     * max(max(min(ab),min(cd)), min(max(ab),max(cd)))
     *
    const __m256d adcb = avx_cast<__m256d>(concat(_mm_alignr_epi8(avx_cast<__m128i>(dc), avx_cast<__m128i>(ba), 8), _mm_alignr_epi8(avx_cast<__m128i>(ba), avx_cast<__m128i>(dc), 8)));
    const __m256d l = _mm256_min_pd(dcba, adcb); // min(ad cd bc ab)
    const __m256d h = _mm256_max_pd(dcba, adcb); // max(ad cd bc ab)
    // max(h3, h1)
    // max(min(h0,h2), min(h3,h1))
    // min(max(l0,l2), max(l3,l1))
    // min(l3, l1)

    const __m256d ll = _mm256_min_pd(h, Reg::permute128<X0, X1>(h, h)); // min(h3h1 h2h0 h1h3 h0h2)
    //const __m256d hh = _mm256_max_pd(h3 ll1_3 l1 l0, h1 ll0_2 l3 l2);
    const __m256d hh = _mm256_max_pd(
            Reg::permute128<X1, Y0>(_mm256_unpackhi_pd(ll, h), l),
            Reg::permute128<X0, Y1>(_mm256_blend_pd(h ll, 0x1), l));
    _mm256_min_pd(hh0, hh1
     */

    //////////////////////////////////////////////////////////////////////////////////
    // max(max(ac), max(bd))
    // max(max(min(ac),min(bd)), min(max(ac),max(bd)))
    // min(max(min(ac),min(bd)), min(max(ac),max(bd)))
    // min(min(ac), min(bd))
    __m128d l = _mm_min_pd(lo128(dcba), hi128(dcba)); // min(bd) min(ac)
    __m128d h = _mm_max_pd(lo128(dcba), hi128(dcba)); // max(bd) max(ac)
    __m128d h0_l0 = _mm_unpacklo_pd(l, h);
    __m128d h1_l1 = _mm_unpackhi_pd(l, h);
    l = _mm_min_pd(h0_l0, h1_l1);
    h = _mm_max_pd(h0_l0, h1_l1);
    return concat(
        _mm_min_pd(l, Reg::permute<X0, X0>(h)),
        _mm_max_pd(h, Reg::permute<X1, X1>(l))
            );
    // extract: 1 cycle
    // min/max: 4 cycles
    // unpacklo/hi: 2 cycles
    // min/max: 4 cycles
    // permute: 1 cycle
    // min/max: 4 cycles
    // insert:  1 cycle
    // ----------------------
    // total:   17 cycles

    /*
    __m256d cdab = Reg::permute<X2, X3, X0, X1>(dcba);
    __m256d l = _mm256_min_pd(dcba, cdab);
    __m256d h = _mm256_max_pd(dcba, cdab);
    __m256d maxmin_ba = Reg::permute128<X0, Y0>(l, h);
    __m256d maxmin_dc = Reg::permute128<X1, Y1>(l, h);

    l = _mm256_min_pd(maxmin_ba, maxmin_dc);
    h = _mm256_max_pd(maxmin_ba, maxmin_dc);

    return _mm256_blend_pd(h, l, 0x55);
    */

    /*
    // a b c d
    // b a d c
    // sort pairs
    __m256d y, l, h;
    __m128d l2, h2;
    y = shuffle<X1, Y0, X3, Y2>(x, x);
    l = _mm256_min_pd(x, y); // min[ab ab cd cd]
    h = _mm256_max_pd(x, y); // max[ab ab cd cd]

    // 1 of 2 is at [0]
    // 1 of 4 is at [1]
    // 1 of 4 is at [2]
    // 1 of 2 is at [3]

    // don't be fooled by unpack here. It works differently for AVX pd than for SSE ps
    x = _mm256_unpacklo_pd(l, h); // l_ab h_ab l_cd h_cd
    l2 = _mm_min_pd(lo128(x), hi128(x)); // l_abcd l(h_ab hcd)
    h2 = _mm_max_pd(lo128(x), hi128(x)); // h(l_ab l_cd) h_abcd

    // either it is:
    return concat(l2, h2);
    // or:
    // concat(_mm_unpacklo_pd(l2, h2), _mm_unpackhi_pd(l2, h2));

    // I'd like to have four useful compares
    const __m128d dc = hi128(dcba);
    const __m128d ba = lo128(dcba);
    const __m256d adcb = avx_cast<__m256d>(concat(_mm_alignr_epi8(avx_cast<__m128i>(dc), avx_cast<__m128i>(ba), 8), _mm_alignr_epi8(avx_cast<__m128i>(ba), avx_cast<__m128i>(dc), 8)));

    const int extraCmp = _mm_movemask_pd(_mm_cmpgt_pd(dc, ba));
    // 0x0: d <= b && c <= a
    // 0x1: d <= b && c >  a
    // 0x2: d >  b && c <= a
    // 0x3: d >  b && c >  a

    switch (_mm256_movemask_pd(_mm256_cmpgt_pd(dcba, adcb))) {
    // impossible: 0x0, 0xf
    case 0x1: // a <= b && b <= c && c <= d && d >  a
        // abcd
        return Reg::permute<X2, X3, X0, X1>(Reg::permute<X0, X1>(dcba, dcba));
    case 0x2: // a <= b && b <= c && c >  d && d <= a
        // dabc
        return Reg::permute<X2, X3, X0, X1>(adcb);
    case 0x3: // a <= b && b <= c && c >  d && d >  a
        // a[bd]c
        if (extraCmp & 2) {
            // abdc
            return Reg::permute<X2, X3, X1, X0>(Reg::permute<X0, X1>(dcba, dcba));
        } else {
            // adbc
            return Reg::permute<X3, X2, X0, X1>(adcb);
        }
    case 0x4: // a <= b && b >  c && c <= d && d <= a
        // cdab;
        return Reg::permute<X2, X3, X0, X1>(dcba);
    case 0x5: // a <= b && b >  c && c <= d && d >  a
        // [ac] < [bd]
        switch (extraCmp) {
        case 0x0: // d <= b && c <= a
            // cadb
            return shuffle<>(dcba, bcda);
        case 0x1: // d <= b && c >  a
        case 0x2: // d >  b && c <= a
        case 0x3: // d >  b && c >  a
        }
    case 0x6: // a <= b && b >  c && c >  d && d <= a
        // d[ac]b
    case 0x7: // a <= b && b >  c && c >  d && d >  a
        // adcb;
        return permute<X1, X0, X3, X2>(permute128<X1, X0>(bcda, bcda));
    case 0x8: // a >  b && b <= c && c <= d && d <= a
        return bcda;
    case 0x9: // a >  b && b <= c && c <= d && d >  a
        // b[ac]d;
    case 0xa: // a >  b && b <= c && c >  d && d <= a
        // [ac] > [bd]
    case 0xb: // a >  b && b <= c && c >  d && d >  a
        // badc;
        return permute128<X1, X0>(dcba);
    case 0xc: // a >  b && b >  c && c <= d && d <= a
        // c[bd]a;
    case 0xd: // a >  b && b >  c && c <= d && d >  a
        // cbad;
        return permute<X1, X0, X3, X2>(bcda);
    case 0xe: // a >  b && b >  c && c >  d && d <= a
        return dcba;
    }
    */
}

} // namespace AVX
} // namespace Vc

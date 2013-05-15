/*  This file is part of the Vc library.

    Copyright (C) 2010-2012 Matthias Kretz <kretz@kde.org>

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

#include <Vc/global.h>
#include <Vc/cpuid.h>
#include "Vc/common/support.h"

#ifdef VC_MSVC
#include <intrin.h>
#endif

namespace Vc
{

#ifdef VC_GCC
    __attribute__((target("no-sse2,no-avx")))
#endif
bool isImplementationSupported(Implementation impl)
{
    CpuId::init();
    // for AVX we need to check for OSXSAVE and AVX

    switch (impl) {
    case ScalarImpl:
        return true;
    case SSE2Impl:
        return CpuId::hasSse2();
    case SSE3Impl:
        return CpuId::hasSse3();
    case SSSE3Impl:
        return CpuId::hasSsse3();
    case SSE41Impl:
        return CpuId::hasSse41();
    case SSE42Impl:
        return CpuId::hasSse42();
    case SSE4aImpl:
        return CpuId::hasSse4a();
    case XopImpl:
        return CpuId::hasXop();
    case Fma4Impl:
        return CpuId::hasFma4();
    case AVXImpl:
#if defined(VC_MSVC) && VC_MSVC >= 160040219 // MSVC 2010 SP1 introduced _xgetbv
        unsigned long long xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
        return (xcrFeatureMask & 0x6) != 0;
#elif !defined(VC_NO_XGETBV)
        if (CpuId::hasOsxsave() && CpuId::hasAvx()) {
            unsigned int eax;
            asm("xgetbv" : "=a"(eax) : "c"(0) : "edx");
            return (eax & 0x06) == 0x06;
        }
#endif
        return false;
    }
    return false;
}

} // namespace Vc

// vim: sw=4 sts=4 et tw=100

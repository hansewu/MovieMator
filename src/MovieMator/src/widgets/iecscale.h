/*
 * Copyright (c) 2015 Meltytech, LLC
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef IECSCALE_H
#define IECSCALE_H

//----------------------------------------------------------------------------
// IEC standard dB scaling -- as borrowed from meterbridge (c) Steve Harris

static inline double IEC_Scale(double dB)
{
    double fScale = 1.0;

    if (dB < -70.0)
        fScale = 0.0;
    else if (dB < -60.0)
        fScale = (dB + 70.0) * 0.0025;
    else if (dB < -50.0)
        fScale = (dB + 60.0) * 0.005 + 0.025;
    else if (dB < -40.0)
        fScale = (dB + 50.0) * 0.0075 + 0.075;
    else if (dB < -30.0)
        fScale = (dB + 40.0) * 0.015 + 0.15;
    else if (dB < -20.0)
        fScale = (dB + 30.0) * 0.02 + 0.3;
    else if (dB < -0.001 || dB > 0.001)  /* if (dB < 0.0f) */
        fScale = (dB + 20.0) * 0.025 + 0.5;

    return fScale;
}

// 此函数有被调用
static inline double IEC_ScaleMax(double dB, double max)
{
    return IEC_Scale(dB) / IEC_Scale(max);
}

#endif // IECSCALE_H

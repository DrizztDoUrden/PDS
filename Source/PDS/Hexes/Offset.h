#pragma once

#include <Hexes/Hexes.h>

#include <CoreMinimal.h>

namespace Hexes
{

    template <class offset, class orientation>
    struct Offset
    {
        FORCEINLINE_DEBUGGABLE Offset(int32 col_, int32 row_) : col(col_), row(row_) {}
        FORCEINLINE_DEBUGGABLE Offset(Int hex) : Offset(orientation{}.HexToQOffset<offset>(hex)) {}
        FORCEINLINE_DEBUGGABLE Int ToHex() const { return orientation{}.OffsetToHex<offset>(*this); }

        int32 col;
        int32 row;
    };

    struct Even
    {
        const int32 value = 1;
    };

    struct Odd
    {
        const int32 value = -1;
    };

    struct Pointy
    {
        template <class offset>
        FORCEINLINE_DEBUGGABLE Int OffsetToHex(Offset<offset, Pointy> p) const
        {
            return {
                p.col - FMath::FloorToInt((p.row + offset{}.value * (p.row & 1)) / 2.f),
                p.row,
            };
        }

        template <class offset>
        FORCEINLINE_DEBUGGABLE Offset<offset, Pointy> HexToOffset(Int h)
        {
            return {
                h.q() + FMath::FloorToInt((h.r() + offset{}.value * (h.r() & 1)) / 2),
                h.r(),
            };
        }
    };

    struct Flat
    {
        template <class offset, class THex>
        FORCEINLINE_DEBUGGABLE Int OffsetToHex(Offset<offset, Flat> p) const
        {
            return {
                p.col,
                p.row - FMath::FloorToInt((p.col + offset{}.value * (p.col & 1)) / 2.f),
            };
        }

        template <class offset>
        FORCEINLINE_DEBUGGABLE Offset<offset, Flat> HexToQOffset(Int hex)
        {
            return {
                hex.q(),
                hex.r() + FMath::FloorToInt((hex.q() + offset{}.value * (hex.q() & 1)) / 2.f),
            };
        }
    };

}

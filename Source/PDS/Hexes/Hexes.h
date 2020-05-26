#pragma once

#include <PDS/Hexes/Base.h>

#include <CoreMinimal.h>

namespace Hexes
{

    struct Float;

    struct Int : Base<int32, Int>
    {
        Int() = default;

        FORCEINLINE_DEBUGGABLE Int(FIntPoint point) : Base(point.X, point.Y) {}
        FORCEINLINE_DEBUGGABLE Int(Item q_, Item r_) : Base(q_, r_) {}
        FORCEINLINE_DEBUGGABLE Int(Item q_, Item r_, Item s_) : Base(q_, r_, s_) {}

        FORCEINLINE_DEBUGGABLE Int operator*(float other) const { return { FMath::RoundToInt(q * other), FMath::RoundToInt(r * other) }; }

        FORCEINLINE_DEBUGGABLE int32 Length() const { return (FMath::Abs(q) + FMath::Abs(r) + FMath::Abs(s())) / 2; }
        FORCEINLINE_DEBUGGABLE int32 Distance(Int other) const { return (*this - other).Length(); }

        Float Lerp(Int, float t) const;

        operator FIntPoint() const { return { q, r }; }
    };

    struct Float : Base<float, Float>
    {
        Float() = default;

        FORCEINLINE_DEBUGGABLE Float(Item q_, Item r_) : Base(q_, r_) {}
        FORCEINLINE_DEBUGGABLE Float(Item q_, Item r_, Item s_) : Base(q_, r_, s_) {}

        FORCEINLINE_DEBUGGABLE Float(const Layout& layout, const FVector2D& p) : Base(0, 0)
        {
            const Orientation& M = layout.orientation;
            const auto pt = FVector2D{
                (p.X - layout.origin.X) / layout.size.X,
                (p.Y - layout.origin.Y) / layout.size.Y
            };
            q = M.b0 * pt.X + M.b1 * pt.Y;
            r = M.b2 * pt.X + M.b3 * pt.Y;
        }

        FORCEINLINE_DEBUGGABLE Float operator*(Item other) const { return { q * other, r * other }; }

        FORCEINLINE_DEBUGGABLE Int Round() const
        {
            auto tq = FMath::RoundToInt(q);
            auto tr = FMath::RoundToInt(r);
            const auto ts = FMath::RoundToInt(s());
            const auto q_diff = FMath::Abs(tq - q);
            const auto r_diff = FMath::Abs(tr - r);
            const auto s_diff = FMath::Abs(ts - s());
            if (q_diff > r_diff && q_diff > s_diff)
                tq = -tr - ts;
            else if (r_diff > s_diff)
                tr = -tq - ts;
            return { tq, tr };
        }

        FORCEINLINE_DEBUGGABLE Float Lerp(Float to, float t) const
        {
            return {
                FMath::Lerp(q, to.q, t),
                FMath::Lerp(r, to.r, t),
            };
        }
    };

    FORCEINLINE_DEBUGGABLE Float Int::Lerp(Int to, float t) const
    {
        return {
            FMath::Lerp<float, float>(q, to.q, t),
            FMath::Lerp<float, float>(r, to.r, t),
        };
    }

}

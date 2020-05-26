#pragma once

#include <PDS/Utility/Iterators.h>

#include <CoreMinimal.h>

namespace Hexes
{

    template <class THex>
    struct Circle
    {
    private:
        struct IteratorCtorLocker {};

    public:
        struct ConstIterator : Iterators::ConstBase<THex, ConstIterator>
        {
        private:
            using Base = Iterators::ConstBase<THex, ConstIterator>;
            friend struct Base;

        public:
            FORCEINLINE_DEBUGGABLE ConstIterator() = default;

            FORCEINLINE_DEBUGGABLE ConstIterator(IteratorCtorLocker, THex center_, int32 radius_)
                : Base({ -radius_, 0 })
                , center(center_), radius(radius_)
            {
                rEnd = radius;
            }

        private:
            FORCEINLINE_DEBUGGABLE bool Equals(const ConstIterator& other) const
            {
                check(this->center == other.center);
                check(this->radius == other.radius);
                return this->GetValue() == other.GetValue();
            }

            FORCEINLINE_DEBUGGABLE TOptional<THex> ShiftForward()
            {
                auto hex = GetValue() - center;
                if (++hex.r > rEnd)
                {
                    if (++hex.q > radius)
                        return {};

                    hex.r = FMath::Max(-radius, -hex.q - radius);
                    rEnd = FMath::Min(radius, -hex.q + radius);
                }
                return hex + center;
            }

            THex center;
            int32 radius;
            int32 rEnd;
        };

        FORCEINLINE_DEBUGGABLE Circle(int32 radius_, THex center_ = {0, 0}) : center(center_), radius(radius_ - 1) {}
        FORCEINLINE_DEBUGGABLE ConstIterator begin() const { return { IteratorCtorLocker{}, center, radius, }; }
        FORCEINLINE_DEBUGGABLE ConstIterator end() const { return {}; }

    private:
        THex center;
        int32 radius;
    };

    template <class THex>
    FORCEINLINE_DEBUGGABLE Circle<THex> MakeCircle(int32 radius, THex center = {0, 0}) { return { radius, center }; }

}

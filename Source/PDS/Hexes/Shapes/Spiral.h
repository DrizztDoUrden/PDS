#pragma once

#include <PDS/Utility/Iterators.h>

#include <CoreMinimal.h>

namespace Hexes
{

    template <class THex>
    struct Spiral
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

            FORCEINLINE_DEBUGGABLE ConstIterator(IteratorCtorLocker, THex center_, int32 radius_, int32 curRadius_)
                : Base(center_ + THex::Int::Direction(4) * (curRadius_ - 1) + (curRadius_ > 1 ? THex::Direction(0) : THex{}))
                , center(center_), radius(radius_), curRadius(curRadius_)
            {
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
                auto v = GetValue();
                while (++cellInSide >= curRadius)
                {
                    cellInSide = 0;
                    if (++side >= 6)
                    {
                        side = 0;
                        v += THex::Int::Direction(4);
                        if (++curRadius > radius)
                            return {};
                    }
                }
                return v + THex::Int::Direction(side);
            }

            THex center;
            int32 radius;
            int32 curRadius;
            uint8 side = 0;
            int32 cellInSide = 1;
        };

        FORCEINLINE_DEBUGGABLE Spiral(int32 radius_, THex center_ = { 0, 0 }, int32 startingRadius_ = 0) : center(center_), radius(radius_), startingRadius(startingRadius_) {}
        FORCEINLINE_DEBUGGABLE ConstIterator begin() const { return { IteratorCtorLocker{}, center, radius, startingRadius, }; }
        FORCEINLINE_DEBUGGABLE ConstIterator end() const { return {}; }

    private:
        THex center;
        int32 radius;
        int32 startingRadius;
    };

    template <class THex>
    FORCEINLINE_DEBUGGABLE Spiral<THex> MakeSpiral(int32 radius, THex center = { 0, 0 }, int32 startingRadius = 0) { return { radius, center, startingRadius }; }

}

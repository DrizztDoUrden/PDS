#pragma once

#include <PDS/Utility/Iterators.h>

#include <CoreMinimal.h>

namespace Hexes
{

    template <class THex>
    struct Ring
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
                : Base(center_ + THex::Int::Direction(4) * (radius_ - 1) + THex::Int::Direction(0))
                , center(center_), radius(radius_)
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
                while (++cellInSide >= radius)
                {
                    cellInSide = 0;
                    if (++side >= 6)
                        return {};
                }
                return GetValue() + THex::Int::Direction(side);
            }

            THex center;
            int32 radius;
            uint8 side = 0;
            int32 cellInSide = 1;
        };

        FORCEINLINE_DEBUGGABLE Ring(int32 radius_, THex center_ = { 0, 0 }) : center(center_), radius(radius_) {}
        FORCEINLINE_DEBUGGABLE ConstIterator begin() const { return { IteratorCtorLocker{}, center, radius, }; }
        FORCEINLINE_DEBUGGABLE ConstIterator end() const { return {}; }

    private:
        THex center;
        int32 radius;
    };

    template <class THex>
    FORCEINLINE_DEBUGGABLE Ring<THex> MakeRing(int32 radius, THex center = { 0, 0 }) { return { radius, center }; }

}

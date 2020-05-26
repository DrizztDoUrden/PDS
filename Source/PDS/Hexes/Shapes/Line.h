#pragma once

#include <PDS/Utility/Iterators.h>
#include <PDS/Hexes/Hexes.h>

#include <CoreMinimal.h>

namespace Hexes
{

    template <class THex>
    struct Line
    {
    private:
        struct IteratorCtorLocker {};

    public:
        struct ConstIterator : Iterators::ConstBidirBase<THex, ConstIterator>
        {
        private:
            using Base = Iterators::ConstBidirBase<THex, ConstIterator>;
            friend struct Base;

        public:
            FORCEINLINE_DEBUGGABLE ConstIterator() = default;

            FORCEINLINE_DEBUGGABLE ConstIterator(IteratorCtorLocker, THex from_, THex to_)
                : Base(MakeFrom(from_).Lerp(MakeTo(to_), 0).Round())
                , from(MakeFrom(from_)), to(MakeTo(to_))
            {
                step.count = FMath::FloorToInt(from_.Distance(to_)) + 1;
                step.size = 1.0f / step.count;
            }

        private:
            constexpr static const auto eps = 1e-6f;

            FORCEINLINE_DEBUGGABLE static Float MakeFrom(THex from) { return { from.q + eps, from.r + eps }; }
            FORCEINLINE_DEBUGGABLE static Float MakeTo(THex to) { return { to.q + eps, to.r + eps }; }

            Float from;
            Float to;

            struct {
                int32 id = 0;
                float size;
                int32 count = 0;
            } step;

            FORCEINLINE_DEBUGGABLE bool Equals(const ConstIterator& other) const
            {
                check(this->from == other.from);
                check(this->to == other.to);
                check(this->step.count == other.step.count);

                return this->step.id == other.step.id;
            }

            FORCEINLINE_DEBUGGABLE TOptional<THex> ShiftForward()
            {
                if (++step.id == step.count)
                    return {};
                return from.Lerp(to, step.size * step.id).Round();
            }

            FORCEINLINE_DEBUGGABLE TOptional<THex> ShiftBack()
            {
                if (--step.id < 0)
                    return {};
                return from.Lerp(to, step.size * step.id).Round();
            }
        };

        FORCEINLINE_DEBUGGABLE Line(THex from_, THex to_) : from(from_), to(to_) {}
        FORCEINLINE_DEBUGGABLE ConstIterator begin() const { return { IteratorCtorLocker{}, from, to }; }
        FORCEINLINE_DEBUGGABLE ConstIterator end() const { return {}; }

    private:
        THex from;
        THex to;
    };

    template <class THex>
    FORCEINLINE_DEBUGGABLE Line<THex> MakeLine(THex from, THex to) { return { from, to }; }

}

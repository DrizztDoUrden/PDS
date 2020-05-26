#pragma once

#include <PDS/Utility/Iterators.h>

#include <CoreMinimal.h>

template <class TValue>
struct Range
{
private:
    struct IteratorCtorLocker {};

public:
    struct ConstIterator : Iterators::ConstBase<TValue, ConstIterator>
    {
    private:
        using Base = Iterators::ConstBase<TValue, ConstIterator>;
        friend struct Base;

    public:
        FORCEINLINE_DEBUGGABLE ConstIterator() = default;

        FORCEINLINE_DEBUGGABLE ConstIterator(IteratorCtorLocker, TValue from, TValue to_)
            : Base(MoveTemp(from))
            , to(to_)
        {
        }

    private:
        FORCEINLINE_DEBUGGABLE bool Equals(const ConstIterator& other) const
        {
            check(this->to == other.to);
            return this->GetValue() == other.GetValue();
        }

        FORCEINLINE_DEBUGGABLE TOptional<TValue> ShiftForward()
        {
            auto v = GetValue();
            if (++v > to)
                return {};
            return v;
        }

        TValue to;
    };

    FORCEINLINE_DEBUGGABLE Range(TValue from_, TValue to_) : from(from_), to(to_) {}
    template <class TRange>
    FORCEINLINE_DEBUGGABLE Range(const TRange& toTakeIndexes) : from(0), to(toTakeIndexes.Num() - 1) {}
    FORCEINLINE_DEBUGGABLE ConstIterator begin() const { return { IteratorCtorLocker{}, from, to, }; }
    FORCEINLINE_DEBUGGABLE ConstIterator end() const { return {}; }

private:
    TValue from;
    TValue to;
};

template <class TValue>
Range<TValue> MakeRange(TValue from, TValue to) { return {from, to}; }

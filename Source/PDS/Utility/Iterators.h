#pragma once

#include <CoreMinimal.h>

template <class TType>
FORCEINLINE_DEBUGGABLE TType ValidateType(TType value)
{
    static_assert(std::is_same_v<decltype(value), TType>);
    return value;
}

namespace Iterators
{
    template <class TValue, class TDerived>
    struct Base;

    template <class TValue, class TDerived>
    struct ConstBase
    {
    public:
        FORCEINLINE_DEBUGGABLE operator bool() const { return value.IsSet(); }

        FORCEINLINE_DEBUGGABLE bool operator ==(const TDerived& other) const
        {
            if (!*this && !other) return true;
            if (!*this || !other) return false;
            return AsDerieved().Equals(other);
        }

        FORCEINLINE_DEBUGGABLE bool operator !=(const TDerived& other) const
        {
            return !(AsDerieved() == other);
        }

        FORCEINLINE_DEBUGGABLE const TValue& GetValue() const
        {
            check(*this);
            return value.GetValue();
        }

        FORCEINLINE_DEBUGGABLE const TValue& operator*() const { return GetValue(); }

        template<class TValue2 = TValue>
        FORCEINLINE_DEBUGGABLE std::enable_if_t<!std::is_pointer_v<TValue2>, const TValue*> operator->() const { return &GetValue(); }

        FORCEINLINE_DEBUGGABLE TDerived& operator++()
        {
            check(*this);
            value = ValidateType<TOptional<TValue>>(AsDerieved().ShiftForward());
            return AsDerieved();
        }

        FORCEINLINE_DEBUGGABLE TDerived operator++(int)
        {
            const auto ret = AsDerieved();
            ++(*this);
            return ret;
        }

    protected:
        ConstBase() = default;
        FORCEINLINE_DEBUGGABLE ConstBase(TValue&& value_) : value(value_) {}

    private:
        TOptional<TValue> value;

        TDerived& AsDerieved() { return static_cast<TDerived&>(*this); }
        const TDerived& AsDerieved() const { return static_cast<const TDerived&>(*this); }

        friend struct Base<TValue, TDerived>;
    };

    template <class TValue, class TDerived>
    struct Base : ConstBase<TValue, TDerived>
    {
    private:
        using TBase = ConstBase<TValue, TDerived>;

    public:
        FORCEINLINE_DEBUGGABLE TValue& GetValue()
        {
            check(*this);
            return value.GetValue();
        }

        FORCEINLINE_DEBUGGABLE TValue& operator*() { return GetValue(); }

        template<class TValue2 = TValue>
        FORCEINLINE_DEBUGGABLE std::enable_if_t<!std::is_pointer_v<TValue2>, TValue*> operator->() { return &GetValue(); }

    protected:
        Base() = default;
        FORCEINLINE_DEBUGGABLE Base(TValue&& value_) : TBase(std::move(value_)) {}

    private:
        TDerived& AsDerieved() { return *reinterpret_cast<TDerived*>(this); }
        const TDerived& AsDerieved() const { return *reinterpret_cast<const TDerived*>(this); }
    };

    template <class TValue, class TDerived>
    struct ConstBidirBase : ConstBase<TValue, TDerived>
    {
    private:
        using Base = ConstBase<TValue, TDerived>;

    public:
        FORCEINLINE_DEBUGGABLE TDerived& operator--()
        {
            check(*this);
            value = ValidateType<TOptional<TValue>>(AsDerieved().ShiftBack());
            return AsDerieved();
        }

        FORCEINLINE_DEBUGGABLE TDerived operator--(int)
        {
            const auto ret = AsDerieved();
            --(*this);
            return ret;
        }

    protected:
        ConstBidirBase() = default;
        FORCEINLINE_DEBUGGABLE ConstBidirBase(TValue&& value_) : Base(std::move(value_)) {}

    private:
        TDerived& AsDerieved() { return *reinterpret_cast<TDerived*>(this); }
        const TDerived& AsDerieved() const { return *reinterpret_cast<const TDerived*>(this); }
    };

    template <class TValue, class TDerived>
    struct BidirBase : Base<TValue, TDerived>
    {
    private:
        using TBase = ConstBase<TValue, TDerived>;

    public:
        FORCEINLINE_DEBUGGABLE TDerived& operator--()
        {
            check(*this);
            value = ValidateType<TOptional<TValue>>(AsDerieved().ShiftBack());
            return AsDerieved();
        }

        FORCEINLINE_DEBUGGABLE TDerived operator--(int)
        {
            const auto ret = AsDerieved();
            --(*this);
            return ret;
        }

    protected:
        BidirBase() = default;
        FORCEINLINE_DEBUGGABLE BidirBase(TValue&& value_) : TBase(std::move(value_)) {}

    private:
        TDerived& AsDerieved() { return *reinterpret_cast<TDerived*>(this); }
        const TDerived& AsDerieved() const { return *reinterpret_cast<const TDerived*>(this); }
    };

}

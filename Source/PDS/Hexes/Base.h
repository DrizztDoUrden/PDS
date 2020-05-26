#pragma once

#include <PDS/Utility/Iterators.h>

#include <CoreMinimal.h>

namespace Hexes
{

    struct Orientation
    {
        const float f0, f1, f2, f3;
        const float b0, b1, b2, b3;
        const float start_angle; // in multiples of 60°

        FORCEINLINE_DEBUGGABLE Orientation(float f0_, float f1_, float f2_, float f3_,
            float b0_, float b1_, float b2_, float b3_,
            float start_angle_)
            : f0(f0_), f1(f1_), f2(f2_), f3(f3_),
            b0(b0_), b1(b1_), b2(b2_), b3(b3_),
            start_angle(start_angle_) {}

        FORCEINLINE_DEBUGGABLE static Orientation Pointy()
        {
            return {
                FMath::Sqrt(3.f), FMath::Sqrt(3.f) / 2.f, 0.f, 3.f / 2.f,
                FMath::Sqrt(3.f) / 3.f, -1.f / 3.f, 0.f, 2.f / 3.f,
                0.5,
            };
        }

        FORCEINLINE_DEBUGGABLE static Orientation Flat()
        {
            return {
                3.f / 2.f, 0.f, FMath::Sqrt(3.f) / 2.f, FMath::Sqrt(3.f),
                2.f / 3.f, 0.f, -1.f / 3.f, FMath::Sqrt(3.f) / 3.f,
                0.f,
            };
        }
    };

    struct Layout {
        const Orientation orientation;
        const FVector2D size;
        const FVector2D origin;

        FORCEINLINE_DEBUGGABLE Layout(Orientation orientation_, FVector2D size_, FVector2D origin_ = FVector2D::ZeroVector)
            : orientation(orientation_), size(size_), origin(origin_) {}

        FORCEINLINE_DEBUGGABLE FVector2D HexCornerOffset(int corner) const
        {
            const auto angle = 2.0 * PI * (orientation.start_angle + corner) / 6;
            return { size.X * FMath::Cos(angle), size.Y * FMath::Sin(angle) };
        }
    };

    template<class TItem, class TDerived>
    struct Base
    {
        using Item = TItem;

        Item q;
        Item r;

        Base() = default;

        FORCEINLINE_DEBUGGABLE Base(Item q_, Item r_) : q(q_), r(r_) {}
        FORCEINLINE_DEBUGGABLE Base(Item q_, Item r_, Item s) : q(q_), r(r_) { check(q + r + s == 0); }

        FORCEINLINE auto s() const { return -q - r; }

        FORCEINLINE_DEBUGGABLE bool operator ==(const TDerived& other) const { return q == other.q && r == other.r; }
        FORCEINLINE_DEBUGGABLE bool operator != (const TDerived& other) const { return !(*this == other); }

        FORCEINLINE_DEBUGGABLE TDerived& operator-() const { return { -q, -r }; }

        FORCEINLINE_DEBUGGABLE TDerived operator+(const TDerived& other) const { return { q + other.q, r + other.r }; }
        FORCEINLINE_DEBUGGABLE TDerived operator-(const TDerived& other) const { return { q - other.q, r - other.r }; }
        FORCEINLINE_DEBUGGABLE TDerived operator*(int32 other) const { return { q * other, r * other }; }

        FORCEINLINE_DEBUGGABLE TDerived& operator+=(const TDerived& other) { return AsDerieved() = AsDerieved() + other; }
        FORCEINLINE_DEBUGGABLE TDerived& operator-=(const TDerived& other) { return AsDerieved() = AsDerieved() - other; }
        FORCEINLINE_DEBUGGABLE TDerived& operator*=(int32 other) { return AsDerieved() = AsDerieved() * other; }
        FORCEINLINE_DEBUGGABLE TDerived& operator*=(float other) { return AsDerieved() = AsDerieved() * other; }

        constexpr static TDerived Direction(int32 direction /* 0 to 5 */)
        {
            switch (direction)
            {
            case 0: return { 1, 0, -1 };
            case 1: return { 1, -1, 0 };
            case 2: return { 0, -1, 1 };
            case 3: return { -1, 0, 1 };
            case 4: return { -1, 1, 0 };
            case 5: return { 0, 1, -1 };
            }
            check(0 <= direction && direction < 6);
            return {};
        }

        FORCEINLINE_DEBUGGABLE TDerived Neighbor(int32 direction) const { return *this + Direction(direction); }

        FORCEINLINE_DEBUGGABLE FVector2D ToPixel(const Layout& layout) const
        {
            const auto& orientation = layout.orientation;
            const auto& x = (orientation.f0 * q + orientation.f1 * r) * layout.size.X;
            const auto& y = (orientation.f2 * q + orientation.f3 * r) * layout.size.Y;
            return { x + layout.origin.X, y + layout.origin.Y };
        }

        FORCEINLINE_DEBUGGABLE auto PolygonCorners(const Layout& layout)
        {
            auto corners = TArray<FVector2D>{};
            const auto center = ToPixel(layout);

            for (int i = 0; i < 6; i++)
            {
                const auto offset = layout.HexCornerOffset(i);
                corners.Add({ center.x + offset.x, center.y + offset.y });
            }

            return corners;
        }

        FORCEINLINE_DEBUGGABLE TDerived RotateLeft(const TDerived& a) const { return { -a.s(), -a.q, }; }
        FORCEINLINE_DEBUGGABLE TDerived RotateRight(const TDerived& a) const { return { -a.r, -a.s(), }; }

    private:
        TDerived& AsDerieved() { return static_cast<TDerived&>(*this); }
    };

}

namespace std
{
    template <class TItem, class TDerieved>
    struct hash<Hexes::Base<TItem, TDerieved>>
    {
        size_t operator()(const Hexes::Base<TItem, TDerieved>& h) const
        {
            const auto item_hash = hash<TItem>{};
            const auto hq = item_hash(h.q);
            const auto hr = item_hash(h.r);
            return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
        }
    };
}

template <class TItem, class TDerieved>
uint32 GetTypeHash(Hexes::Base<TItem, TDerieved> hex)
{
    using Hex = Hexes::Base<TItem, TDerieved>;
    const auto hasher = std::hash<Hex>{};
    return static_cast<uint32>(hasher(hex));
}

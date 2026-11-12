#include <cstddef>
#include <ostream>
#include <string_view>

enum class WagonType {
    FIRST_CLASS,
    SECOND_CLASS,
    SLEEPER,
    DINING
};

class WagonSeries {
private:
    WagonType wagon_type;
    std::size_t number_of_seats;

    static constexpr std::size_t get_default_seats(WagonType wagon_type) {
        switch (wagon_type) {
            case WagonType::FIRST_CLASS: return 54;
            case WagonType::SECOND_CLASS: return 80;
            case WagonType::SLEEPER: return 30;
            case WagonType:: DINING: return 32;
        }
        return 0;
    }

    static constexpr std::string_view type_to_string(WagonType wagon_type) {
        switch (wagon_type) {
            case WagonType::FIRST_CLASS: return "1 klasa";
            case WagonType::SECOND_CLASS: return "2 klasa";
            case WagonType::SLEEPER: return "sypialny";
            case WagonType::DINING: return "restauracyjny";
        }
        return "";
    }

public:
    WagonSeries() = delete;

    constexpr WagonSeries(WagonType wagon_type, std::size_t number_of_seats) 
            : wagon_type(wagon_type), number_of_seats(number_of_seats) {}

    constexpr WagonSeries(WagonType wagon_type)
            : WagonSeries(wagon_type, get_default_seats(wagon_type)) {}

    WagonSeries(const WagonSeries& other) = default;

    WagonSeries(WagonSeries&& other) = default;

    WagonSeries& operator=(const WagonSeries& other) = default;

    WagonSeries& operator=(WagonSeries&& other) = default;

    constexpr bool operator==(const WagonSeries& other) const {
        return wagon_type == other.wagon_type
            && number_of_seats == other.number_of_seats;
    }

    constexpr bool operator!=(const WagonSeries& other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(
            std::ostream& os,
            const WagonSeries& wagon_series) {
        return os << "[" << type_to_string(wagon_series.wagon_type)
                << ", miejsca: " << wagon_series.number_of_seats << "]";
    }

    constexpr WagonType type() const {
        return wagon_type;
    }

    constexpr std::size_t seats() const {
        return number_of_seats;
    }
};

inline constexpr WagonSeries STANDARD_FIRST_CLASS = WagonType::FIRST_CLASS;
inline constexpr WagonSeries STANDARD_SECOND_CLASS = WagonType::SECOND_CLASS;
inline constexpr WagonSeries STANDARD_SLEEPER = WagonType::SLEEPER;
inline constexpr WagonSeries STANDARD_DINING = WagonType::DINING;


class Wagon {
    // TODO
};

class Locomotive {
    // TODO
};

class Train {
    // TODO
};

class Depot {
    // TODO
};

class Dispatcher {
    // TODO
};
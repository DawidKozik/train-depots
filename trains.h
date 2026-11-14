#include <cstddef>
#include <ostream>
#include <string_view>
#include <string>
#include <compare>
#include <utility>
#include <deque>

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
public:
    using unique_id_t = std::size_t;

    Wagon() : number_(0), series_(STANDARD_SECOND_CLASS) {}

    explicit Wagon(const WagonSeries& wagon_series) 
            : number_(next_id_++),
            series_(wagon_series) {
        ++in_service_;
    }

    Wagon(const Wagon& other) 
            : number_(other.number_ == 0 ? 0 : next_id_++),
            series_(other.number_ == 0 ? STANDARD_SECOND_CLASS : other.series_) {
        if (other.number_ != 0) {
            ++in_service_;
        }
    }

    Wagon (Wagon&& other) 
            : number_(other.number_), series_(other.series_) {
        other.number_ = 0;
    }

    ~Wagon() {
        if (number_ != 0) {
            --in_service_;
        }
    }

    Wagon& operator=(const Wagon& other) {
        // If both are empty then we don't do anything.
        if (number_ == 0 && other.number_ == 0) {
            return *this;
        }

        // If other is empty then the wagon ceases to exist.
        if (other.number_ == 0) {
            number_ = 0;
            --in_service_;
            return *this;
        }

        // If wagon is empty then we built new wagon.
        if (number_ == 0) {
            number_ = next_id_++;
            series_ = other.series_;
            ++in_service_;
            return *this;
        }

        // If both wagon and other exist then we rebuild the wagon.
        series_ = other.series_;
        return *this;
    }

    Wagon& operator=(Wagon&& other) {
        if (this == &other) {
            return *this;
        }

        if (number_ != 0) {
            --in_service_;
        }
        number_ = other.number_;
        series_ = other.series_;
        other.number_ = 0;
        return *this;
    }

    bool operator==(const Wagon& other) const {
        return number_ == other.number_;
    }

    std::strong_ordering operator<=>(const Wagon& other) const {
        return number_ <=> other.number_;
    }

    friend std::ostream& operator<<(std::ostream& os, const Wagon& wagon) {
        if (wagon.number_ == 0) {
            return os << "Wagon pusty";
        }
        return os << "Wagon nr " << wagon.number_ << " " << wagon.series_;
    }

    operator WagonSeries() const {
        return series_;
    }

    explicit operator bool() const {
        return number_ != 0;
    }

    unique_id_t number() const {
        return number_;
    }

    WagonSeries series() const {
        return series_;
    }

    static std::size_t in_service() {
        return in_service_;
    }

    static std::size_t ever_built() {
        return next_id_ - 1;
    }

private:
    unique_id_t number_;
    WagonSeries series_;
    inline static constinit unique_id_t next_id_ = 1;
    inline static constinit std::size_t in_service_ = 0;
};

class Locomotive {
public:
    // Standard constructors
    Locomotive() = default;

    explicit Locomotive(std::string_view designation, std::size_t max_wagons)
            : designation_(designation.empty() ? "?" : designation), 
            max_wagons_(max_wagons) {}

    // Moving constructor and assingning operator
    Locomotive(Locomotive&& other) noexcept 
            : designation_(std::exchange(other.designation_, {})),
            max_wagons_(std::exchange(other.max_wagons_, 0)) {}

    Locomotive& operator=(Locomotive&& other) noexcept { 
        if (this == &other) {
            return *this;
        }

        designation_ = std::exchange(other.designation_, {});
        max_wagons_ = std::exchange(other.max_wagons_, 0);
        return *this;
    }

    // Disabling copying
    Locomotive(const Locomotive&) = delete;
    Locomotive& operator=(const Locomotive&) = delete;

    std::string_view designation() const noexcept {
        return designation_;
    }

    std::size_t max_wagons() const noexcept {
        return max_wagons_;
    }

    explicit operator bool() const noexcept {
        return !designation_.empty();
    }

    friend std::ostream& operator<<(std::ostream& os, const Locomotive& locomotive) {
        if (locomotive.designation_.empty()){
            return os << "brak lokomotywy";
        }
        return os << locomotive.designation_ << " (uciąg: " << locomotive.max_wagons_ << ")";
    }
private:
    std::string designation_;
    std::size_t max_wagons_ = 0;
};

class Train {
public:
    Train() = default;
    explicit Train(Locomotive&& locomotive)
            : locomotive_(std::move(locomotive), ) {}

    explicit Train(Locomotive&& locomotive, std::initializer_list<Wagon> wagons)
            : locomotive_(std::move(locomotive)) {
                while (wagons_.size() < locomotive_.designation()) {

                }
            }
private:
    Locomotive locomotive_;
    std::deque<Wagon> wagons_;
    std::size_t seats_ = 0; 
};

class Depot {
    // TODO
};

class Dispatcher {
    // TODO
};
#ifndef TRAINS_H
#define TRAINS_H

#include <cstddef>
#include <ostream>
#include <string_view>
#include <string>
#include <compare>
#include <utility>
#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <deque>
#include <queue>
#include <list>

enum class WagonType {
    FIRST_CLASS,
    SECOND_CLASS,
    SLEEPER,
    DINING
};

class WagonSeries {
public:
    WagonSeries() = delete;

    explicit constexpr WagonSeries(WagonType wagon_type, std::size_t number_of_seats) 
            : wagon_type_(wagon_type), number_of_seats_(number_of_seats) {}

    constexpr WagonSeries(WagonType wagon_type)
            : WagonSeries(wagon_type, get_default_seats(wagon_type)) {}

    WagonSeries(const WagonSeries& other) = default;

    WagonSeries(WagonSeries&& other) = default;

    WagonSeries& operator=(const WagonSeries& other) = default;

    WagonSeries& operator=(WagonSeries&& other) = default;

    constexpr bool operator==(const WagonSeries& other) const noexcept = default;

    friend std::ostream& operator<<(
            std::ostream& os,
            const WagonSeries& wagon_series) {
        return os << "[" << type_to_string(wagon_series.wagon_type_)
                << ", miejsca: " << wagon_series.number_of_seats_ << "]";
    }

    constexpr WagonType type() const {
        return wagon_type_;
    }

    constexpr std::size_t seats() const {
        return number_of_seats_;
    }

private:
    WagonType wagon_type_;
    std::size_t number_of_seats_;

    static constexpr std::size_t get_default_seats(WagonType wagon_type) {
        switch (wagon_type) {
            case WagonType::FIRST_CLASS: return 54;
            case WagonType::SECOND_CLASS: return 80;
            case WagonType::SLEEPER: return 30;
            case WagonType::DINING: return 32;
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
};

inline constexpr WagonSeries STANDARD_FIRST_CLASS = WagonType::FIRST_CLASS;
inline constexpr WagonSeries STANDARD_SECOND_CLASS = WagonType::SECOND_CLASS;
inline constexpr WagonSeries STANDARD_SLEEPER = WagonType::SLEEPER;
inline constexpr WagonSeries STANDARD_DINING = WagonType::DINING;


class Wagon {
public:
    using unique_id_t = std::size_t;

    Wagon() = default;

    explicit Wagon(const WagonSeries& wagon_series) 
            : number_(next_id_++),
            series_(wagon_series) {
        ++in_service_;
    }

    Wagon(const Wagon& other) 
            : number_(other.number_ == 0 ? 0 : next_id_++),
            series_(other.series_) {
        if (other.number_ != 0) {
            ++in_service_;
        }
    }

    Wagon(Wagon&& other) noexcept
            : number_(std::exchange(other.number_, 0)), series_(std::move(other.series_)) {}

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

        // If wagon is empty then we build new wagon.
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
        number_ = std::exchange(other.number_, 0);
        series_ = std::move(other.series_);
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
    unique_id_t number_ = 0;
    WagonSeries series_ = STANDARD_SECOND_CLASS;
    inline static constinit unique_id_t next_id_ = 1;
    inline static constinit std::size_t in_service_ = 0;
};

class Locomotive {
public:
    Locomotive() = default;

    explicit Locomotive(std::string_view designation, std::size_t max_wagons)
            : designation_(designation.empty() ? "?" : designation), 
            max_wagons_(max_wagons) {}

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

    friend std::ostream& operator<<(
        std::ostream& os, 
        const Locomotive& locomotive) {

        if (locomotive.designation_.empty()) {
            return os << "brak lokomotywy";
        }
        return os << locomotive.designation_ << 
            " (uciąg: " << locomotive.max_wagons_ << ")";
    }
private:
    std::string designation_;
    std::size_t max_wagons_ = 0;
};

class Train {
public:
    Train() = default;

    explicit Train(Locomotive&& locomotive, 
        std::initializer_list<Wagon> wagons = {})
            : locomotive_(std::move(locomotive)) {
        for (const auto& wagon : wagons) {
            *this += wagon;
            if (wagons_.size() >= locomotive_.max_wagons()) {
                break;
            }
        }
    }

    Train(Train&& other) noexcept
            : locomotive_(std::move(other.locomotive_)),
            wagons_(std::move(other.wagons_)),
            seats_(std::exchange(other.seats_, 0)) {}
    
    Train& operator=(Train&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        locomotive_ = std::move(other.locomotive_);
        wagons_ = std::move(other.wagons_);
        seats_ = std::exchange(other.seats_, 0);
        return *this;
    }

    Train(const Train&) = delete;
    Train& operator=(const Train&) = delete;

    // If the += operator recived an lvalue, then it attaches a copy of the wagon.
    Train& operator+=(const Wagon& wagon) {
        // Check whether attaching the wagon is possible
        if (locomotive_ && wagon && wagons_.size() < locomotive_.max_wagons()) {
            wagons_.push_back(wagon);
            seats_ += wagon.series().seats();
        }
        return *this;
    }

    // If the += operator recived an rvalue, then it moves the wagon.
    Train& operator+=(Wagon&& wagon) {
        // Check whether attaching the wagon is possible
        if (locomotive_ && wagon && wagons_.size() < locomotive_.max_wagons()) {
            const std::size_t wagon_seats = wagon.series().seats();
            wagons_.push_back(std::move(wagon));
            seats_ += wagon_seats;
        }
        return *this;
    }

    // Returns n-th wagon.
    const Wagon& operator[](std::size_t index) const noexcept {
        assert(index < wagons_.size());
        return wagons_[index];
    }

    std::size_t seats() const noexcept {
        return seats_;
    }

    std::size_t capacity() const noexcept {
        return locomotive_.max_wagons();
    }

    std::size_t size() const noexcept {
        return wagons_.size();
    }
    
    const Locomotive& locomotive() const noexcept {
        return locomotive_;
    }

    explicit operator bool() const noexcept {
        return static_cast<bool>(locomotive_);
    }

    friend std::ostream& operator<<(std::ostream& os, 
        const Train& train) {

        os << "Skład: " << train.locomotive_;
        for (const Wagon& wagon : train.wagons_) {
            os << "\n\t" << wagon;
        }
        return os;
    }

private:
    Locomotive locomotive_;
    std::deque<Wagon> wagons_;
    std::size_t seats_ = 0; 
};

class Depot {
public:
    Depot() = default;

    Depot(const Depot&) = delete;
    Depot& operator=(const Depot&) = delete;

    Depot& operator+=(Locomotive&& locomotive) {
        if (locomotive) {
            locomotives_.push(std::move(locomotive));
        }
        return *this;
    }

    Depot& operator+=(Wagon&& wagon) {
        if (wagon) {
            wagons_.push_back(std::move(wagon));
        }
        return *this;
    }

    Locomotive release_locomotive() noexcept {
        if (locomotives_.empty()) {
            return {};
        }
        Locomotive released_locomotive(std::move(locomotives_.front()));
        locomotives_.pop();
        return released_locomotive;
    }

    Wagon release_wagon(const WagonSeries& series) noexcept {
        // Finds iterator for first matching wagon in the depot
        // based on its series.
        auto matching_wagon = std::find_if(wagons_.begin(), wagons_.end(), 
        [&series](const Wagon& wagon) {
            return wagon.series() == series;
        });
        // Checks if the matching wagon actually exists.
        if (matching_wagon == wagons_.end()) {
            return {};
        }
        Wagon released_wagon(std::move(*matching_wagon));
        wagons_.erase(matching_wagon);
        return released_wagon;
    }

    std::size_t locomotives() const noexcept {
        return locomotives_.size();
    }

    std::size_t wagons() const noexcept {
        return wagons_.size();
    }

    std::size_t wagons(const WagonSeries& series) const noexcept {
        return static_cast<std::size_t>(
            std::count_if(wagons_.begin(), wagons_.end(), 
            [&series](const Wagon& wagon) {
                return wagon.series() == series;
            }));
    }

private:
    std::queue<Locomotive> locomotives_; 
    std::list<Wagon> wagons_; 
};

inline Depot& main_depot() {
    static Depot depot;
    // Initializing a static value with an instantly called lambda function
    // guarantees, that the lambda function will be only called once.
    static const bool initialized = []() {
        depot += Locomotive("EU07-001", 8);
        depot += Locomotive("EP09-020", 12);
        depot += Wagon(STANDARD_FIRST_CLASS);
        depot += Wagon(STANDARD_SECOND_CLASS);
        depot += Wagon(STANDARD_SECOND_CLASS);
        depot += Wagon(STANDARD_SECOND_CLASS);
        depot += Wagon(STANDARD_SECOND_CLASS);
        depot += Wagon(STANDARD_SLEEPER);
        depot += Wagon(STANDARD_SLEEPER);
        depot += Wagon(STANDARD_DINING);
        return true;
    }();
    // A suppressor for "unused variable" warning.
    (void)initialized;

    return depot;
}

class Dispatcher {
public:
    Dispatcher() : depot_(main_depot()) {}
    explicit Dispatcher(Depot& depot) noexcept : depot_(depot) {}
    Dispatcher(const Dispatcher&) noexcept = default;

    Dispatcher& operator=(const Dispatcher&) = delete;

    const Depot& yard() const noexcept {
        return depot_;
    }

    Train assemble(const WagonSeries& series, std::size_t demanded_count) {
        // Optimisation avoiding linear search for wagons count
        if (depot_.locomotives() == 0) { 
            return {}; 
        }

        Train created_train(depot_.release_locomotive());
        // Limit stopping the for-loop if it collected enough wagons or there is
        // no more wagons of demanded series or the locomotive can't carry more
        const std::size_t limit = std::min({
            demanded_count,
            created_train.capacity(),
            depot_.wagons(series)
        });
        for (std::size_t i = 0; i < limit; ++i) {
            created_train += depot_.release_wagon(series);
        }
        
        return created_train;
    }
private:
    Depot& depot_;
};

#endif // TRAINS_H
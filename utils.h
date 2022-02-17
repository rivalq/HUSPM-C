enum class OptionType { Some, None };

template<typename T>
struct Option {
    OptionType type;
    union {
        struct { T value; }; // Some
        struct {};           // None
    };
};

enum class ResultType { Ok, Error };

template<typename T, typename E>
struct Result {
    ResultType type;
    union {
        struct { T value; }; // Ok
        struct { E error; }; // Error
    };
};
enum class ErrorCode { NOT_SUBSET, INVALID };

#ifndef LIBARGPARSE_ARGUMENT_HPP
#define LIBARGPARSE_ARGUMENT_HPP

#include <string>
#include <optional>
#include <type_traits>
#include <charconv>
#include <cstring>

namespace argparse {

struct ArgumentParser;

struct Argument final
{
public:
    enum Type
    {
        String,  /// when present, next argument must be a value for the `Argument`
        Boolean, /// when present, next argument must be an `Argument`
    };

    explicit Argument(
        const std::string &shortName, const std::string &longName, const std::string &description,
        Type type = String, bool required = false)
        :
        _shortName(shortName),
        _longName(longName),
        _description(description),
        _type(type),
        _required(required)
    {
        if (this->_type == Boolean)
        {
            this->_value = "false";
        }
    }

    explicit Argument(
        const std::string &shortName, const std::string &longName, const std::string &description,
        const std::string &defaultValue,
        Type type = String, bool required = false)
        :
        Argument(shortName, longName, description, type, required)
    {
        this->_defaultValue = defaultValue;
    }

    inline const auto &shortName() const
    {
        return this->_shortName;
    }

    inline const auto &longName() const
    {
        return this->_longName;
    }

    inline const auto &description() const
    {
        return this->_description;
    }

    inline const auto &type() const
    {
        return this->_type;
    }

    inline const auto &required() const
    {
        return this->_required;
    }

    inline const auto &defaultValue() const
    {
        return this->_defaultValue;
    }

    /**
     * at least one variant must be set
     */
    inline bool isValid() const
    {
        return this->_shortName.size() > 0 || this->_longName.size() > 0;
    }

    /**
     * is a short name present
     */
    inline bool hasShortName() const
    {
        return this->_shortName.size() > 0;
    }

    /**
     * is a long name present
     */
    inline bool hasLongName() const
    {
        return this->_longName.size() > 0;
    }

    /**
     * does the argument have a default value
     */
    inline bool hasDefaultValue() const
    {
        return this->_defaultValue.has_value();
    }

    /**
     * is a value set for the argument
     */
    inline bool hasValue() const
    {
        return this->_value.has_value();
    }

    /**
     * returns the name of the option, longName is preferred, falls back to shortName
     */
    inline const std::string &name() const
    {
        static const std::string empty;

        if (this->hasLongName())
        {
            return this->_longName;
        }
        else if (this->hasShortName())
        {
            return this->_shortName;
        }
        else
        {
            return empty;
        }
    }

    /**
     * attempt to convert the value to the given type
     */
    template<typename T = std::string>
    inline auto get(bool *ok = nullptr) const
    {
        std::optional<T> result = std::nullopt;
        if (!this->hasValue() && this->hasDefaultValue())
        {
            result = Argument::convert_helper<T>(this->_defaultValue.value());
        }
        else
        {
            result = Argument::convert_helper<T>(this->_value.value());
        }

        if (result.has_value())
        {
            if (ok) *ok = true;
            return result.value();
        }
        else
        {
            if (ok) *ok = false;
            return T{};
        }
    }

    void setValue(const std::string &value)
    {
        this->_value = value;
    }

    inline const auto &value() const
    {
        return this->_value;
    }

    void setPresent(bool present)
    {
        this->_present = true;
    }

    inline const auto &present() const
    {
        return this->_present;
    }

private:
    Argument() = delete;

    std::string _shortName;
    std::string _longName;
    std::string _description;
    Type _type = String;
    bool _required = false;

    std::optional<std::string> _value;
    bool _present = false;

    std::optional<std::string> _defaultValue;

private:
    /**
     * reset argument to an unparsed state, called before registering in the parser
     */
    void reset()
    {
        this->_value.reset();
        this->_present = false;

        if (this->_type == Boolean)
        {
            this->_value = "false";
        }
    }

    friend argparse::ArgumentParser;

    /**
     * convertion helper from string to several data types
     *
     * supported:
     *  - std::string
     *  - bool
     *  - integral types
     *  - floating point types
     *
     * on unsupported input or casting errors, a `nullopt` is returned
     */
    template<typename T = std::string>
    static const std::optional<T> convert_helper(const std::string &value)
    {
        try
        {
            // check string first, as this may be the most required usage
            if constexpr (std::is_same_v<T, std::string>)
            {
                return value;
            }
            // check boolean before integral types, because bool is an integral type too
            else if constexpr (std::is_same_v<T, bool>)
            {
                if (value == "true")
                {
                    return true;
                }
                else if (value == "false")
                {
                    return false;
                }
                else
                {
                    return std::nullopt;
                }
            }
            // check all integral types
            else if constexpr (std::is_integral_v<T>)
            {
                T parsed{};
                const char* end = std::strchr(value.c_str(), 0);
                const auto res = std::from_chars(value.c_str(), end, parsed);
                if (!static_cast<bool>(res.ec) && res.ptr == end)
                {
                    return parsed;
                }
                else
                {
                    return std::nullopt;
                }
            }
            // check floating point types
            else if constexpr (std::is_floating_point_v<T> && std::is_same_v<T, float>)
            {
                return std::stof(value);
            }
            else if constexpr (std::is_floating_point_v<T> && std::is_same_v<T, double>)
            {
                return std::stod(value);
            }
            else if constexpr (std::is_floating_point_v<T> && std::is_same_v<T, long double>)
            {
                return std::stold(value);
            }
            // unsupported input
            else
            {
                return std::nullopt;
            }
        }
        catch (...)
        {
            return std::nullopt;
        }
    }
};

} // namespace argparse

#endif // LIBARGPARSE_ARGUMENT_HPP

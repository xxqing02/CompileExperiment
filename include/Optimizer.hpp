#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include <charconv>
#include <concepts>
#include <optional>
#include <string>
#include <exception>

namespace PL0
{
    constexpr bool _isDelimiter(char c)
    {
        return false;
    }

    template <typename... Delimiters>
    constexpr bool _isDelimiter(char c, char delimiter, Delimiters... delimiters)
    {
        return c == delimiter || _isDelimiter(c, delimiters...);
    }

    template <typename... Delimiters>
    std::vector<std::string> split(std::string str, Delimiters... delimiters)
    {
        std::vector<std::string> result;

        if (str.size() > 0 && _isDelimiter(str[0], delimiters...)) {
            result.push_back("");
        }
        size_t start = 0, end = 0;
        while (end <= str.size()) {
            if (end == str.size() || _isDelimiter(str[end], delimiters...)) {
                if (start != end) {
                    result.push_back(str.substr(start, end - start));
                }
                start = end + 1;
            }
            end++;
        }
        if (str.size() > 0 && _isDelimiter(str[str.size() - 1], delimiters...)) {
            result.push_back("");
        }

        return result;
    }

    class Quadruple
    {
    public:
        Quadruple() = default;
        Quadruple(const std::string& _op, const std::string& _arg1, const std::string& _arg2,
            const std::string& _result)
            : op(_op), arg1(_arg1), arg2(_arg2), result(_result)
        {
        }

    public:
        std::string op;
        std::string arg1;
        std::string arg2;
        std::string result;
    };

    template <typename T>
        requires std::integral<T> || std::floating_point<T>
    std::optional<T> string_to_number(const std::string & str)
    {
        T value = 0;
        auto result = std::from_chars(str.data(), str.data() + str.size(), value);

        if (result.ec == std::errc{} && result.ptr == str.data() + str.size()) {
            return value;
        }
        else {
            return std::nullopt;
        }
    }

    /**
     * @brief A node in the Directed Acyclic Graph (DAG) representation of the quadruples.
     */
    class _Optimizer_DAGNode
    {
    public:
        using Node = _Optimizer_DAGNode;
        using NodePtr = std::shared_ptr<Node>;
        enum class Type : std::int8_t
        {
            VAR = 0,
            CONST = 1,
        };

    public:
        _Optimizer_DAGNode()
        {
        }

    public:
        std::vector<std::string> varNames;  // Variables that map to this node.
        std::string value;  // The value of this node; Can be var name, constant or operator.
        std::vector<NodePtr> children;  // Children of this node.
        Type type;                      // Type of this node; Can be Type::VAR or Type::CONST.
    };

    /**
     * @brief Optimizer for quadruple (3-address code) representation.
     */
    class Optimizer
    {
    public:
        using Node = _Optimizer_DAGNode;
        using NodePtr = std::shared_ptr<Node>;

    public:
        explicit Optimizer() = default;

        void buildDAG(std::vector<Quadruple>& quads);

        std::vector<Quadruple> colloectQuadruples();

    private:
        template <typename T>
        bool isConstant(const std::string& str)
        {
            return string_to_number<T>(str).has_value();
        }

    private:
        template <typename T>
        T calculate(const std::string& op, const std::string& arg1, const std::string& arg2)
        {
            std::optional<T> realArg1 = string_to_number<T>(arg1), realArg2 = string_to_number<T>(arg2);
            if (!realArg1.has_value() || !realArg2.has_value()) {
                throw "Invalid argument for calculation.";
            }
            if (op == "+") {
                return realArg1.value() + realArg2.value();
            }
            else if (op == "-") {
                return realArg1.value() - realArg2.value();
            }
            else if (op == "*") {
                return realArg1.value() * realArg2.value();
            }
            else if (op == "/") {
                return realArg1.value() / realArg2.value();
            }
            else {
                throw "Invalid operator for calculation.";
            }
        }

        /**
         * @brief Whether a node exists in the DAG (can be mapped by a variable name).
         *
         * @note 1 var name maps to 1 node; 1 node containes 1 or more var names.
         */
        bool isNodeExists(std::string varName);

        /**
         * @brief Map a variable name to a node in the DAG. If the map already exists, then update it.
         *
         * @note If there is no map from `x` to a node, then `isNodeExists(x)` would return false.
         *       By calling this function, the map from `x` to `nodeN` is created, and
         *       `isNodeExists(x)` would become true.
         */
        void mapVarNameToNode(std::string x, NodePtr nodeN);

    private:
        // A vector of all nodes in the DAG.
        std::vector<NodePtr> m_nodes;

        // A hash map mapping variable names to their corresponding nodes.
        std::unordered_map<std::string, NodePtr> m_varName2nodePtr;
    };

}  // namespace PL0
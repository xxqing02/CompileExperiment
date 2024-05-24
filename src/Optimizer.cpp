#include "Optimizer.hpp"
#include <algorithm>
#include <format>
#include <functional>

namespace PL0
{

    void Optimizer::buildDAG(std::vector<Quadruple>& quads)
    {
        // Case1: op, y, z, x
        // Case2: op, y, _, x
        // Case3: = , y, _, x

        // Traverse every quad in `quads`:
        for (auto [op, y, z, x] : quads) {
            bool isNodeYNew = false, isNodeZNew = false;

            NodePtr nodeY = nullptr;
            // If `nodeY` exists (can be mapped from `y`):
            if (isNodeExists(y)) {
                // Get `nodeY` directly from the map.
                nodeY = m_varName2nodePtr[y];
            }
            // Else if `nodeY` does not exist (cannot be mapped from `y`):
            else {
                // Create a new node for `y` and map from `y` to `nodeY`.
                nodeY = std::make_shared<Node>();
                nodeY->varNames.push_back(y);
                nodeY->value = y;
                nodeY->type = string_to_number<double>(y) ? Node::Type::CONST : Node::Type::VAR;
                mapVarNameToNode(y, nodeY);
                isNodeYNew = true;
            }

            NodePtr nodeZ = nullptr;
            // Skip if `z` is empty.
            if (!z.empty()) {
                // If `nodeZ` exists (can be mapped from `z`):
                if (isNodeExists(z)) {
                    // Get `nodeZ` directly from the map.
                    nodeZ = m_varName2nodePtr[z];
                }
                // Else if `nodeZ` does not exist (cannot be mapped from `z`):
                else {
                    // Create a new node for `z` and map from `z` to `nodeZ`.
                    nodeZ = std::make_shared<Node>();
                    nodeZ->varNames.push_back(z);
                    nodeZ->value = z;
                    nodeZ->type = string_to_number<double>(z) ? Node::Type::CONST : Node::Type::VAR;
                    mapVarNameToNode(z, nodeZ);
                    isNodeZNew = true;
                }
            }

            // `nodeN` is the key node that marks this operation (i.e., current quadruple).
            NodePtr nodeN = nullptr;

            // ------ Case1: op, y, z, x ------
            if (!y.empty() && !z.empty()) {
                // If `y` and `z` are both constants
                if (nodeY->type == Node::Type::CONST && nodeZ->type == Node::Type::CONST) {
                    std::string p =
                        std::format("{}", calculate<double>(op, nodeY->value, nodeZ->value));

                    NodePtr nodeP = nullptr;
                    // If `nodeP` exists (can be mapped from `p`):
                    if (m_varName2nodePtr.contains(p)) {
                        nodeP = m_varName2nodePtr[p];
                    }
                    // Else if `nodeP` does not exist (cannot be mapped from `p`):
                    else {
                        // @note Do not add var name to `nodeP`.
                        nodeP = std::make_shared<Node>();
                        nodeP->value = p;
                        nodeP->type = Node::Type::CONST;
                        mapVarNameToNode(p, nodeP);
                    }

                    // If `nodeY` (or `nodeZ`) is newly created, erase the map.
                    // After erase, `isNodeExists(y)` (or `isNodeExists(z)`) would return false.
                    if (isNodeYNew) {
                        m_varName2nodePtr.erase(y);
                    }
                    if (isNodeZNew) {
                        m_varName2nodePtr.erase(z);
                    }

                    // Assign `nodeP` to `nodeN`.
                    nodeN = nodeP;
                }
                // Else if either `y` or `z` is a variable:
                else if (nodeY->type == Node::Type::VAR || nodeZ->type == Node::Type::VAR) {

                    // Find a node whose value is `op` and whose children are `nodeY` and `nodeZ`.
                    for (auto node : m_nodes) {
                        if (node->children.size() == 2 && node->value == op &&
                            node->children[0] == m_varName2nodePtr[y] &&
                            node->children[1] == m_varName2nodePtr[z]) {
                            // If such a node exists, assign it to `nodeN` and break the loop.
                            nodeN = node;
                            break;
                        }
                    }

                    // If such a node does not exist:
                    if (nodeN == nullptr) {
                        // Create a node whose children are `nodeY` and `nodeZ`.
                        nodeN = std::make_shared<Node>();
                        nodeN->children.push_back(nodeY);
                        nodeN->children.push_back(nodeZ);
                        nodeN->value = op;
                        nodeN->type = Node::Type::VAR;
                    }
                }
            }

            // ------ Case2: op, y, _, x ------
            else if (z.empty() && op != "=") {
                throw "Unary operation is not supported.";
            }

            // ------ Case3: = , y, _, x ------
            else if (z.empty() && op == "=") {
                if (isNodeYNew) {
                    // Fuck off, constant shit.
                    m_varName2nodePtr.erase(y);
                    nodeY->varNames.pop_back();
                }
                nodeN = nodeY;
            }

            // ------ Unknown case ------
            else {
                throw "Unknown case for building DAG.";
            }

            nodeN->varNames.push_back(x);

            // Try to find where `x` is in map
            auto nodeX_it_map = m_varName2nodePtr.find(x);
            if (nodeX_it_map != m_varName2nodePtr.end()) {
                NodePtr nodeX = nodeX_it_map->second;
                auto varNameX_it = std::ranges::find(nodeX->varNames, x);
                // @note Check for iterators's validation is skipped because there must be
                //       a map from `x` to `nodeX` and `x` must be in `nodeX->varNames`.
                nodeX->varNames.erase(varNameX_it);
            }

            // Replace original `x` or add new `x`.
            m_varName2nodePtr[x] = nodeN;
            if (!std::ranges::contains(m_nodes, nodeN)) {
                m_nodes.push_back(nodeN);
            }
        }
    }

    std::vector<Quadruple> Optimizer::colloectQuadruples()
    {
        std::vector<Quadruple> result;
        for (const auto& node : m_nodes) {
            auto varNameIter = node->varNames.begin();
            if (varNameIter == node->varNames.end()) {
                continue;
            }
            if (node->type == Node::Type::CONST) {
                result.emplace_back("=", node->value, "", *varNameIter);
            }
            else {
                std::string arg1 = node->children[0]->type == Node::Type::CONST
                    ? node->children[0]->value
                    : node->children[0]->varNames[0];
                std::string arg2 = node->children[1]->type == Node::Type::CONST
                    ? node->children[1]->value
                    : node->children[1]->varNames[0];
                result.emplace_back(node->value, arg1, arg2, *varNameIter);
            }
            for (varNameIter += 1; varNameIter != node->varNames.end(); ++varNameIter) {
                if (node->type == Node::Type::CONST) {
                    result.emplace_back("=", node->value, "", *varNameIter);
                }
                else {
                    result.emplace_back("=", node->varNames[0], "", *varNameIter);
                }
            }
        }
        return result;
    }

    /**
     * @brief Whether a node exists in the DAG (can be mapped by a variable name).
     *
     * @note 1 var name maps to 1 node; 1 node containes 1 or more var names.
     */
    bool Optimizer::isNodeExists(std::string varName)
    {
        return m_varName2nodePtr.contains(varName);
    }

    /**
     * @brief Map a variable name to a node in the DAG. If the map already exists, then update it.
     *
     * @note If there is no map from `x` to a node, then `isNodeExists(x)` would return false.
     *       By calling this function, the map from `x` to `nodeN` is created, and
     *       `isNodeExists(x)` would become true.
     */
    void Optimizer::mapVarNameToNode(std::string x, NodePtr nodeN)
    {
        m_varName2nodePtr[x] = nodeN;
    }

}  // namespace plazy
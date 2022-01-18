#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#define _ISDIGIT(c) ((c) >= '0' && (c) <= '9')
#define _ISDIGIT_NON_ZERO(c) ((c) >= '1' && (c) <= '9')

enum JsonParseState
{
    PARSE_SUCCESS,
    PARSE_BUFFER_EMPTY,
    PARSE_INVALID_VALUE,
    PARSE_NO_VALUE_PARSED,
    PARSE_ROOT_NOT_SINGLE_VALUE,
    PARSE_MISS_QUATATION_MARK,
    PARSE_INVALID_STRING_ESCAPE,
    PARSE_INVALID_STRING_CHAR,
    PARSE_INVALID_UNICODE_HEX,
    PARSE_INVALID_UNICODE_SURROGATE,
    PARSE_NUMBER_TOO_BIG,
    PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
    PARSE_MISS_COMMA_OR_CURLY_BRACKET,
    PARSE_KEY_VALUE_PAIR_MISS_KEY,
    PARSE_KEY_VALUE_PAIR_MISS_VALUE,
    PARSE_NULL_PTR_ERROR
};

static const char *JsonErrMsg[]{
    "Success",
    "Buffer is empty",
    "Invalid value",
    "No value parsed",
    "Root is not single value",
    "Miss quatation mark",
    "Invalid string escape",
    "Invalid string char",
    "Invalid unicode hex",
    "Invalid unicode surrogate",
    "Number too big",
    "Miss comma or square bracket",
    "Miss comma or curly bracket",
    "Key-value pair miss key",
    "Key-value pair miss value",
    "Get null pointer when parsing"};

struct JsonParseErr
{
    size_t pos;
    JsonParseState state;

    bool success() const
    {
        return state == PARSE_SUCCESS;
    }

    void print() const
    {
        if (state != PARSE_SUCCESS)
            printf("Json Parse Error %d: %s at position %d\n", (int)state,
                   JsonErrMsg[state], (int)pos);
    }
};

enum JsonNodeType
{
    JsonNullType = 0,
    JsonBoolType,
    JsonNumberType,
    JsonStringType,
    JsonArrayType,
    JsonObjectType
};

struct JsonNode;

struct JsonNull
{
};

using JsonBool = bool;

#ifdef JSON_PARSER_USE_DOUBLE
using JsonNum = double;
#else
using JsonNum = float;
#endif

using JsonStr = std::string;
using JsonArr = std::vector<JsonNode *>;

#ifndef JSON_PARSER_ALLOW_REPEATED_KEY
using JsonObj = std::unordered_map<JsonStr, JsonNode *>;
#else
using JsonObj = std::unordered_multimap<JsonStr, JsonNode *>;
#endif

struct JsonNode
{
private:
    std::variant<JsonNull, JsonBool, JsonNum, JsonStr, JsonArr, JsonObj> value;

    // No type check
    inline JsonBool _getBool() const { return std::get<JsonBool>(value); }
    inline JsonNum _getNum() const { return std::get<JsonNum>(value); }
    inline const JsonStr &_getStr() const { return std::get<JsonStr>(value); }
    inline const JsonArr &_getArr() const { return std::get<JsonArr>(value); }
    inline const JsonObj &_getObj() const { return std::get<JsonObj>(value); }

    void free()
    {
        switch (value.index())
        {
        case JsonArrayType:
            for (auto &p : _getArr())
                p->free();
            std::get<JsonArr>(value).clear();
            break;
        case JsonObjectType:
            for (auto &p : _getObj())
                p.second->free();
            std::get<JsonObj>(value).clear();
            break;
        }
    }

public:
    friend class JsonParser;
    JsonNode() = default;

    JsonNodeType getType() const
    {
        return JsonNodeType(value.index());
    }

    std::string getTypeStr() const
    {
        switch (value.index())
        {
        case JsonNullType:
            return "null";
        case JsonBoolType:
            return "bool";
        case JsonNumberType:
            return "number";
        case JsonStringType:
            return "string";
        case JsonArrayType:
            return "array";
        }
        return "object";
    }

    // Do type check
    JsonBool getBool() const
    {
        if (!isBool())
            throw std::runtime_error("Json visit node type error: Expect bool type");
        return std::get<JsonBool>(value);
    }
    JsonNum getNum() const
    {
        if (!isNum())
            throw std::runtime_error("Json visit node type error: Expect number type");
        return std::get<JsonNum>(value);
    }
    const JsonStr &getStr() const
    {
        if (!isStr())
            throw std::runtime_error("Json visit node type error: Expect string type");
        return std::get<JsonStr>(value);
    }
    const JsonArr &getArr() const
    {
        if (!isArr())
            throw std::runtime_error("Json visit node type error: Expect array type");
        return std::get<JsonArr>(value);
    }
    const JsonObj &getObj() const
    {
        if (!isObj())
            throw std::runtime_error("Json visit node type error: Expect object type");
        return std::get<JsonObj>(value);
    }

    inline bool isNull() const { return value.index() == 0; }
    inline bool isBool() const { return value.index() == 1; }
    inline bool isNum() const { return value.index() == 2; }
    inline bool isStr() const { return value.index() == 3; }
    inline bool isArr() const { return value.index() == 4; }
    inline bool isObj() const { return value.index() == 5; }

    inline int toInt() const { return int(getNum() + 0.5); }

    JsonNode(const JsonNode &) = delete;
    JsonNode &operator=(const JsonNode &) = delete;

    JsonNode &operator=(JsonBool b)
    {
        value.emplace<JsonBool>(b);
        return *this;
    }
    JsonNode &operator=(JsonNum d)
    {
        value.emplace<JsonNum>(d);
        return *this;
    }
    JsonNode &operator=(const JsonStr &s)
    {
        value.emplace<JsonStr>(s);
        return *this;
    }

    bool operator==(JsonBool b) const { return getBool() == b; }
    bool operator==(JsonNum d) const { return getNum() == d; }
    bool operator==(int i) const { return toInt() == i; }
    bool operator==(const JsonStr &s) const { return getStr() == s; }

    JsonStr toString() const
    {
        switch (value.index())
        {
        case JsonNullType:
            return "null";
        case JsonBoolType:
            return _getBool() ? "true" : "false";
        case JsonNumberType:
            return std::to_string(_getNum());
        case JsonStringType:
            return '\"' + _getStr() + '\"';
        case JsonArrayType:
        {
            JsonStr result("[");
            for (auto it = _getArr().begin(); it != _getArr().end(); ++it)
            {
                result += (*it)->toString();
                if (it + 1 != _getArr().end())
                    result += ", ";
            }
            result += ']';
            return result;
        }
        default:
        {
            JsonStr result("{");
            size_t count = 0;
            for (auto each : _getObj())
            {
                result += '"' + each.first + "\": " + each.second->toString();
                if (++count < _getObj().size())
                    result += ", ";
            }
            return result += '}';
        }
        }
    }

    friend std::ostream &operator<<(std::ostream &os, const JsonNode &node)
    {
        return os << node.toString();
    }

    JsonNode *deepCopy() const
    {
        JsonNode *result = new JsonNode;
        switch (value.index())
        {
        case JsonNullType:
            return result;
        case JsonBoolType:
            result->value.emplace<JsonBool>(_getBool());
            return result;
        case JsonNumberType:
            result->value.emplace<JsonNum>(_getNum());
            return result;
        case JsonStringType:
            result->value.emplace<JsonStr>(_getStr());
            return result;
        case JsonArrayType:
        {
            JsonArr tmp;
            for (auto &p : _getArr())
                tmp.push_back(p->deepCopy());
            result->value.emplace<JsonArr>(std::move(tmp));
            return result;
        }
        default:
        {
            JsonObj tmp;
            for (auto &p : _getObj())
                tmp.insert(std::make_pair(p.first, p.second->deepCopy()));
            result->value.emplace<JsonObj>(std::move(tmp));
            return result;
        }
        }
    }

    bool findKey(const JsonStr &key) const
    {
        if (!isObj())
            throw std::runtime_error("Json visit node type error: Expect object type");
        return _getObj().find(key) != _getObj().end();
    }
    const JsonNode &operator[](size_t i) const
    {
        if (!isArr())
            throw std::runtime_error("Json visit node type error: Expect array type");
        return *(_getArr()[i]);
    }
    const JsonNode &operator[](const JsonStr &s) const
    {
        if (!isObj())
            throw std::runtime_error("Json visit node type error: Expect object type");
        auto it = _getObj().find(s);
        if (it == _getObj().end())
            throw std::runtime_error("Json key not found");
        return *(it->second);
    }

    ~JsonNode() { free(); }
};

class JsonParser
{
private:
    JsonNode *_root;
    size_t _pos;
    JsonStr _buffer;

public:
    JsonParser() : _root(nullptr), _pos(0), _buffer("") {}

    JsonParser(const JsonParser &) = delete;
    JsonParser &operator=(const JsonParser &) = delete;

    bool loadFromFile(const char *filename);
    JsonParseErr parse(const JsonStr &buf = "");
    const JsonNode &root() const
    {
        if (_root == nullptr)
            throw std::runtime_error("Visiting null pointer");
        return *_root;
    }

    void clear()
    {
        delete _root;
        _root = nullptr;
    }

    ~JsonParser() { clear(); }

private:
    inline bool reachEnd() { return (_pos == _buffer.size()); }

    JsonParseErr parseValue(JsonNode *node);
    JsonParseErr parseSpace();
    JsonParseErr parseNull(JsonNode *node);
    JsonParseErr parseBool(JsonNode *node);
    JsonParseErr parseNumber(JsonNode *node);
    JsonParseErr parseHex4(uint64_t &u);
    JsonParseErr encodeUTF8(uint64_t u, JsonStr &s);
    JsonParseErr parseString(JsonStr &s);
    JsonParseErr parseArray(JsonNode *node);
    JsonParseErr parseObject(JsonNode *node);
};

inline bool JsonParser::loadFromFile(const char *filename)
{
    FILE *fp;
    if (!(fp = fopen(filename, "rt")))
    {
        printf("Failed to load from file: %s\n", filename);
        return false;
    }
    char ch;
    _buffer.clear();
    while (true)
    {
        fread(&ch, 1, 1, fp);
        if (!feof(fp))
            _buffer.push_back(ch);
        else
            break;
    }
    fclose(fp);
    return true;
}

inline JsonParseErr JsonParser::parse(const JsonStr &buf)
{
    if (buf.size())
        _buffer = buf;
    _pos = 0;
    parseSpace();
    if (reachEnd())
        return {0, PARSE_BUFFER_EMPTY};
    clear();
    _root = new JsonNode;
    JsonParseErr err = parseValue(_root);
    if (err.state != PARSE_SUCCESS)
        return err;
    parseSpace();
    if (!reachEnd())
        return {_pos, PARSE_ROOT_NOT_SINGLE_VALUE};
    return {0, PARSE_SUCCESS};
}

inline JsonParseErr JsonParser::parseValue(JsonNode *node)
{
    if (node == nullptr)
        return {0, PARSE_NULL_PTR_ERROR};
    parseSpace();
    if (reachEnd())
        return {0, PARSE_NO_VALUE_PARSED};

    JsonParseErr err;
    switch (_buffer[_pos])
    {
    case 't':
    case 'f':
        err = parseBool(node);
        break;
    case 'n':
        err = parseNull(node);
        break;
    case '"':
    {
        JsonStr tmp;
        err = parseString(tmp);
        node->value.emplace<JsonStr>(std::move(tmp));
        break;
    }
    default:
        err = parseNumber(node);
        break;
    case '[':
        err = parseArray(node);
        break;
    case '{':
        err = parseObject(node);
        break;
    }
    if (err.state != PARSE_SUCCESS)
        return err;
    return {0, PARSE_SUCCESS};
}

inline JsonParseErr JsonParser::parseSpace()
{
    while (_buffer[_pos] == ' ' || _buffer[_pos] == '\t' ||
           _buffer[_pos] == '\n' || _buffer[_pos] == '\r')
        _pos++;
    return {0, PARSE_SUCCESS};
}

inline JsonParseErr JsonParser::parseNull(JsonNode *node)
{
    if (node == nullptr)
        return {0, PARSE_NULL_PTR_ERROR};
    if (_buffer[_pos] == 'n' && _buffer[_pos + 1] == 'u' &&
        _buffer[_pos + 2] == 'l' && _buffer[_pos + 3] == 'l')
    {
        _pos += 4;
        node->value.emplace<JsonNull>(JsonNull());
        return {0, PARSE_SUCCESS};
    }
    return {_pos, PARSE_INVALID_VALUE};
}

inline JsonParseErr JsonParser::parseBool(JsonNode *node)
{
    if (node == nullptr)
        return {0, PARSE_NULL_PTR_ERROR};
    if (_buffer[_pos] == 't' && _buffer[_pos + 1] == 'r' &&
        _buffer[_pos + 2] == 'u' && _buffer[_pos + 3] == 'e')
    {
        _pos += 4;
        *node = true;
        return {0, PARSE_SUCCESS};
    }
    if (_buffer[_pos] == 'f' && _buffer[_pos + 1] == 'a' &&
        _buffer[_pos + 2] == 'l' && _buffer[_pos + 3] == 's' &&
        _buffer[_pos + 4] == 'e')
    {
        _pos += 5;
        *node = false;
        return {0, PARSE_SUCCESS};
    }
    return {_pos, PARSE_INVALID_VALUE};
}

inline JsonParseErr JsonParser::parseNumber(JsonNode *node)
{
    if (node == nullptr)
        return {0, PARSE_NULL_PTR_ERROR};
    size_t p = _pos;
    if (_buffer[p] == '-' && !_ISDIGIT(_buffer[p + 1]))
        return {_pos, PARSE_INVALID_VALUE};
    p += (_buffer[p] == '-');
    if (_buffer[p] == '0')
        p++;
    else
    {
        if (!_ISDIGIT_NON_ZERO(_buffer[p]))
            return {_pos, PARSE_INVALID_VALUE};
        while (_ISDIGIT(_buffer[p]))
            p++;
    }
    if (_buffer[p] == '.')
    {
        p++;
        if (!_ISDIGIT(_buffer[p]))
            return {_pos, PARSE_INVALID_VALUE};
        while (_ISDIGIT(_buffer[p]))
            p++;
    }
    if (_buffer[p] == 'e' || _buffer[p] == 'E')
    {
        p++;
        if (_buffer[p] == '+' || _buffer[p] == '-')
            p++;
        if (!_ISDIGIT(_buffer[p]))
            return {_pos, PARSE_INVALID_VALUE};
        while (_ISDIGIT(_buffer[p]))
            p++;
    }
    errno = 0;
    JsonNum num = (JsonNum)strtod((const char *)&_buffer[_pos], NULL);
#ifdef JSON_PARSER_USE_DOUBLE
    if (errno == ERANGE &&
        (num == HUGE_VAL || num == -HUGE_VAL))
        return {_pos, PARSE_NUMBER_TOO_BIG};
#else
    if (errno == ERANGE &&
        (num == HUGE_VALF || num == -HUGE_VALF))
        return {_pos, PARSE_NUMBER_TOO_BIG};
#endif
    *node = num;
    _pos = p;
    return {0, PARSE_SUCCESS};
}

inline JsonParseErr JsonParser::parseHex4(uint64_t &u)
{
    u = 0;
    size_t _pos0 = _pos;
    for (; _pos < _pos0 + 4; _pos++)
    {
        u <<= 4;
        if (_buffer[_pos] >= '0' && _buffer[_pos] <= '9')
            u |= _buffer[_pos] - '0';
        else if (_buffer[_pos] >= 'A' && _buffer[_pos] <= 'F')
            u |= _buffer[_pos] - 'A' + 10;
        else if (_buffer[_pos] >= 'a' && _buffer[_pos] <= 'f')
            u |= _buffer[_pos] - 'a' + 10;
        else
            return {_pos, PARSE_INVALID_UNICODE_HEX};
    }
    _pos--;
    return {0, PARSE_SUCCESS};
}

inline JsonParseErr JsonParser::encodeUTF8(uint64_t u, JsonStr &s)
{
    if (u > 0x10FFFF)
        return {_pos, PARSE_INVALID_UNICODE_HEX};
    if (u <= 0x7F)
        s.push_back(u & 0xFF);
    else if (u <= 0x7FF)
    {
        s.push_back(0xC0 | ((u >> 6) & 0xFF));
        s.push_back(0x80 | (u & 0x3F));
    }
    else if (u <= 0xFFFF)
    {
        s.push_back(0xE0 | ((u >> 12) & 0xFF));
        s.push_back(0x80 | ((u >> 6) & 0x3F));
        s.push_back(0x80 | (u & 0x3F));
    }
    else
    {
        s.push_back(0xF0 | ((u >> 18) & 0xFF));
        s.push_back(0x80 | ((u >> 12) & 0x3F));
        s.push_back(0x80 | ((u >> 6) & 0x3F));
        s.push_back(0x80 | (u & 0x3F));
    }
    return {0, PARSE_SUCCESS};
}

inline JsonParseErr JsonParser::parseString(JsonStr &s)
{
    s.clear();
    _pos++;
    while (_buffer[_pos] != '\"')
    {
        uint64_t u, l;
        JsonParseErr err;
        switch (_buffer[_pos])
        {
        case '\\':
            switch (_buffer[++_pos])
            {
            case '\"':
            case '\\':
            case '/':
                s.push_back(_buffer[_pos]);
                break;
            case 'b':
                s.push_back('\b');
                break;
            case 'f':
                s.push_back('\f');
                break;
            case 'n':
                s.push_back('\n');
                break;
            case 'r':
                s.push_back('\r');
                break;
            case 't':
                s.push_back('\t');
                break;
            case 'u':
                _pos++;
                err = parseHex4(u);
                if (err.state != PARSE_SUCCESS)
                    return err;
                if (u >= 0xD800 && u <= 0xDBFF)
                {
                    if (_buffer[_pos + 1] != '\\' ||
                        _buffer[_pos + 2] != 'u')
                        return {_pos, PARSE_INVALID_UNICODE_SURROGATE};
                    _pos += 2;
                    err = parseHex4(l);
                    if (err.state != PARSE_SUCCESS)
                        return err;
                    if (l < 0xDC00 || l > 0xDFFF)
                        return {_pos, PARSE_INVALID_UNICODE_SURROGATE};
                    u = (((u - 0xD800) << 10) | (l - 0xDC00)) +
                        0x10000;
                }
                err = encodeUTF8(u, s);
                if (err.state != PARSE_SUCCESS)
                    return err;
                break;
            default:
                return {_pos, PARSE_INVALID_STRING_ESCAPE};
            }
            break;

        case '\0':
            return {_pos, PARSE_MISS_QUATATION_MARK};
        default:
            if ((unsigned char)_buffer[_pos] < 0x20)
                return {_pos, PARSE_INVALID_STRING_CHAR};
            s.push_back(_buffer[_pos]);
        }
        _pos++;
    }
    _pos++;
    return {0, PARSE_SUCCESS};
}

inline JsonParseErr JsonParser::parseArray(JsonNode *node)
{
    if (node == nullptr)
        return {0, PARSE_NULL_PTR_ERROR};
    _pos++;
    parseSpace();
    JsonArr tmp;
    if (_buffer[_pos] == ']')
    {
        _pos++;
        node->value.emplace<JsonArr>(std::move(tmp));
        return {0, PARSE_SUCCESS};
    }

    while (true)
    {
        JsonNode *val = new JsonNode;
        JsonParseErr err = parseValue(val);
        if (err.state != PARSE_SUCCESS)
        {
            delete val;
            return err;
        }

        tmp.push_back(val);
        parseSpace();
        if (_buffer[_pos] == ',')
        {
            _pos++;
            parseSpace();
        }
        else if (_buffer[_pos] == ']')
        {
            _pos++;
            node->value.emplace<JsonArr>(std::move(tmp));
            return {0, PARSE_SUCCESS};
        }
        else
            return {_pos, PARSE_MISS_COMMA_OR_SQUARE_BRACKET};
    }
}
inline JsonParseErr JsonParser::parseObject(JsonNode *node)
{
    if (node == nullptr)
        return {0, PARSE_NULL_PTR_ERROR};
    _pos++;
    parseSpace();
    JsonObj tmp;
    if (_buffer[_pos] == '}')
    {
        _pos++;
        node->value.emplace<JsonObj>(tmp);
        return {0, PARSE_SUCCESS};
    }
    JsonParseErr err;

    while (true)
    {
        if (_buffer[_pos] != '"')
            return {_pos, PARSE_KEY_VALUE_PAIR_MISS_KEY};
        JsonStr key;
        err = parseString(key);
        if (err.state != PARSE_SUCCESS)
            return err;
        parseSpace();
        if (_buffer[_pos] == ':')
        {
            _pos++;
            parseSpace();
        }
        else
            return {_pos, PARSE_KEY_VALUE_PAIR_MISS_VALUE};

        JsonNode *val = new JsonNode;
        err = parseValue(val);
        if (err.state != PARSE_SUCCESS)
        {
            delete val;
            return err;
        }
        tmp.insert(std::pair<JsonStr, JsonNode *>(key, val));
        parseSpace();
        if (_buffer[_pos] == ',')
        {
            _pos++;
            parseSpace();
        }
        else if (_buffer[_pos] == '}')
        {
            _pos++;
            node->value.emplace<JsonObj>(tmp);
            return {0, PARSE_SUCCESS};
        }
        else
            return {_pos, PARSE_MISS_COMMA_OR_CURLY_BRACKET};
    }
}

#endif // JSON_PARSER_HPP

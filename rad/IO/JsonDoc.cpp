#include "JsonDoc.h"
#include "rad/IO/Logging.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/error.h"
#include "rapidjson/error/en.h"

namespace rad
{

JsonDoc::JsonDoc()
{
}

JsonDoc::~JsonDoc()
{
}

bool JsonDoc::ParseFile(const rad::FilePath& filePath)
{
    std::string json = rad::File::ReadAll(filePath);
    if (!Parse(json))
    {
        size_t errorOffset = GetParseErrorOffset();
        size_t errorLineNo = std::count(json.begin(), json.begin() + errorOffset, '\n') + 1;
        LogGlobal(Error, "JsonDoc: %s: parse error around line %u: %s",
            (const char*)filePath.u8string().c_str(), errorLineNo, GetParseError());
        return false;
    }
    return true;
}

bool JsonDoc::Parse(std::string_view str)
{
    if (m_doc.Parse<
        rapidjson::ParseFlag::kParseCommentsFlag |
        rapidjson::ParseFlag::kParseTrailingCommasFlag |
        rapidjson::ParseFlag::kParseNanAndInfFlag>
        (str.data()).HasParseError())
    {
        return false;
    }
    else
    {
        return true;
    }
}

const char* JsonDoc::GetParseError()
{
    return rapidjson::GetParseError_En(m_doc.GetParseError());
}

size_t JsonDoc::GetParseErrorOffset()
{
    return m_doc.GetErrorOffset();
}

JsonValueRef JsonDoc::GetRoot()
{
    if (m_doc.IsObject())
    {
        JsonValue& jRoot = m_doc.GetObject();
        return jRoot;
    }
    else
    {
        return nullptr;
    }
}

rapidjson::StringBuffer JsonDoc::Stringify()
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    m_doc.Accept(writer);
    return buffer;
}

rapidjson::StringBuffer JsonDoc::StringifyPretty()
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    m_doc.Accept(writer);
    return buffer;
}

} // namespace rad

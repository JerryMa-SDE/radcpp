#include "Json.h"
#include "Logging.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/error.h"
#include "rapidjson/error/en.h"

namespace rad
{

boost::json::value LoadJsonFromFile(const FilePath& path)
{
    if (path.has_filename() && Exists(path))
    {
        boost::json::error_code ec;
        boost::json::monotonic_resource mr;
        boost::json::parse_options opt = {};
        opt.allow_comments = true;
        opt.allow_trailing_commas = true;
        opt.allow_invalid_utf8 = true;
        opt.allow_infinity_and_nan = true;
        boost::json::value jRoot = boost::json::parse(rad::File::ReadAll(path), ec, &mr, opt);
        if (ec)
        {
            LogGlobal(Error, "LoadJsonFromFile: %s: %s",
                (const char*)path.u8string().c_str(), ec.message().c_str());
        }
        return jRoot;
    }
    return {};
}

} // namespace rad

namespace rapid {

JsonDoc::JsonDoc()
{
}

JsonDoc::~JsonDoc()
{
}

bool JsonDoc::ParseFile(const rad::FilePath& filePath)
{
    std::string jsonString = rad::File::ReadAll(filePath, rad::FileAccessRead);
    if (Parse(jsonString))
    {
        LogGlobal(Debug, "%s: %s parsed successfully.",
            __FUNCTION__, (const char*)filePath.u8string().c_str());
        return true;
    }
    else
    {
        size_t errorOffset = GetParseErrorOffset();
        size_t errorLineNo = std::count(jsonString.begin(), jsonString.begin() + errorOffset, '\n') + 1;
        LogGlobal(Error, "%s: parse error in file %s, around line %u: %s",
            __FUNCTION__, (const char*)filePath.u8string().c_str(), errorLineNo, GetParseError());
        return false;
    }
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

std::string JsonDoc::Stringify()
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    m_doc.Accept(writer);
    return buffer.GetString();
}

std::string JsonDoc::StringifyPretty()
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    m_doc.Accept(writer);
    return buffer.GetString();
}

} // namespace rapid


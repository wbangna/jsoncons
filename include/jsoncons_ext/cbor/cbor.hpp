// Copyright 2017 Daniel Parker
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://github.com/danielaparker/jsoncons for latest version

#ifndef JSONCONS_CBOR_CBOR_HPP
#define JSONCONS_CBOR_CBOR_HPP

#include <string>
#include <vector>
#include <memory>
#include <type_traits> // std::enable_if
#include <istream> // std::basic_istream
#include <jsoncons/json.hpp>
#include <jsoncons/json_filter.hpp>
#include <jsoncons/config/binary_detail.hpp>
#include <jsoncons_ext/cbor/cbor_reader.hpp>
#include <jsoncons_ext/cbor/cbor_encoder.hpp>

namespace jsoncons { namespace cbor {

// encode_cbor

template<class Json>
void encode_cbor(const Json& j, std::basic_ostream<typename Json::char_type>& os)
{
    encode_cbor(j,os,cbor_options());
}

template<class Json>
void encode_cbor(const Json& j, std::vector<uint8_t>& v)
{
    encode_cbor(j,v,cbor_options());
}

template<class Json>
void encode_cbor(const Json& j, std::basic_ostream<typename Json::char_type>& os, const cbor_encode_options& options)
{
    typedef typename Json::char_type char_type;
    basic_cbor_encoder<char_type> encoder(os, options);
    j.dump(encoder);
}

template<class Json>
void encode_cbor(const Json& j, std::vector<uint8_t>& v, const cbor_encode_options& options)
{
    typedef typename Json::char_type char_type;
    basic_cbor_encoder<char_type,jsoncons::bytes_result> encoder(v, options);
    j.dump(encoder);
}

// decode_cbor

template<class Json>
typename std::enable_if<std::is_same<typename Json::char_type,char>::value,Json>::type 
decode_cbor(const std::vector<uint8_t>& v)
{
    jsoncons::json_decoder<Json> decoder;
    basic_cbor_reader<jsoncons::bytes_source> parser(v, decoder);
    std::error_code ec;
    parser.read(ec);
    if (ec)
    {
        throw ser_error(ec,parser.line_number(),parser.column_number());
    }
    return decoder.get_result();
}

template<class Json>
typename std::enable_if<!std::is_same<typename Json::char_type,char>::value,Json>::type 
decode_cbor(const std::vector<uint8_t>& v)
{
    jsoncons::json_decoder<Json> decoder;
    json_content_handler_adaptor<jsoncons::json_content_handler,jsoncons::json_decoder<Json>> adaptor(decoder);
    basic_cbor_reader<jsoncons::bytes_source> parser(v, adaptor);
    std::error_code ec;
    parser.read(ec);
    if (ec)
    {
        throw ser_error(ec,parser.line_number(),parser.column_number());
    }
    return decoder.get_result();
}

template<class Json>
typename std::enable_if<std::is_same<typename Json::char_type,char>::value,Json>::type 
decode_cbor(std::basic_istream<typename Json::char_type>& is)
{
    //typedef typename Json::char_type char_type;

    jsoncons::json_decoder<Json> decoder;
    cbor_reader parser(is, decoder);
    std::error_code ec;
    parser.read(ec);
    if (ec)
    {
        throw ser_error(ec,parser.line_number(),parser.column_number());
    }
    return decoder.get_result();
}

template<class Json>
typename std::enable_if<!std::is_same<typename Json::char_type,char>::value,Json>::type 
decode_cbor(std::basic_istream<typename Json::char_type>& is)
{
    //typedef typename Json::char_type char_type;

    jsoncons::json_decoder<Json> decoder;
    json_content_handler_adaptor<jsoncons::json_content_handler,jsoncons::json_decoder<Json>> adaptor(decoder);
    cbor_reader parser(is, adaptor);
    std::error_code ec;
    parser.read(ec);
    if (ec)
    {
        throw ser_error(ec,parser.line_number(),parser.column_number());
    }
    return decoder.get_result();
}
  
#if !defined(JSONCONS_NO_DEPRECATED)
template<class Json>
std::vector<uint8_t> encode_cbor(const Json& j)
{
    std::vector<uint8_t> v;
    encode_cbor(j, v);
    return v;
}
#endif

}}

#endif

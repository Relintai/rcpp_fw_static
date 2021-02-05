/**
 *
 *  HttpFileUploadRequest.cc
 *  An Tao
 *
 *  Copyright 2018, An Tao.  All rights reserved.
 *  https://github.com/an-tao/drogon
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Drogon
 *
 */

#include "http_file_upload_request.h"
#include <core/upload_file.h>
#include <core/utilities.h>

using namespace drogon;

HttpFileUploadRequest::HttpFileUploadRequest(
    const std::vector<UploadFile> &files)
    : HttpRequest(nullptr),
      boundary_(utils::genRandomString(32)),
      files_(files)
{
    setMethod(drogon::Post);
    setVersion(drogon::Version::kHttp11);
    setContentType("Content-Type: multipart/form-data; boundary=" + boundary_ +
                   "\r\n");
    contentType_ = CT_MULTIPART_FORM_DATA;
}
//=====================================================================================================================
// Copyright (c) 2018 libnetco project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef HANDLER_H
#define HANDLER_H

#include <functional>
#include <memory>

namespace rapid { namespace core {

class TcpConnection;

using Handler = std::function<void(std::shared_ptr<TcpConnection>)>;

} }

#endif // HANDLER_H

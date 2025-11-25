#ifndef COMM_H
#define COMM_H
#include <sstream>
#include <signal.h>
#include <stdlib.h>
const int EPOLLSIZE = 1024;

namespace comm
{
namespace conv
{
/**
 * @brief 类型转换模板
 *
 * @tparam D 目标类型
 * @tparam S 源类型
 * @param s 源类型值
 *
 * @return 目标类型值
 */
template <typename D, typename S>
D conv( const S& s );

}
}  // namespace util

template <typename D, typename S>
D comm::conv::conv( const S& s )
{
    std::stringstream ss;
    ss << s;
    D d;
    ss >> d;
    return d;
}

#endif /* main_h */

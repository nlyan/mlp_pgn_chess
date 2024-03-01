#pragma once

namespace mlp::chess
{

template<class... Ts>
struct overloaded : Ts ... { using Ts::operator()...; };

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

} // namespace mlp::chess

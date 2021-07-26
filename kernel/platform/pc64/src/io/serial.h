#pragma once

namespace platform {
class Serial {
public:
  static void Init();
  static void WaitTxRdy();
  static void Tx(char ch);
};
} // namespace platform
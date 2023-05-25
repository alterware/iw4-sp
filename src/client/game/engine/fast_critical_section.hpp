#pragma once

namespace game::engine {
class fast_critical_section_scope_read {
public:
  fast_critical_section_scope_read(FastCriticalSection* cs);
  ~fast_critical_section_scope_read();

private:
  FastCriticalSection* cs_;
};

class fast_critical_section_scope_write {
public:
  fast_critical_section_scope_write(FastCriticalSection* cs);
  ~fast_critical_section_scope_write();

private:
  FastCriticalSection* cs_;
};
} // namespace game::engine

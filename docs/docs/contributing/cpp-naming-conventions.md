---
title: C++ Naming Conventions
id: cpp-naming-conventions
---

## 1. Files

**snake_case**

```sh
client/main.cpp
ecs/registry.h
```

### 1.1 C++ files extensions

**.cpp** for source & **.hpp** for headers.

```sh
main.cpp
main.h
```

In addition, you must use **.tcc** when your header file declares a templated type.
The **.h** only declares the type, the implementation is in the **.tcc** file.

```sh
smart_pointer.h # Includes smart_pointer.tcc at the end
smart_pointer.tcc
```

## 2. Types

**PascalCase**

```cpp
class PlayerController {};
struct Position {};
enum class Direction { North, South };
using Index = std::uint32_t;
```

## 3. Functions

**snake_case**

```cpp
void update_position(int id);
int get_position(int id);
```

## 4. Variables

`snake_case`

```cpp
int health_point;
std::string player_name;
```

## 5. Prefixes

### 5.1 Member variables `m_`

```cpp
class Player
{
 private:
    std::string m_name;
    int m_health;
};
```

### 5.2 Static member variables `s_`

```cpp
class Player
{
 public:
    static std::size_t s_player_count;
};
```

### 5.3 Global variables `g_`

```cpp
int g_frame_count;

int main()
{
    return 0;
}
```

### 5.4 Thread-local variable `tl_`

```cpp
thread_local std::string log_context;
```

### 5.5 Constants `k_`

```cpp
constexpr float k_pi = 3.14159f;
```

### 5.6 Template parameters `T`

```cpp
template <typename T>
class Array {};

template <typename TKey, typename TValue>
class Dictionary {};
```

### 5.7 Interfaces class `I`

```cpp
IRenderer {
    virtual void render() = 0;
};
```

## 6. Macros

**FULL_CAPS_WITH_UNDERSCORES**.

```cpp
#define LOG_DEBUG(x)
#define ENGINE_VERSION "1.2.0"
```

## 7. Namespaces

Short **lowercase** name.

```cpp
namespace net { ... } // == networking
namespace io { ... }  // == file I/O
```

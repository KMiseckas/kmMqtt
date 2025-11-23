# Contributing to CleanMQTT

Thank you for your interest in contributing to CleanMQTT! This document provides guidelines and instructions for contributing to the project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [How to Contribute](#how-to-contribute)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Testing Guidelines](#testing-guidelines)
- [Pull Request Process](#pull-request-process)
- [Reporting Bugs](#reporting-bugs)
- [Suggesting Enhancements](#suggesting-enhancements)
- [Community](#community)

---

## Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inclusive environment for all contributors. We expect everyone to:

- Be respectful and considerate
- Accept constructive criticism gracefully
- Focus on what's best for the community
- Show empathy towards others

### Unacceptable Behavior

- Harassment, trolling, or discriminatory language
- Personal attacks or insults
- Publishing others' private information
- Any conduct inappropriate in a professional setting

---

## Getting Started

### Prerequisites

Before contributing, ensure you have:

- **C++ Compiler**: GCC 5.0+, Clang 3.4+, or MSVC 2015+
- **CMake**: Version 3.5 or higher
- **Git**: For version control
- **Code Editor/IDE**: Your choice (VS Code, CLion, Visual Studio, etc.)

### Setting Up Your Development Environment

1. **Fork the Repository**

   Click the "Fork" button on the [CleanMQTT GitHub repository](https://github.com/KMiseckas/CleanMQTT).

2. **Clone Your Fork**

   ```bash
   git clone https://github.com/YOUR_USERNAME/CleanMQTT.git
   cd CleanMQTT
   ```

3. **Add Upstream Remote**

   ```bash
   git remote add upstream https://github.com/KMiseckas/CleanMQTT.git
   ```

4. **Build the Project**

   ```bash
   # Configure
   cmake -S . -B build -DBUILD_UNIT_TESTS=ON -DBUILD_EXAMPLES=ON
   
   # Build
   cmake --build build
   ```

5. **Run Tests**

   ```bash
   ./build/tests/cleanMqttTests
   ```

---

## How to Contribute

There are many ways to contribute to CleanMQTT:

### 🐛 Reporting Bugs

Found a bug? Please open an issue with:
- Clear, descriptive title
- Steps to reproduce
- Expected vs. actual behavior
- Environment details (OS, compiler, version)
- Minimal code example if applicable

### ✨ Suggesting Features

Have an idea? Open an issue with:
- Clear description of the feature
- Use cases and benefits
- Potential implementation approach
- Consideration of breaking changes

### 📝 Improving Documentation

Documentation improvements are always welcome:
- Fix typos or unclear explanations
- Add examples
- Improve API documentation
- Translate documentation

### 💻 Contributing Code

Code contributions are highly valued:
- Bug fixes
- New features
- Performance improvements
- Code refactoring
- Test additions

---

## Development Workflow

### 1. Create a Branch

Create a descriptive branch name:

```bash
# For features
git checkout -b feature/add-tls-support

# For bug fixes
git checkout -b fix/connection-timeout

# For documentation
git checkout -b docs/improve-examples
```

### 2. Make Your Changes

- Write clean, maintainable code
- Follow the coding standards (see below)
- Add tests for new functionality
- Update documentation as needed

### 3. Test Your Changes

```bash
# Build with tests
cmake -S . -B build -DBUILD_UNIT_TESTS=ON

# Run unit tests
./build/tests/cleanMqttTests

# Run with sanitizers (recommended)
cmake -S . -B build --preset linux-gcc-ubsan-asan
cmake --build build
./build/tests/cleanMqttTests
```

### 4. Commit Your Changes

Write clear, descriptive commit messages:

```bash
git add .
git commit -m "Add TLS support for secure connections

- Implement SSL socket wrapper
- Add certificate validation
- Update documentation
- Add tests for TLS connections

Fixes #123"
```

**Commit Message Guidelines:**
- Use present tense ("Add feature" not "Added feature")
- First line: brief summary (50 chars max)
- Blank line after summary
- Detailed description if needed
- Reference issue numbers

### 5. Push and Create Pull Request

```bash
git push origin feature/add-tls-support
```

Then open a Pull Request on GitHub.

---

## Coding Standards

### C++ Style Guidelines

CleanMQTT follows these coding conventions:

#### Naming Conventions

```cpp
// Classes: PascalCase
class MqttClient { };
class ByteBuffer { };

// Methods: camelCase
void publishMessage();
bool isConnected();

// Member variables: camelCase
private:
    int connectionTimeout;
    std::string clientId;

// Constants: UPPER_CASE
const int MAX_PACKET_SIZE = 256;

// Namespaces: camelCase
namespace cleanMqtt {
namespace mqtt {
```

#### Formatting

- **Indentation**: Tabs (as per existing code)
- **Braces**: Opening brace on same line for functions, new line for classes
- **Line Length**: Aim for 100 characters, but not strict
- **Spacing**: Space after keywords, around operators

```cpp
// Function braces
void myFunction() {
    // code
}

// Class braces
class MyClass
{
public:
    MyClass();
};

// Conditionals
if (condition) {
    // code
} else {
    // code
}

// Loops
for (int i = 0; i < 10; ++i) {
    // code
}
```

#### Header Guards

Use `#pragma once` for header guards:

```cpp
#pragma once

namespace cleanMqtt {
    // declarations
}
```

#### Comments

```cpp
// Single-line comments for brief explanations

/**
 * Multi-line comments for detailed documentation
 * Use for public API functions
 * 
 * @param parameter Description
 * @return Description
 */
```

### Modern C++ Practices

- Use C++14 features where appropriate
- Prefer `auto` for complex types
- Use `nullptr` instead of `NULL`
- Use smart pointers for ownership
- Prefer move semantics over copying
- Use RAII for resource management

```cpp
// Good
auto client = std::make_unique<MqttClient>();
auto payload = ByteBuffer("data");
client->publish("topic", std::move(payload), options);

// Avoid
MqttClient* client = new MqttClient();  // Manual memory management
```

---

## Testing Guidelines

### Writing Tests

All new features and bug fixes should include tests.

#### Test Structure

```cpp
#include "TestFramework.h"

TEST(CategoryName, TestName) {
    // Arrange
    MqttClient client;
    
    // Act
    auto result = client.connect(args, address);
    
    // Assert
    ASSERT_EQUAL(result, ClientError::Success);
}
```

#### Test Categories

- **Unit Tests**: Test individual components in isolation
- **Integration Tests**: Test component interactions
- **End-to-End Tests**: Test complete workflows

#### Coverage Goals

- Aim for >80% code coverage
- Critical paths should have 100% coverage
- Test both success and failure cases

### Running Tests

```bash
# Basic test run
./build/tests/cleanMqttTests

# With sanitizers
cmake --preset linux-gcc-ubsan-asan
cmake --build build
./build/tests/cleanMqttTests

# With coverage
cmake --preset linux-gcc-coverage
cmake --build build
./build/tests/cleanMqttTests
cmake --build build --target coverage_report
# View: build/coverage/index.html
```

---

## Pull Request Process

### Before Submitting

- [ ] Code builds without errors
- [ ] All tests pass
- [ ] Code follows style guidelines
- [ ] Documentation is updated
- [ ] Commit messages are clear
- [ ] Branch is up-to-date with main

### PR Template

When creating a PR, include:

```markdown
## Description
Brief description of changes

## Motivation
Why is this change needed?

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
How has this been tested?

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Comments added for complex code
- [ ] Documentation updated
- [ ] Tests added/updated
- [ ] All tests pass
- [ ] No new warnings
```

### Review Process

1. **Automated Checks**: CI/CD runs tests and checks
2. **Code Review**: Maintainers review your code
3. **Feedback**: Address any requested changes
4. **Approval**: Once approved, PR will be merged

### After Merge

- Delete your feature branch
- Update your local main branch
- Celebrate! 🎉

```bash
git checkout main
git pull upstream main
git branch -d feature/your-feature
```

---

## Reporting Bugs

### Before Reporting

1. Check existing issues for duplicates
2. Verify the bug with the latest version
3. Determine if it's actually a bug

### Bug Report Template

```markdown
**Describe the bug**
Clear description of the bug

**To Reproduce**
Steps to reproduce:
1. Configure client with...
2. Call method...
3. Observe error...

**Expected behavior**
What should happen

**Actual behavior**
What actually happens

**Environment**
- OS: [e.g., Ubuntu 20.04]
- Compiler: [e.g., GCC 9.3]
- CleanMQTT Version: [e.g., 0.0.1]
- Build Type: [Debug/Release]

**Additional context**
Code samples, logs, etc.
```

---

## Suggesting Enhancements

### Enhancement Template

```markdown
**Feature Description**
Clear description of the feature

**Use Case**
Why is this needed? Who benefits?

**Proposed Solution**
How could this be implemented?

**Alternatives Considered**
Other approaches considered

**Additional Context**
Mockups, examples, etc.
```

---

## Community

### Communication Channels

- **GitHub Issues**: Bug reports and feature requests
- **GitHub Discussions**: General questions and discussions
- **Pull Requests**: Code contributions and reviews

### Getting Help

- Check the [documentation](README.md)
- Search [existing issues](https://github.com/KMiseckas/CleanMQTT/issues)
- Ask in [Discussions](https://github.com/KMiseckas/CleanMQTT/discussions)

### Recognition

Contributors are recognized in:
- Project README
- Release notes
- Git history

---

## Development Tips

### Useful Commands

```bash
# Update from upstream
git fetch upstream
git merge upstream/main

# Run specific test
./build/tests/cleanMqttTests --filter="TestName"

# Check for memory leaks
valgrind ./build/tests/cleanMqttTests

# Format code (if using clang-format)
clang-format -i src/**/*.cpp include/**/*.h
```

### Debugging

```bash
# Debug build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# With GDB
gdb ./build/tests/cleanMqttTests

# With LLDB
lldb ./build/tests/cleanMqttTests
```

### Build Options Reference

```bash
# Enable all features for development
cmake -S . -B build \
  -DBUILD_UNIT_TESTS=ON \
  -DBUILD_EXAMPLES=ON \
  -DBUILD_BENCHMARKING=ON \
  -DENABLE_LOGS=ON \
  -DENABLE_WARNINGS_AS_ERRORS=ON
```

---

## License

By contributing to CleanMQTT, you agree that your contributions will be licensed under the same license as the project.

---

## Questions?

If you have questions about contributing, feel free to:
- Open an issue with the "question" label
- Start a discussion on GitHub Discussions
- Reach out to the maintainers

Thank you for contributing to CleanMQTT! 🚀

---

[◀ Back to Documentation Index](README.md)

#!/bin/bash
cat <<'EOF' > "main.cpp"
#include <iostream>
int main() {
    std::cout << "Hello from PMan C++ project!" << std::endl;
    return 0;
}
EOF

cat <<'EOF' > "Makefile"
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
all: main
main: main.cpp
	$(CXX) $(CXXFLAGS) -o main main.cpp
clean:
	rm -f main
EOF

if [ "$PMAN_USE_README" = "true" ]; then
  cat <<EOF > README.md
# $PMAN_PROJECT_NAME
**Author:** $PMAN_AUTHOR ($PMAN_EMAIL)
EOF
fi

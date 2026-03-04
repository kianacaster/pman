#ifndef TEMPLATES_H
#define TEMPLATES_H

const char *python_main_template = 
    "def main():\n"
    "    print(\"Hello from DInit!\")\n\n"
    "if __name__ == \"__main__\":\n"
    "    main()\n";

const char *python_gitignore_template = 
    "__pycache__/\n"
    "*.py[cod]\n"
    "*$py.class\n"
    ".venv/\n"
    "venv/\n"
    "ENV/\n";

const char *python_readme_template = 
    "# %s\n\n"
    "**Author:** %s (%s)\n\n"
    "## Setup\n"
    "```bash\n"
    "python -m venv .venv\n"
    "source .venv/bin/activate\n"
    "pip install -r requirements.txt\n"
    "```\n\n"
    "## Usage\n"
    "```bash\n"
    "python main.py\n"
    "```\n";

const char *c_main_template = 
    "#include <stdio.h>\n\n"
    "int main() {\n"
    "    printf(\"Hello from DInit C project!\\n\");\n"
    "    return 0;\n"
    "}\n";

const char *c_readme_template = 
    "# %s\n\n"
    "**Author:** %s (%s)\n\n"
    "## Build & Run\n"
    "```bash\n"
    "make\n"
    "./main\n"
    "```\n";

const char *cpp_main_template = 
    "#include <iostream>\n\n"
    "int main() {\n"
    "    std::cout << \"Hello from DInit C++ project!\" << std::endl;\n"
    "    return 0;\n"
    "}\n";

const char *cpp_makefile_template = 
    "CXX = g++\n"
    "CXXFLAGS = -Wall -Wextra -std=c++17\n\n"
    "all: main\n\n"
    "main: main.cpp\n"
    "\t$(CXX) $(CXXFLAGS) -o main main.cpp\n\n"
    "clean:\n"
    "\trm -f main\n";

const char *cpp_readme_template = 
    "# %s\n\n"
    "**Author:** %s (%s)\n\n"
    "## Build & Run\n"
    "```bash\n"
    "make\n"
    "./main\n"
    "```\n";

const char *java_main_template = 
    "public class Main {\n"
    "    public static void main(String[] args) {\n"
    "        System.out.println(\"Hello from DInit Java project!\");\n"
    "    }\n"
    "}\n";

const char *java_gitignore_template = 
    "*.class\n"
    "*.jar\n";

const char *java_readme_template = 
    "# %s\n\n"
    "**Author:** %s (%s)\n\n"
    "## Build & Run\n"
    "```bash\n"
    "javac Main.java\n"
    "java Main\n"
    "```\n";

const char *go_main_template = 
    "package main\n\n"
    "import \"fmt\"\n\n"
    "func main() {\n"
    "    fmt.Println(\"Hello from DInit Go project!\")\n"
    "}\n";

const char *go_readme_template = 
    "# %s\n\n"
    "**Author:** %s (%s)\n\n"
    "## Run\n"
    "```bash\n"
    "go run main.go\n"
    "```\n";

const char *rust_readme_template = 
    "# %s\n\n"
    "**Author:** %s (%s)\n\n"
    "## Build & Run\n"
    "```bash\n"
    "cargo run\n"
    "```\n";

const char *node_index_template = 
    "console.log('Hello from DInit Node project!');\n";

const char *node_gitignore_template = 
    "node_modules/\n"
    "dist/\n"
    ".env\n";

const char *node_readme_template = 
    "# %s\n\n"
    "**Author:** %s (%s)\n\n"
    "## Install & Run\n"
    "```bash\n"
    "npm install\n"
    "npm start\n"
    "```\n";

const char *csharp_readme_template = 
    "# %s\n\n"
    "**Author:** %s (%s)\n\n"
    "## Run\n"
    "```bash\n"
    "dotnet run\n"
    "```\n";

const char *ruby_main_template = 
    "puts 'Hello from DInit Ruby project!'\n";

const char *ruby_readme_template = 
    "# %s\n\n"
    "**Author:** %s (%s)\n\n"
    "## Run\n"
    "```bash\n"
    "ruby main.rb\n"
    "```\n";

const char *php_index_template = 
    "<?php\n"
    "echo \"Hello from DInit PHP project!\\n\";\n"
    "?>\n";

const char *php_readme_template = 
    "# %s\n\n"
    "**Author:** %s (%s)\n\n"
    "## Run\n"
    "```bash\n"
    "php index.php\n"
    "```\n";

const char *html_index_template = 
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "    <meta charset=\"UTF-8\">\n"
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "    <title>DInit Web Project</title>\n"
    "    <link rel=\"stylesheet\" href=\"style.css\">\n"
    "</head>\n"
    "<body>\n"
    "    <h1>Hello from DInit Web project!</h1>\n"
    "    <script src=\"script.js\"></script>\n"
    "</body>\n"
    "</html>\n";

const char *html_style_template = 
    "body {\n"
    "    font-family: sans-serif;\n"
    "    text-align: center;\n"
    "    margin-top: 50px;\n"
    "}\n";

const char *html_script_template = 
    "console.log('Hello from DInit Web project!');\n";

const char *html_readme_template = 
    "# %s\n\n"
    "**Author:** %s (%s)\n\n"
    "## Usage\n"
    "Open `index.html` in your browser.\n";

const char *bash_script_template = 
    "#!/bin/bash\n\n"
    "echo \"Hello from DInit Bash project!\"\n";

const char *bash_readme_template = 
    "# %s\n\n"
    "**Author:** %s (%s)\n\n"
    "## Run\n"
    "```bash\n"
    "./main.sh\n"
    "```\n";

const char *mit_license_template = 
    "MIT License\n\n"
    "Copyright (c) 2024 %s\n\n"
    "Permission is hereby granted, free of charge, to any person obtaining a copy\n"
    "of this software and associated documentation files (the \"Software\"), to deal\n"
    "in the Software without restriction, including without limitation the rights\n"
    "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
    "copies of the Software, and to permit persons to whom the Software is\n"
    "furnished to do so, subject to the following conditions:\n\n"
    "The above copyright notice and this permission notice shall be included in all\n"
    "copies or substantial portions of the Software.\n\n"
    "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
    "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
    "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
    "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
    "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
    "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
    "SOFTWARE.\n";

#endif

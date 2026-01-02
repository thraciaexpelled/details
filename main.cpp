#include <QApplication>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextEdit>

#include <fstream>
#include <cstdio>
#include <regex>
#include <string>

const int WIDTH = 800;
const int HEIGHT = 800;

static auto read_file(std::string_view path) -> std::string {
    constexpr auto read_size = std::size_t(4096);
    auto stream = std::ifstream(path.data());
    stream.exceptions(std::ios_base::badbit);

    if (not stream) {
        throw std::ios_base::failure("file does not exist");
    }

    auto out = std::string();
    auto buf = std::string(read_size, '\0');
    while (stream.read(& buf[0], read_size)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}

static auto strip_escape_sequence(const std::string& input) -> std::string {
    printf("WARNING: escape sequences won't be rendered in the output\n");
    std::regex escapeRegex(R"((\x1B\[[0-?]*[ -/]*[@-~]))");
    return std::regex_replace(input, escapeRegex, "");
}

auto main(int argc, char **argv) -> int
{
    QApplication app (argc, argv);
    QWidget window;
    window.setFixedSize(WIDTH, HEIGHT);

    // textEdit font
    QFont font("monospace");
    font.setStyleHint(QFont::Monospace);

    auto text = QString();
    std::string data = read_file("/dev/stdin");
    std::string cleanedData = strip_escape_sequence(data);
    text.append(cleanedData);

    QTextEdit *textEdit = new QTextEdit(&window);
    textEdit->setFont(font);
    textEdit->setFixedSize(WIDTH, HEIGHT);
    textEdit->setReadOnly(true);
    textEdit->setText(text);

    window.show();
    return app.exec();
}

#include <QApplication>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QTextEdit>
#include <QVBoxLayout>

#include <format>
#include <fstream>
#include <cstdio>
#include <regex>
#include <string>
#include <vector>

#define LEN(x) sizeof(x) / sizeof(x[0])

const int WIDTH = 800;
const int HEIGHT = 800;

const auto VERSION = "1.0.0";

static auto read_file(std::string_view path) -> std::string {
    constexpr auto read_size = std::size_t(4096);
    auto stream = std::ifstream(path.data());
    stream.exceptions(std::ios_base::badbit);
    if (not stream) {
        throw std::ios_base::failure("file does not exist");
    }
    auto out = std::string();
    auto buf = std::string(read_size, '\0');
    while (stream.read(&buf[0], read_size)) {
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

template <typename T>
static auto arrayAsVector(T *arr[], int optionalLength = 0) -> std::vector<T*> {
    if (optionalLength == 0)
        optionalLength = LEN(arr);
    std::vector<T*> vectorizedArray;
    for (int i = 0; i < optionalLength; ++i) {
        vectorizedArray.push_back(arr[i]);
    }
    return vectorizedArray;
}

auto main(int argc, char **argv) -> int {
    for (const auto &a : arrayAsVector(argv, argc)) {
        printf("INFO: item: %s\n", a);
    }

    QApplication app(argc, argv);
    QWidget window;
    window.setFixedSize(WIDTH, HEIGHT);
    window.setWindowTitle(QString::fromStdString(std::format("details - v{}", VERSION)));

    QFont font("monospace");
    font.setStyleHint(QFont::Monospace);

    std::string data = read_file("/dev/stdin");
    std::string cleanedData = strip_escape_sequence(data);
    QString text = QString::fromStdString(cleanedData);

    // bullshit
    auto *layout = new QVBoxLayout(&window);
    layout->setContentsMargins(0, 0, 0, 0);

    // text
    auto *textEdit = new QTextEdit(&window);
    textEdit->setFont(font);
    textEdit->setReadOnly(true);
    textEdit->setText(text);
    layout->addWidget(textEdit);

    // status bar
    auto lines = [&cleanedData]() {
        int c = 0;
        for (int i = 0; i < (int)cleanedData.length(); ++i) {
            if (cleanedData[i] == '\n') {
                c++;
            }
        }
        return c;
    };

    std::string dataInfo = std::format("Lines: {}, Characters: {}", lines(), cleanedData.length());
    QString QDataInfo = QString::fromStdString(dataInfo);

    auto *statusBar = new QStatusBar(&window);
    statusBar->showMessage(QDataInfo);
    layout->addWidget(statusBar);

    window.show();
    return app.exec();
}

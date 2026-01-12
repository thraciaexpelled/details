#include <QApplication>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QTextEdit>
#include <QVBoxLayout>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <fstream>
#include <memory>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

// specific headers for cross-platform compatibility
#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

const int WIDTH = 800;
const int HEIGHT = 800;

const auto VERSION = "1.1.0";

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

std::string get_command_output(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;

    // Use unique_ptr with a custom deleter to ensure pclose is always called
    // even if an exception is thrown.
    std::unique_ptr<FILE, decltype(&PCLOSE)> pipe(POPEN(cmd.c_str(), "r"), PCLOSE);

    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    // Read the output a chunk at a time
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}

auto main(int argc, char **argv) -> int {
    bool execute_and_read_cmd = false;

    if (argc > 1) {
        execute_and_read_cmd = true;
    }   

    QApplication app(argc, argv);
    QWidget window;
    window.setFixedSize(WIDTH, HEIGHT);
    window.setWindowTitle(QString::fromStdString(std::format("details - v{}", VERSION)));

    QFont font("monospace");
    font.setStyleHint(QFont::Monospace);

    std::string data;

    if (!execute_and_read_cmd) {
        data = read_file("/dev/stdin");
    } else {
        std::vector<std::string> cmd;
        for (int i = 1; i < argc; ++i) {
            cmd.push_back(std::string(argv[i]));
        }
        auto cmdline = fmt::format("{}", fmt::join(cmd, " "));
        data = get_command_output(cmdline);
    }

    if (data.empty()) {
        QMessageBox *msg = new QMessageBox(&window);
        msg->setIcon(QMessageBox::Warning);
        msg->setText("clangjesus is sorry");
        msg->setInformativeText("As soon as you click ok, you'll see an empty screen; why? Because you just encountered a bug.");
        msg->setWindowTitle("Bug");
        msg->exec();
    }

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

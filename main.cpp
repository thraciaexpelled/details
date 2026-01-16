#include <QApplication>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QTextEdit>
#include <QVBoxLayout>

#include <cstdio>
#include <cstdlib>
#include <format>
#include <fstream>
#include <regex>
#include <string>

const int WIDTH = 800;
const int HEIGHT = 800;

#ifndef VERSION
#define VERSION "2.1.0"
#endif

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

static auto pad(int length) -> std::string {
    std::string padout;
    for (int i = 0; i < length; ++i) {
        padout.append(" ");
    }
    return padout;
}

auto main(int argc, char **argv) -> int {
    bool read_from_filename = false;

    if (argc > 1) {
        read_from_filename = true;
    }

    QApplication app(argc, argv);
    QWidget window;
    window.setFixedSize(WIDTH, HEIGHT);
    window.setWindowTitle(QString::fromStdString(std::format("details v{}", VERSION)));

    QFont font("monospace");
    font.setStyleHint(QFont::Monospace);

    std::string data, filepath;

    if (!read_from_filename) {
        data = read_file("/dev/stdin");
        filepath = "Standard Input";
    } else {
        filepath = std::string(argv[1]);
        data = read_file(filepath);

        for (int i = 0; i < (int)data.length(); ++i) {
            if ((unsigned char)data[i] > 127) {
                QMessageBox *msg = new QMessageBox(&window);
                msg->setIcon(QMessageBox::Warning);
                msg->setText("Output has non-ASCII characters");
                msg->setInformativeText("Details has found non-ASCII characters in the output; which means the target file is possibly a binary. Click OK to continue anyway");
                msg->setWindowTitle("details - Binary File");
                msg->exec();
                break;
            }
        }
    }

    std::string cleanedData = strip_escape_sequence(data);
    QString text = QString::fromStdString(cleanedData);

    // bullshit
    auto *layout = new QVBoxLayout(&window);
    layout->setContentsMargins(0, 0, 0, 0);

    // text
    auto *textEdit = new QPlainTextEdit(&window);
    textEdit->setFont(font);
    textEdit->setReadOnly(true);
    textEdit->setPlainText(text);
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

    std::string dataInfo =
        std::format("Lines: {}, Characters: {}{}{}", lines(), cleanedData.length(), pad(WIDTH / 2), filepath);
    QString QDataInfo = QString::fromStdString(dataInfo);

    auto *statusBar = new QStatusBar(&window);
    statusBar->showMessage(QDataInfo);
    layout->addWidget(statusBar);

    window.show();
    return app.exec();
}

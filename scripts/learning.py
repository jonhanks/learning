#!/usr/bin/python3

import os.path
import random
import shutil
import subprocess
import sys
import tempfile

sys.path.insert(0, '/usr/lib/python3/dist-packages')

from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.Qsci import *


class LearningMainWindow(QMainWindow):
    def __init__(self):
        super(LearningMainWindow, self).__init__()

        self.setGeometry(100, 100, 800, 400)
        self.setWindowTitle("Learning Editor")

        self.__frm = QFrame(self)
        self.__frm.setStyleSheet("QWidget { background-color: #ffeaeaea }")
        self.__lyt = QVBoxLayout()
        self.__frm.setLayout(self.__lyt)
        self.setCentralWidget(self.__frm)
        self.__myFont = QFont()
        self.__myFont.setPointSize(14)

        self.__btn = QPushButton("Try it out!")
        # self.__btn.setFixedWidth(50)
        self.__btn.setFixedHeight(50)
        self.__btn.clicked.connect(self.__btn_action)
        self.__btn.setFont(self.__myFont)
        self.__lyt.addWidget(self.__btn)

        self.__editor = QsciScintilla()
        self.__editor.setText("""al_draw_filled_circle(static_cast<float>(100.0),
        static_cast<float>(150.0),
        static_cast<float>(15.0),
        al_map_rgb(0, 255, 0));""")
        self.__editor.setLexer(None)
        self.__editor.setUtf8(True)
        self.__editor.setFont(self.__myFont)

        self.__lyt.addWidget(self.__editor)

        self.__plugdir = '../cmake-build-debug/plugins'

        self.show()

    def setPluginDir(self, dirname):
        self.__plugdir = dirname

    def __parseProgram(self):
        text = self.__editor.text()
        includes = []
        program = []
        for line in text.split("\n"):
            if line.startswith("#include"):
                includes.append(line)
            else:
                program.append(line)
        return {"includes": "\n".join(includes), "program": "\n".join(program)}

    def __btn_action(self):
        parsedProgram = self.__parseProgram()
        program = """
#include "plugin_api.h"


#include <boost/config.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

""" + parsedProgram["includes"] + """

class SimplePlugin: public lrn::SimpleDrawApi
{
public:
    SimplePlugin() {}
    ~SimplePlugin() override {}
    std::string name() const { return "Simple"; }
    void draw() override {
""" + parsedProgram["program"] + """
    }
};

extern "C" BOOST_SYMBOL_EXPORT boost::shared_ptr<lrn::SimpleDrawApi> plugin_factory()
{
    return boost::make_shared<SimplePlugin>();
}
"""
        chars='abcdefghijklmnopqrstuvwxyz0123456789'
        postfix = ''.join([random.choice(chars) for x in range(5)])

        fname = os.path.join(self.__plugdir, "plugin" + postfix + ".cpp")

        with open(fname, 'wt') as f:
            f.write(program)

        outfile = os.path.join(self.__plugdir, 'libplugin' + postfix) # + '.so')
        args = [
            '/usr/bin/g++',
            '-std=c++14',
            '-g3',
            '-fPIC',
            '-shared',
            fname,
            '-I' + self.__plugdir,
            '-I/opt/boost-1.67.0/include',
            '-L/opt/boost-1.67.0/lib',
            '-lallegro',
            '-lallegro_primitives',
            '-o',
            outfile,
        ]
        print("Running {0}".format(args))
        result = subprocess.run(args, timeout=10)
        if result.returncode != 0:
            print("Building failed")
            return
        os.rename(outfile, outfile + '.so')

with tempfile.TemporaryDirectory() as plugdir:
    subprocess.run(['../cmake-build-debug/learning', '-a', os.path.join(plugdir, 'plugin_api.h')])
    subprocess.Popen(['../cmake-build-debug/learning', '-p', plugdir])
    app = QApplication(sys.argv)
    QApplication.setStyle(QStyleFactory.create("Fusion"))
    myGUI = LearningMainWindow()
    myGUI.setPluginDir(plugdir)
    sys.exit(app.exec_())

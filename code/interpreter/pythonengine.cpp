#include "../utility/logger.h"
#include "pythonengine.h"
#include "python.h"

PythonEngine::PythonEngine() = default;

PythonEngine::~PythonEngine() {
    if (Py_IsInitialized()) { Py_Finalize(); }
}

// Get output from std out
std::string PythonEngine::getStdout(bool clear) {
    m_stdout_value = PyObject_GetAttrString(m_stdout, "value");
    std::string str_val(PyUnicode_AsUTF8(m_stdout_value));
    if (clear) PyObject_SetAttrString(m_stdout, "value", PyUnicode_FromString(""));
    return str_val;
}

// Get output from std error
std::string PythonEngine::getStderr(bool clear) {
    m_stderr_value = PyObject_GetAttrString(m_stderr, "value");
    std::string str_val(PyUnicode_AsUTF8(m_stderr_value));
    if (clear) PyObject_SetAttrString(m_stderr, "value", PyUnicode_FromString(""));
    return str_val;
}

void PythonEngine::append_module(std::string name, PyObject *(*init_func)()) {
    std::pair<std::string, PyObject *(*)(void)> module(name, init_func);
    m_embedded_modules.push_back(module);
}

bool PythonEngine::initialize() {
#ifndef NDEBUG
    debug() << "Initializing Python engine...";
#endif
    wchar_t program_name[] = L"Minotaur-CPP";
    Py_SetProgramName(program_name);
    for (auto &m_embedded_module : m_embedded_modules) {
        PyImport_AppendInittab(m_embedded_module.first.c_str(), m_embedded_module.second);
    }
    Py_Initialize();

    // Add the import path for our scripts
#ifndef NDEBUG
    debug() << "Adding import path for scripts";
#endif
    m_main_module = PyImport_AddModule("__main__");
    PyRun_SimpleString("import sys, os\n"
                           "root_path = os.getcwd()\n"
                           "if root_path[-1] == os.sep:\n"
                           "\troot_path = root_path[:len(root_path) - 1]\n"
                           "sys.path.insert(0, root_path + os.sep + \""
                           PYTHON_SCRIPT_DIR
                           "\")\n");
#ifndef NDEBUG
    debug() << "Redirecting stdout and stderr";
#endif
    PyRun_SimpleString("class CatchOut:\n"
                           "\tdef __init__(self):\n"
                           "\t\tself.value = ''\n"
                           "\tdef write(self, txt):\n"
                           "\t\tself.value += txt\n"
                           "\tdef flush(self):\n"
                           "\t\tself.value = ''\n"
                           "catchStdout = CatchOut()\n"
                           "catchStderr = CatchOut()\n"
                           "sys.stdout = catchStdout\n"
                           "sys.stderr = catchStderr\n");
    m_stdout = PyObject_GetAttrString(m_main_module, "catchStdout");
    m_stderr = PyObject_GetAttrString(m_main_module, "catchStderr");
#ifndef NDEBUG
    debug() << "Python engine initialized";
#endif
    return (bool) Py_IsInitialized();
}

bool PythonEngine::stopEngine() {
    if (!Py_IsInitialized()) {
        return true;
    }
#ifndef NDEBUG
    debug() << "Stopping Python engine";
#endif
    Py_Finalize();
    m_main_module = nullptr;
    m_stderr = nullptr;
    m_stdout = nullptr;
    m_stdout_value = nullptr;
    m_stderr_value = nullptr;
    return !Py_IsInitialized();
}

bool PythonEngine::isReady() {
    return static_cast<bool>(Py_IsInitialized());
}

bool PythonEngine::run(std::string script, std::string *out, std::string *err) {
    if (!Py_IsInitialized()) {
        return false;
    }
    PyRun_SimpleString(script.c_str());
    *out = getStdout();
    *err = getStderr();
    return err->empty();
}

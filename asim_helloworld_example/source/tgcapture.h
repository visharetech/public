#ifndef _TGCAPTURE_H_
#define _TGCAPTURE_H_

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <queue>
#include <type_traits>
#include <string.h>
#include <unordered_set>
#include <limits.h>
#include "tgcommon.h"

#include "common_with_hls.h"
#include "xmem.h"

//#define DEBUG 1
typedef struct{
    unsigned int width;
    unsigned int len;
    bool dynamic_array;
    datatype_t datatype;
    std::queue<uint8_t> content;
}signal_info_t;

enum PARENT_FUNCTION_STATUS{
    PARENT_FUNC_NONE,
    PARENT_FUNC_MATCHED,
    PARENT_FUNC_NOT_MATCHED
};

class CCapture {
public:
    enum CAPTURE_STATUS{
        ON_OPEN,
        ON_CAPTURE
    };

public:
    CCapture(){
        test_count = 0;
    }

    static void set_parent_func(const char *func_name) {
        std::string str(func_name);
        std::string toRemove = "_impl";

        size_t pos = str.rfind(toRemove);
        if (pos != std::string::npos) {
            str.erase(pos, toRemove.length());
        }

        parent_func = str;
    }

    void set_logfile(const char *path){
        filepath = path;
    }

    void init_var(std::istringstream &paralist_str) {
    }

    enum PARENT_FUNCTION_STATUS is_parent_func(const char *func_name) {
        if (parent_func.empty()){
            return PARENT_FUNC_NONE;
        }

        std::string str_func_name(func_name);
        return (str_func_name == parent_func) ? PARENT_FUNC_MATCHED : PARENT_FUNC_NOT_MATCHED;
    }

    // Variadic template function to handle multiple variables
    template <typename T, typename... Args>
    typename std::enable_if<!std::is_class<T>::value, void>::type
    init_var(std::istringstream &paralist_str, const T &data, const Args... args){
        std::string varname;
        std::getline (paralist_str, varname, ',');
        varname = trim_string(varname);
#if DEBUG
        std::cout << (int)get_datatype(data) << ' ' << varname << ' ' << sizeof(data)<< '\n';
#endif
        signal_info_t info;
        info.width = sizeof(data);
        info.len = 0;
        info.dynamic_array = false;
        info.datatype = get_datatype(data);
        sinfo[varname] = info;

        save_order.push_back(varname);

        init_var(paralist_str, args...); // Recursively dump the remaining variables
    }


    template <typename T, typename... Args>
    typename std::enable_if<std::is_class<T>::value, void>::type
    init_var(std::istringstream &paralist_str, const T &data, const Args... args){
        std::string varname;
        std::getline (paralist_str, varname, ',');
        varname = trim_string(varname);
#if DEBUG
        std::cout << "struct:" << (int)get_datatype(data) << ' ' << varname << ' ' << sizeof(data)<< '\n';
#endif
        override_capture(*this, ON_OPEN, varname, data);

        init_var(paralist_str, args...); // Recursively dump the remaining variables
    }

    void capture(bool inc_count, std::istringstream &sn){
        if (inc_count){
            ++test_count;
        }
    }

    template <typename T, std::size_t N, typename... Args>
    void capture(bool inc_count, std::istringstream &sn, const T(&value)[N], Args&&... args) {
        std::string varname;
        std::getline(sn, varname, ',');
        varname = trim_string(varname);
#if DEBUG
        std::cout << "array:" << (int)get_datatype(value) << varname << ','<< sizeof(T) << "---" <<'\n';
#endif
        signal_info_t &siginfo = sinfo[varname];

        write_data(&siginfo, &value[0], sizeof(T), N);

        capture(inc_count, sn, args...);
    }


    template <typename T, typename... Args>
    typename std::enable_if<std::is_class<T>::value>::type
    capture(bool inc_count, std::istringstream &sn, const T& value, Args... args) {

        std::string varname;
        std::getline(sn, varname, ',');
        varname = trim_string(varname);
#if DEBUG
        std::cout << "struct:" << (int)get_datatype(value) << ',' << varname << ','<< sizeof(T) <<'\n';
#endif
        override_capture(*this, ON_CAPTURE, varname, value);
        capture(inc_count, sn, args...);
    }


    template <typename T, typename... Args>\
    typename std::enable_if<!std::is_pointer<T>::value && !std::is_array<T>::value && !std::is_class<T>::value>::type
    capture(bool inc_count, std::istringstream &sn, const T& value, Args... args) {
        std::string varname;
        std::getline(sn, varname, ',');
        varname = trim_string(varname);
        
#if DEBUG
        std::cout << "native:" << (int)get_datatype(value) << ',' << varname << ','<< sizeof(T) <<'\n';
#endif
        signal_info_t &siginfo = sinfo[varname];

        write_data(&siginfo, &value, sizeof(value), 1);

        capture(inc_count, sn, args...);
    }

    template <typename T, typename... Args>
    typename std::enable_if<std::is_pointer<T>::value>::type
    capture(bool inc_count, std::istringstream &sn, const T& value, size_t num_elements, Args... args) {
        std::string varname;
        std::getline(sn, varname, ',');
        varname = trim_string(varname);

        std::string numElementStr;
        std::getline(sn, numElementStr, ',');
#if DEBUG
        std::cout << "pointer:" << (int)get_datatype(value) << ',' << sizeof(*value) << ',' << varname << ',' <<num_elements << '\n';
#endif

        signal_info_t &siginfo = sinfo[varname];


        if (value == NULL){
            auto iter = warn_msg.find(varname);
            if (iter == warn_msg.end()) {
                std::cout << "*** WARNING: " << varname << " is NULL during capture data ***\n";
                warn_msg.insert(varname);
            }

            uint8_t *fillzero = new uint8_t[sizeof(*value) * num_elements];
            memset(fillzero, 0, sizeof(*value) * num_elements);

            if (siginfo.dynamic_array){
                write_data(&siginfo, fillzero, sizeof(*value), num_elements);
            } else {
                write_data(&siginfo, fillzero, sizeof(*value) * num_elements, 1);
            }

            delete [] fillzero;
        } else {
            if (siginfo.dynamic_array) {
                write_data(&siginfo, value, sizeof(*value), num_elements);
            } else {
                write_data(&siginfo, value, sizeof(*value) * num_elements, 1);
            }
        }
        capture(inc_count, sn, args...);
    }

    void close(){
        std::ofstream file(filepath, std::ios::binary);
        std::cout << "write file to " << filepath << '\n';

        short num = 0;
        for (auto & s : sinfo) {
            if (s.second.len != 0) {
                num++;
            }
        }

        file.write(reinterpret_cast<const char*>(&num), sizeof(num));
        file.write(reinterpret_cast<const char*>(&test_count), sizeof(test_count));

        std::cout << "capture count:" << test_count << '\n';

        char emptyBuf[64];
        memset(emptyBuf, 0, 64);

        for (const std::string & signame : save_order) {
            const signal_info_t & info = sinfo[signame];
            if (info.len != 0) {
                int slen = signame.length();
                file.write(reinterpret_cast<const char*>(signame.c_str()), sizeof(char) * slen);
                file.write(reinterpret_cast<const char*>(emptyBuf), 64-slen); //fill zero byte
                file.write(reinterpret_cast<const char*>(&info.datatype), sizeof(info.datatype));
                file.write(reinterpret_cast<const char*>(&info.width), sizeof(info.width));
                file.write(reinterpret_cast<const char*>(&info.len), sizeof(info.len));
            }
        }

        for (const std::string & signame : save_order) {
            signal_info_t & info = sinfo[signame];
            if (info.len != 0) {
                int w = info.width;
                //cout << "signal name:" << signalNames[s] << "-> reg [" << qLen[signalNames[s]] << ":0] " << signalNames[s] << "_q[0:" << dataq[signalNames[s]].size() << "];\n";
                std::cout << "signal name:" << signame << "-> width: " << w << " qLen: " << info.len << " queue size:" << info.content.size() << std::endl;
                while (!info.content.empty()) {
                    char value = info.content.front();
                    file.write(&value, 1);
                    info.content.pop();
                }
            }
            info.len = 0;
        }
        file.close();
    }

private:
    std::unordered_map<std::string, signal_info_t> sinfo;
    std::vector<std::string> save_order;

    std::string trim_string(const std::string& str){
        auto start = str.find_first_not_of(" \t\n\r");
        auto end = str.find_last_not_of(" \t\n\r");
        if (start == std::string::npos || end == std::string::npos) {
            return "";
        }
        return str.substr(start, end - start + 1);
    }

    void write_data(signal_info_t *siginfo, const void *data, size_t width, size_t qlen){
        const uint8_t *data8 = (const uint8_t*)data;
        for(size_t i=0; i<width * qlen; i++){
            siginfo->content.push(data8[i]);
        }
        siginfo->width = width;
        siginfo->len += qlen;
        //std::cout  <<  siginfo->len << '\n';
    }


    uint32_t test_count;
    std::string filepath;

    static std::string parent_func;

    //Show the warning message only once
    //if dedicated variable is found inside warn_msg, do not show any more. 
    static std::unordered_set<std::string> warn_msg;
};

//specialized for struct type
template<typename T>
void override_capture(CCapture &cap, enum CCapture::CAPTURE_STATUS mode, const std::string& var_name, T &mv);

#define QUOTE(...) #__VA_ARGS__

extern CCapture *g_capture;
extern std::vector<CCapture*> capture_vec;

#define tgOpen(filepath, ...)                           \
    static CCapture cap;                                \
    static bool isOpened = false;                       \
    static unsigned int capture_count = 0;              \
    if (!isOpened) {                                    \
        std::string argvstr = QUOTE(__VA_ARGS__);       \
        std::istringstream argvstr_ss(argvstr);         \
        capture_vec.push_back(&cap);                    \
        cap.set_logfile(filepath);                      \
        cap.init_var(argvstr_ss, __VA_ARGS__);          \
        isOpened = true;                                \
    }

#define tgCaptureBeforeCall(...) do {                       \
        if (capture_count < MAX_CAPTURE_COUNT) {            \
            std::string argvstr = QUOTE(__VA_ARGS__);       \
            std::istringstream argvstr_ss(argvstr);         \
            cap.capture(false, argvstr_ss, __VA_ARGS__);    \
            enum PARENT_FUNCTION_STATUS parent_func_status = cap.is_parent_func(__func__);    \
            if (parent_func_status == PARENT_FUNC_NONE || parent_func_status == PARENT_FUNC_MATCHED) {   \
                g_capture = &cap;                           \
            }                                               \
        }                                                   \
    } while (0);

#define tgCaptureAfterCall(...) do {                        \
        if (capture_count < MAX_CAPTURE_COUNT) {            \
            std::string argvstr = QUOTE(__VA_ARGS__);       \
            std::istringstream argvstr_ss(argvstr);         \
            cap.capture(true, argvstr_ss, __VA_ARGS__);     \
            enum PARENT_FUNCTION_STATUS parent_func_status = cap.is_parent_func(__func__);   \
            if (parent_func_status == PARENT_FUNC_NONE || parent_func_status == PARENT_FUNC_MATCHED) {  \
                ++capture_count;                            \
                g_capture = NULL;                           \
            }                                               \
        }                                                   \
    } while (0);


#define tgClose()               do {    \
        enum PARENT_FUNCTION_STATUS parent_func_status = cap.is_parent_func(__func__); \
        if (parent_func_status == PARENT_FUNC_MATCHED) {    \
            if (capture_count == MAX_CAPTURE_COUNT) {       \
                printf("*** Capture Parent and Child function ***\n");  \
                capture_count = UINT_MAX;                   \
                for (CCapture *cap_ptr: capture_vec) {      \
                    cap_ptr->close();                       \
                }                                           \
                g_capture = NULL;                           \
                printf("Capture Closed !\n\n\n\n");         \
            }                                               \
        } else if (parent_func_status == PARENT_FUNC_NONE) { \
            if (capture_count == MAX_CAPTURE_COUNT) {       \
                capture_count = UINT_MAX;                   \
                cap.close();                                \
                g_capture = NULL;                           \
                printf("Capture Closed !\n\n\n\n");         \
            }                                               \
        }                                                   \
    } while (0);



#if CAPTURE_COSIM
    //Specify the func name, 
    //tgSetParentFunc can be placed inside parent function
    #define tgSetParentFunc() CCapture::set_parent_func(__func__);


    #define tgCapureFuncArbiter(...) do {                           \
            if (g_capture != NULL) {                                \
                std::string argvstr = QUOTE(__VA_ARGS__);           \
                std::istringstream argvstr_ss(argvstr);             \
                g_capture->capture(false, argvstr_ss, __VA_ARGS__); \
            }                                                       \
        } while (0);
#else
    #define tgSetParentFunc()
    #define tgCapureFuncArbiter(...)
#endif




#endif
